//-----------------------------------------------------------------------------
//								DtmShader.cpp
//								=============
//
// Estompage / ombrage des MNT (tire de XDtmShader, adapte a JUCE)
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/01/2022
//-----------------------------------------------------------------------------

#include <cmath>
#include "DtmShader.h"

// Preferences d'affichage
DtmShader::ShaderMode DtmShader::m_Mode = DtmShader::ShaderMode::Shading;
double DtmShader::m_dIsoStep = 25.;
double DtmShader::m_dSolarAzimuth = 135.;
double DtmShader::m_dSolarZenith = 45.;
double DtmShader::m_dOpacity = 50.; 
std::vector<double> DtmShader::m_Z;
std::vector<juce::Colour> DtmShader::m_Colour;


bool DtmShader::AddAltitude(double z)
{
  if (m_Z.size() < 2)
    return false;
  if (z < m_Z[0]) {
    m_Z.insert(m_Z.begin(), z);
    m_Colour.insert(m_Colour.begin(), juce::Colour(127, 127, 127));
    return true;
  }
  for (int i = 1; i < m_Z.size(); i++) {
    if ((z >= m_Z[i - 1]) && (z < m_Z[i])) {
      m_Z.insert(m_Z.begin() + i, z);
      m_Colour.insert(m_Colour.begin() + i, juce::Colour(127, 127, 127));
      return true;
    }
  }
  m_Z.push_back(z);
  m_Colour.push_back(juce::Colour(127, 127, 127));
  return true;
}

//==============================================================================
// Constructeur
//==============================================================================
DtmShader::DtmShader(double gsd)
{
  m_dGSD = gsd;
  if (m_Z.size() < 1) { // Premier shader : on initialise
    m_Z.push_back(-999.); // No data
    m_Z.push_back(0.);
    m_Z.push_back(200.);
    m_Z.push_back(400.);
    m_Z.push_back(600.);
    m_Z.push_back(3500.);
    m_Colour.clear();
    m_Colour.push_back(juce::Colour((uint8_t)255, (uint8_t)0, (uint8_t)0, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)3, (uint8_t)34, (uint8_t)76, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)64, (uint8_t)128, (uint8_t)128, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)255, (uint8_t)255, (uint8_t)0, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)255, (uint8_t)128, (uint8_t)0, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)128, (uint8_t)64, (uint8_t)0, (uint8_t)255));
    m_Colour.push_back(juce::Colour((uint8_t)240, (uint8_t)240, (uint8_t)240, (uint8_t)255));
  }
}

//-----------------------------------------------------------------------------
// Fonction d'estompage d'un pixel à partir de l'altitude du pixel, de l'altitude du pixel juste au dessus,
// de l'altitude du pixel juste à sa droite
//-----------------------------------------------------------------------------
double DtmShader::CoefEstompage(float altC, float altH, float altD, float angleH, float angleV)
{
  double deltaX = m_dGSD, deltaY = m_dGSD; // angleH = 135, angleV = 45;
  if (m_dGSD <= 0.)
    deltaX = deltaY = 25.;
  if ((m_dGSD > 0.) && (m_dGSD < 0.1))    // Donnees en geographiques
    deltaX = deltaY = m_dGSD * 111319.49;  // 1 degre a l'Equateur

  // Recherche de la direction de la normale a la surface du mnt
  double dY[3], dX[3], normale[3];

  // Initialisation des vecteurs de la surface du pixel considéré
  dY[0] = 0;
  dY[1] = deltaY;
  dY[2] = (double)(altH - altC);

  dX[0] = deltaX;
  dX[1] = 0;
  dX[2] = (double)(altD - altC);

  // Direction de la normale a la surface
  normale[0] = -(dY[1] * dX[2]);
  normale[1] = -(dX[0] * dY[2]);
  normale[2] = dX[0] * dY[1];

  // Determination de l'angle entre la normale et la direction de la lumière
  // Modification des teintes en fonction de cet angle
  double correction;
  double DirLum[3];
  
  // Passage d'une représentation angulaire de la direction de la lumiere en représentation cartésienne
  DirLum[0] = cos(XPI / 180 * angleV * -1) * sin(XPI / 180 * angleH);
  DirLum[1] = cos(XPI / 180 * angleV * -1) * cos(XPI / 180 * angleH);
  DirLum[2] = sin(XPI / 180 * angleV * -1);

  // Correction correspond au cosinus entre la normale et la lumiere
  double scalaire;
  scalaire = (normale[0] * DirLum[0]) + (normale[1] * DirLum[1]) + (normale[2] * DirLum[2]);

  double normenormale;
  double normeDirLum;
  normenormale = (normale[0] * normale[0]) + (normale[1] * normale[1]) + (normale[2] * normale[2]);

  normeDirLum = (DirLum[0] * DirLum[0]) + (DirLum[1] * DirLum[1]) + (DirLum[2] * DirLum[2]);
  if ((normenormale * normeDirLum) > 0)
    correction = scalaire / sqrt(normenormale * normeDirLum);
  else correction = 0;

  // Traduction de cette correction en correction finale
  if (correction >= 0)
    correction = 0;
  else correction = -correction;

  return correction;
}

