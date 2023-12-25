//-----------------------------------------------------------------------------
//								XWebMercator.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 19/11/2012
//-----------------------------------------------------------------------------

#include "XWebMercator.h"
#include "XGeoPref.h"

//-----------------------------------------------------------------------------
// Choix des projections par defaut
//-----------------------------------------------------------------------------
bool XWebMercator::SetDefaultProjection(XProjCode start_proj, XProjCode end_proj)
{
  /*
  if (start_proj == RGF93) {
    m_StartProjection = RGF93;
    m_EndProjection = WebMercator;
    return true;
  }
  if (start_proj == WebMercator) {
    m_StartProjection = WebMercator;
    m_EndProjection = RGF93;
    return true;
  }
  return false;*/
  m_StartProjection = start_proj;
  m_EndProjection = end_proj;
  return true;
}

//-----------------------------------------------------------------------------
// Conversion avec passage en degree decimaux
//-----------------------------------------------------------------------------
bool XWebMercator::ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z)
{
  if ((m_StartProjection != RGF93)&&(m_EndProjection != RGF93))
    return Convert(Xi, Yi, Xf, Yf, Z);

  double xi = Xi, yi = Yi;
  if (m_StartProjection == RGF93) {
    xi *= (XPI / 180.);
    yi *= (XPI / 180.);
  }
  bool flag = Convert(xi, yi, Xf, Yf, Z);
  if (m_EndProjection == RGF93) {
    Xf *= (180. / XPI);
    Yf *= (180. / XPI);
  }
  return flag;
}

//-----------------------------------------------------------------------------
// Conversion normale
//-----------------------------------------------------------------------------
bool XWebMercator::Convert(double Xi, double Yi, double& Xf, double& Yf, double Z)
{
  double a = 6378137;
  if ((m_StartProjection == RGF93)&&(m_EndProjection == WebMercator)) {
    Xf = Xi * a;
    Yf = a * log(tan(Yi * 0.5  + XPI4));
    return true;
  }
  if ((m_StartProjection == WebMercator)&&(m_EndProjection == RGF93)) {
    Xf = Xi / a;
    Yf = 2. * (atan(exp(Yi / a)) - XPI4);
    return true;
  }
  if (m_StartProjection == WebMercator) {
    double xrgf = Xi / a;
    double yrgf = 2. * (atan(exp(Yi / a)) - XPI4);
    XGeoPref pref;
    return pref.Convert(RGF93, m_EndProjection, xrgf, yrgf, Xf, Yf);
  }
  if (m_EndProjection == WebMercator) {
    double xrgf, yrgf;
    XGeoPref pref;
    bool flag = pref.Convert(m_StartProjection, RGF93, Xi, Yi, xrgf, yrgf);
    Xf = xrgf * a;
    Yf = a * log(tan(yrgf * 0.5  + XPI4));
    return flag;
  }

  return false;
}
