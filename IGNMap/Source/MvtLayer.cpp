//-----------------------------------------------------------------------------
//								MvtLayer.cpp
//								============
//
// Gestion des flux de donnees Mapbox Vector Tile
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/06/2025
//-----------------------------------------------------------------------------

#include "MvtLayer.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolGeod/XTransfoGeod.h"
#include "vtzero/vector_tile.hpp"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
MvtLayer::MvtLayer(std::string server, std::string format, uint32_t tileW, uint32_t tileH, uint32_t max_zoom)
{
  m_strServer = server;
  m_strFormat = format;

  m_nTileW = tileW;
  m_nTileH = tileH;
  m_nMaxZoom = max_zoom;

  CreateCacheDir("MVT");
}

//-----------------------------------------------------------------------------
// Chargement d'un cadre en WebMercator a un zoom donne
//-----------------------------------------------------------------------------
bool MvtLayer::LoadFrame(const XFrame& F, int zoomlevel)
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
      juce::Image image = LoadMvt(filename);
      if (!image.isValid()) {
        juce::File badFile(filename); // Le fichier est peut etre corrompu
        badFile.deleteFile();
        continue;
      }
      g.drawImageAt(image, j * m_nTileW, i * m_nTileH);
    }
  }

  double resol = Resol(zoomlevel);
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
juce::String MvtLayer::LoadTile(int x, int y, int zoomlevel)
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
/*
juce::Image& MvtLayer::GetAreaImage(const XFrame& F, double gsd)
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
    S = Resol(zoom) * cos(latitude);  // GSD a l'Equateur x cos(latitude)
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
*/
juce::Image& MvtLayer::GetAreaImage(const XFrame& F, double gsd)
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
    S = Resol(zoom) * cos(latitude);  // GSD a l'Equateur x cos(latitude)
    if (S < gsd + gsd * 0.1) {
      osm_zoom = zoom;
      break;
    }
  }
  m_nLastZoom = osm_zoom;
  m_ProjImage = juce::Image(juce::Image::ARGB, (int)(F.Width() / gsd), (int)(F.Height() / gsd), true);

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

  LoadFrameProj(FwebMerc, osm_zoom);

  return m_ProjImage;
}

//-----------------------------------------------------------------------------
// Attributs de l'objet OsmLayer
//-----------------------------------------------------------------------------
bool MvtLayer::ReadAttributes(std::vector<std::string>& V)
{
  V.clear();
  V.push_back("Server"); V.push_back(m_strServer.toStdString());
  V.push_back("Url"); V.push_back(m_strRequest.toStdString());
  V.push_back("TileW"); V.push_back(juce::String(m_nTileW).toStdString());
  V.push_back("TileH"); V.push_back(juce::String(m_nTileH).toStdString());
  V.push_back("GSD"); V.push_back(juce::String(m_LastGsd).toStdString());
  V.push_back("Zoom level"); V.push_back(juce::String(m_nLastZoom).toStdString());
  return true;
}

//-----------------------------------------------------------------------------
// Structure pour lire les geometries
//-----------------------------------------------------------------------------
struct MvtLayer::geom_handler {

  std::vector<double> points;
  std::vector<int> parts;

  void points_begin(uint32_t count) {
    points.reserve(2 * count);
    parts.push_back(count);
  }

  void points_point(vtzero::point point) noexcept {
    points.push_back(point.x);
    points.push_back(point.y);
  }

  void points_end() const noexcept {
  }

  void ring_begin(uint32_t count) {
    points.reserve(2 * count);
    parts.push_back(count);
  }

  void ring_point(vtzero::point point) noexcept {
    points.push_back(point.x);
    points.push_back(point.y);
  }

  void ring_end(vtzero::ring_type) const noexcept {
  }

  void linestring_begin(uint32_t count) {
    points.reserve(2 * count);
    parts.push_back(count);
  }

  void linestring_point(vtzero::point point) noexcept {
    points.push_back(point.x);
    points.push_back(point.y);
  }

  void linestring_end() const noexcept {
  }

};