//-----------------------------------------------------------------------------
// Calcul de la pente
//-----------------------------------------------------------------------------
double DtmShader::CoefPente(float altC, float altH, float altD)
{
  double d = m_dGSD;
  if (d <= 0.) d = 25;
  double costheta = d / sqrt((altD - altC) * (altD - altC) + (altH - altC) * (altH - altC) + d * d);
  return costheta;
}

//-----------------------------------------------------------------------------
// Indique si l'on a une isohypse
//-----------------------------------------------------------------------------
int DtmShader::Isohypse(float altC, float altH, float altD)
{
  double step = m_dIsoStep;
  int nb_isoC = (int)ceil(altC / step);
  int nb_isoH = (int)ceil(altH / step);
  int nb_isoD = (int)ceil(altD / step);
  int nb_iso = (int)round(altC / step);

  if (nb_isoC != nb_isoH)
    return nb_iso;
  if (nb_isoC != nb_isoD)
    return nb_iso;

  return -9999;
}

//-----------------------------------------------------------------------------
// Calcul de l'estompage sur une ligne
//-----------------------------------------------------------------------------
bool DtmShader::EstompLine(float* lineR, float* lineS, float* lineT, uint32_t W, uint8_t* rgba, uint32_t num)
{
  float* ptr = lineS;
  double coef = 1., r, g, b, a;
  float val;
  int index = 0, nb_iso;
  float angleH = (float)m_dSolarAzimuth, angleV = (float)m_dSolarZenith;
  if (m_Mode == ShaderMode::Shading) {
    angleH = 135.; angleV = 45.;
  }
  if (m_Mode == ShaderMode::Light_Shading) {
    angleH = 135.; angleV = 65.;
  }

  for (uint32_t i = 0; i < W; i++) {
    val = *ptr;
    r = g = b = a = 255;
    uint32_t pix_col = m_Colour[0].getARGB();

    if (val <= m_Z[0]) { // No data
      ::memcpy(&rgba[4 * i], &pix_col, 4 * sizeof(uint8_t));
      ptr++;
      continue;
    }
    if (val <= m_Z[1]) { // Sous la mer
      coef = (m_Z[1] - val);
      index = 1;
      r = m_Colour[index].getRed();
      g = m_Colour[index].getGreen();
      b = m_Colour[index].getBlue();
      a = m_Colour[index].getAlpha();
    }

    for (int j = 2; j < m_Z.size(); j++) {
      if ((val > m_Z[j - 1]) && (val <= m_Z[j])) {
        coef = (m_Z[j] - val) / (m_Z[j] - m_Z[j-1]);
        index = j;
        r = m_Colour[j].getRed() * coef + m_Colour[j+1].getRed() * (1 - coef);
        g = m_Colour[j].getGreen() * coef + m_Colour[j+1].getGreen() * (1 - coef);
        b = m_Colour[j].getBlue() * coef + m_Colour[j+1].getBlue() * (1 - coef);
        a = m_Colour[j].getAlpha() * coef + m_Colour[j+1].getAlpha() * (1 - coef);
      }
    }

    if (val > m_Z[m_Z.size() - 1]) {
      index = (int)m_Z.size();
      r = m_Colour[index].getRed();
      g = m_Colour[index].getGreen();
      b = m_Colour[index].getBlue();
      a = m_Colour[index].getAlpha();
    }

    switch (m_Mode) {
    case ShaderMode::Altitude: coef = 1.;
      break;

    case ShaderMode::Shading: // Estompage
    case ShaderMode::Light_Shading: // Estompage leger
    case ShaderMode::Free_Shading: // Estompage Libre
    case ShaderMode::Opacity: // Opacite
      if (num == 0) {
        if (i < (W - 1))
          coef = CoefEstompage(lineT[i], val, lineT[i + 1], angleH, angleV);
        else
          coef = CoefEstompage(lineS[i - 1], lineR[i - 1], val, angleH, angleV);
      }
      else {
        if (i < (W - 1))
          coef = CoefEstompage(val, lineR[i], lineS[i + 1], angleH, angleV);
        else
          coef = CoefEstompage(lineS[i - 1], lineR[i - 1], val, angleH, angleV);
      }
      break;

    case ShaderMode::Slope: // Pente
      if (num == 0) {
        if (i < (W - 1))
          coef = CoefPente(lineT[i], val, lineT[i + 1]);
        else
          coef = CoefPente(lineS[i - 1], lineR[i - 1], val);
      }
      else {
        if (i < (W - 1))
          coef = CoefPente(val, lineR[i], lineS[i + 1]);
        else
          coef = CoefPente(lineS[i - 1], lineR[i - 1], val);
      }
      break;

    case ShaderMode::Colour: // Aplat de couleurs
      coef = 1.;
      r = m_Colour[index].getRed();
      g = m_Colour[index].getGreen();
      b = m_Colour[index].getBlue();
      a = m_Colour[index].getAlpha();
      break;

    case ShaderMode::Shading_Colour: // Aplat + estompage
      if (num == 0) {
        if (i < (W - 1))
          coef = CoefEstompage(lineT[i], val, lineT[i + 1]);
        else
          coef = CoefEstompage(lineS[i - 1], lineR[i - 1], val);
      }
      else {
        if (i < (W - 1))
          coef = CoefEstompage(val, lineR[i], lineS[i + 1]);
        else
          coef = CoefEstompage(lineS[i - 1], lineR[i - 1], val);
      }
      if (index > 0) {
        r = m_Colour[index].getRed();
        g = m_Colour[index].getGreen();
        b = m_Colour[index].getBlue();
        a = m_Colour[index].getAlpha();
      }
      break;

    case ShaderMode::Contour: // Isohypses
      coef = 1.;
      a = r = g = b = 0;
      if (num == 0) {
        if (i < (W - 1))
          nb_iso = Isohypse(lineT[i], val, lineT[i + 1]);
        else
          nb_iso = Isohypse(lineS[i - 1], lineR[i - 1], val);
      }
      else {
        if (i < (W - 1))
          nb_iso = Isohypse(val, lineR[i], lineS[i + 1]);
        else
          nb_iso = Isohypse(lineS[i - 1], lineR[i - 1], val);
      }

      if (nb_iso > -9999) {
        double cote = nb_iso * m_dIsoStep;
        for (int j = 0; j < m_Z.size(); j++) {
          index = j;
          if (cote < m_Z[j])
            break;
        }
        r = m_Colour[index].getRed();
        g = m_Colour[index].getGreen();
        b = m_Colour[index].getBlue();
        a = m_Colour[index].getAlpha();
      }
      break;

    }

    pix_col = juce::Colour(juce::Colour::fromRGBA((juce::uint8)round(r* coef), (juce::uint8)round(g* coef), 
                                                  (juce::uint8)round(b* coef), (juce::uint8)round(a))).getARGB();
    if (m_Mode == ShaderMode::Opacity)
      pix_col = juce::Colour(juce::Colour::fromRGBA((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)round((1.-coef)*255.))).getARGB();

    ::memcpy(&rgba[4 * i], &pix_col, 4 * sizeof(uint8_t));
    ptr++;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Calcul de l'estompage
//-----------------------------------------------------------------------------
bool DtmShader::ConvertImage(juce::Image* rawImage, juce::Image* rgbImage)
{
  if ((rawImage->getWidth() != rgbImage->getWidth()) || (rawImage->getHeight() != rgbImage->getHeight()))
    return false;
  if (rawImage->getFormat() != juce::Image::PixelFormat::ARGB)
    return false;
  if (rgbImage->getFormat() != juce::Image::PixelFormat::ARGB)
    return false;
  int w = rawImage->getWidth(), h = rawImage->getHeight();

  juce::Image::BitmapData rawData(*rawImage, juce::Image::BitmapData::readOnly);
  juce::Image::BitmapData rgbData(*rgbImage, juce::Image::BitmapData::writeOnly);

  EstompLine((float*)rawData.getLinePointer(0), (float*)rawData.getLinePointer(0), (float*)rawData.getLinePointer(1), 
              w, rgbData.getLinePointer(0), 0);
  for (int i = 1; i < h - 1; i++) {
    EstompLine((float*)rawData.getLinePointer(i - 1), (float*)rawData.getLinePointer(i), (float*)rawData.getLinePointer(i + 1), 
                w, rgbData.getLinePointer(i), i);
  }
  EstompLine((float*)rawData.getLinePointer(h - 2), (float*)rawData.getLinePointer(h - 1), (float*)rawData.getLinePointer(h - 1),
                w, rgbData.getLinePointer(h - 1), h - 1);
  return true;
}

//-----------------------------------------------------------------------------
// Renvoie la couleur associee a une altitude
//-----------------------------------------------------------------------------
juce::Colour DtmShader::Colour(double val)
{
  if (m_Z.size() != (m_Colour.size() - 1))
    return juce::Colours::red;
  if (m_Z.size() < 2)
    return juce::Colours::red;
  if (val < m_Z[0]) // No data
    return m_Colour[0];

  if (val <= m_Z[1]) // Sous la mer
    return m_Colour[1];

  if (val > m_Z[m_Z.size() - 1]) // Au dessus de l'Everest ...
    return m_Colour[m_Z.size()];

  double coef;
  for (int j = 2; j < m_Z.size(); j++) {
    if ((val > m_Z[j - 1]) && (val <= m_Z[j])) {
      coef = (m_Z[j] - val) / (m_Z[j] - m_Z[j - 1]);
      return juce::Colour(
        juce::uint8(m_Colour[j].getRed() * coef + m_Colour[j + 1].getRed() * (1 - coef)),
        juce::uint8(m_Colour[j].getGreen() * coef + m_Colour[j + 1].getGreen() * (1 - coef)),
        juce::uint8(m_Colour[j].getBlue() * coef + m_Colour[j + 1].getBlue() * (1 - coef)),
        juce::uint8(m_Colour[j].getAlpha() * coef + m_Colour[j + 1].getAlpha() * (1 - coef)));
    }
  }

  return m_Colour[m_Colour.size() - 1];
}

//-----------------------------------------------------------------------------
// Calcul des intervals altimetriques optimaux
//-----------------------------------------------------------------------------
void DtmShader::AutomaticRange(double zmin, double zmax)
{
  if (m_Z.size() <= 1)
    return;
  double delta = (zmax - zmin) / (m_Z.size() - 1);
  for (int i = 1; i < m_Z.size(); i++)
    m_Z[i] = zmin + (i - 1) * delta;
}
