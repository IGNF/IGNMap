//-----------------------------------------------------------------------------
//								AffineImage.cpp
//								===============
//
// Gestion des images georeferencees avec une affinite (par exemple cliche aerien)
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/05/2024
//-----------------------------------------------------------------------------

#include "AffineImage.h"

//-----------------------------------------------------------------------------
// Analyse l'image source
//-----------------------------------------------------------------------------
bool RotationImage::AnalyzeImage(std::string path)
{
  m_nW = m_nH = 0;
  if (m_Image.AnalyzeImage(path)) { // Ouverture des formats geographiques (TIFF, COG, JP2, ...)
    m_nW = m_Image.Width();
    m_nH = m_Image.Height();
    m_O.X = m_Image.Width() * 0.5;
    m_O.Y = m_Image.Height() * 0.5;
    return true;
  }
  juce::File file(path);
  juce::Image image = juce::ImageFileFormat::loadFrom(file);  // Image classique (JPEG, PNG, GIF)
  if (!image.isValid())
    return false;
  m_strRequest = path;
  m_nW = image.getWidth();
  m_nH = image.getHeight();
  m_O.X = m_nW * 0.5;
  m_O.Y = m_nH * 0.5;
  return true;
}

//-----------------------------------------------------------------------------
// Renvoie une image pour couvrir un cadre
//-----------------------------------------------------------------------------
juce::Image& RotationImage::GetAreaImage(const XFrame& F, double gsd)
{
  if ((F == m_LastFrame) && (gsd == m_LastGsd))
    return m_ProjImage;
  m_LastFrame = F;
  m_LastGsd = gsd;

  // Calcul des pixels necessaires de l'image
  double u, v, minU = m_nW, maxU = 0, minV = m_nH, maxV = 0;
  for (double y = F.Ymin; y <= F.Ymax; y += gsd) {
    for (double x = F.Xmin; x <= F.Xmax; x += gsd) {
      Ground2Image(x, y, &u, &v);
      minU = XMin(minU, u);
      maxU = XMax(maxU, u);
      minV = XMin(minV, v);
      maxV = XMax(maxV, v);
    }
  }
  minU = XMax(0., minU);
  minV = XMax(0., minV);
  maxU = XMin((double)m_nW, maxU);
  maxV = XMin((double)m_nH, maxV);

  // Calcul des coordonnees du cadre en geometrie image
  uint32_t u0 = (uint32_t)minU;
  uint32_t v0 = (uint32_t)minV;
  uint32_t win = (uint32_t)(maxU - minU);
  uint32_t hin = (uint32_t)(maxV - minV);
  uint32_t factor = (uint32_t)(gsd / m_dGsd);
  if (factor < 1) factor = 1;
  uint32_t wtmp = win / factor, htmp = hin / factor;
  if (m_Image.IsValid()) {  // Cas des images geographiques (TIFF, COG, JP2, ...)
    m_SourceImage = juce::Image(juce::Image::PixelFormat::ARGB, wtmp, htmp, true, juce::SoftwareImageType());
    { // Necessaire pour fixer le scope du BitmapData
      juce::Image::BitmapData bitmap(m_SourceImage, juce::Image::BitmapData::readWrite);

      if (factor == 1)
        m_Image.GetArea(u0, v0, win, hin, bitmap.data);
      else
        m_Image.GetZoomArea(u0, v0, win, hin, bitmap.data, factor);

      if (m_Image.NbSample() == 1)
        XBaseImage::Gray2RGB(bitmap.data, wtmp * htmp);  // Passage en RGB

      if (m_ToneMapper != nullptr)
        m_ToneMapper->process_8bit_rgb_image(bitmap.data, wtmp, htmp);

      uint8_t r = 0, g = 0, b = 0, alpha = 255;
      XBaseImage::RGB2BGRA(bitmap.data, wtmp * htmp, r, g, b, alpha);
      XBaseImage::OffsetArea(bitmap.data, wtmp * 4, bitmap.height, bitmap.lineStride);
    }
  }
  else {
    juce::File file(m_strRequest);
    juce::Image image = juce::ImageFileFormat::loadFrom(file);  // Image classique (JPEG, PNG, GIF)
    if (!image.isValid())
      return m_ProjImage;
    m_SourceImage = image.getClippedImage(juce::Rectangle<int>(u0, v0, win, hin))
                          .rescaled(wtmp, htmp)
                          .convertedToFormat(juce::Image::ARGB);
  }

  // Reechantillonage dans la projection souhaitee
  XTransfoRotation transfo(m_dRot, m_C, m_dGsd, m_O, u0, v0, factor);
  transfo.SetEndFrame(F);
  Resample(&transfo);
  m_ProjImage = m_ProjImage.rescaled((int)(F.Width() / gsd), (int)(F.Height() / gsd));
  return m_ProjImage;
}

