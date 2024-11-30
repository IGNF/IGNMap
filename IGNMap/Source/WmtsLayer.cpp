//-----------------------------------------------------------------------------
//								WmtsLayer.cpp
//								=============
//
// Gestion des flux WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/11/2023
//-----------------------------------------------------------------------------

#include "WmtsLayer.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolGeod/XTransfoGeod.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
WmtsLayer::WmtsLayer(std::string server, std::string layer, std::string TMS, std::string format,
  uint32_t tileW, uint32_t tileH, uint32_t max_zoom, std::string apikey)
{
  m_strServer = server;
  m_strLayer = layer;
  m_strFormat = format;
  m_strTileMatrixSet = TMS;
  m_strApiKey = apikey;

  m_nTileW = tileW;
  m_nTileH = tileH;
  m_nMaxZoom = max_zoom;

  CreateCacheDir("WMTS");
}

//-----------------------------------------------------------------------------
// Chargement d'un cadre en WebMercator a un zoom donne
//-----------------------------------------------------------------------------
bool WmtsLayer::LoadFrame(const XFrame& F, int zoomlevel)
{
  // Dalles a charger pour couvrir l'emprise
  double a = 6378137;
  double xmin = (1 + (F.Xmin / a) / XPI) * 0.5 * pow(2, zoomlevel);
  int firstX = (int)floor(xmin);
  double xmax = (1 + (F.Xmax / a) / XPI) * 0.5 * pow(2, zoomlevel);
  int lastX = (int)ceil(xmax);
  double ymin = (1 - (F.Ymax / a) / XPI) * 0.5 * pow(2, zoomlevel);
  int firstY = (int)floor(ymin);
  double ymax = (1 - (F.Ymin / a) / XPI) * 0.5 * pow(2, zoomlevel);
  int lastY = (int)ceil(ymax);

  int nb_tilex = lastX - firstX;
  int nb_tiley = lastY - firstY;
  m_SourceImage = juce::Image(juce::Image::PixelFormat::ARGB, nb_tilex * m_nTileW, nb_tiley * m_nTileH, true);
  juce::Graphics g(m_SourceImage);
  g.setOpacity(1.0f);

  for (int i = 0; i < nb_tiley; i++) {
    for (int j = 0; j < nb_tilex; j++) {
      int x = firstX + j;
      int y = firstY + i;
      juce::String filename = LoadTile(x, y, zoomlevel);
      juce::Image image = juce::ImageFileFormat::loadFrom(juce::File(filename));
      if (!image.isValid()) {
        juce::File badFile(filename); // Le fichier est peut etre corrompu
        badFile.deleteFile();
        continue;
      }
      g.drawImageAt(image, j * m_nTileW, i * m_nTileH);
    }
  }

  double resol = 6378137. * 2 * XPI / pow(2, (zoomlevel + 8));
  int xcrop = XRint((xmin - firstX) * m_nTileW);
  int ycrop = XRint((ymin - firstY) * m_nTileH);
  int wcrop = XRint(F.Width() / resol);
  int hcrop = XRint(F.Height() / resol);
  m_SourceImage = m_SourceImage.getClippedImage(juce::Rectangle<int>(xcrop, ycrop, wcrop, hcrop));

  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
juce::String WmtsLayer::LoadTile(int x, int y, int zoomlevel)
{
  juce::String filename = m_Cache.getFullPathName() + juce::File::getSeparatorString() + m_strLayer + "_" +
    juce::String(zoomlevel) + "_" + juce::String(x) + "_" + juce::String(y) + "." + m_strFormat;
  juce::File cache(filename);
  if (cache.existsAsFile()) // Le fichier a deja ete telecharge
    return filename;

  // Telechargement du fichier
  m_strRequest = "https://" + m_strServer + "?LAYER=" + m_strLayer + "&EXCEPTIONS=text/xml&FORMAT=image/" + m_strFormat + 
    "&SERVICE=WMTS&VERSION=1.0.0&REQUEST=GetTile&STYLE=normal&TILEMATRIXSET=" + m_strTileMatrixSet +
    "&TILEMATRIX=" + juce::String(zoomlevel) + "&TILEROW=" + juce::String(y) + "&TILECOL=" + juce::String(x);
  if (m_strApiKey.isNotEmpty())
    m_strRequest += ("&apikey=" + m_strApiKey);
  juce::URL url(m_strRequest);
  juce::URL::DownloadTaskOptions options;
  std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(filename, options);
  if (task.get() == nullptr)
    return filename;
  int count = 0;
  while (task.get()->isFinished() == false)
  {
    juce::Thread::sleep(50);
    count++;
    if (count > 100) break;
  }
  return filename;
}

//-----------------------------------------------------------------------------
// Renvoie une image pour couvrir un cadre
//-----------------------------------------------------------------------------
juce::Image& WmtsLayer::GetAreaImage(const XFrame& F, double gsd)
{
  if ((F == m_LastFrame) && (gsd == m_LastGsd))
    return m_ProjImage;
  m_LastFrame = F;
  m_LastGsd = gsd;
  uint32_t wout = (uint32_t)(F.Width() / gsd);
  uint32_t hout = (uint32_t)(F.Height() / gsd);

  // Caclul du niveau de zoom OSM
  XGeoPref pref;
  double S, longitude, latitude;
  int osm_zoom = m_nMaxZoom;
  pref.Convert(pref.Projection(), XGeoProjection::RGF93, F.Center().X, F.Center().Y, longitude, latitude);

  for (int zoom = 0; zoom <= (int)m_nMaxZoom; zoom++) {
    S = 6378137. * cos(latitude) * 2 * XPI / pow(2, (zoom + 8));  // GSD a l'Equateur x cos(latitude)
    if (S < gsd + gsd * 0.5) {
      osm_zoom = zoom;
      break;
    }
  }

  // Si on est en WebMercator
  if (pref.Projection() == WebMercator) {
    LoadFrame(F, osm_zoom);
    m_ProjImage = m_SourceImage.rescaled(wout, hout);
    return m_ProjImage;
  }

  // Calcul de l'emprise en WebMercator
  XWebMercator geod;
  double x0, y0, x1, y1, x2, y2, x3, y3;
  geod.SetDefaultProjection(pref.Projection(), WebMercator);
  geod.Convert(F.Xmin, F.Ymin, x0, y0);
  geod.Convert(F.Xmin, F.Ymax, x1, y1);
  geod.Convert(F.Xmax, F.Ymax, x2, y2);
  geod.Convert(F.Xmax, F.Ymin, x3, y3);

  XFrame FwebMerc;
  FwebMerc.Xmin = XMin(x0, x1) - 5. * gsd;  // On ajoute un buffer pour eviter les pixels blancs
  FwebMerc.Xmax = XMax(x2, x3) + 5. * gsd;  // en bord d'image reechantillonnee
  FwebMerc.Ymin = XMin(y0, y3) - 5. * gsd;
  FwebMerc.Ymax = XMax(y1, y2) + 5. * gsd;

  LoadFrame(FwebMerc, osm_zoom);
  m_SourceImage = m_SourceImage.rescaled((int)(FwebMerc.Width() / gsd), (int)(FwebMerc.Height() / gsd));

  // Reechantillonage dans la projection souhaitee
  XTransfoGeodInterpol transfo(&geod);
  transfo.SetStartFrame(FwebMerc);
  transfo.SetEndFrame(F);
  transfo.SetResolution(gsd);
  transfo.AutoCalibration();
  Resample(&transfo);

  return m_ProjImage;
}

//-----------------------------------------------------------------------------
// Attributs de l'objet WmtsLayer
//-----------------------------------------------------------------------------
bool WmtsLayer::ReadAttributes(std::vector<std::string>& V)
{
  V.clear();
  V.push_back("Server"); V.push_back(m_strServer.toStdString());
  V.push_back("Layer"); V.push_back(m_strLayer.toStdString());
  V.push_back("Url"); V.push_back(m_strRequest.toStdString());
  V.push_back("TileW"); V.push_back(juce::String(m_nTileW).toStdString());
  V.push_back("TileH"); V.push_back(juce::String(m_nTileH).toStdString());
  V.push_back("GSD"); V.push_back(juce::String(m_LastGsd).toStdString());
  return true;
}

//-----------------------------------------------------------------------------
// Recherche de la projection
//-----------------------------------------------------------------------------
bool WmtsLayerTMS::FindProjection()
{
  m_ProjCode = XGeoProjection::Unknown;
  for (int proj = XGeoProjection::RGF93; proj < XGeoProjection::NC_RGNC91_UTM59; proj++) {
    juce::String code = "EPSG:" + juce::String(XGeoProjection::EPSGCode((XGeoProjection::XProjCode)proj));
    if (m_TMS.Crs() == code) {
      m_ProjCode = (XGeoProjection::XProjCode)proj;
      break;
    }
  }
  if (m_ProjCode == XGeoProjection::Unknown)
    return false;
  XGeoPref pref;
  XFrame projF, geoF = XGeoProjection::FrameGeo(pref.Projection());
  pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, projF.Xmin, projF.Ymin);
  pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, projF.Xmax, projF.Ymax);
  m_Frame = projF;

  CreateCacheDir(m_strId);
  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
