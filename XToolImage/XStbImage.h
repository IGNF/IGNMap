//-----------------------------------------------------------------------------
//								XStbImage.h
//								===========
//
// Utilisation de la bibliotheque STB pour lire differents formats
// https://github.com/nothings/stb
// 
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 28/01/2026
//-----------------------------------------------------------------------------

#ifndef XSTBIMAGE_H
#define XSTBIMAGE_H

#include "XBaseImage.h"

class XStbImage : public XBaseImage {
protected:
  bool		  m_bValid;	// Indique si l'image est valide
  uint8_t*  m_Data;
  uint32_t  m_nDataSize;
  std::string m_strFilename;
public:
  XStbImage(const char* filename);
  virtual ~XStbImage();

  virtual std::string Format() { return "PNG"; }
  bool IsValid() { return m_bValid; }
  virtual inline bool NeedFile() { return false; }

  virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
  virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area) { return GetArea(file, 0, num, m_nW, 1, area); }

  virtual uint32_t FileSize() { return 0; }
};


#endif //XSTBIMAGE_H

