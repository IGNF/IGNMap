//-----------------------------------------------------------------------------
//								XInternetMap.h
//								==============
//
// Fonctions utilitaires pour Google Maps, Bing Maps, Geoportail ...
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/03/2024
//-----------------------------------------------------------------------------

#include "XInternetMap.h"
#include "../XToolGeod/XGeoPref.h"

//==============================================================================
// URL pour voir un point sur Google Maps
//==============================================================================
std::string XInternetMap::GoogleMapsUrl(const XPt2D& C, double scale)
{
  // Calcul des parametres
  double lat, lon;
  double s = scale;

  XGeoPref Pref;
  Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, C.X, C.Y, lon, lat);
  if (Pref.Projection() == XGeoProjection::RGF93)
    s *= (2 * XPI * 6378000. / 360.);
  if (C == XPt2D(0, 0)) {
    XFrame geoF = XGeoProjection::FrameGeo(Pref.Projection());
    lon = geoF.Center().X;
    lat = geoF.Center().Y;
  }

  s /= 2468;
  int zoom = 0;
  for (int i = 17; i >= 0; i--) {
    if (s < pow(2, 17 - i)) {
      zoom = i + 1;
      break;
    }
  }
  if (zoom > 17) zoom = 17;

  std::string map_type;
  if (Pref.GoogleMode() == 0) map_type = "roadmap";
  if (Pref.GoogleMode() == 1) map_type = "satellite";
  if (Pref.GoogleMode() == 2) map_type = "terrain";

  // https://www.google.com/maps/@?api=1&map_action=map&center=-33.712206,150.311941&zoom=12&basemap=terrain
  char buf[512];
  snprintf(buf, 512, "https://www.google.com/maps/@?api=1&map_action=map&center=%lf,%lf&zoom=%d&basemap=%s",
    lat, lon, zoom, map_type.c_str());
  if (Pref.GoogleMode() == 3) // Streetview
    snprintf(buf, 512, "https://www.google.com/maps/@?api=1&map_action=pano&viewpoint=%lf,%lf", lat, lon);

  return buf;
}

//==============================================================================
// URL pour voir un point sur Bing Maps
//==============================================================================
std::string XInternetMap::BingMapsUrl(const XPt2D& C, double scale)
{
  // Calcul des parametres
  double lat, lon;
  double s = scale;
  XGeoPref Pref;
  Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, C.X, C.Y, lon, lat);
  if (Pref.Projection() == XGeoProjection::RGF93)
    s *= (2 * XPI * 6378000. / 360.);
  if (C == XPt2D(0, 0)) {
    XFrame geoF = XGeoProjection::FrameGeo(Pref.Projection());
    lon = geoF.Center().X;
    lat = geoF.Center().Y;
  }

  s /= 2500;
  int zoom = 0;
  for (int i = 18; i >= 0; i--) {
    if (s < pow(2, 18 - i)) {
      zoom = i;
      break;
    }
  }
  if (zoom > 18) zoom = 18;

  // Style de la carte
  std::string map_type;
  if (Pref.VEMode() == 0) map_type = "r";
  if (Pref.VEMode() == 1) map_type = "a";
  if (Pref.VEMode() == 2) map_type = "h";
  if (Pref.VEMode() == 3) map_type = "o";
  if (Pref.VEMode() == 4) map_type = "b";

  // http://bing.com/maps/default.aspx?cp=43.901683~-69.522416&lvl=12&style=r
  char buf[512];
  snprintf(buf, 512, "https://bing.com/maps/default.aspx?cp=%lf~%lf&lvl=%d&style=%s",
    lat, lon, zoom, map_type.c_str());

  return buf;
}