juce::String WmtsLayerTMS::LoadTile(uint32_t x, uint32_t y, std::string idLevel)
{
  juce::String filename = m_Cache.getFullPathName() + juce::File::getSeparatorString() + m_strId + "_" +
    idLevel + "_" + juce::String(x) + "_" + juce::String(y);
  juce::File cache(filename);
  if (cache.existsAsFile()) // Le fichier a deja ete telecharge
    return filename;

  // Telechargement du fichier
  m_strRequest = m_strServer + "?LAYER=" + m_strId + "&EXCEPTIONS=text/xml&FORMAT=" + m_strFormat +
    "&SERVICE=WMTS&VERSION=1.0.0&REQUEST=GetTile&STYLE=normal&TILEMATRIXSET=" + m_TMS.Id() +
    "&TILEMATRIX=" + idLevel + "&TILEROW=" + juce::String(y) + "&TILECOL=" + juce::String(x);
  if (m_strApiKey.isNotEmpty())
    m_strRequest += ("&apikey=" + m_strApiKey);

  juce::URL url(m_strRequest);
  juce::URL::DownloadTaskOptions options;
  std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(filename, options);
  if (task.get() == nullptr)
    return filename;
  int count = 0;
  while (task.get()->isFinished() == false)
  {
    juce::Thread::sleep(50);
    count++;
    if (count > 100) break;
  }
  return filename;
}

