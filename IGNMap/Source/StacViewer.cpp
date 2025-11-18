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
  juce::String server = m_StacServer + "pictures/" + m_Id + "/sd.jpg";
  juce::WebInputStream web_image(juce::URL(server), false);
  juce::JPEGImageFormat format;
  m_Image = format.decodeImage(web_image);
  m_bThumb = false;
  SetImage();
  juce::MouseCursor::hideWaitCursor();
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

  int scaleFactor = m_Image.getWidth() / (m_Stac.m_ImageComponent.getWidth() * 2);
  if (scaleFactor < 1) scaleFactor = 1;
  juce::Image image = m_Image.rescaled(m_Image.getWidth() / scaleFactor, m_Image.getHeight() / scaleFactor);

  int Wima = image.getWidth(), Hima = image.getHeight();
  int Wcomp = m_Stac.m_ImageComponent.getWidth() / 2, Hcomp = m_Stac.m_ImageComponent.getHeight();
  
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
  
  m_Stac.m_ImageComponent.setImage(final);
}

