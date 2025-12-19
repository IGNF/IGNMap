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
// Double-clic souris
//-----------------------------------------------------------------------------
void StacViewer::mouseDoubleClick(const juce::MouseEvent& event)
{
  if (!m_bThumb)
    return;
  juce::MouseCursor::showWaitCursor();
  juce::String server = m_StacServer + "pictures/" + m_Id + "/hd.jpg";
  juce::WebInputStream web_image(juce::URL(server), false);
  juce::JPEGImageFormat format;
  m_Image = format.decodeImage(web_image);
  m_bThumb = false;
  m_nTx = 180;
  m_nTy = 0;
  SetImage();
  juce::MouseCursor::hideWaitCursor();
}

//-----------------------------------------------------------------------------
// Drag souris
//-----------------------------------------------------------------------------
void StacViewer::mouseDrag(const juce::MouseEvent& event)
{
  m_nTx -= event.getDistanceFromDragStartX();
  m_nTy -= event.getDistanceFromDragStartY();
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
    m_nTx -= 5 * down;
    SetImage();
    return true;
  }
  if (key.getKeyCode() == juce::KeyPress::rightKey) {
    m_nTx += 5 * down;
    SetImage();
    return true;
  }
  if (key.getKeyCode() == juce::KeyPress::upKey) {
    m_nTy += 1 * down;
    if (m_nTy >= 90) m_nTy = 89;
    SetImage();
    return true;
  }
  if (key.getKeyCode() == juce::KeyPress::downKey) {
    m_nTy -= 1 * down;
    if (m_nTy <= -45) m_nTy = -44;
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
  static juce::Time time;
  juce::Time now = juce::Time::getCurrentTime();
  if ((now.toMilliseconds() - time.toMilliseconds()) < (juce::int64)500)
    return;
  time = now;
  if (dist(m_Pos, XPt2D(X, Y)) < 10.)
    return;
  m_Pos = XPt2D(X, Y);

  XGeoPref pref;
  double lon = 0., lat = 0., radius = 500.;
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
  juce::String idStr = id.toString();
  if (idStr == m_Id)
    return;
  m_Id = idStr;
  
  m_Stac.m_Tree.setRootItem(nullptr);
  m_Stac.m_RootItem.reset();
  if (first.hasProperty("properties")) {
    juce::var prop = first["properties"];
    m_Stac.m_RootItem.reset(new JsonTreeItem("properties", prop));
    m_Stac.m_Tree.setRootItem(m_Stac.m_RootItem.get());

    if (prop.hasProperty("view:azimuth"))
      m_Azimuth = (double)prop["view:azimuth"];

    if (prop.hasProperty("exif")) {
      juce::var exif = prop["exif"];
      if (exif.hasProperty("Xmp.GPano.ProjectionType"))
        m_Projection = exif["Xmp.GPano.ProjectionType"].toString();
    }
  }

  server = m_StacServer + "pictures/" + m_Id + "/thumb.jpg";
  juce::WebInputStream web_image(juce::URL(server), false);
  juce::JPEGImageFormat format;
  m_Image = format.decodeImage(web_image);
  m_Stac.m_ImageComponent.setImage(m_Image);
  m_bThumb = true;
}

//-----------------------------------------------------------------------------
// Fixe l'image dans le composant
//-----------------------------------------------------------------------------
void StacViewer::SetImage()
{
  if (m_Image.isNull())
    return;
  if (m_bThumb)
    return;
  if (m_Projection != "equirectangular") {
    m_Stac.m_ImageComponent.setImage(m_Image);
    return;
  }
  ComputeSphereImage();
}

//-----------------------------------------------------------------------------
// Calcul d'une image a partir d'une image equirectangulaire
//-----------------------------------------------------------------------------
void StacViewer::ComputeSphereImage()
{
  juce::Image image(juce::Image::RGB, m_Stac.m_ImageComponent.getWidth(), m_Stac.m_ImageComponent.getHeight(), true, juce::SoftwareImageType());
  {
    juce::Image::BitmapData bitmap(image, juce::Image::BitmapData::readWrite);
    juce::Image::BitmapData source(m_Image, juce::Image::BitmapData::readOnly);

    int x = 0, y = 0, sh2 = source.height / 2, sw2 = source.width / 2;
    double startAlpha = (m_nTx % 360) * XPI / 180., startBeta = m_nTy * XPI / 180., alpha = 0., beta = 0., focal = bitmap.width / 2;

    for (int line = 0; line < bitmap.height; line++) {
      if ((bitmap.height / 2 - line) != 0)
        beta = startBeta + atan((bitmap.height / 2 - line) / focal);
      else
        beta = startBeta;
      
      y = XRint(sh2 - sh2 * beta / (XPI / 2.));
      if (y < 0) y = 0;
      if (y >= source.height) y = source.height - 1;

      for (int col = 0; col < bitmap.width; col++) {
        if ((col - bitmap.width / 2) != 0)
          alpha = startAlpha + atan((col - bitmap.width / 2) / focal);
        else
          alpha = startAlpha;

        alpha = fmod(alpha, 2 * XPI);
        x = XRint(source.width * alpha / (2 * XPI));
        if (x < 0) 
          x = source.width + x;
        if (x >= source.width) 
          x = x - source.width;

        bitmap.setPixelColour(col, line, source.getPixelColour(x, y));
      }
    }
  }

  m_Stac.m_ImageComponent.setImage(image);
}