//-----------------------------------------------------------------------------
// Lecture d'une Mapbox Vector Tile
//-----------------------------------------------------------------------------
juce::Image MvtLayer::LoadMvt(juce::String filename)
{
  std::ifstream in;
  in.open(filename.toStdString(), std::ios::binary);
  if (!in.is_open())
    return juce::Image(); // Image nulle = invalide

  // get length of file:
  in.seekg(0, in.end);
  std::streampos length = in.tellg();
  in.seekg(0, in.beg);
  if (length < 1)
    return juce::Image(); // Image nulle = invalide

  char* buffer = new(std::nothrow) char[length];
  if (buffer == nullptr)
    return juce::Image(); // Image nulle = invalide
  in.read(buffer, length);

  if (!in) {  // Lecture incorrecte
    delete[] buffer;
    return juce::Image(); // Image nulle = invalide
  }
  in.close();

  // Creation de l'image vide
  juce::Image image(juce::Image::ARGB, m_nTileW, m_nTileH, true);
  juce::Graphics g(image);
  g.setColour(juce::Colours::black);
  g.drawRect(0, 0, m_nTileW, m_nTileH, 1);
  {
    vtzero::vector_tile tile(buffer, length);
    while (auto layer = tile.next_layer()) {
      //std::cout << "*********" << layer.name().to_string() << "*********" << std::endl;
      float factor = layer.extent() / m_nTileW;
      while (auto feature = layer.next_feature()) {
        /*
        while (auto property = feature.next_property()) {
          std::cout << property.key().to_string() << "\t:\t";
          switch ((int)property.value().type()) {
          case 1: std::cout << property.value().string_value().to_string() << std::endl; break;	// String
          case 2: std::cout << property.value().float_value() << std::endl; break;	// Float
          case 3: std::cout << property.value().double_value() << std::endl; break;	// Double
          case 4: std::cout << property.value().int_value() << std::endl; break;	// Int
          case 5: std::cout << property.value().uint_value() << std::endl; break;	// Uint
          case 6: std::cout << property.value().sint_value() << std::endl; break;	// Sint
          case 7: std::cout << property.value().bool_value() << std::endl; break;	// bool
          default: std::cout << "Unknown type";
          }
        }
        */
        vtzero::geometry geom = feature.geometry();
        if (geom.type() == vtzero::GeomType::UNKNOWN)
          continue;

        geom_handler handler;
        vtzero::decode_geometry(geom, handler);
        juce::Path path;
        int index = 0;
        float X = 0.f, Y = 0.f, d = 1.f;

        // Dessin des points et multi-points
        if (geom.type() == vtzero::GeomType::POINT) {
          g.setColour(juce::Colours::green);
          for (int i = 0; i < handler.points.size() / 2; i++) {
            X = (float)(handler.points[2 * i] / factor);
            Y = (float)(handler.points[2 * i + 1] / factor);
            //path.startNewSubPath(X, Y);
            //path.addEllipse(X - d, Y - d, 2 * d, 2 * d);
            g.drawEllipse(X - d, Y - d, 2 * d, 2 * d, 1.0f);
          }
          continue;
        }

        // Dessin des polylignes et des polygones
        for (int parts = 0; parts < handler.parts.size(); parts++) {
          X = (float)(handler.points[2 * index] / factor);
          Y = (float)(handler.points[2 * index + 1] / factor);
          path.startNewSubPath(X, Y);
          for (int i = 1; i < handler.parts[parts]; i++) {
            index++;
            X = (float)(handler.points[2 * index] / factor);
            Y = (float)(handler.points[2 * index + 1] / factor);
            path.lineTo(X, Y);
          }
          if (geom.type() == vtzero::GeomType::POLYGON)
            path.closeSubPath();
        }

        if (geom.type() == vtzero::GeomType::LINESTRING) {
          g.setColour(juce::Colours::blue);
          g.strokePath(path, juce::PathStrokeType(2.f, juce::PathStrokeType::beveled));
          path.clear();
          continue;
        }

        if (geom.type() == vtzero::GeomType::POLYGON) {
          g.setColour(juce::Colours::blue);
          g.strokePath(path, juce::PathStrokeType(2.f, juce::PathStrokeType::beveled));
          g.setFillType(juce::FillType(juce::Colours::red));
          g.fillPath(path);
          path.clear();
          continue;
        }

      } // Feature
    } // Layer
  }

  delete[] buffer;

  return image;
}

