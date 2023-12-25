//-----------------------------------------------------------------------------
//								XColor.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 13/1/2023
//-----------------------------------------------------------------------------

#ifndef XCOLOR_H
#define XCOLOR_H

#include "../XTool/XBase.h"

class XARGBColor {
protected:
  struct ARGB {
    uint8_t b;   // Ordre Windows ...
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };

  union {
    uint32_t  m_Val;
    ARGB    m_ARGB;
  };

public:
  XARGBColor(uint32_t v = 0) { m_Val = v;}
  XARGBColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) { m_ARGB.a = a; m_ARGB.r = r; m_ARGB.g = g; m_ARGB.b = b;}

  inline uint8_t A() { return m_ARGB.a;}
  inline uint8_t R() { return m_ARGB.r;}
  inline uint8_t G() { return m_ARGB.g;}
  inline uint8_t B() { return m_ARGB.b;}

  inline void A(uint8_t v) { m_ARGB.a = v;}
  inline void R(uint8_t v) { m_ARGB.r = v;}
  inline void G(uint8_t v) { m_ARGB.g = v;}
  inline void B(uint8_t v) { m_ARGB.b = v;}

  inline uint32_t  Val() { return m_Val;}
  inline void Val(uint32_t v) { m_Val = v;}

  void CopyRGB(uint8_t* buf) { buf[0] = m_ARGB.r; buf[1] = m_ARGB.g; buf[2] = m_ARGB.b;}
  void CopyBGR(uint8_t* buf) { buf[0] = m_ARGB.b; buf[1] = m_ARGB.g; buf[2] = m_ARGB.r;}
  void CopyARGB(uint8_t* buf) { buf[0] = m_ARGB.a; buf[1] = m_ARGB.r; buf[2] = m_ARGB.g; buf[3] = m_ARGB.b;}
  void CopyBGRA(uint8_t* buf) { buf[0] = m_ARGB.b; buf[1] = m_ARGB.g; buf[2] = m_ARGB.r; buf[3] = m_ARGB.a;}

  void Copy(uint8_t* buf, bool bgrOrder, bool alpha)
    { if (bgrOrder) {if(alpha) CopyBGRA(buf); else CopyBGR(buf);} else {if(alpha) CopyARGB(buf); else CopyRGB(buf);}}

};


#endif // XCOLOR_H
