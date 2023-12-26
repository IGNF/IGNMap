//-----------------------------------------------------------------------------
//								DtmShader.h
//								===========
//
// Estompage / ombrage des MNT (tire de XDtmShader, adapte a JUCE)
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/01/2022
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XBase.h"

class DtmShader {
protected:
  double CoefEstompage(float altC, float altH, float altD, float angleH = 135., float angleV = 45.);
  double CoefPente(float altC, float altH, float altD);
  int Isohypse(float altC, float altH, float altD);

  bool EstompLine(float* lineR, float* lineS, float* lineT, uint32_t w, uint8_t* rgba, uint32_t num);

  double  m_dGSD;   // Pas terrain du MNT

public:
  DtmShader(double gsd = 25.);
  bool ConvertImage(juce::Image* rawImage, juce::Image* rgbImage);

  enum class ShaderMode { Altitude = 0, Shading, Light_Shading, Free_Shading, Slope, Colour, Shading_Colour, Contour};

  static std::vector<double> m_Z;     // Plages d'altitude
  static std::vector<juce::Colour> m_Colour;  // Plages de couleur
  static ShaderMode    m_Mode;  // Mode d'affichage
  static double m_dIsoStep; // Pas des isohypses
  static double m_dSolarAzimuth;  // Angle azimuthal en degres
  static double m_dSolarZenith;   // Angle zenithal en degres
  static double m_dOpacity; // Opacite

  static bool AddAltitude(double z);
  static juce::Colour Colour(double z);
};