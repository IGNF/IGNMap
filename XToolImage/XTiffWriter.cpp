//-----------------------------------------------------------------------------
//								XTiffWriter.cpp
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/10/2000
//-----------------------------------------------------------------------------

#include "XTiffWriter.h"


//-----------------------------------------------------------------------------
// Fixe la table des couleurs
//-----------------------------------------------------------------------------
bool XTiffWriter::SetColorMap(uint8_t* color_map, int nbcomp, bool swap)
{
	if (color_map == NULL) {
		delete[] m_ColorMap;
		m_ColorMap = NULL;
		return true;
	}

	if (m_ColorMap != NULL)
		delete[] m_ColorMap;
	m_ColorMap = new uint16_t[256 * 3];
	if (m_ColorMap == NULL)
		return false;
  int pos = 0;
  if (swap) pos = 2;
	// Conversion de la palette en triplet RGB 8 bits en palette 16 bits par plan
	for (int i = 0; i < 256; i++) {
    m_ColorMap[i] = (uint16_t)color_map[nbcomp * i + pos] * 256;
    m_ColorMap[i + 256] = (uint16_t)color_map[nbcomp * i + 1] * 256;
    m_ColorMap[i + 512] = (uint16_t)color_map[nbcomp * i + 2 - pos] * 256;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Fixe la table des couleurs avec une table TIFF (uint16_t de taille 256 * 3)
//-----------------------------------------------------------------------------
bool XTiffWriter::SetColorMap(uint16_t* color_map)
{
  if (color_map == NULL) {
    delete[] m_ColorMap;
    m_ColorMap = NULL;
    return true;
  }

  if (m_ColorMap != NULL)
    delete[] m_ColorMap;
  m_ColorMap = new uint16_t[256 * 3];
  if (m_ColorMap == NULL)
    return false;
  for (int i = 0; i < 256*3; i++)
    m_ColorMap[i] = color_map[i];
  return true;
}

//-----------------------------------------------------------------------------
// Ecriture de l'entete Tiff
//-----------------------------------------------------------------------------
bool XTiffWriter::Write(const char* filename, uint32_t w, uint32_t h, uint16_t nbSample,
                        uint16_t nbBits, uint8_t* buf, uint16_t format)
{
	m_Out.open(filename, std::ios::out | std::ios::binary);
	if (!m_Out.good())
		return XErrorError(m_Error,"Impossible de creer le fichier Tiff", XError::eIOOpen);

	WriteHeader();

	bool geotiff = false;
	if (m_dGsd > 0) geotiff = true;

	uint16_t nbtag = 14;
  if (geotiff) {
    nbtag += 2;
    if (m_nEpsg > 0) nbtag++;
  }
	if (m_ColorMap != NULL) nbtag++;
  if (format > 1) nbtag++;
	m_Out.write((char*)&nbtag, sizeof(uint16_t));
	uint32_t sizetag = 12L;
	uint32_t offset = (uint32_t)m_Out.tellp() + nbtag * sizetag + sizeof(uint32_t);
	// Ecriture des tags
	WriteTag32(254, 0);			// NewSubfileType 
	WriteTag32(256, w);			// ImageWidth	
	WriteTag32(257, h);			// ImageLength

	if (nbSample == 1)
		WriteTag16(258, nbBits);
	else {
		WriteTag(258,SHORT, nbSample, offset);		// BitsPerSample
		offset += nbSample * sizeof(uint16_t);
	}

	WriteTag16(259, 1);			// Compression
	// Photo. Interpretation
	if (nbSample == 1) {
		if (m_ColorMap != NULL)
			WriteTag16(262, 3);		// Image Palette
		else
			WriteTag16(262, 1);		// Image N&B
	} else
		WriteTag16(262, 2);			// Image RGB

	WriteTag(273, LONG, h, offset);				// StripOffsets
	offset += h * sizeof(uint32_t);

	WriteTag16(277, nbSample);	// SamplesPerPixel
	WriteTag32(278, 1);			// RowsPerStrip

	WriteTag(279, LONG, h, offset);				// StripByteCounts	
	offset += h * sizeof(uint32_t);

	WriteTag(282, RATIONAL, 1, offset);      	// XResolution	
	offset += 2 * sizeof(uint32_t);
	WriteTag(283, RATIONAL, 1, offset);      	// YResolution	
	offset += 2 * sizeof(uint32_t);

	WriteTag16(284, 1);			// PlanarConfiguration
  WriteTag16(296, 3);			// ResolutionUnit

  if (format > 1) {       // Format 1 : non-signe, 2 : signe, 3 : flottant, 4 : undefined
    if (nbSample == 1)
      WriteTag16(339, format);
    else {
      WriteTag(339,SHORT, nbSample, offset);		// Format
      offset += nbSample * sizeof(uint16_t);
    }
  }

  if (m_ColorMap != NULL) {		// ColorMap
    WriteTag(320, SHORT, 256*3, offset);
    offset += (256 * 3 * sizeof(uint16_t));
  }

	// Tags GeoTiff
	if (geotiff) {
		WriteTag(33550, DOUBLE, 3, offset);				// ModelPixelScaleTag
		offset += 3 * sizeof(double);
		WriteTag(33922, DOUBLE, 6, offset);				// ModelTiepointTag
		offset += 6 * sizeof(double);
    if (m_nEpsg > 0) {
      WriteTag(34735, SHORT, 16, offset);     // 1 + 3 = 4 clés
      offset += 16 * sizeof(uint16_t);
    }
	}

	uint32_t nextIFD = 0;
	m_Out.write((char*)&nextIFD, sizeof(uint32_t));

	// Ecriture des tableaux de donnees
	if (nbSample > 1)
		for (uint32_t i = 0; i < nbSample; i++)
			m_Out.write((char*)&nbBits, sizeof(uint16_t));	// BitsPerSample

	uint32_t bytecount = w * nbSample * (nbBits / 8);
	for (uint32_t i = 0; i < h; i++) {						// StripOffsets
		m_Out.write((char*)&offset, sizeof(uint32_t));
		offset += bytecount;
	}	

  for(uint32_t i = 0; i < h; i++)						// StripByteCounts
    m_Out.write((char*)&bytecount, sizeof(uint32_t));

	uint32_t resol[2] = { 100, 1};
	m_Out.write((char*)resol, 2*sizeof(uint32_t));		// XResolution
	m_Out.write((char*)resol, 2*sizeof(uint32_t));		// YResolution

  if ((format > 1)&&(nbSample > 1))
    for (uint32_t i = 0; i < nbSample; i++)
      m_Out.write((char*)&format, sizeof(uint16_t));	// Format

	if (m_ColorMap != NULL)
		m_Out.write((char*)m_ColorMap, 256 * 3L * sizeof(uint16_t));

	// Donnees GeoTiff
	if (geotiff) {
		double zero = 0.;
		m_Out.write((char*)&m_dGsd, sizeof(double));	// ScaleX
		m_Out.write((char*)&m_dGsd, sizeof(double));	// ScaleY
		m_Out.write((char*)&zero, sizeof(double));		// ScaleZ

		m_Out.write((char*)&zero, sizeof(double));		// i
		m_Out.write((char*)&zero, sizeof(double));		// j
		m_Out.write((char*)&zero, sizeof(double));		// k
		m_Out.write((char*)&m_dXmin, sizeof(double));	// X
		m_Out.write((char*)&m_dYmax, sizeof(double));	// Y
		m_Out.write((char*)&zero, sizeof(double));		// Z

    if (m_nEpsg > 0) {
      uint16_t geokey[4] = { 1 , 1 , 0 , 3};  // Version 1 des specifications, 3 champs
      m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
      // GTModelTypeGeoKey : ModelTypeProjected -> 1
      geokey[0] = 1024; geokey[1] = 0; geokey[2] =  1; geokey[3] =  1;
      m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
      // GTRasterTypeGeoKey : RasterPixelIsArea -> 1
      geokey[0] = 1025; geokey[1] = 0; geokey[2] =  1; geokey[3] =  1;
      m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
      //geokey[0] = 2054; geokey[1] = 0; geokey[2] =  1; geokey[3] =  9102;
      //m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
      // ProjectedCSTypeGeoKey :
      geokey[0] = 3072; geokey[1] = 0; geokey[2] =  1; geokey[3] =  m_nEpsg;
      m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
      //geokey[0] = 3076; geokey[1] = 0; geokey[2] =  1; geokey[3] =  9103;
      //m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
    }
	}

	if (buf != NULL)
		m_Out.write((char*)buf, h * bytecount);
	m_Out.close();
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture de l'entete Tiff en image dallee
//-----------------------------------------------------------------------------
bool XTiffWriter::WriteTiled(const char* filename, uint32_t w, uint32_t h, uint16_t nbSample,
														uint16_t nbBits, uint8_t* buf, uint16_t format, uint32_t tileW, uint32_t tileH)
{
	m_Out.open(filename, std::ios::out | std::ios::binary);
	if (!m_Out.good())
		return XErrorError(m_Error, "Impossible de creer le fichier Tiff", XError::eIOOpen);

	WriteHeader();

	bool geotiff = false;
	if (m_dGsd > 0) geotiff = true;

	uint16_t nbtag = 15;
	if (geotiff) {
		nbtag += 2;
		if (m_nEpsg > 0) nbtag++;
	}
	if (m_ColorMap != NULL) nbtag++;
	if (format > 1) nbtag++;
	m_Out.write((char*)&nbtag, sizeof(uint16_t));
	uint32_t sizetag = 12L;
	uint32_t offset = (uint32_t)m_Out.tellp() + nbtag * sizetag + sizeof(uint32_t);
	// Ecriture des tags
	WriteTag32(254, 0);			// NewSubfileType 
	WriteTag32(256, w);			// ImageWidth	
	WriteTag32(257, h);			// ImageLength

	if (nbSample == 1)
		WriteTag16(258, nbBits);
	else {
		WriteTag(258, SHORT, nbSample, offset);		// BitsPerSample
		offset += nbSample * sizeof(uint16_t);
	}

	WriteTag16(259, 1);			// Compression
	// Photo. Interpretation
	if (nbSample == 1) {
		if (m_ColorMap != NULL)
			WriteTag16(262, 3);		// Image Palette
		else
			WriteTag16(262, 1);		// Image N&B
	}
	else
		WriteTag16(262, 2);			// Image RGB

	WriteTag16(277, nbSample);	// SamplesPerPixel

	WriteTag(282, RATIONAL, 1, offset);      	// XResolution	
	offset += 2 * sizeof(uint32_t);
	WriteTag(283, RATIONAL, 1, offset);      	// YResolution	
	offset += 2 * sizeof(uint32_t);

	WriteTag16(284, 1);			// PlanarConfiguration
	WriteTag16(296, 3);			// ResolutionUnit

	WriteTag32(322, tileW);	// TileWidth
	WriteTag32(323, tileH);	// TileLength

	uint32_t nbTileW = w / tileW;
	if ((w % tileW) != 0) nbTileW++;
	uint32_t nbTileH = h / tileH;
	if ((h % tileH) != 0) nbTileH++;
	uint32_t nbTile = nbTileW * nbTileH;

	WriteTag(324, LONG, nbTile, offset);				// TileOffsets
	offset += nbTile * sizeof(uint32_t);
	WriteTag(325, LONG, nbTile, offset);				// TileByteCounts	
	offset += nbTile * sizeof(uint32_t);

	if (format > 1) {       // Format 1 : non-signe, 2 : signe, 3 : flottant, 4 : undefined
		if (nbSample == 1)
			WriteTag16(339, format);
		else {
			WriteTag(339, SHORT, nbSample, offset);		// Format
			offset += nbSample * sizeof(uint16_t);
		}
	}

	if (m_ColorMap != NULL) {		// ColorMap
		WriteTag(320, SHORT, 256 * 3, offset);
		offset += (256 * 3 * sizeof(uint16_t));
	}

	// Tags GeoTiff
	if (geotiff) {
		WriteTag(33550, DOUBLE, 3, offset);				// ModelPixelScaleTag
		offset += 3 * sizeof(double);
		WriteTag(33922, DOUBLE, 6, offset);				// ModelTiepointTag
		offset += 6 * sizeof(double);
		if (m_nEpsg > 0) {
			WriteTag(34735, SHORT, 16, offset);     // 1 + 3 = 4 clés
			offset += 16 * sizeof(uint16_t);
		}
	}

	uint32_t nextIFD = 0;
	m_Out.write((char*)&nextIFD, sizeof(uint32_t));

	// Ecriture des tableaux de donnees
	if (nbSample > 1)
		for (uint32_t i = 0; i < nbSample; i++)
			m_Out.write((char*)&nbBits, sizeof(uint16_t));	// BitsPerSample

	uint32_t resol[2] = { 100, 1 };
	m_Out.write((char*)resol, 2 * sizeof(uint32_t));		// XResolution
	m_Out.write((char*)resol, 2 * sizeof(uint32_t));		// YResolution

	uint32_t bytecount = tileH * tileW * nbSample * (nbBits / 8);
	for (uint32_t i = 0; i < nbTile; i++) {						// TileOffsets
		m_Out.write((char*)&offset, sizeof(uint32_t));
		offset += bytecount;
	}

	for (uint32_t i = 0; i < nbTile; i++)						// TileByteCounts
		m_Out.write((char*)&bytecount, sizeof(uint32_t));


	if ((format > 1) && (nbSample > 1))
		for (uint32_t i = 0; i < nbSample; i++)
			m_Out.write((char*)&format, sizeof(uint16_t));	// Format

	if (m_ColorMap != NULL)
		m_Out.write((char*)m_ColorMap, 256 * 3L * sizeof(uint16_t));

	// Donnees GeoTiff
	if (geotiff) {
		double zero = 0.;
		m_Out.write((char*)&m_dGsd, sizeof(double));	// ScaleX
		m_Out.write((char*)&m_dGsd, sizeof(double));	// ScaleY
		m_Out.write((char*)&zero, sizeof(double));		// ScaleZ

		m_Out.write((char*)&zero, sizeof(double));		// i
		m_Out.write((char*)&zero, sizeof(double));		// j
		m_Out.write((char*)&zero, sizeof(double));		// k
		m_Out.write((char*)&m_dXmin, sizeof(double));	// X
		m_Out.write((char*)&m_dYmax, sizeof(double));	// Y
		m_Out.write((char*)&zero, sizeof(double));		// Z

		if (m_nEpsg > 0) {
			uint16_t geokey[4] = { 1 , 1 , 0 , 3 };  // Version 1 des specifications, 3 champs
			m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
			// GTModelTypeGeoKey : ModelTypeProjected -> 1
			geokey[0] = 1024; geokey[1] = 0; geokey[2] = 1; geokey[3] = 1;
			m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
			// GTRasterTypeGeoKey : RasterPixelIsArea -> 1
			geokey[0] = 1025; geokey[1] = 0; geokey[2] = 1; geokey[3] = 1;
			m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
			//geokey[0] = 2054; geokey[1] = 0; geokey[2] =  1; geokey[3] =  9102;
			//m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
			// ProjectedCSTypeGeoKey :
			geokey[0] = 3072; geokey[1] = 0; geokey[2] = 1; geokey[3] = m_nEpsg;
			m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
			//geokey[0] = 3076; geokey[1] = 0; geokey[2] =  1; geokey[3] =  9103;
			//m_Out.write((char*)geokey, 4 * sizeof(uint16_t));
		}
	}

	if (buf != NULL)
		m_Out.write((char*)buf, nbTile * bytecount);
	m_Out.close();
	return true;
}

//-----------------------------------------------------------------------------
// Fonction CheckByteOrder : renvoie le type de CPU	
//-----------------------------------------------------------------------------
uint16_t XTiffWriter::CheckByteOrder()
{
	short w = 0x0001;
	char *b = (char*) &w;
	return(b[0] ? LSB_FIRST : MSB_FIRST);
}

//-----------------------------------------------------------------------------
// Ecriture de l'entete Tiff
//-----------------------------------------------------------------------------
bool XTiffWriter::WriteHeader()
{
  if (CheckByteOrder() == LSB_FIRST){
    m_Out.put(0x49);
    m_Out.put(0x49);
    m_Out.put(0x2A);
    m_Out.put(0x00);
  }
  else {
    m_Out.put(0x4D);
    m_Out.put(0x4D);
    m_Out.put(0x00);
    m_Out.put(0x2A);
  }
  uint32_t offset = 0x08;
  m_Out.write((char*)&offset, sizeof(uint32_t));
  return m_Out.good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un tag
//-----------------------------------------------------------------------------
bool XTiffWriter::WriteTag(uint16_t id, uint16_t type, uint32_t count, uint32_t offset)
{
	m_Out.write((char*)&id, sizeof(uint16_t));
	m_Out.write((char*)&type, sizeof(uint16_t));
	m_Out.write((char*)&count, sizeof(uint32_t));
	m_Out.write((char*)&offset, sizeof(uint32_t));
	return m_Out.good();
}

bool XTiffWriter::WriteTag16(uint16_t id, uint16_t value)
{
	uint16_t type = 3;
	uint32_t count = 1;
	uint16_t zero = 0;
	m_Out.write((char*)&id, sizeof(uint16_t));
	m_Out.write((char*)&type, sizeof(uint16_t));
	m_Out.write((char*)&count, sizeof(uint32_t));
	m_Out.write((char*)&value, sizeof(uint16_t));
	m_Out.write((char*)&zero, sizeof(uint16_t));
	return m_Out.good();
}

bool XTiffWriter::WriteTag32(uint16_t id, uint32_t value)
{
	uint16_t type = 4;
	uint32_t count = 1;
	m_Out.write((char*)&id, sizeof(uint16_t));
	m_Out.write((char*)&type, sizeof(uint16_t));
	m_Out.write((char*)&count, sizeof(uint32_t));
	m_Out.write((char*)&value, sizeof(uint32_t));
	return m_Out.good();
}
