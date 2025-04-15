//-----------------------------------------------------------------------------
//								XTiffTileImage.cpp
//								==================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 09/06/2021
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>
#include "XTiffTileImage.h"
#include "XLzwCodec.h"
#include "XZlibCodec.h"
#include "XJpegCodec.h"
#include "XWebPCodec.h"
#include "XPackBitsCodec.h"
#include "XPredictor.h"

// Gestion de la memoire partagee
uint8_t*     XTiffTileImage::m_gBuffer = NULL;   // Buffer global de lecture
uint32_t    XTiffTileImage::m_gBufSize = 0;     // Taille du buffer
uint8_t*     XTiffTileImage::m_gTile = NULL;     // Tile globale
uint32_t    XTiffTileImage::m_gTileSize = 0;    // Taille de la tile globale
uint8_t*     XTiffTileImage::m_gPlaneTile = NULL;// Tile globale pour les images par plans de couleurs
XTiffTileImage* XTiffTileImage::m_gLastImage = NULL; // Derniere image utilisee

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XTiffTileImage::XTiffTileImage()
{
	m_TileOffsets = m_TileCounts = NULL;
	m_ColorMap = NULL;
  m_Tile = NULL;
	m_JpegTables = NULL;
	Clear();
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
void XTiffTileImage::Clear()
{
	if (m_TileOffsets != NULL)
		delete[] m_TileOffsets;
	if (m_TileOffsets != NULL)
		delete[] m_TileCounts;
	if (m_ColorMap != NULL)
		delete[] m_ColorMap;
	if (m_JpegTables != NULL)
		delete[] m_JpegTables;
	m_TileOffsets = NULL;
	m_TileCounts = NULL;
	m_ColorMap = NULL;
  m_Tile = NULL;
	m_JpegTables = NULL;

	m_nW = m_nH = m_nTileWidth = m_nTileHeight = m_nNbTile = 0;
  m_nPixSize = m_nPhotInt = m_nCompression = m_nPredictor = m_nColorMapSize = 0;
	m_dX0 = m_dY0 = m_dGSD = 0.;
	m_nLastTile = 0xFFFFFFFF;
	m_nJpegTablesSize = 0;
  if (m_gLastImage == this) m_gLastImage = NULL;
}

//-----------------------------------------------------------------------------
// Metadonnees de l'image sous forme de cles / valeurs
//-----------------------------------------------------------------------------
std::string XTiffTileImage::Metadata()
{
  std::ostringstream out;
  out << XBaseImage::Metadata();
  out << "Nb Tiles:" << m_nNbTile << ";TileW:" << m_nTileWidth << ";TileH:" << m_nTileHeight
    << ";Compression:" << XTiffReader::CompressionString(m_nCompression) << ";Predictor:" << m_nPredictor
    << ";PhotInt:" << XTiffReader::PhotIntString(m_nPhotInt) << ";PlanarConfiguration:" << m_nPlanarConfig
    << ";SampleFormat:" << m_nSampleFormat << ";";
  return out.str();
}

//-----------------------------------------------------------------------------
// Fixe les caracteristiques de l'image
//-----------------------------------------------------------------------------
bool XTiffTileImage::SetTiffReader(XBaseTiffReader* reader)
{
	if ((reader->TileWidth() < 1) || (reader->TileHeight() < 1))
		return false;
	Clear();

	if (!reader->GetTileInfo(&m_nNbTile, &m_TileOffsets, &m_TileCounts))
		return false;
	reader->GetJpegTablesInfo(&m_JpegTables, &m_nJpegTablesSize);
  reader->GetColorMap(&m_ColorMap, &m_nColorMapSize);

	m_nW = reader->Width();
	m_nH = reader->Height();
	m_nTileWidth = reader->TileWidth();
	m_nTileHeight = reader->TileHeight();

	m_nNbBits = reader->NbBits();
	m_nNbSample = reader->NbSample();
  m_nSampleFormat = reader->SampleFormat();
  m_nPixSize = PixSize();
	if (m_nPixSize == 0)
		return false;
	m_nPhotInt = reader->PhotInt();
  m_nPlanarConfig = reader->PlanarConfig();
	m_nCompression = reader->Compression();
	m_nPredictor = reader->Predictor();

	m_dX0 = reader->X0();
	m_dY0 = reader->Y0();
	m_dGSD = reader->GSD();

	return AllocBuffer();
}

//-----------------------------------------------------------------------------
// Allocation des buffers de lecture
//-----------------------------------------------------------------------------
bool XTiffTileImage::AllocBuffer()
{
	if (m_nNbTile < 1)
		return false;
	uint32_t maxsize = 0;
	for (uint32_t i = 0; i < m_nNbTile; i++)
		if (m_TileCounts[i] > maxsize)
			maxsize = m_TileCounts[i];

  if (maxsize > m_gBufSize) {
    if (m_gBuffer != NULL) delete[] m_gBuffer;
    m_gBuffer = new uint8_t[maxsize];
    if (m_gBuffer == NULL)
      return false;
    m_gBufSize = maxsize;
  }
  uint32_t tileSize = m_nTileWidth * m_nTileHeight * m_nPixSize;
  if (tileSize > m_gTileSize) {
    if (m_gTile != NULL) {
      delete[] m_gTile;
      m_gLastImage = NULL;
    }
    m_gTile = new uint8_t[tileSize];
    if (m_gTile == NULL)
      return false;
    m_gTileSize = tileSize;
    if (m_nPlanarConfig == 2) {
      if (m_gPlaneTile != NULL)
        delete[] m_gPlaneTile;
      m_gPlaneTile = new uint8_t[tileSize];
      if (m_gPlaneTile == NULL)
        return false;
    }
  }

	return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une Tile
//-----------------------------------------------------------------------------
bool XTiffTileImage::LoadTile(XFile* file, uint32_t x, uint32_t y)
{
	uint32_t nbTileW = (uint32_t)ceil((double)m_nW / (double)m_nTileWidth);
	uint32_t nbTileH = (uint32_t)ceil((double)m_nH / (double)m_nTileHeight);

	if ((x > nbTileW) || (y > nbTileH))
		return false;
	uint32_t numTile = y * nbTileW + x;
	if (numTile > m_nNbTile)
		return false;
  if ((numTile == m_nLastTile)&&(m_gLastImage == this))	// La Tile est deja chargee
		return true;
  if (m_nPlanarConfig == 1) {
    file->Seek(m_TileOffsets[numTile]);
    uint32_t nBytesRead = file->Read((char*)m_gBuffer, m_TileCounts[numTile]);
    if (nBytesRead != m_TileCounts[numTile])
      return false;
    m_nLastTile = numTile;
    if (!Decompress())
      return false;
  } else {
    if (!LoadPlaneTile(file, numTile))
      return false;
  }
  m_nLastTile = numTile;
  m_gLastImage = this;
	return PostProcess();
}

//-----------------------------------------------------------------------------
// Chargement d'une Tile dans une image en plans couleurs
//-----------------------------------------------------------------------------
bool XTiffTileImage::LoadPlaneTile(XFile* file, uint32_t numTile)
{
  uint32_t nbTileW = (uint32_t)ceil((double)m_nW / (double)m_nTileWidth);
  uint32_t nbTileH = (uint32_t)ceil((double)m_nH / (double)m_nTileHeight);

  uint16_t pixSize = m_nPixSize;
  m_nPixSize = m_nNbBits / 8; // Pour la decompression
  for (uint16_t i = 0; i < m_nNbSample; i++) {
    if (m_ChannelHints != NULL)
      if ((m_ChannelHints[0] != i)&&(m_ChannelHints[1] != i)&&(m_ChannelHints[2] != i))
        continue;
    uint32_t num = numTile + i * nbTileW * nbTileH;
    file->Seek(m_TileOffsets[num]);
    uint32_t nBytesRead = file->Read((char*)m_gBuffer, m_TileCounts[num]);
    if (nBytesRead != m_TileCounts[num])
      return false;
    m_nLastTile = num;
    if (!Decompress())
      return false;
    uint8_t *ptrPlane = m_gPlaneTile, *ptrTile = m_Tile;
    ptrPlane += (i * m_nPixSize);
    for (uint32_t j = 0; j < m_nTileWidth * m_nTileHeight; j++) {
      memcpy(ptrPlane, ptrTile, m_nPixSize);
      ptrPlane += (m_nNbSample * m_nPixSize);
      ptrTile += m_nPixSize;
    }
  }
  ::memcpy(m_Tile, m_gPlaneTile, m_nTileWidth * m_nTileHeight * m_nNbSample * m_nPixSize);
  m_nPixSize = pixSize;
  return true;
}

//-----------------------------------------------------------------------------
// Decompression d'une Tile
//-----------------------------------------------------------------------------
bool XTiffTileImage::Decompress()
{
	if ((m_nCompression == XTiffReader::UNCOMPRESSED1) || (m_nCompression == XTiffReader::UNCOMPRESSED2)) {
    ::memcpy(m_Tile, m_gBuffer, m_TileCounts[m_nLastTile]);
		return true;
	}
	if (m_nCompression == XTiffReader::PACKBITS) {
		XPackBitsCodec codec;
    return codec.Decompress(m_gBuffer, m_TileCounts[m_nLastTile], m_Tile, m_nTileWidth * m_nTileHeight * m_nPixSize);
	}
	if (m_nCompression == XTiffReader::LZW) {
		XLzwCodec codec;
    codec.SetDataIO(m_gBuffer, m_Tile, m_nTileHeight*m_nTileWidth*m_nPixSize);
		codec.Decompress();
		XPredictor::Decode(m_Tile, m_nTileWidth, m_nTileHeight, m_nPixSize, m_nNbBits, m_nPredictor);
		return true;
	}
	if (m_nCompression == XTiffReader::DEFLATE) {
		XZlibCodec codec;
    bool flag = codec.Decompress(m_gBuffer, m_TileCounts[m_nLastTile], m_Tile, m_nTileWidth * m_nTileHeight * m_nPixSize);
		XPredictor::Decode(m_Tile, m_nTileWidth, m_nTileHeight, m_nPixSize, m_nNbBits, m_nPredictor);
    return flag;
	}
	if ((m_nCompression == XTiffReader::JPEG)||(m_nCompression == XTiffReader::JPEGv2)) {
		XJpegCodec codec;
		if (m_nPhotInt == XTiffReader::YCBCR)
      return codec.DecompressRaw(m_gBuffer, m_TileCounts[m_nLastTile], m_Tile, m_nTileWidth * m_nTileHeight * m_nPixSize,
																	m_JpegTables, m_nJpegTablesSize);
    return codec.Decompress(m_gBuffer, m_TileCounts[m_nLastTile], m_Tile, m_nTileWidth * m_nTileHeight * m_nPixSize,
														m_JpegTables, m_nJpegTablesSize);
	}
	if (m_nCompression == XTiffReader::WEBP) {
		XWebPCodec codec;
		return codec.Decompress(m_gBuffer, m_TileCounts[m_nLastTile], m_Tile, m_nTileWidth * m_nTileHeight * m_nPixSize, m_nTileWidth * m_nPixSize);
	}

	return false;
}

//-----------------------------------------------------------------------------
// Applique un post-processing sur la derniere strip chargee si necessaire
//-----------------------------------------------------------------------------
bool XTiffTileImage::PostProcess()
{
	// Cas des images 1 bit
	if ((m_nNbBits == 1) && (m_nNbSample == 1)) {
		int byteW = m_nTileWidth / 8L;
		if ((m_nTileWidth % 8L) != 0)
			byteW++;
		uint8_t* tmpTile = new uint8_t[m_nTileWidth * m_nTileHeight * m_nPixSize];
		if (tmpTile == NULL)
			return false;
		bool negatif = false;
		if ((m_nPhotInt == XTiffReader::WHITEISZERO))
			negatif = true;
		for (uint32_t num_line = 0; num_line < m_nTileHeight; num_line++) {
			uint8_t* line = &tmpTile[m_nTileWidth * m_nPixSize * num_line];
			uint8_t* bit = &m_Tile[byteW * num_line];
			int n = 0;
			if (negatif) {
				for (int i = 0; i < (int)(byteW); i++)
					for (int j = 7; (j >= 0); j--)
						line[n++] = (1 - ((bit[i] >> j) & 1)) * 255;
			}
			else {
				for (int i = 0; i < (int)(byteW); i++)
					for (int j = 7; (j >= 0); j--)
						line[n++] = ((bit[i] >> j) & 1) * 255;
			}
		}
		uint8_t* oldTile = m_Tile;
		m_Tile = tmpTile;
		delete[] oldTile;
		return true;
	}

	// Cas des images WHITEISZERO
	if ((m_nPhotInt == XTiffReader::WHITEISZERO) && (m_nNbSample == 1)) {
		;
	}

	// Cas des images YCBCR)
	if ((m_nPhotInt == XTiffReader::YCBCR) && (m_nNbSample == 3))
		return XBaseImage::YCbCr2RGB(m_Tile, m_nTileWidth, m_nTileHeight);

	// Cas des images CMYK
	if ((m_nPhotInt == XTiffReader::CMYKPHOT) && (m_nNbSample == 4))
		return XBaseImage::CMYK2RGBA(m_Tile, m_nTileWidth, m_nTileHeight);

	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une ROI
//-----------------------------------------------------------------------------
bool XTiffTileImage::GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint32_t startX = (uint32_t)floor((double)x / (double)m_nTileWidth);
	uint32_t startY = (uint32_t)floor((double)y / (double)m_nTileHeight);
	uint32_t endX = (uint32_t)floor((double)(x + w - 1) / (double)m_nTileWidth);
	uint32_t endY = (uint32_t)floor((double)(y + h - 1) / (double)m_nTileHeight);

  m_Tile = m_gTile;
  //m_nLastTile = 0xFFFFFFFF;
	for (uint32_t i = startY; i <= endY; i++) {
		for (uint32_t j = startX; j <= endX; j++) {
			if (!LoadTile(file, j, i))
				return false;
			if (!CopyTile(j, i, x, y, w, h, area))
				return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Copie les pixels d'une tile dans une ROI
//-----------------------------------------------------------------------------
bool XTiffTileImage::CopyTile(uint32_t tX, uint32_t tY, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	// Intersection dans la tile en X
	uint32_t startX = tX * m_nTileWidth;
	if (startX >= x)
		startX = 0;
	else
		startX = x - startX;
	uint32_t endX = (tX + 1) * m_nTileWidth;
	if (endX <= x + w)
		endX = m_nTileWidth;
	else
		endX = m_nTileWidth - (endX - (x + w));
	// Intersection dans la tile en Y
	uint32_t startY = tY * m_nTileHeight;
	if (startY >= y)
		startY = 0;
	else
		startY = y - startY;
	uint32_t endY = (tY + 1) * m_nTileHeight;
	if (endY <= y + h)
		endY = m_nTileHeight;
	else
		endY = m_nTileHeight - (endY - (y + h));

	// Debut dans la ROI
	uint32_t X0 = 0;
	uint32_t Y0 = 0;
	if (tX * m_nTileWidth > x)
		X0 = tX * m_nTileWidth - x;
	if (tY * m_nTileHeight > y)
		Y0 = tY * m_nTileHeight - y;

	// Copie dans la ROI
	uint32_t lineSize = (endX - startX) * m_nPixSize;
	uint32_t nbline = endY - startY;
	for (uint32_t i = 0; i < nbline; i++) {
		uint8_t* source = &m_Tile[((i + startY) * m_nTileWidth + startX) * m_nPixSize];
		uint8_t* dest = &area[(Y0 * w + i * w + X0) * m_nPixSize];
		if (((Y0 * w + i * w + X0) * m_nPixSize + lineSize) > (w * h * m_nPixSize))
			return false;
    ::memcpy(dest, source, lineSize);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une ligne de pixels
//-----------------------------------------------------------------------------
bool XTiffTileImage::GetLine(XFile* file, uint32_t num, uint8_t* area)
{
	return GetArea(file, 0, num, m_nW, 1, area);
}

//-----------------------------------------------------------------------------
// Recuperation d'une ROI avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XTiffTileImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	if (factor == 0) return false;
	if (factor == 1) return GetArea(file, x, y, w, h, area);
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint32_t startX = (uint32_t)floor((double)x / (double)m_nTileWidth);
	uint32_t startY = (uint32_t)floor((double)y / (double)m_nTileHeight);
	uint32_t endX = (uint32_t)floor((double)(x + w - 1) / (double)m_nTileWidth);
	uint32_t endY = (uint32_t)floor((double)(y + h - 1) / (double)m_nTileHeight);

  m_Tile = m_gTile;
  //m_nLastTile = 0xFFFFFFFF;
  for (uint32_t i = startY; i <= endY; i++) {
		for (uint32_t j = startX; j <= endX; j++) {
			if (!LoadTile(file, j, i))
				return false;
			if (!CopyZoomTile(j, i, x, y, w, h, area, factor))
				return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Copie les pixels d'une tile dans une ROI avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XTiffTileImage::CopyZoomTile(uint32_t tX, uint32_t tY, uint32_t x, uint32_t y, uint32_t w, uint32_t h, 
																	uint8_t* area, uint32_t factor)
{
	uint32_t wout = w / factor;
	uint32_t hout = h / factor;

	// Copie dans la ROI
	uint32_t ycur = y;
	for (uint32_t numli = 0; numli < hout; numli++) {
		if ((ycur < tY * m_nTileHeight) || (ycur > (tY + 1) * m_nTileHeight)) {
			ycur += factor;
			continue;
		}
		uint32_t numTileLine = (ycur - tY * m_nTileHeight);
    if (numTileLine >= m_nTileHeight)
      break;
		uint32_t xcur = x;
		for (uint32_t numco = 0; numco < wout; numco++) {
			if ((xcur < tX * m_nTileWidth) || (xcur > (tX + 1) * m_nTileWidth)) {
				xcur += factor;
				continue;
			}
			uint32_t numTileCol = (xcur - tX * m_nTileWidth);
      if (numTileCol >= m_nTileWidth)
        break;
			::memcpy(&area[(numli * wout + numco) * m_nPixSize], &m_Tile[(numTileLine* m_nTileWidth + numTileCol)* m_nPixSize], m_nPixSize);
			xcur += factor;
		}
		ycur += factor;
	}

	return true;
}
