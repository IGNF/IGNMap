//-----------------------------------------------------------------------------
//								XGeoPref.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 04/09/2007
//-----------------------------------------------------------------------------

#include "XGeoPref.h"

XLambert XGeoPref::m_Lambert;
XDomGeod XGeoPref::m_DomGeod;
XNCGeod XGeoPref::m_NCGeod;
XWebMercator XGeoPref::m_WebMercator;
XGeoProjection::XProjCode XGeoPref::m_Projection = XGeoProjection::Lambert2E;	// Projection par defaut
XGeoProjection::XProjCode XGeoPref::m_ProjecView = XGeoProjection::Lambert2E;	// Projection par defaut

// Preferences Internet
uint8_t XGeoPref::m_nGoogleMode = 0;
uint8_t XGeoPref::m_nVEMode = 0;
uint8_t XGeoPref::m_nGeoportMode = 0;
bool XGeoPref::m_bAutoSynchro = false;
bool XGeoPref::m_bCadastre = false;

// Preferences de geo-referencement
bool XGeoPref::m_bGeorefTfw = true;
bool XGeoPref::m_bGeorefTab = true;
bool XGeoPref::m_bGeorefGxt = true;
bool XGeoPref::m_bGeorefXml = true;
bool XGeoPref::m_bGeorefGrf = true;

//-----------------------------------------------------------------------------
// Initialisation du moteur de projection
//-----------------------------------------------------------------------------
bool XGeoPref::InitProjection(const char* gridfile, XGeoProjection::XProjCode projection)
{
	m_Projection = projection;
  m_ProjecView = projection;
  return m_Lambert.ReadGrid(gridfile);
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XGeoPref::Convert(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                       double& Xf, double& Yf, double Z)
{
  if ((start_proj <= ETRS89TM32)&&(end_proj <= ETRS89TM32)) // Projection metropolitaine
    return m_Lambert.Convert(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);
  if (m_DomGeod.SetDefaultProjection(start_proj, end_proj))
    return m_DomGeod.Convert(Xi, Yi, Xf, Yf, Z);
  if (m_NCGeod.SetDefaultProjection(start_proj, end_proj))
    return m_NCGeod.Convert(Xi, Yi, Xf, Yf, Z);
  // Projection WebMercator
  if (start_proj == XGeoProjection::WebMercator) {
    double lon, lat;
    m_WebMercator.SetDefaultProjection(start_proj, XGeoProjection::RGF93);
    m_WebMercator.Convert(Xi, Yi, lon, lat, Z);
    return Convert(XGeoProjection::RGF93, end_proj, lon, lat, Xf, Yf, Z);
  }
  if (end_proj == XGeoProjection::WebMercator) {
    double lon, lat;
    if (!Convert(start_proj, XGeoProjection::RGF93, Xi, Yi, lon, lat, Z))
      return false;
    m_WebMercator.SetDefaultProjection(XGeoProjection::RGF93, end_proj);
    return m_WebMercator.Convert(lon, lat, Xf, Yf, Z);
  }
  return false;
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XGeoPref::ConvertDeg(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                          double& Xf, double& Yf, double Z)
{
  if ((start_proj != RGF93)&&(end_proj != RGF93))
    return Convert(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);

  double xi = Xi, yi = Yi;
  if (start_proj == RGF93) {
    xi *= (XPI / 180.);
    yi *= (XPI / 180.);
  }
  bool flag = Convert(start_proj, end_proj, xi, yi, Xf, Yf, Z);
  if (end_proj == RGF93) {
    Xf *= (180. / XPI);
    Yf *= (180. / XPI);
  }
  return flag;
}

//-----------------------------------------------------------------------------
// Alteration Lineaire
//-----------------------------------------------------------------------------
double XGeoPref::AltLin(XProjCode start_proj, double Xi, double Yi)
{
  if (start_proj <= LambertCC50) // Projection metropolitaine
    return m_Lambert.AltLin(start_proj, Xi, Yi);

  return m_DomGeod.AltLin(start_proj, Xi, Yi);
}