//-----------------------------------------------------------------------------
// Lecture d'une Mapbox Vector Tile et ecriture directe dans l'image finale
//-----------------------------------------------------------------------------
bool MvtLayer::LoadMvt(juce::String filename, double X0, double Y0, double GSD0)
{
  if (!m_ProjImage.isValid())
    return false;
  std::ifstream in;
  in.open(filename.toStdString(), std::ios::binary);
  if (!in.is_open())
    return false;

  // get length of file:
  in.seekg(0, in.end);
  std::streampos length = in.tellg();
  in.seekg(0, in.beg);
  if (length < 1)
    return false;

  char* buffer = new(std::nothrow) char[length];
  if (buffer == nullptr)
    return false;
  in.read(buffer, length);

  if (!in) {  // Lecture incorrecte
    delete[] buffer;
    return false;
  }
  in.close();

  XGeoPref pref;
  juce::Graphics g(m_ProjImage);
  g.excludeClipRegion(juce::Rectangle<int>(0, 0, m_ProjImage.getWidth(), m_ProjImage.getHeight()));
  {
    vtzero::vector_tile tile(buffer, length);
    while (auto layer = tile.next_layer()) {
      //std::cout << "*********" << layer.name().to_string() << "*********" << std::endl;
      float factor = layer.extent() / m_nTileW;
      while (auto feature = layer.next_feature()) {
        /*
        while (auto property = feature.next_property()) {
          std::cout << property.key().to_string() << "\t:\t";
          switch ((int)property.value().type()) {
          case 1: std::cout << property.value().string_value().to_string() << std::endl; break;	// String
          case 2: std::cout << property.value().float_value() << std::endl; break;	// Float
          case 3: std::cout << property.value().double_value() << std::endl; break;	// Double
          case 4: std::cout << property.value().int_value() << std::endl; break;	// Int
          case 5: std::cout << property.value().uint_value() << std::endl; break;	// Uint
          case 6: std::cout << property.value().sint_value() << std::endl; break;	// Sint
          case 7: std::cout << property.value().bool_value() << std::endl; break;	// bool
          default: std::cout << "Unknown type";
          }
        }
        */
        vtzero::geometry geom = feature.geometry();
        if (geom.type() == vtzero::GeomType::UNKNOWN)
          continue;

        geom_handler handler;
        vtzero::decode_geometry(geom, handler);
    
        double Xima = 0., Yima = 0., Xter = 0., Yter = 0., d = 2.;

        // Dessin des points et multi-points
        if (geom.type() == vtzero::GeomType::POINT) {
          g.setColour(juce::Colours::green);
          for (int i = 0; i < handler.points.size() / 2; i++) {
            Xima = X0 + (handler.points[2 * i] / factor) * GSD0;
            Yima = Y0 - (handler.points[2 * i + 1] / factor) * GSD0;
            pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
            if (!m_LastFrame.IsIn(XPt2D(Xter, Yter)))
              continue;
            Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
            Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;
            //path.startNewSubPath(X, Y);
            //path.addEllipse(X - d, Y - d, 2 * d, 2 * d);
            g.drawEllipse(Xter - d, Yter - d, 2 * d, 2 * d, 2.0f);
          }
          continue;
        }

        juce::Path path;
        path.preallocateSpace((handler.points.size() * 3) / 2);
        int index = 0;

        // Dessin des polylignes et des polygones
        for (int parts = 0; parts < handler.parts.size(); parts++) {
          Xima = X0 + (handler.points[2 * index] / factor) * GSD0;
          Yima = Y0 - (handler.points[2 * index + 1] / factor) * GSD0;
          pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
          Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
          Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;
          path.startNewSubPath(Xter, Yter);
          for (int i = 1; i < handler.parts[parts]; i++) {
            index++;
            Xima = X0 + (handler.points[2 * index] / factor) * GSD0;
            Yima = Y0 - (handler.points[2 * index + 1] / factor) * GSD0;
            pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
            Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
            Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;
            path.lineTo(Xter, Yter);
          }
          if (geom.type() == vtzero::GeomType::POLYGON)
            path.closeSubPath();
          index++;
        }

        if (geom.type() == vtzero::GeomType::LINESTRING) {
          g.setColour(juce::Colours::blue);
          g.strokePath(path, juce::PathStrokeType(2.f, juce::PathStrokeType::beveled));
          continue;
        }

        if (geom.type() == vtzero::GeomType::POLYGON) {
          g.setColour(juce::Colours::blue);
          g.strokePath(path, juce::PathStrokeType(2.f, juce::PathStrokeType::beveled));
          g.setFillType(juce::FillType(juce::Colours::red));
          g.fillPath(path);
          continue;
        }

      } // Feature
    } // Layer
  }

  delete[] buffer;

  return true;
}

bool MvtLayer::LoadFrameProj(const XFrame& F, int zoomlevel)
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

  for (int i = 0; i < nb_tiley; i++) {
    for (int j = 0; j < nb_tilex; j++) {
      int x = firstX + j;
      int y = firstY + i;
      double GSD0 = Resol(zoomlevel);
      double X0 = x * Swath(zoomlevel) - XPI * a;
      double Y0 = XPI * a - y * Swath(zoomlevel);
      juce::String filename = LoadTile(x, y, zoomlevel);
      bool flag = LoadMvt(filename, X0, Y0, GSD0);
      if (!flag) {
        juce::File badFile(filename); // Le fichier est peut etre corrompu
        badFile.deleteFile();
        continue;
      }
    }
  }

  return true;
}