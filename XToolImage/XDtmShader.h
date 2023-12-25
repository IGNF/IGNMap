//-----------------------------------------------------------------------------
//								XDtmShader.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
//
// 18/06/2021
//
// Shader pour convertir un MNT 32bits en image RGB
//-----------------------------------------------------------------------------

#ifndef XDTMSHADER_H
#define XDTMSHADER_H

#include <vector>
#include "../XTool/XBase.h"
#include "../XToolAlgo/XColor.h"

class XDtmShader {
public :
  enum ShaderMode { Altitude = 0, Shading, Light_Shading, Free_Shading, Slope, Colour, Shading_Colour, Contour};

protected:
  double CoefEstompage(float altC, float altH, float altD, float angleH = 135., float angleV = 45.);
  double CoefPente(float altC, float altH, float altD);
  int Isohypse(float altC, float altH, float altD);

  bool EstompLine(float* lineR, float* lineS, float* lineT, uint32_t w, uint8_t* rgb, uint32_t num);

  double  m_dGSD;       // Pas terrain du MNT
  bool    m_bBGRorder;  // Ordre BGR
  bool    m_bAlphaMode; // Pixel avec 4 composants

public:
  static std::vector<double> m_Z;           // Plages d'altitude
  static std::vector<XARGBColor> m_Color;   // Plages de couleur
  static int    m_Mode;           // Mode d'affichage
  static double m_dIsoStep;       // Pas des isohypses
  static double m_dSolarAzimuth;  // Angle azimuthal en degres
  static double m_dSolarZenith;   // Angle zenithal en degres

  XDtmShader(double gsd = 25., bool bgrOrder = false, bool alphaMode = false);
  bool ConvertArea(float* area, uint32_t w, uint32_t h, uint8_t* rgb, uint32_t area_off = 0, uint32_t rgb_off = 0);
};

#endif // XDTMSHADER_H