//-----------------------------------------------------------------------------
// Chargement d'un cadre en projection native a un zoom donne
//-----------------------------------------------------------------------------
bool WmtsLayerTMS::LoadFrame(const XFrame& F, int numMatrix)
{
  if (numMatrix >= m_TMS.NbTile())
    return false;
  XTileMatrix tm = m_TMS.Tile(numMatrix);
  double gsd = tm.GSD();
  XTileMatrixLimits limits = m_Limits[numMatrix];

  // Dalles a charger pour couvrir l'emprise
  double xmin = (F.Xmin - tm.XTL()) / (gsd * tm.TileW());
  int firstX = (int)floor(xmin);
  double xmax = (F.Xmax - tm.XTL()) / (gsd * tm.TileW());
  int lastX = (int)ceil(xmax);
  double ymin = (tm.YTL() - F.Ymax) / (gsd * tm.TileH());
  int firstY = (int)floor(ymin);
  double ymax = (tm.YTL() - F.Ymin) / (gsd * tm.TileH());
  int lastY = (int)ceil(ymax);

  int nb_tilex = lastX - firstX;
  int nb_tiley = lastY - firstY;
  m_SourceImage = juce::Image(juce::Image::PixelFormat::ARGB, nb_tilex * tm.TileW(), nb_tiley * tm.TileH(), true);
  juce::Graphics g(m_SourceImage);
  g.setOpacity(1.0f);

  int nb_image_drawn = 0;
  for (uint32_t i = 0; i < nb_tiley; i++) {
    uint32_t y = firstY + i;
    if ((y < limits.MinTileRow()) || (y > limits.MaxTileRow()))
      continue;
    for (uint32_t j = 0; j < nb_tilex; j++) {
      uint32_t x = firstX + j;
      if ((x < limits.MinTileCol()) || (x > limits.MaxTileCol()))
        continue;
      juce::String filename = LoadTile(x, y, tm.Id());
      juce::Image image = juce::ImageFileFormat::loadFrom(juce::File(filename));
      if (!image.isValid()) {
        juce::File badFile(filename); // Le fichier est peut-etre corrompu
        badFile.deleteFile();
        continue;
      }
      g.drawImageAt(image, j * tm.TileW(), i * tm.TileH());
      nb_image_drawn++;
    }
  }
  if (nb_image_drawn < 1) // L'image est vide
    return false;

  int xcrop = XRint((xmin - firstX) * tm.TileW());
  int ycrop = XRint((ymin - firstY) * tm.TileH());
  int wcrop = XRint(F.Width() / gsd);
  int hcrop = XRint(F.Height() / gsd);
  m_SourceImage = m_SourceImage.getClippedImage(juce::Rectangle<int>(xcrop, ycrop, wcrop, hcrop));

  return true;
}

