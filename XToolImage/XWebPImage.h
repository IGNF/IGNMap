//-----------------------------------------------------------------------------
//								XWebPImage.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 19/12/2022
//-----------------------------------------------------------------------------

#ifndef  XWEBPIMAGE_H
#define XWEBPIMAGE_H

extern "C" {
#include "../libwebp-1.2.4/src/webp/decode.h"
}

#include "XBaseImage.h"

class XWebPImage : public XBaseImage {
protected:
  bool											m_bValid;	// Indique si l'image est valide
  uint8_t*                     m_Data;
  uint32_t                    m_nDataSize;
public:
  XWebPImage(const char* filename);
  virtual ~XWebPImage();

  virtual std::string Format() { return "WEBP"; }
  bool IsValid() { return m_bValid; }
  virtual inline bool NeedFile() { return false; }

  virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
  virtual bool GetWebPArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area,
                           uint32_t factor = 1, uint32_t wout = 0, uint32_t hout = 0);
  virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area) { return GetArea(file, 0, num, m_nW, 1, area); }

  virtual uint32_t FileSize() { return 0; }
};

#endif //XWEBPIMAGE_H


