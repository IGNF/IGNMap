//-----------------------------------------------------------------------------
//								XStlWriter.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 3/12/2020
//-----------------------------------------------------------------------------

#ifndef _XSTLWRITER_H
#define _XSTLWRITER_H

#include "../XTool/XBase.h"

class XStlWriter {
public:
  XStlWriter() {;}
  bool SetOptions(double scale, double exag, double pas, double zmin);

  bool WriteStlFile(std::ostream* out, float* T, uint32_t nbW, uint32_t nbH,
                    bool edge = false, XWait* wait = NULL);

protected:
  double  m_dScale;
  double  m_dExag;
  double  m_dPas;
  double  m_dZmin;

  bool WriteTriangle(std::ostream* out,
                     const float& xA, const float& yA, const float& zA,
                     const float& xB, const float& yB, const float& zB,
                     const float& xC, const float& yC, const float& zC);
};

#endif // _XSTLWRITER_H