//-----------------------------------------------------------------------------
// Attributs de l'objet
//-----------------------------------------------------------------------------
bool RotationImage::ReadAttributes(std::vector<std::string>& V)
{
  V.clear();
  V.push_back("GSD"); V.push_back(juce::String(m_dGsd).toStdString());
  V.push_back("Rotation"); V.push_back(juce::String(m_dRot * 180. / XPI).toStdString());
  V.push_back("Center X"); V.push_back(juce::String(m_C.X).toStdString());
  V.push_back("Center Y"); V.push_back(juce::String(m_C.Y).toStdString());
  return true;
}

//-----------------------------------------------------------------------------
// Calcul du cadre terrain de l'image en fonction de la rotation
//-----------------------------------------------------------------------------
bool RotationImage::ComputeFrame()
{
  if ((m_nW < 1)||(m_nH < 1))
    return false;
  double X0, Y0, X1, Y1, X2, Y2, X3, Y3;
  Image2Ground(0, 0, &X0, &Y0);
  Image2Ground(m_nW, 0, &X1, &Y1);
  Image2Ground(m_nW, m_nH, &X2, &Y2);
  Image2Ground(0, m_nH, &X3, &Y3);

  m_Frame.Xmin = XMin(X0, X1);
  m_Frame.Xmin = XMin(m_Frame.Xmin, X2);
  m_Frame.Xmin = XMin(m_Frame.Xmin, X3);

  m_Frame.Ymin = XMin(Y0, Y1);
  m_Frame.Ymin = XMin(m_Frame.Ymin, Y2);
  m_Frame.Ymin = XMin(m_Frame.Ymin, Y3);

  m_Frame.Xmax = XMax(X0, X1);
  m_Frame.Xmax = XMax(m_Frame.Xmax, X2);
  m_Frame.Xmax = XMax(m_Frame.Xmax, X3);

  m_Frame.Ymax = XMax(Y0, Y1);
  m_Frame.Ymax = XMax(m_Frame.Ymax, Y2);
  m_Frame.Ymax = XMax(m_Frame.Ymax, Y3);

  return true;
}

//-----------------------------------------------------------------------------
// Passage coordonnees Image -> Terrain
//-----------------------------------------------------------------------------
void RotationImage::Image2Ground(double u, double v, double* x, double* y)
{
  // Passage en coordoonnees centrees
  double Ua = u - m_O.X;
  double Va = m_O.Y - v;
  // Rotation
  double Ub = cos(m_dRot) * Ua - sin(m_dRot) * Va;
  double Vb = sin(m_dRot) * Ua + cos(m_dRot) * Va;
  // Passage en coordoonnees terrain
  *x = m_C.X + Ub * m_dGsd;
  *y = m_C.Y + Vb * m_dGsd;
}

//-----------------------------------------------------------------------------
// Passage coordonnees Terrain -> Image
//-----------------------------------------------------------------------------
void RotationImage::Ground2Image(double x, double y, double* u, double* v)
{
  // Rotation inverse en coordonnees terrain
  double Xa = cos(m_dRot) * (x - m_C.X) + sin(m_dRot) * (y - m_C.Y);
  double Ya = -sin(m_dRot) * (x - m_C.X) + cos(m_dRot) * (y - m_C.Y);
  // Passage en coordonnees pixel centrees
  *u = Xa / m_dGsd;
  *v = Ya / m_dGsd;
  // Passage en coordonnees image
  *u = *u + m_O.X;
  *v = m_O.Y - *v;
}

//-----------------------------------------------------------------------------
// Transformation directe
//-----------------------------------------------------------------------------
void XTransfoRotation::Direct(double x, double y, double* u, double* v)
{
  // Coordonnees image d'arrivee -> coordonnees terrain
  double X = x * m_dGsd * m_Factor + m_Ff.Xmin;
  double Y = m_Ff.Ymax - y * m_dGsd * m_Factor;
  // Rotation inverse en coordonnees terrain
  double Xa = cosT * (X - m_C.X) + sinT * (Y - m_C.Y);
  double Ya = -sinT * (X - m_C.X) + cosT * (Y - m_C.Y);
  // Passage en coordonnees pixel centrees
  *u = Xa / m_dGsd;
  *v = Ya / m_dGsd;
  // Passage en coordonnees image
  *u = *u + m_O.X;
  *v = m_O.Y - *v;
  // Passage dans l'extrait
  *u -= m_U0;
  *v -= m_V0;
  // Passage a l'echelle
  *u /= m_Factor;
  *v /= m_Factor;
}

//-----------------------------------------------------------------------------
// Dimension de l'image reechantillonnee
//-----------------------------------------------------------------------------
void XTransfoRotation::Dimension(int , int , int* wout, int* hout)
{
  *wout = (int)(m_Ff.Width() / (m_dGsd * m_Factor));
  *hout = (int)(m_Ff.Height() / (m_dGsd * m_Factor));
}