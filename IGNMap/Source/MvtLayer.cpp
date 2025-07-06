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


void MvtTile::Clear()
{ 
  if (m_Tile != nullptr)
    delete m_Tile;
  m_Tile = nullptr;
  if (m_Buffer != nullptr) 
    delete[] m_Buffer; 
  m_Buffer = nullptr;
  m_bLoaded = false; 
  m_dX0 = m_dY0 = m_dGsd = 0.;
  m_nLength = 0;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
bool MvtTile::Load(juce::String filename, double X0, double Y0, double GSD0)
{
  Clear();
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

  m_Buffer = new(std::nothrow) char[length];
  if (m_Buffer == nullptr)
    return false;
  in.read(m_Buffer, length);

  if (!in) {  // Lecture incorrecte
    Clear();
    return false;
  }
  in.close();

  m_dX0 = X0;
  m_dY0 = Y0;
  m_dGsd = GSD0;
  m_bLoaded = true;
  m_nLength = length;

  m_Tile = new vtzero::vector_tile(m_Buffer, m_nLength);
  return true;
}

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
  m_nLastZoom = 0;

  m_LineWidth = 1.f;
  m_Repres = false;

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

  //LoadFrameProj(FwebMerc, osm_zoom);
  DrawWithStyle(FwebMerc, osm_zoom);

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
  juce::Colour pen, fill;
  float line_width = 1.f;
  juce::String text_field;
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

        pen = juce::Colours::red;
        fill = juce::Colours::transparentWhite;
        line_width = 1.f;
        if (!FindStyle(layer.name().to_string(), &feature, &pen, &fill, &line_width, &text_field))
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
            if (text_field.isEmpty()) {
              g.setFillType(juce::FillType(fill));
              g.drawEllipse(Xter - d, Yter - d, 2 * d, 2 * d, 2.0f);
            }
            else {
              g.drawSingleLineText(text_field, Xter, Yter);
            }
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
          //line_width = 2.f;
          g.setColour(pen);
          g.strokePath(path, juce::PathStrokeType(line_width, juce::PathStrokeType::beveled));
          continue;
        }

        if (geom.type() == vtzero::GeomType::POLYGON) {
          //line_width = 2.f;
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
bool MvtLayer::FindStyle(juce::String layername, vtzero::feature* feature, juce::Colour* pen, juce::Colour* fill, float* line_width, juce::String* text)
{
  bool found = false;
  *text = "";
  //if (layername != "bati_surf")
  //  return false;
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
      if (m_nLastZoom < (int)layer["minzoom"])
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

    //if (!found)
    //  continue;

    // Lecture des informations PAINT
    if (!layer.hasProperty("paint"))
      continue;
    juce::var paint = layer["paint"];
    juce::String val;
    if (paint.hasProperty("line-color")) {
      if (paint["line-color"].hasProperty("stops")) {
        juce::var stops = paint["line-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *pen = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        *pen = juce::Colour::fromString(paint["line-color"].toString());
      }
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("fill-outline-color")) {
      *pen = juce::Colour::fromString(paint["fill-outline-color"].toString());
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("line-width")) {
      if (paint["line-width"].hasProperty("stops")) {
        juce::var stops = paint["line-width"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *line_width = stops[s][1].toString().getFloatValue();
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else
        *line_width = paint["line_width"].toString().getFloatValue();
    }
    if (paint.hasProperty("fill-color")) {
      if (paint["fill-color"].hasProperty("stops")) {
        juce::var stops = paint["fill-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *fill = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        *fill = juce::Colour::fromString(paint["fill-color"].toString());
      }
      *fill = fill->withAlpha(1.f);
    }
    if (paint.hasProperty("fill-opacity")) {
      *fill = fill->withAlpha(paint["fill-opacity"].toString().getFloatValue());
    }

    // Lecture des informations LAYOUT
    if (layer.hasProperty("layout")) {
      juce::var layout = layer["layout"];
      if (layout.hasProperty("text-field")) {
        std::string field = layout["text-field"].toString().toStdString();
        feature->reset_property();
        while (auto property = feature->next_property()) {
          std::string key = property.key().to_string();
          key = "{" + key + "}";
          if (key == field) {
            if ((int)property.value().type() == 1) {
              *text = property.value().string_value().to_string();
            }
            break;
          }
        }
      }
      if (paint.hasProperty("text-color")) {
        *pen = juce::Colour::fromString(paint["text-color"].toString());
        *pen = pen->withAlpha(1.f);
      }

    }



    return true;
  }
  return false;
}


bool MvtLayer::DrawWithStyle(const XFrame& F, int zoomlevel)
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

  std::vector<MvtTile> T;
  T.resize(nb_tiley * nb_tilex);

  // Lecture des styles dans l'ordre
  for (int cmpt = 0; cmpt < m_StyleLayers.size(); cmpt++) {
    juce::var layer = m_StyleLayers[cmpt];
    // Niveaux de zoom
    if (layer.hasProperty("minzoom"))
      if (zoomlevel < (int)layer["minzoom"])
        continue;
    if (layer.hasProperty("maxzoom"))
      if (zoomlevel > (int)layer["maxzoom"])
        continue;

    // Source
    if (!layer.hasProperty("source-layer"))
      continue;
    m_Repres = false; // On reinitialise les stylos et pinceaux
    
    // Lecture des tiles
    int index = 0;
    for (int i = 0; i < nb_tiley; i++) {
      for (int j = 0; j < nb_tilex; j++) {
        int x = firstX + j;
        int y = firstY + i;
        double GSD0 = Resol(zoomlevel);
        double X0 = x * Swath(zoomlevel) - XPI * a;
        double Y0 = XPI * a - y * Swath(zoomlevel);
        if (!T[index].IsLoaded()) {
          juce::String filename = LoadTile(x, y, zoomlevel);
          bool flag = T[index].Load(filename, X0, Y0, GSD0);
          if (!flag) {
            juce::File badFile(filename); // Le fichier est peut etre corrompu
            badFile.deleteFile();
            continue;
          }
        }
        LoadMvt(&T[index], X0, Y0, GSD0, layer);
        index++;
      }
    }

  } // endfor cmpt

  T.clear();
  return true;
}



bool MvtLayer::LoadMvt(MvtTile* T, double X0, double Y0, double GSD0, juce::var& style)
{
  //if (!m_ProjImage.isValid())
  //  return false;

  XGeoPref pref;
  juce::Graphics g(m_ProjImage);
  //g.excludeClipRegion(juce::Rectangle<int>(0, 0, m_ProjImage.getWidth(), m_ProjImage.getHeight()));
  //g.setOpacity(1.f);

  std::string source_layer = style["source-layer"].toString().toStdString();
  
  //vtzero::vector_tile tile(T.Buffer(), T.Length());
  //vtzero::vector_tile* tile = T.Tile();
  //tile->reset_layer();
  //vtzero::vector_tile tile = *(T.Tile());
  
  while (auto layer = T->Tile()->next_layer()) {
    if (source_layer != layer.name().to_string())
      continue;
    float factor = layer.extent() / m_nTileW;
    bool paint_init = false;
    juce::Colour pen, fill;
    float line_width = 1.f;
    juce::String text_field;
    juce::Path path;

    while (auto feature = layer.next_feature()) {

      // Filtrage
      if (style.hasProperty("filter")) {
        juce::var filter = style["filter"];
        if (filter.size() >= 3) {
          feature.reset_property();

          if (filter[0].toString() == "==") {
            bool filtering = false;
            while (auto property = feature.next_property()) {
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
            while (auto property = feature.next_property()) {
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
        }
      }

      // Lecture des couleurs
      /*
      if (!paint_init) {
        if (ReadStylePaint(style, &pen, &fill, &line_width, &text_field))
          paint_init = true;
        else
          continue;
      }
      */
      if (!m_Repres) 
        ReadStylePaint(style);

       g.setColour(m_PenColor);
       g.setFillType(juce::FillType(m_FillColor));
     

      // Lecture de la geometrie
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
          //g.setColour(pen);
          if (text_field.isEmpty()) {
            g.setFillType(juce::FillType(fill));
            g.drawEllipse(Xter - d, Yter - d, 2 * d, 2 * d, 2.0f);
          }
          else {
            g.drawSingleLineText(text_field, Xter, Yter);
          }
        }
        continue;
      }

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
        //g.setColour(pen);
        g.strokePath(path, juce::PathStrokeType(m_LineWidth, juce::PathStrokeType::beveled));
        continue;
      }

      if (geom.type() == vtzero::GeomType::POLYGON) {
        //g.setColour(pen);
        //g.setFillType(juce::FillType(fill));
        g.strokePath(path, juce::PathStrokeType(m_LineWidth, juce::PathStrokeType::beveled));
        g.fillPath(path);
        continue;
      }

    } // feature
    return true;
  } // layer
  T->Tile()->reset_layer();

  return false;
}


  bool MvtLayer::ReadStylePaint(juce::var& layer, juce::Colour* pen, juce::Colour* fill, float* line_width, juce::String* text) const
  {
    // Lecture des informations PAINT
    if (!layer.hasProperty("paint"))
      return false;
    juce::var paint = layer["paint"];
    juce::String val;
    if (paint.hasProperty("line-color")) {
      if (paint["line-color"].hasProperty("stops")) {
        juce::var stops = paint["line-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *pen = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        *pen = juce::Colour::fromString(paint["line-color"].toString());
      }
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("fill-outline-color")) {
      *pen = juce::Colour::fromString(paint["fill-outline-color"].toString());
      *pen = pen->withAlpha(1.f);
    }

    if (paint.hasProperty("line-width")) {
      if (paint["line-width"].hasProperty("stops")) {
        juce::var stops = paint["line-width"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *line_width = stops[s][1].toString().getFloatValue();
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else
        *line_width = paint["line_width"].toString().getFloatValue();
    }
    if (paint.hasProperty("fill-color")) {
      if (paint["fill-color"].hasProperty("stops")) {
        juce::var stops = paint["fill-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            *fill = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        *fill = juce::Colour::fromString(paint["fill-color"].toString());
      }
      *fill = fill->withAlpha(1.f);
    }
    if (paint.hasProperty("fill-opacity")) {
      *fill = fill->withAlpha(paint["fill-opacity"].toString().getFloatValue());
    }

    return true;
  }


  bool MvtLayer::ReadStylePaint(juce::var& layer)
  {
    // Lecture des informations PAINT
    if (!layer.hasProperty("paint"))
      return false;
    juce::var paint = layer["paint"];
    juce::String val;
    if (paint.hasProperty("line-color")) {
      if (paint["line-color"].hasProperty("stops")) {
        juce::var stops = paint["line-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            m_PenColor = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        m_PenColor = juce::Colour::fromString(paint["line-color"].toString());
      }
      m_PenColor = m_PenColor.withAlpha(1.f);
    }

    if (paint.hasProperty("fill-outline-color")) {
      m_PenColor = juce::Colour::fromString(paint["fill-outline-color"].toString());
      m_PenColor = m_PenColor.withAlpha(1.f);
    }

    if (paint.hasProperty("line-width")) {
      if (paint["line-width"].hasProperty("stops")) {
        juce::var stops = paint["line-width"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            m_LineWidth = stops[s][1].toString().getFloatValue();
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else
        m_LineWidth = paint["line_width"].toString().getFloatValue();
    }
    if (paint.hasProperty("fill-color")) {
      if (paint["fill-color"].hasProperty("stops")) {
        juce::var stops = paint["fill-color"]["stops"];
        for (int s = 0; s < stops.size(); s++) {
          if (stops[s].size() == 2) {
            m_FillColor = juce::Colour::fromString(stops[s][1].toString());
            if (m_nLastZoom <= (int)stops[s][0])
              break;
          }
        }
      }
      else {
        m_FillColor = juce::Colour::fromString(paint["fill-color"].toString());
      }
      m_FillColor = m_FillColor.withAlpha(1.f);
    }
    if (paint.hasProperty("fill-opacity")) {
      m_FillColor = m_FillColor.withAlpha(paint["fill-opacity"].toString().getFloatValue());
    }
    m_Repres = true;

    return true;
  }