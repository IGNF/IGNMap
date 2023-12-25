//-----------------------------------------------------------------------------
//								XBigTiffReader.cpp
//								==================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/07/2021
//-----------------------------------------------------------------------------

#include <cstring>
#include "XBigTiffReader.h"
#include "../XTool/XFile.h"

//-----------------------------------------------------------------------------
// Lecture de l'entete du fichier TIFF
//-----------------------------------------------------------------------------
bool XBigTiffReader::ReadHeader(std::istream* in)
{
  uint16_t identifier, version, bytesize, zero;
  in->read((char*)&identifier, sizeof(uint16_t));
  if ((identifier != 0x4949) && (identifier != 0x4D4D))
    return false;
  if (identifier == 0x4949) 
    m_bByteOrder = true; // LSB_FIRST;
  if (identifier == 0x4D4D)
    m_bByteOrder = false; // MSB_FIRST;

  m_Endian.Read(in, m_bByteOrder, &version, sizeof(version));
  m_Endian.Read(in, m_bByteOrder, &bytesize, sizeof(bytesize));
  m_Endian.Read(in, m_bByteOrder, &zero, sizeof(zero));

  m_Endian.Read(in, m_bByteOrder, &m_nIFDOffset, sizeof(m_nIFDOffset));

  if ((version != 43)||(bytesize != 8)||(zero != 0))
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un IFD
//-----------------------------------------------------------------------------
bool XBigTiffReader::ReadIFD(std::istream* in, uint64_t offset, BigTiffIFD* ifd)
{
  uint64_t nb_entries = 0;
  //in->seekg(offset);
  XFile::Seek(in, offset);
  m_Endian.Read(in, m_bByteOrder, &nb_entries, sizeof(nb_entries));
 
  for (uint16_t i = 0; i < nb_entries; i++) {
    BigTiffTag tag;
    m_Endian.Read(in, m_bByteOrder, &tag.TagId, sizeof(tag.TagId));
    m_Endian.Read(in, m_bByteOrder, &tag.DataType, sizeof(tag.DataType));
    m_Endian.Read(in, m_bByteOrder, &tag.DataCount, sizeof(tag.DataCount));
    m_Endian.Read(in, m_bByteOrder, &tag.DataOffset, sizeof(tag.DataOffset));
    ifd->TagList.push_back(tag);
  }
  m_Endian.Read(in, m_bByteOrder, &ifd->NextIFDOffset, sizeof(ifd->NextIFDOffset));
  return in->good();
}

//-----------------------------------------------------------------------------
// Lecture
//-----------------------------------------------------------------------------
bool XBigTiffReader::Read(std::istream* in)
{
  if (!ReadHeader(in))
    return false;
  m_IFD.clear();
  uint64_t offset = m_nIFDOffset;
  while (offset != 0) {
    BigTiffIFD ifd;
    if (!ReadIFD(in, offset, &ifd))
      break;
    m_IFD.push_back(ifd);
    offset = ifd.NextIFDOffset;
  }
  m_nActiveIFD = 0;
  return in->good();
}

//-----------------------------------------------------------------------------
// Reherche d'un tag
//-----------------------------------------------------------------------------
bool XBigTiffReader::FindTag(eTagID id, BigTiffTag* T)
{
  for (uint32_t i = 0; i < m_IFD[m_nActiveIFD].TagList.size(); i++) {
    if (m_IFD[m_nActiveIFD].TagList[i].TagId == id) {
      *T = m_IFD[m_nActiveIFD].TagList[i];
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// Taille des donnees contenues dans un tag
//-----------------------------------------------------------------------------
uint32_t XBigTiffReader::DataSize(BigTiffTag* T)
{
  return T->DataCount * TypeSize((eDataType)T->DataType);
}

//-----------------------------------------------------------------------------
// Lecture des donnees stockees dans un tag
//-----------------------------------------------------------------------------
uint32_t XBigTiffReader::ReadDataInTag(BigTiffTag* T, uint32_t defaut)
{
  uint16_t* retour16;
  uint32_t* retour32;
  if (DataSize(T) <= 8) {
    switch (T->DataType) {
    case BYTE: return T->DataOffset;
    case ASCII:	return T->DataOffset;
    case SHORT:
      retour16 = (uint16_t*)&T->DataOffset;
      if (m_Endian.ByteOrder() == m_bByteOrder)
        return retour16[0];
      else
        return retour16[3];
      break;
    case LONG:       
      retour32 = (uint32_t*)&T->DataOffset;
      if (m_Endian.ByteOrder() == m_bByteOrder)
        return retour32[0];
      else
        return retour32[1];
    case LONG8 :
    case SLONG8 :
      return T->DataOffset;
    } /* endswitch */
  }
  return defaut;
}

//-----------------------------------------------------------------------------
// Lecture des donnees stockees dans un tag
//-----------------------------------------------------------------------------
uint32_t XBigTiffReader::ReadIdTag(eTagID id, uint32_t defaut)
{
  BigTiffTag T;
  if (!FindTag(id, &T))
    return defaut;
  return ReadDataInTag(&T, defaut);
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de donnees
//-----------------------------------------------------------------------------
bool XBigTiffReader::ReadDataArray(std::istream* in, eTagID id, void* V, int size)
{
  BigTiffTag T;
  if (!FindTag(id, &T))
    return false;
  uint32_t datasize = DataSize(&T);
  if (datasize <= 4) {
    ::memcpy(V, &T.DataOffset, datasize);
    return true;
  }
  //in->seekg(T.DataOffset);
  XFile::Seek(in, T.DataOffset);
  m_Endian.ReadArray(in, m_bByteOrder, V, size, T.DataCount);
 
  return in->good();
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de SHORT ou LONG
//-----------------------------------------------------------------------------
uint32_t* XBigTiffReader::ReadUintArray(std::istream* in, eTagID id, uint32_t* nb_elt)
{
  BigTiffTag T;
  if (!FindTag(id, &T))
    return NULL;
  uint32_t datasize = DataSize(&T);
  if ((T.DataType != SHORT) && (T.DataType != LONG))
    return NULL;
  uint32_t* V = new uint32_t[T.DataCount];
  if (V == NULL)
    return NULL;
  *nb_elt = T.DataCount;
  if (datasize <= sizeof(T.DataOffset)) {  // Cas ou le DataOffset contient le tableau
    if (T.DataType == LONG) {
      uint32_t* ptr = (uint32_t*)&(T.DataOffset);
      for (uint32_t i = 0; i < *nb_elt; i++) {
        V[i] = *ptr; ptr++;
      }
      return V;
    }
    uint16_t* ptr = (uint16_t*)&(T.DataOffset);
    for (uint32_t i = 0; i < *nb_elt; i++) {
      V[i] = *ptr; ptr++;
    }
    return V;
  }

  XFile::Seek(in, T.DataOffset);
  if (T.DataType == LONG) {
    m_Endian.ReadArray(in, m_bByteOrder, V, sizeof(uint32_t), T.DataCount);
    return V;
  }
  // Cas SHORT
  uint16_t* data = new uint16_t[T.DataCount];
  if (data == NULL) {
    delete[] V;
    return NULL;
  }
  m_Endian.ReadArray(in, m_bByteOrder, data, sizeof(uint16_t), T.DataCount);
  for (uint32_t i = 0; i < T.DataCount; i++)
    V[i] = data[i];
  delete[] data;
  return V;
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de SHORT, LONG ou LONG8
//-----------------------------------------------------------------------------
uint64_t* XBigTiffReader::ReadUint64Array(std::istream* in, eTagID id, uint32_t* nb_elt)
{
  BigTiffTag T;
  if (!FindTag(id, &T))
    return NULL;
  uint32_t datasize = DataSize(&T);
  if ((T.DataType != SHORT) && (T.DataType != LONG) && (T.DataType != LONG8))
    return NULL;
  uint64_t* V = new uint64_t[T.DataCount];
  if (V == NULL)
    return NULL;
  *nb_elt = T.DataCount;
  if (datasize <= 8) {  // Cas ou le DataOffset contient le tableau
    uint8_t* ptr = (uint8_t*)&(T.DataOffset);
    for (uint32_t i = 0; i < T.DataCount; i++) {
      if (T.DataType == SHORT)
        V[i] = *((uint16_t*)ptr);
      if (T.DataType == LONG)
        V[i] = *((uint32_t*)ptr);
      if (T.DataType == LONG8)
        V[i] = *((uint64_t*)ptr);
      ptr += TypeSize((eDataType)T.DataType);
    }
    return V;
  }
  //in->seekg(T.DataOffset);
  XFile::Seek(in, T.DataOffset);
  // Cas LONG8
  if (T.DataType == LONG8) {
    m_Endian.ReadArray(in, m_bByteOrder, V, sizeof(uint64_t), T.DataCount);
    return V;
  }

  // Cas LONG
  if (T.DataType == LONG) {
    uint32_t* data32 = new uint32_t[T.DataCount];
    if (data32 == NULL) {
      delete[] V;
      return NULL;
    }
    m_Endian.ReadArray(in, m_bByteOrder, data32, sizeof(uint32_t), T.DataCount);
    for (uint32_t i = 0; i < T.DataCount; i++)
      V[i] = data32[i];
    delete[] data32;
    return V;
  }
  // Cas SHORT
  uint16_t* data16 = new uint16_t[T.DataCount];
  if (data16 == NULL) {
    delete[] V;
    return NULL;
  }
  m_Endian.ReadArray(in, m_bByteOrder, data16, sizeof(uint16_t), T.DataCount);
  for (uint32_t i = 0; i < T.DataCount; i++)
    V[i] = data16[i];
  delete[] data16;
  return V;
}

//-----------------------------------------------------------------------------
// Analyse d'un IFD
//-----------------------------------------------------------------------------
bool XBigTiffReader::AnalyzeIFD(std::istream* in)
{
  BigTiffTag T;
  Clear();

  m_nWidth = ReadIdTag(IMAGEWIDTH);
  m_nHeight = ReadIdTag(IMAGEHEIGHT);
  m_nRowsPerStrip = ReadIdTag(ROWSPERSTRIP);
  m_nTileWidth = ReadIdTag(TILEWIDTH);
  m_nTileHeight = ReadIdTag(TILELENGTH);
  m_nNewSubFileType = ReadIdTag(NEWSUBFILETYPE);

  m_nNbBits = m_nNbSample = m_nCompression = m_nPredictor = m_nPhotInt = m_nPlanarConfig = 0;
  m_nNbSample = (uint16_t)ReadIdTag(SAMPLESPERPIXEL, 1);
  m_nSampleFormat = (uint16_t)ReadIdTag(SAMPLEFORMAT, 1);
  m_nCompression = (uint16_t)ReadIdTag(COMPRESSION, 1);
  m_nPredictor = (uint16_t)ReadIdTag(PREDICTOR, 1);
  m_nPhotInt = (uint16_t)ReadIdTag(PHOTOMETRICINTERPRETATION);
  m_nPlanarConfig = (uint16_t)ReadIdTag(PLANARCONFIGURATION, 1);

  // Lecture du nombre de bits / sample
  if (m_nNbSample == 1)
    m_nNbBits = (uint16_t)ReadIdTag(BITSPERSAMPLE, 1);
  else {
    uint32_t nbBits;
    uint32_t* bits = ReadUintArray(in, BITSPERSAMPLE, &nbBits);
    if (bits != NULL) {
      m_nNbBits = bits[0];
      delete[] bits;
    }
  }

  // Lecture des tableaux
  m_StripOffsets = ReadUint64Array(in, STRIPOFFSETS, &m_nNbStripOffsets);
  m_StripCounts = ReadUint64Array(in, STRIPBYTECOUNTS, &m_nNbStripCounts);
  m_TileOffsets = ReadUint64Array(in, TILEOFFSETS, &m_nNbTileOffsets);
  m_TileCounts = ReadUint64Array(in, TILEBYTECOUNTS, &m_nNbTileCounts);

  // Lecture de la table des couleurs (image palette)
  if (m_nPhotInt == (uint16_t)RGBPALETTE) {
    if (FindTag(COLORMAP, &T)) {
      m_ColorMap = new uint16_t[T.DataCount];
      m_nColorMapSize = T.DataCount;
      if (!ReadDataArray(in, COLORMAP, m_ColorMap, sizeof(uint16_t))) {
        delete[] m_ColorMap;
        m_ColorMap = NULL;
      }
    }
  }

  // Lecture des tables JPEG
  if (FindTag(JPEGTABLES, &T)) {
    m_JpegTables = new uint8_t[T.DataCount];
    if (!ReadDataArray(in, JPEGTABLES, m_JpegTables, sizeof(uint8_t))) {
      delete[] m_JpegTables;
      m_JpegTables = NULL;
    }
    else
      m_nJpegTablesSize = T.DataCount;
  }
  if (FindTag(YCBCRSUBSAMPLING, &T)) {
    if (T.DataCount == 2)
      ReadDataArray(in, YCBCRSUBSAMPLING, m_YCbCrSub, sizeof(uint16_t));
  }
  if (FindTag(REFERENCEBLACKWHITE, &T)) {
    if (T.DataCount == 6) {
      ReadDataArray(in, REFERENCEBLACKWHITE, m_ReferenceBlack, 2*sizeof(uint32_t));
    }
  }

  // Lecture du GEOTIFF
  m_dGSD = 0.;
  m_dX0 = 0.;
  m_dY0 = 0.;
  double* data = NULL;
  
  if (FindTag(ModelPixelScaleTag, &T)) {
    if (T.DataCount == 3) {
      data = new double[T.DataCount];
      if (ReadDataArray(in, ModelPixelScaleTag, data, sizeof(double))) {
        m_dGSD = data[0];
      }
      delete[] data;
    }
  }
  if (FindTag(ModelTiepointTag, &T)) {
    if (T.DataCount >= 6) {
      data = new double[T.DataCount];
      if (ReadDataArray(in, ModelTiepointTag, data, sizeof(double))) {
        m_dX0 = data[3] - data[0] * m_dGSD;
        m_dY0 = data[4] - data[1] * m_dGSD;
      }
      delete[] data;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Impression des informations brutes des IFD
//-----------------------------------------------------------------------------
void XBigTiffReader::PrintIFDTag(std::ostream* out)
{
  for (uint32_t i = 0; i < m_IFD.size(); i++) {
    *out << "*********" << std::endl << "IFD " << i << "\tType\tCount\tOffset" << std::endl;
    for (uint32_t j = 0; j < m_IFD[i].TagList.size(); j++) {
      *out << m_IFD[i].TagList[j].TagId << "\t" << m_IFD[i].TagList[j].DataType << "\t"
        << m_IFD[i].TagList[j].DataCount << "\t" << m_IFD[i].TagList[j].DataOffset << std::endl;
    }
  }
}

