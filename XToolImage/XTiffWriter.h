//-----------------------------------------------------------------------------
//								XTiffWriter.h
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/10/2000
//-----------------------------------------------------------------------------

#ifndef _XTIFFWRITER_H
#define _XTIFFWRITER_H

#include <fstream>
#include "../XTool/XBase.h"

class XTiffWriter {
protected:
	enum { LSB_FIRST = 0, MSB_FIRST = 1};
	enum { BYTE = 1, ASCII = 2, SHORT = 3, LONG = 4, RATIONAL = 5,
				 SBYTE = 6, UNDEFINED = 7, SSHORT = 8, SLONG = 9, SRATIONAL = 10,
				 FLOAT = 11, DOUBLE = 12};

	std::ofstream m_Out;
	XError*				m_Error;
	double				m_dXmin;
	double				m_dYmax;
	double				m_dGsd;
  uint16_t        m_nEpsg;
	uint16_t*				m_ColorMap;

	uint16_t CheckByteOrder();
	bool WriteHeader();
	bool WriteTag(uint16_t id, uint16_t type, uint32_t count, uint32_t offset);
	bool WriteTag16(uint16_t id, uint16_t value);
	bool WriteTag32(uint16_t id, uint32_t value);

public:
	XTiffWriter(XError* error = NULL) { m_Error = error; m_dGsd = -1e38; m_dXmin = m_dYmax = 0.; m_nEpsg = 0; m_ColorMap = NULL; }
	virtual ~XTiffWriter() { if (m_ColorMap != NULL) delete[] m_ColorMap;}

  void SetGeoTiff(double xmin, double ymax, double gsd, uint16_t epsg = 0)
          { m_dXmin = xmin; m_dYmax = ymax; m_dGsd = gsd; m_nEpsg = epsg;}
  bool SetColorMap(uint8_t* color_map, int nbcomp = 3, bool swap = false);
  bool SetColorMap(uint16_t* color_map);

	bool Write(const char* filename, uint32_t w, uint32_t h, uint16_t nbSample = 1, 
             uint16_t nbBits = 8, uint8_t* buf = NULL, uint16_t format = 0);
};


#endif //_XTIFFWRITER_H
