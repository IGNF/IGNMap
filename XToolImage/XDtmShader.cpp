//-----------------------------------------------------------------------------
//								XDtmShader.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
//
// 18/06/2021
//
// Shader pour convertir un MNT 32bits en image RGB
//-----------------------------------------------------------------------------

#include "XDtmShader.h"
#include <cmath>
#include <cstring>

// Preferences d'affichage
int XDtmShader::m_Mode = XDtmShader::Shading;
double XDtmShader::m_dIsoStep = 25.;
double XDtmShader::m_dSolarAzimuth = 135.;
double XDtmShader::m_dSolarZenith = 45.;
std::vector<double> XDtmShader::m_Z;
std::vector<XARGBColor> XDtmShader::m_Color;

//==============================================================================
// Constructeur
//==============================================================================
XDtmShader::XDtmShader(double gsd,  bool bgrOrder, bool alphaMode)
{
  m_dGSD = gsd;
  m_bBGRorder = bgrOrder;
  m_bAlphaMode = alphaMode;
  if (m_Z.size() < 1) { // Premier shader : on initialise
    m_Z.push_back(-999.); // No data
    m_Z.push_back(0.);
    m_Z.push_back(200.);
    m_Z.push_back(400.);
    m_Z.push_back(600.);
    m_Z.push_back(5500.);
    m_Color.clear();
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)255, (uint8_t)0, (uint8_t)0));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)3, (uint8_t)34, (uint8_t)76));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)64, (uint8_t)128, (uint8_t)128));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)255, (uint8_t)255, (uint8_t)0));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)255, (uint8_t)128, (uint8_t)0));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)128, (uint8_t)64, (uint8_t)0));
    m_Color.push_back(XARGBColor((uint8_t)255, (uint8_t)240, (uint8_t)240, (uint8_t)240));
  }
}

//-----------------------------------------------------------------------------
// Fonction d'estompage d'un pixel à partir de l'altitude du pixel, de l'altitude du pixel juste au dessus,
// de l'altitude du pixel juste à sa droite
//-----------------------------------------------------------------------------
double XDtmShader::CoefEstompage(float altC, float altH, float altD, float angleH, float angleV)
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
  if (normenormale * normeDirLum > 0)
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
double XDtmShader::CoefPente(float altC, float altH, float altD)
{
  double d = m_dGSD;
  if (d <= 0.) d = 25;
  double costheta = d / sqrt((altD - altC) * (altD - altC) + (altH - altC) * (altH - altC) + d * d);
  return costheta;
}

//-----------------------------------------------------------------------------
// Indique si l'on a une isohypse
//-----------------------------------------------------------------------------
int XDtmShader::Isohypse(float altC, float altH, float altD)
{
  double step = m_dIsoStep;
  int nb_isoC = (int)ceil(altC / step);
  int nb_isoH = (int)ceil(altH / step);
  int nb_isoD = (int)ceil(altD / step);
  int nb_iso = XRint(altC / step);

  if (nb_isoC != nb_isoH)
    return nb_iso;
  if (nb_isoC != nb_isoD)
    return nb_iso;

  return -9999;
}

//-----------------------------------------------------------------------------
// Calcul de l'estompage sur une ligne
//-----------------------------------------------------------------------------
bool XDtmShader::EstompLine(float* lineR, float* lineS, float* lineT, uint32_t W, uint8_t* rgba, uint32_t num)
{
  float* ptr = lineS;
  double coef = 1., r, g, b, a;
  float val;
  int index = 0, nb_iso;
  uint32_t nbByte = 3;
  if (m_bAlphaMode) nbByte = 4;
  float angleH = (float)m_dSolarAzimuth, angleV = (float)m_dSolarZenith;
  if (m_Mode == Shading) {
    angleH = 135.; angleV = 45.;
  }
  if (m_Mode == Light_Shading) {
    angleH = 135.; angleV = 65.;
  }

  for (uint32_t i = 0; i < W; i++) {
    val = *ptr;
    r = g = b = a = 255;
    XARGBColor pix_col = m_Color[0];
    uint8_t* ptr_rgb = &rgba[nbByte * i];

    if (val <= m_Z[0]) { // No data
      pix_col.Copy(ptr_rgb, m_bBGRorder, m_bAlphaMode);
      ptr++;
      continue;
    }
    if (val <= m_Z[1]) { // Sous la mer
      coef = (m_Z[1] - val);
      index = 1;
      r = m_Color[index].R();
      g = m_Color[index].G();
      b = m_Color[index].B();
      a = m_Color[index].A();
    }

    for (int j = 2; j < m_Z.size(); j++) {
      if ((val > m_Z[j - 1]) && (val <= m_Z[j])) {
        coef = (m_Z[j] - val) / (m_Z[j] - m_Z[j-1]);
        index = j;
        r = m_Color[j].R() * coef + m_Color[j+1].R() * (1 - coef);
        g = m_Color[j].G() * coef + m_Color[j+1].G() * (1 - coef);
        b = m_Color[j].B() * coef + m_Color[j+1].B() * (1 - coef);
        a = m_Color[j].A() * coef + m_Color[j+1].A() * (1 - coef);
      }
    }

    if (val > m_Z[m_Z.size() - 1]) {
      index = (int)m_Z.size();
      r = m_Color[index].R();
      g = m_Color[index].G();
      b = m_Color[index].B();
      a = m_Color[index].A();
    }

    switch (m_Mode) {
    case Altitude: coef = 1.;
      break;

    case Shading: // Estompage
    case Light_Shading: // Estompage leger
    case Free_Shading: // Estompage Libre
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

    case Slope: // Pente
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

    case Colour: // Aplat de couleurs
      coef = 1.;
      r = m_Color[index].R();
      g = m_Color[index].G();
      b = m_Color[index].B();
      a = m_Color[index].A();
      break;

    case Shading_Colour: // Aplat + estompage
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
        r = m_Color[index].R();
        g = m_Color[index].G();
        b = m_Color[index].B();
        a = m_Color[index].A();
      }
      break;

    case Contour: // Isohypses
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
        r = m_Color[index].R();
        g = m_Color[index].G();
        b = m_Color[index].B();
        a = m_Color[index].A();
      }
      break;

    }

    pix_col = XARGBColor(round(a), round(r* coef), round(g* coef), round(b* coef));
    pix_col.Copy(ptr_rgb, m_bBGRorder, m_bAlphaMode);
    ptr++;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Calcul de l'estompage
//-----------------------------------------------------------------------------
bool XDtmShader::ConvertArea(float* area, uint32_t w, uint32_t h, uint8_t* rgb, uint32_t area_off, uint32_t rgb_off)
{
  uint32_t areaW = w, rgbW = w * 3L + rgb_off;
  if (m_bAlphaMode)
    rgbW = w * 4L + rgb_off;

  EstompLine(area, area, &area[areaW], w, rgb, 0);
  for (uint32_t i = 1; i < h - 1; i++) {
    EstompLine(&area[(i - 1) * areaW], &area[i * areaW], &area[(i + 1) * areaW], w, &rgb[i * rgbW], i);
  }
  EstompLine(&area[(h - 2) * areaW], &area[(h - 1) * areaW], &area[(h - 1) * areaW], w, &rgb[(h-1) * rgbW], h-1);
  return true;
}
