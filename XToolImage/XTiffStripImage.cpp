//-----------------------------------------------------------------------------
//								XTiffStripImage.cpp
//								===================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 15/06/2021
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>
#include "XTiffStripImage.h"
#include "XLzwCodec.h"
#include "XZlibCodec.h"
#include "XJpegCodec.h"
#include "XWebPCodec.h"
#include "XPackBitsCodec.h"
#include "XPredictor.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XTiffStripImage::XTiffStripImage()
{
	m_StripOffsets = m_StripCounts = NULL;
	m_ColorMap = NULL;
  m_Buffer = m_Strip = m_PlaneStrip = NULL;
	m_JpegTables = NULL;
	Clear();
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
void XTiffStripImage::Clear()
{
	if (m_StripOffsets != NULL)
		delete[] m_StripOffsets;
	if (m_StripOffsets != NULL)
		delete[] m_StripCounts;
	if (m_ColorMap != NULL)
		delete[] m_ColorMap;
	if (m_Buffer != NULL)
		delete[] m_Buffer;
	if (m_Strip != NULL)
		delete[] m_Strip;
  if (m_PlaneStrip != NULL)
    delete[] m_PlaneStrip;
	if (m_JpegTables != NULL)
		delete[] m_JpegTables;
	m_StripOffsets = NULL;
	m_StripCounts = NULL;
	m_ColorMap = NULL;
  m_Buffer = m_Strip = m_PlaneStrip = NULL;
	m_JpegTables = NULL;

	m_nW = m_nH = m_nRowsPerStrip = m_nNbStrip = 0;
  m_nPixSize = m_nPhotInt = m_nCompression = m_nPredictor = m_nColorMapSize = 0;
	m_dX0 = m_dY0 = m_dGSD = 0.;
	m_nLastStrip = 0xFFFFFFFF;
	m_nJpegTablesSize = 0;
}

//-----------------------------------------------------------------------------
// Metadonnees de l'image sous forme de cles / valeurs
//-----------------------------------------------------------------------------
std::string XTiffStripImage::Metadata()
{
  std::ostringstream out;
  out << XBaseImage::Metadata();
  out << "Nb Strip:" << m_nNbStrip << ";RowsPerStrip:" << m_nRowsPerStrip 
      << ";Compression:" << XTiffReader::CompressionString(m_nCompression) << ";Predictor:" << m_nPredictor
      << ";PhotInt:" << XTiffReader::PhotIntString(m_nPhotInt) << ";PlanarConfiguration:" << m_nPlanarConfig
      << ";SampleFormat:" << m_nSampleFormat << ";";
  return out.str();
}

//-----------------------------------------------------------------------------
// Fixe les caracteristiques de l'image
//-----------------------------------------------------------------------------
bool XTiffStripImage::SetTiffReader(XBaseTiffReader* reader)
{
	if (reader->RowsPerStrip() < 1)
		return false;
	Clear();

	if (!reader->GetStripInfo(&m_nNbStrip, &m_StripOffsets, &m_StripCounts))
		return false;
	reader->GetJpegTablesInfo(&m_JpegTables, &m_nJpegTablesSize);
  reader->GetColorMap(&m_ColorMap, &m_nColorMapSize);

	m_nW = reader->Width();
	m_nH = reader->Height();
	m_nRowsPerStrip = reader->RowsPerStrip();

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
bool XTiffStripImage::AllocBuffer()
{
	if (m_nNbStrip < 1)
		return false;
	uint64_t maxsize = 0;
	for (uint32_t i = 0; i < m_nNbStrip; i++)
		if (m_StripCounts[i] > maxsize)
			maxsize = m_StripCounts[i];
	m_Buffer = new uint8_t[maxsize];
	if (m_Buffer == NULL)
		return false;
	m_Strip = new uint8_t[m_nRowsPerStrip * m_nW * m_nPixSize];
	if (m_Strip == NULL) {
		delete[] m_Buffer;
		m_Buffer = NULL;
		return false;
	}
  if (m_nPlanarConfig == 2) {
    m_PlaneStrip = new uint8_t[m_nRowsPerStrip * m_nW * m_nPixSize];
    if (m_PlaneStrip == NULL) {
      delete[] m_Buffer;
      m_Buffer = NULL;
      delete[] m_Strip;
      m_Strip = NULL;
      return false;
    }
  }

	return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une Strip
//-----------------------------------------------------------------------------
bool XTiffStripImage::LoadStrip(XFile* file, uint32_t num)
{
	if (num >= m_nNbStrip)
		return false;
	if (num == m_nLastStrip)	// La Strip est deja chargee
		return true;
  if (m_nPlanarConfig == 1) {
    file->Seek(m_StripOffsets[num]);
    uint64_t nBytesRead = file->Read((char*)m_Buffer, m_StripCounts[num]);
    if (nBytesRead != m_StripCounts[num])
      return false;
    m_nLastStrip = num;
    if (!Decompress())
      return false;
  } else {
    if (!LoadPlaneStrip(file, num))
      return false;
  }
	return PostProcess();
}

//-----------------------------------------------------------------------------
// Chargement d'une Strip dans une image en plans couleurs
//-----------------------------------------------------------------------------
bool XTiffStripImage::LoadPlaneStrip(XFile* file, uint32_t numStrip)
{
  uint16_t pixSize = m_nPixSize;
  m_nPixSize = m_nNbBits / 8; // Pour la decompression
  for (uint16_t i = 0; i < m_nNbSample; i++) {
    if (m_ChannelHints != NULL)
      if ((m_ChannelHints[0] != i)&&(m_ChannelHints[1] != i)&&(m_ChannelHints[2] != i))
        continue;
    uint32_t num = numStrip + i * (m_nNbStrip / m_nNbSample);
    file->Seek(m_StripOffsets[num]);
    uint32_t nBytesRead = file->Read((char*)m_Buffer, m_StripCounts[num]);
    if (nBytesRead != m_StripCounts[num])
      return false;
    m_nLastStrip = num;
    if (!Decompress())
      return false;
    uint8_t *ptrPlane = m_Strip, *ptrStrip = m_PlaneStrip;
    ptrStrip += (i * m_nPixSize);
    for (uint32_t j = 0; j < m_nW * m_nRowsPerStrip; j++) {
      memcpy(ptrStrip, ptrPlane, m_nPixSize);
      ptrStrip += (m_nNbSample * m_nPixSize);
      ptrPlane += m_nPixSize;
    }
  }
  ::memcpy(m_Strip, m_PlaneStrip, m_nRowsPerStrip * m_nW * m_nPixSize * m_nNbSample);
  m_nPixSize = pixSize;
  return true;
}

//-----------------------------------------------------------------------------
// Decompression d'une Tile
//-----------------------------------------------------------------------------
bool XTiffStripImage::Decompress()
{
	if ((m_nCompression == XTiffReader::UNCOMPRESSED1) || (m_nCompression == XTiffReader::UNCOMPRESSED2)) {
		std::memcpy(m_Strip, m_Buffer, m_StripCounts[m_nLastStrip]);
		return true;
	}
	if (m_nCompression == XTiffReader::PACKBITS) {
		XPackBitsCodec codec;
		return codec.Decompress(m_Buffer, m_StripCounts[m_nLastStrip], m_Strip, m_nW * m_nRowsPerStrip * m_nPixSize);
	}
	if (m_nCompression == XTiffReader::LZW) {
		XLzwCodec codec;
		codec.SetDataIO(m_Buffer, m_Strip, m_nRowsPerStrip * m_nW * m_nPixSize);
		codec.Decompress();
    //Predictor();
    XPredictor predictor;
    predictor.Decode(m_Strip, m_nW, m_nRowsPerStrip, m_nPixSize, m_nNbBits, m_nPredictor);
		return true;
	}
	if (m_nCompression == XTiffReader::DEFLATE) {
		XZlibCodec codec;
		bool flag = codec.Decompress(m_Buffer, m_StripCounts[m_nLastStrip], m_Strip, m_nW * m_nRowsPerStrip * m_nPixSize);
    //Predictor();
    XPredictor predictor;
    predictor.Decode(m_Strip, m_nW, m_nRowsPerStrip, m_nPixSize, m_nNbBits, m_nPredictor);
    return flag;
	}
	if ((m_nCompression == XTiffReader::JPEG) || (m_nCompression == XTiffReader::JPEGv2)) {
		XJpegCodec codec;
		if (m_nPhotInt == XTiffReader::YCBCR)
			return codec.DecompressRaw(m_Buffer, m_StripCounts[m_nLastStrip], m_Strip, m_nW * m_nRowsPerStrip * m_nPixSize,
				m_JpegTables, m_nJpegTablesSize);
		return codec.Decompress(m_Buffer, m_StripCounts[m_nLastStrip], m_Strip, m_nW * m_nRowsPerStrip * m_nPixSize,
														m_JpegTables, m_nJpegTablesSize);
	}
	if (m_nCompression == XTiffReader::WEBP) {
		XWebPCodec codec;
		return codec.Decompress(m_Buffer, m_StripCounts[m_nLastStrip], m_Strip, m_nW * m_nRowsPerStrip * m_nPixSize, m_nW * m_nPixSize);
	}

	return false;
}

//-----------------------------------------------------------------------------
// Applique un post-processing sur la derniere strip chargee si necessaire
//-----------------------------------------------------------------------------
bool XTiffStripImage::PostProcess()
{
	// Cas des images 1 bit
	if ((m_nNbBits == 1) && (m_nNbSample == 1)) {
		int byteW = m_nW / 8L;
		if ((m_nW % 8L) != 0)
			byteW++;
		uint8_t* tmpStrip = new uint8_t[m_nRowsPerStrip * m_nW * m_nPixSize];
		if (tmpStrip == NULL)
			return false;
		bool negatif = false;
		if ((m_nPhotInt == XTiffReader::WHITEISZERO))
			negatif = true;
		for (uint32_t num_line = 0; num_line < m_nRowsPerStrip; num_line++) {
			uint8_t* line = &tmpStrip[m_nW * m_nPixSize * num_line];
			uint8_t* bit = &m_Strip[byteW * num_line];
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
		uint8_t* oldStrip = m_Strip;
		m_Strip = tmpStrip;
		delete[] oldStrip;
		return true;
	}

	// Case des images WHITEISZERO
	if ((m_nPhotInt == XTiffReader::WHITEISZERO) && (m_nNbSample == 1)) {
		;
	}

	// Cas des images YCBCR)
	if ((m_nPhotInt == XTiffReader::YCBCR) && (m_nNbSample == 3)) 
		return XBaseImage::YCbCr2RGB(m_Strip, m_nW, m_nRowsPerStrip);

	// Cas des images CMYK
	if ((m_nPhotInt == XTiffReader::CMYKPHOT) && (m_nNbSample == 4))
		return XBaseImage::CMYK2RGBA(m_Strip, m_nW, m_nRowsPerStrip);

	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une ROI
//-----------------------------------------------------------------------------
bool XTiffStripImage::GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint32_t startRow = (uint32_t) floor((double)y / (double)m_nRowsPerStrip);
	uint32_t endRow = (uint32_t)floor((double)(y + h - 1) / (double)m_nRowsPerStrip);

	for (uint32_t i = startRow; i <= endRow; i++) {
		if (!LoadStrip(file, i))
			return false;
		if (!CopyStrip(i, x, y, w, h, area))
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Copie les pixels d'une tile dans une ROI
//-----------------------------------------------------------------------------
bool XTiffStripImage::CopyStrip(uint32_t numStrip, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	// Interesection dans la tile en Y
	uint32_t startY = numStrip * m_nRowsPerStrip;
	if (startY >= y)
		startY = 0;
	else
		startY = y - startY;
	uint32_t endY = (numStrip + 1) * m_nRowsPerStrip;
	if (endY <= y + h)
		endY = m_nRowsPerStrip;
	else
		endY = m_nRowsPerStrip - (endY - (y + h));

	// Debut dans la ROI
	uint32_t Y0 = 0;
	if (numStrip * m_nRowsPerStrip > y)
		Y0 = numStrip * m_nRowsPerStrip - y;

	// Copie dans la ROI
	uint32_t lineSize = w * m_nPixSize;
	uint32_t nbline = endY - startY;
	for (uint32_t i = 0; i < nbline; i++) {
		uint8_t* source = &m_Strip[((i + startY) * m_nW + x) * m_nPixSize];
		uint8_t* dest = &area[(Y0 * w + i * w) * m_nPixSize];
		::memcpy(dest, source, lineSize);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une ligne de pixels
//-----------------------------------------------------------------------------
bool XTiffStripImage::GetLine(XFile* file, uint32_t num, uint8_t* area)
{
	if (num >= m_nH)
		return false;
	uint32_t numStrip = num / m_nRowsPerStrip;
	if (!LoadStrip(file, numStrip))
		return false;
	uint32_t numLine = num % m_nRowsPerStrip;
	::memcpy(area, &m_Strip[numLine * m_nW * m_nPixSize], m_nW * m_nPixSize);
	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une ROI avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XTiffStripImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	if (factor == 0) return false;
	if (factor == 1) return GetArea(file, x, y, w, h, area);
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint8_t* line = AllocArea(m_nW, 1);
	if (line == NULL)
		return false;

	uint32_t xpos = x * m_nPixSize;
	uint32_t wout = w / factor;
	uint32_t hout = h / factor;
	uint32_t maxW = XMin(xpos + w * m_nPixSize, m_nPixSize * (m_nW - 1));
	uint32_t maxH = XMin(y + h, m_nH - 1);

	uint32_t i = y;
	for (uint32_t numli = 0; numli < hout; numli++) {
		GetLine(file, i, line);
		uint32_t j = xpos;
		uint8_t* buf = &area[numli * (m_nPixSize * wout)];
		for (uint32_t numpx = 0; numpx < wout; numpx++) {
			::memcpy(buf, &line[j], m_nPixSize);
			buf += m_nPixSize;
			j += (factor * m_nPixSize);
			if (j > maxW)
				break;
		};
		i += factor;
		if (i > maxH)
			break;
	}
	delete[] line;
	return true;
}
