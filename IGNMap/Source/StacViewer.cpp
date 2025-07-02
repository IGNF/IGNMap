//-----------------------------------------------------------------------------
//								StacViewer.cpp
//								==============
//
// Utilisation d'un catalogue STAC pour trouver une image proche d'un point
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/06/2025
//-----------------------------------------------------------------------------

#include "StacViewer.h"
#include "ClassViewer.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XTool/XGeoBase.h"


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void StacViewer::mouseDown(const juce::MouseEvent& event)
{
  if (event.y <= getTitleBarHeight()) {
    juce::DocumentWindow::mouseDown(event);
    return;
  }

}

//-----------------------------------------------------------------------------
// Fixe le point de recherche
//-----------------------------------------------------------------------------
void StacViewer::SetTarget(const double& X, const double& Y, const double& Z)
{
  XGeoPref pref;
  double lon = 0., lat = 0., radius = 1000.;
  pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, X - radius, Y - radius, lon, lat, Z);
  juce::String server = m_StacServer + "search?bbox=" + juce::String(lon) + "," + juce::String(lat) + ",";
  pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, X + radius, Y + radius, lon, lat, Z);
  server += juce::String(lon) + "," + juce::String(lat) + "&limit=20";

  juce::URL url(server);
  juce::WebInputStream web(url, false);
  juce::String content = web.readEntireStreamAsString();

  juce::var parsedJSON = juce::JSON::parse(content);

  if (!parsedJSON.hasProperty("features"))
    return;
  juce::var features = parsedJSON["features"];
  if (features.size() < 1)
    return;
  juce::var first = features[0];
  juce::var id = first["id"];
  if (!id.isString())
    return;
  juce::String idString = id.toString();

  server = m_StacServer + "pictures/" + idString + "/hd.jpg";
  juce::WebInputStream web_image(juce::URL(server), false);
  juce::JPEGImageFormat format;

  juce::Image image = format.decodeImage(web_image);
  m_ImageComponent.setImage(image);

  // Creation d'un GEOJSON
  //juce::File geojson = m_Cache.getNonexistentChildFile("stac", ".json");
  if (m_Class != nullptr) {
    gClassViewerMgr.RemoveViewer(m_Class);
    m_Base->ClearSelection();
    m_Base->RemoveClass(m_Class->Layer()->Name().c_str(), m_Class->Name().c_str());
  }
  juce::File geojson = m_Cache.getChildFile("Stac.json");
  geojson.replaceWithText(content);
  m_Class = GeoTools::ImportGeoJson(geojson.getFullPathName(), m_Base);
  GeoTools::ColorizeClasses(m_Base);
  sendActionMessage("UpdateVector");
}