//-----------------------------------------------------------------------------
// Renvoie une image pour couvrir un cadre
//-----------------------------------------------------------------------------
juce::Image& WmtsLayerTMS::GetAreaImage(const XFrame& F, double gsd)
{
  if ((F == m_LastFrame) && (gsd == m_LastGsd))
    return m_ProjImage;
  m_LastFrame = F;
  m_LastGsd = gsd;
  uint32_t wout = (uint32_t)(F.Width() / gsd);
  uint32_t hout = (uint32_t)(F.Height() / gsd);

  // Recherche du niveau de zoom adapte
  int index = m_TMS.NbTile() - 1;
  for (int i = 0; i < m_TMS.NbTile(); i++) {
    if (gsd >= m_TMS.Tile(i).GSD()) {
      index = i;
      if (i > 0) {
        if (gsd > (m_TMS.Tile(i).GSD() + m_TMS.Tile(i-1).GSD()) * 0.5)
          index = i - 1;
      }
      break;
    }
  }

  m_ProjImage = juce::Image();
  // Si on est dans la bonne projection
  XGeoPref pref;
  if (pref.Projection() == m_ProjCode) {
    if (!LoadFrame(F, index))
      return m_ProjImage;
    m_ProjImage = m_SourceImage.rescaled(wout, hout);
    return m_ProjImage;
  }

  // Calcul de l'emprise dans la projection de la couche TMS
  XWebMercator geod;
  double x0, y0, x1, y1, x2, y2, x3, y3;
  geod.SetDefaultProjection(pref.Projection(), m_ProjCode);
  geod.Convert(F.Xmin, F.Ymin, x0, y0);
  geod.Convert(F.Xmin, F.Ymax, x1, y1);
  geod.Convert(F.Xmax, F.Ymax, x2, y2);
  geod.Convert(F.Xmax, F.Ymin, x3, y3);

  XFrame FwebMerc;
  FwebMerc.Xmin = XMin(x0, x1);
  FwebMerc.Xmax = XMax(x2, x3);
  FwebMerc.Ymin = XMin(y0, y3);
  FwebMerc.Ymax = XMax(y1, y2);

  if (!LoadFrame(FwebMerc, index))
    return m_ProjImage;
  m_SourceImage = m_SourceImage.rescaled((int)(FwebMerc.Width() / gsd), (int)(FwebMerc.Height() / gsd));

  // Reechantillonage dans la projection souhaitee
  XTransfoGeodInterpol transfo(&geod);
  transfo.SetStartFrame(FwebMerc);
  transfo.SetEndFrame(F);
  transfo.SetResolution(gsd);
  transfo.AutoCalibration();
  Resample(&transfo);

  return m_ProjImage;
}