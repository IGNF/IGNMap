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
  m_ProjImage = juce::Image(juce::Image::ARGB, (int)wout, (int)hout, true);

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
  g.setOpacity(1.f);
  {
    vtzero::vector_tile tile(buffer, length);
    while (auto layer = tile.next_layer()) {
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
        juce::Colour pen, fill;
        float line_width = 1.f;
        if (!FindStyle(layer.name().to_string(), &feature, &pen, &fill, &line_width))
          continue;

        vtzero::geometry geom = feature.geometry();
        if (geom.type() == vtzero::GeomType::UNKNOWN)
          continue;

        geom_handler handler;
        vtzero::decode_geometry(geom, handler);
    
        double Xima = 0., Yima = 0., Xter = 0., Yter = 0., d = 2.;

        // Dessin des points et multi-points
        if (geom.type() == vtzero::GeomType::POINT) {
          for (int i = 0; i < handler.points.size() / 2; i++) {
            Xima = X0 + (handler.points[2 * i] / factor) * GSD0;
            Yima = Y0 - (handler.points[2 * i + 1] / factor) * GSD0;
            pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
            if (!m_LastFrame.IsIn(XPt2D(Xter, Yter)))
              continue;
            Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
            Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;
            g.setColour(pen);
            g.setFillType(juce::FillType(fill));
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
          line_width = 2.f;
          g.setColour(pen);
          g.strokePath(path, juce::PathStrokeType(line_width, juce::PathStrokeType::beveled));
          continue;
        }

        if (geom.type() == vtzero::GeomType::POLYGON) {
          line_width = 2.f;
          g.setColour(pen);
          g.setFillType(juce::FillType(fill));
          g.strokePath(path, juce::PathStrokeType(line_width, juce::PathStrokeType::beveled));
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

//-----------------------------------------------------------------------------
// Lecture d'un fichier de style JSON
//-----------------------------------------------------------------------------
bool MvtLayer::LoadStyle(juce::String server)
{
  juce::URL url(server);
  juce::WebInputStream web(url, false);
  juce::String content = web.readEntireStreamAsString();

  juce::var parsedJSON = juce::JSON::parse(content);

  if (!parsedJSON.hasProperty("layers"))
    return false;
  m_StyleLayers = parsedJSON["layers"];
  if (m_StyleLayers.size() < 1)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Recherche d'un style
//-----------------------------------------------------------------------------
bool MvtLayer::FindStyle(juce::String layername, vtzero::feature* feature, juce::Colour* pen, juce::Colour* fill, float* line_width)
{
  bool found = false;
  for (int i = 0; i < m_StyleLayers.size(); i++) {
    found = false;
    juce::var layer = m_StyleLayers[i];
    if (!layer.hasProperty("source-layer"))
      continue;
    juce::String source_layer = layer["source-layer"].toString();
    if (source_layer != layername)
      continue;
    // Niveaux de zoom
    if (layer.hasProperty("minzoom"))
      if (m_nLastZoom <= (int)layer["minzoom"])
        continue;
    if (layer.hasProperty("maxzoom"))
      if (m_nLastZoom > (int)layer["maxzoom"])
        continue;

    // Filtrage
    if (layer.hasProperty("filter")) {
      juce::var filter = layer["filter"];
      if (!filter.isArray())
        continue;
      if (filter.size() < 3)
        continue;
      feature->reset_property();

      if (filter[0].toString() == "==") {
        bool filtering = false;
        while (auto property = feature->next_property()) {
          if (property.key().to_string() == filter[1].toString()) {
            if ((int)property.value().type() == 1) {
              if (property.value().string_value().to_string() == filter[2].toString())
                filtering = true;
            }
            break;
          }
        } //end while
        if (!filtering)
          continue;
      }

      if (filter[0].toString() == "in") {
        bool filtering = false;
        while (auto property = feature->next_property()) {
          if (property.key().to_string() == filter[1].toString()) {
            if ((int)property.value().type() == 1) {
              std::string val = property.value().string_value().to_string();
              for (int k = 2; k < filter.size(); k++) {
                if (val == filter[k].toString())
                  filtering = true;
              }
            }
            break;
          }
        } //end while
        if (!filtering)
          continue;
      }
      found = true; // Si on arrive ici, c'est que l'objet correspond
    }
    if (!found)
      continue;

    if (!layer.hasProperty("paint"))
      continue;
    juce::var paint = layer["paint"];
    juce::String val;
    if (paint.hasProperty("line-color")) {
      if (paint["line-color"].hasProperty("stops")) {
        juce::var stops = paint["line-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            if (m_nLastZoom >= (int)stops[s][0])
              *pen = juce::Colour::fromString(stops[s][1].toString());
          }
        }
      }
      else {
        *pen = juce::Colour::fromString(paint["line-color"].toString());
      }
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("fill-outline-color")) {
      val = paint["fill-outline-color"].toString();
      *pen = juce::Colour::fromString(paint["fill-outline-color"].toString());
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("line-width")) {
      if (paint["line-width"].hasProperty("stops")) {
        juce::var stops = paint["line-width"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            if (m_nLastZoom >= (int)stops[s][0])
              *line_width = stops[s][1].toString().getFloatValue();
          }
        }
      }
      else
        *line_width = paint["line_width"].toString().getFloatValue();
    }
    if (paint.hasProperty("fill-color")) {
      val = paint["fill-color"].toString();
      *fill = juce::Colour::fromString(paint["fill-color"].toString());
      *fill = fill->withAlpha(1.f);
    }
    if (paint.hasProperty("fill-opacity")) {
      val = paint["fill-opacity"].toString();
      *fill = fill->withAlpha(paint["fill-opacity"].toString().getFloatValue());
    }
    return true;
  }
  return false;
}