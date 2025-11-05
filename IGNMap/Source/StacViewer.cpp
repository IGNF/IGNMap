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
// Clic souris
//-----------------------------------------------------------------------------
void StacViewer::mouseDown(const juce::MouseEvent& event)
{
  if (event.y <= getTitleBarHeight()) {
    juce::DocumentWindow::mouseDown(event);
    return;
  }

}

//-----------------------------------------------------------------------------
// Drag souris
//-----------------------------------------------------------------------------
void StacViewer::mouseDrag(const juce::MouseEvent& event)
{
  int x = event.getDistanceFromDragStartX();
  m_nTx -= x;
  SetImage();
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool StacViewer::keyPressed(const juce::KeyPress& key)
{
  int down = 1;
  if (key.isCurrentlyDown())
    down = 5;
  if (key.getKeyCode() == juce::KeyPress::leftKey) {
    m_nTx -= 20 * down;
    SetImage();
    return true;
  }
  if (key.getKeyCode() == juce::KeyPress::rightKey) {
    m_nTx += 20 * down;
    SetImage();
    return true;
  }
  return false;	// On transmet l'evenement sans le traiter
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
  server += juce::String(lon) + "," + juce::String(lat) + "&limit=1";

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

  m_Image = format.decodeImage(web_image);
  SetImage();

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


//-----------------------------------------------------------------------------
// Fixe l'image dans le composant
//-----------------------------------------------------------------------------
void StacViewer::SetImage()
{
  if (m_Image.isNull())
    return;
  int scaleFactor = m_Image.getWidth() / (m_ImageComponent.getWidth() * 2);
  if (scaleFactor < 1) scaleFactor = 1;
  juce::Image image = m_Image.rescaled(m_Image.getWidth() / scaleFactor, m_Image.getHeight() / scaleFactor);

  int Wima = image.getWidth(), Hima = image.getHeight();
  int Wcomp = m_ImageComponent.getWidth() / 2, Hcomp = m_ImageComponent.getHeight();
  
  int W = Wcomp * Hima / Hcomp;
  int X0 = ((Wima - W) / 2 + m_nTx) % Wima;
  if (X0 < 0)
    X0 = Wima + X0;
  int Wcrop = Wima - X0;
  if (Wcrop > W) Wcrop = W;

  juce::Image crop(juce::Image::PixelFormat::ARGB, W, Hima, true, juce::SoftwareImageType());
  juce::Graphics g(crop);
  {
    g.drawImage(image, 0, 0, Wcrop, Hima, X0, 0, Wcrop, Hima);
    if (Wcrop < W)
      g.drawImage(image, Wcrop, 0, (W - Wcrop), Hima, 0, 0, (W - Wcrop), Hima);
  }
  juce::AffineTransform transform = juce::AffineTransform::fromTargetPoints(0, 0, 0, 0, W / 2, Hima / 2,  W, Hima / 2, W, 0, W * 2, 0);
  juce::Image final(juce::Image::PixelFormat::ARGB, W * 2, Hima, true, juce::SoftwareImageType());
  juce::Graphics gfinal(final);
  {
    gfinal.drawImageTransformed(crop, transform);
  }
  
  m_ImageComponent.setImage(final);
}

