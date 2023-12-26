//-----------------------------------------------------------------------------
//								OsmLayer.cpp
//								============
//
// Gestion des flux de donnees OSM raster
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 10/11/2023
//-----------------------------------------------------------------------------

#include "OsmLayer.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolGeod/XTransfoGeod.h"

//-----------------------------------------------------------------------------
// Lecture d'un serveur dans un fichier XML
//-----------------------------------------------------------------------------
bool OsmServer::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
  XParserXML server = parser->FindSubParser("/osm_server", num);
  if (server.IsEmpty())
    return XErrorError(error, "TOsmServer::XmlRead", XError::eBadFormat);

  m_strName = server.ReadNode("/osm_server/name");
  m_strKey = server.ReadNode("/osm_server/api_key");
  m_strFormat = server.ReadNode("/osm_server/format");
  m_nW = (uint16_t)server.ReadNodeAsUInt32("/osm_server/tileW");
  m_nH = (uint16_t)server.ReadNodeAsUInt32("/osm_server/tileH");
  m_nZoom = (uint16_t)server.ReadNodeAsUInt32("/osm_server/maxZoom");

  if (m_nW == 0) m_nW = 256;
  if (m_nH == 0) m_nH = 256;
  if (m_nZoom == 0) m_nZoom = 19;
  if (m_strFormat.size() < 1) m_strFormat = "png";

  return true;
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
OsmLayer::OsmLayer(std::string server, std::string apikey, std::string format,
  uint32_t tileW, uint32_t tileH, uint32_t max_zoom)
{   
  m_strServer = server;
  m_strKey = apikey;
  m_strFormat = format;

  m_nTileW = tileW;
  m_nTileH = tileH;
  m_nMaxZoom = max_zoom;

  CreateCacheDir("OSM");
}

//-----------------------------------------------------------------------------
// Chargement d'un cadre en WebMercator a un zoom donne
//-----------------------------------------------------------------------------
bool OsmLayer::LoadFrame(const XFrame& F, int zoomlevel)
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

  double osm_resol = 6378137. * 2 * XPI / pow(2, (zoomlevel + 8));
  int xcrop = XRint((xmin - firstX) * m_nTileW);
  int ycrop = XRint((ymin - firstY) * m_nTileH);
  int wcrop = XRint(F.Width() / osm_resol);
  int hcrop = XRint(F.Height() / osm_resol);
  m_SourceImage = m_SourceImage.getClippedImage(juce::Rectangle<int>(xcrop, ycrop, wcrop, hcrop));

  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
juce::String OsmLayer::LoadTile(int x, int y, int zoomlevel)
{
  juce::String filename = m_Cache.getFullPathName() + juce::File::getSeparatorString() + 
                          juce::String(zoomlevel) + "_" + juce::String(x) + "_" + juce::String(y) + "." + m_strFormat;
  juce::File cache(filename);
  if (cache.existsAsFile()) // Le fichier a deja ete telecharge
    return filename;

  // Telechargement du fichier
  m_strRequest = "https://" + m_strServer + "/" + juce::String(zoomlevel) + "/" + juce::String(x) + "/" + juce::String(y) + "." + m_strFormat;
  juce::URL url(m_strRequest);
  juce::URL::DownloadTaskOptions options;
  std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(filename, options);
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
juce::Image& OsmLayer::GetAreaImage(const XFrame& F, double gsd)
{
  if ((F == m_LastFrame) && (gsd == m_LastGsd))
    return m_ProjImage;
  m_LastFrame = F;
  m_LastGsd = gsd;
  uint32_t wout = F.Width() / gsd;
  uint32_t hout = F.Height() / gsd;

  // Caclul du niveau de zoom OSM
  XGeoPref pref;
  double S, longitude, latitude;
  int osm_zoom = m_nMaxZoom;
  pref.Convert(pref.Projection(), XGeoProjection::RGF93, F.Center().X, F.Center().Y, longitude, latitude);

  for (int zoom = 0; zoom <= m_nMaxZoom; zoom++) {
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
  FwebMerc.Xmin = XMin(x0, x1);
  FwebMerc.Xmax = XMax(x2, x3);
  FwebMerc.Ymin = XMin(y0, y3);
  FwebMerc.Ymax = XMax(y1, y2);

  LoadFrame(FwebMerc, osm_zoom);
  m_SourceImage = m_SourceImage.rescaled(FwebMerc.Width() / gsd, FwebMerc.Height() / gsd);

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
// Attributs de l'objet OsmLayer
//-----------------------------------------------------------------------------
bool OsmLayer::ReadAttributes(std::vector<std::string>& V)
{
  V.clear();
  V.push_back("Server"); V.push_back(m_strServer.toStdString());
  V.push_back("Url"); V.push_back(m_strRequest.toStdString());
  V.push_back("TileW"); V.push_back(juce::String(m_nTileW).toStdString());
  V.push_back("TileH"); V.push_back(juce::String(m_nTileH).toStdString());
  V.push_back("GSD"); V.push_back(juce::String(m_LastGsd).toStdString());
  return true;
}