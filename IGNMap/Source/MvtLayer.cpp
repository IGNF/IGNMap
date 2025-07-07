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
// Reinitialisation d'une dalle
//-----------------------------------------------------------------------------
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
  m_bPaintProperties = false;

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
    if (S < gsd + gsd * 0.8) {
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

  m_Layer.clear();
  for (int cmpt = 0; cmpt < m_StyleLayers.size(); cmpt++) {
    juce::var layer = m_StyleLayers[cmpt];
    MvtStyleLayer styleLayer;
    if (styleLayer.Read(layer))
      m_Layer.push_back(styleLayer);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Affichage dans l'ordre du fichier de style
//-----------------------------------------------------------------------------
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
  for (int cmpt = 0; cmpt < m_Layer.size(); cmpt++) {
    //if (cmpt != 119)  // Bati Quelconque
    //  continue;
    MvtStyleLayer layer = m_Layer[cmpt];
    // Niveaux de zoom
    if (!layer.TestZoomLevel(zoomlevel))
      continue;

    m_bPaintProperties = false; // On reinitialise les stylos et pinceaux

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
        LoadMvt(&T[index], X0, Y0, GSD0, &layer);
        index++;
      }
    }

  } // endfor cmpt

  T.clear();
  return true;
}

//-----------------------------------------------------------------------------
// Affichage d'une tile en fonction d'un style
//-----------------------------------------------------------------------------
bool MvtLayer::LoadMvt(MvtTile* T, double X0, double Y0, double GSD0, MvtStyleLayer* style)
{
  XGeoPref pref;
  juce::Graphics g(m_ProjImage);
  g.excludeClipRegion(juce::Rectangle<int>(0, 0, m_ProjImage.getWidth(), m_ProjImage.getHeight()));
  g.setOpacity(1.f);

  //if (style->Type() != MvtStyleLayer::line)
  //  return true;

  // Emprise de la dalle dans l'image
  /*
  double x0, y0, x1, y1, x2, y2, x3, y3;
  pref.Convert(XGeoProjection::WebMercator, pref.Projection(), X0, Y0, x0, y0);
  pref.Convert(XGeoProjection::WebMercator, pref.Projection(), X0 + m_nTileW * GSD0, Y0, x1, y1);
  pref.Convert(XGeoProjection::WebMercator, pref.Projection(), X0 + m_nTileW * GSD0, Y0 - m_nTileH * GSD0, x2, y2);
  pref.Convert(XGeoProjection::WebMercator, pref.Projection(), X0, Y0 - m_nTileH * GSD0, x3, y3);
  double xmin = XMin(x0, x3);
  double xmax = XMax(x1, x2);
  double ymin = XMin(y2, y3);
  double ymax = XMax(y0, y1);
  xmin = (xmin - m_LastFrame.Xmin) / m_LastGsd;
  xmax = (xmax - m_LastFrame.Xmin) / m_LastGsd;
  ymin = (m_LastFrame.Ymax - ymin) / m_LastGsd;
  ymax = (m_LastFrame.Ymax - ymax) / m_LastGsd;
  g.excludeClipRegion(juce::Rectangle<int>(xmin, ymax, xmax - xmin, ymax - ymin));
  */


  auto layer = T->Tile()->get_layer_by_name(style->SourceLayer().toStdString());
  if (layer.empty())
    return false;


  float factor = layer.extent() / m_nTileW;
  
  float line_width = 1.f;
  juce::String text_field = style->TextField();
  juce::Path path;

  juce::Colour pen;
  juce::FillType fillType;

  style->SetStyle((int)m_nLastZoom, &pen, &fillType, &line_width);
  g.setColour(pen);
  if (style->Type() == MvtStyleLayer::fill)
    g.setFillType(fillType);

  while (auto feature = layer.next_feature()) {

		bool filtering = false;
		switch (style->FilterType()) {
		case MvtStyleLayer::equal:
		case MvtStyleLayer::in:
			while (auto property = feature.next_property()) {
				if (property.key().to_string() == style->FilterAtt()) {
					if ((int)property.value().type() == 1)
						filtering = style->TestAtt(property.value().string_value().to_string());
					break;
				}
			}
			break;
		default: filtering = true;
		}

		if (!filtering)
			continue;

   // Lecture de la geometrie
    vtzero::geometry geom = feature.geometry();
    if (geom.type() == vtzero::GeomType::UNKNOWN)
      continue;

    geom_handler handler;
    vtzero::decode_geometry(geom, handler);

    double Xima = 0., Yima = 0., Xter = 0., Yter = 0., d = 2.;

    // Dessin des points et multi-points
    if ((geom.type() == vtzero::GeomType::POINT)&&(style->Type() == MvtStyleLayer::symbol)) {
      for (int i = 0; i < handler.points.size() / 2; i++) {
        Xima = X0 + (handler.points[2 * i] / factor) * GSD0;
        Yima = Y0 - (handler.points[2 * i + 1] / factor) * GSD0;
        pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
        if (!m_LastFrame.IsIn(XPt2D(Xter, Yter)))
          continue;
        Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
        Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;

        if (text_field.isEmpty()) {
          g.drawEllipse(Xter - d, Yter - d, 2 * d, 2 * d, 2.0f);
        }
        else {
          while (auto property = feature.next_property()) {
            if (property.key().to_string() == text_field) {
              if ((int)property.value().type() == 1)
                g.drawSingleLineText(property.value().string_value().to_string(), Xter, Yter);
              break;
            }
          }
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

    //if (geom.type() == vtzero::GeomType::LINESTRING) {
    if (style->Type() == MvtStyleLayer::line) {
      g.strokePath(path, juce::PathStrokeType(line_width, juce::PathStrokeType::beveled));
      continue;
    }

    if ((geom.type() == vtzero::GeomType::POLYGON)&& (style->Type() == MvtStyleLayer::fill)) {
      g.setColour(pen);
      g.strokePath(path, juce::PathStrokeType(line_width, juce::PathStrokeType::beveled));
      g.setFillType(fillType);
      g.fillPath(path);
      continue;
    }

  } // feature

  return true;

}

//-----------------------------------------------------------------------------
// Nettoyage d'un style
//-----------------------------------------------------------------------------
	void MvtStyleLayer::Clear()
	{
		m_Type = noType;
		m_SourceLayer = "";
		m_MinZoom = m_MaxZoom = -1;
		m_Visibility = false;
		m_LineColor.clear();
		m_LineStops.clear();
		m_TextColor.clear();
		m_TextStops.clear();
		m_OutlineColor.clear();
		m_OutlineStops.clear();
		m_FillType.clear();
		m_FillStops.clear();
		m_FilterType = noFilter;
		m_FilterAtt = "";
		m_FilterVal.clear();
		m_TextField = "";
    m_Width.clear();
    m_WidthStops.clear();
	}

//-----------------------------------------------------------------------------
// Test la validite d'un niveau de zoom pour ce style
//-----------------------------------------------------------------------------
  bool MvtStyleLayer::TestZoomLevel(int zoomlevel) const
  {
    if (m_MinZoom >= 0)
      if (zoomlevel < m_MinZoom)
        return false;
    if (m_MaxZoom >= 0)
      if (zoomlevel > m_MaxZoom)
        return false;
    return true;
  }

//-----------------------------------------------------------------------------
// Fixe le style pour le dessin
//-----------------------------------------------------------------------------
  bool MvtStyleLayer::SetStyle(int zoomlevel, juce::Colour* pen, juce::FillType* fillType, float* lineWidth)
  {
    if (m_Type == noType)
      return false;

    if (m_Width.size() > 0) {
      if (m_WidthStops.size() > 0) {
        if (zoomlevel <= m_WidthStops[0])
          *lineWidth = m_Width[0];
        else
          for (int i = 0; i < m_WidthStops.size(); i++) {
            if (zoomlevel >= m_WidthStops[i])
              *lineWidth = m_Width[i];
          }
      }
      else
        *lineWidth = m_Width[0];
    }
    else
      *lineWidth = 1.f;

    if (m_Type == line) {
      if (m_LineColor.size() > 0) {
        if (m_LineStops.size() > 0) {
          if (zoomlevel <= m_LineStops[0])
            *pen = m_LineColor[0];
          else
            for (int i = 0; i < m_LineStops.size(); i++) {
              if (zoomlevel >= m_LineStops[i])
                *pen = m_LineColor[i];
            }
        }
        else
          *pen = m_LineColor[0];
      }
      else
        *pen = juce::Colour();

      return true;
    }

    if (m_Type == fill) {
      if (m_OutlineColor.size() > 0) {
        if (m_OutlineStops.size() > 0) {
          if (zoomlevel <= m_OutlineStops[0])
            *pen = m_OutlineColor[0];
          else
            for (int i = 0; i < m_OutlineStops.size(); i++) {
              if (zoomlevel >= m_OutlineStops[i])
                *pen = m_OutlineColor[i];
            }
        }
        *pen = m_OutlineColor[0];
      }
      else
        *pen = juce::Colour();

      if (m_FillType.size() > 0) {
        if (m_FillStops.size() > 0) {
          if (zoomlevel <= m_FillStops[0])
            *fillType = m_FillType[0];
          else
            for (int i = 0; i < m_FillStops.size(); i++) {
              if (zoomlevel >= m_FillStops[i])
                *fillType = m_FillType[i];
            }
        }
        else
          *fillType = m_FillType[0];
      }
      else
        *fillType = juce::FillType();

      return true;
    }

    if (m_Type == symbol) {
      if (m_TextColor.size() > 0) {
        if (m_TextStops.size() > 0) {
          if (zoomlevel <= m_TextStops[0])
            *pen = m_TextColor[0];
          else
            for (int i = 0; i < m_TextStops.size(); i++) {
              if (zoomlevel >= m_TextStops[i])
                *pen = m_TextColor[i];
            }
        }
        else
          *pen = m_TextColor[0];
      }
      else
        *pen = juce::Colour();

      return true;
    }

    return false;
  }


//-----------------------------------------------------------------------------
// Lecture d'un style
//-----------------------------------------------------------------------------
  bool MvtStyleLayer::Read(const juce::var& layer)
  {
    Clear();

    // Niveaux de zoom Min et Max
    if (layer.hasProperty("minzoom"))
      m_MinZoom = (int)layer["minzoom"];
    if (layer.hasProperty("maxzoom"))
      m_MaxZoom = (int)layer["maxzoom"];

    // Source
    if (layer.hasProperty("source-layer"))
      m_SourceLayer = layer["source-layer"].toString();

    // Type
    if (layer.hasProperty("type")) {
      juce::String type = layer["type"].toString();
      if (type == "fill") m_Type = fill;
      if (type == "line") m_Type = line;
      if (type == "symbol") m_Type = symbol;
    }

    // Lecture des informations LAYOUT
    if (layer.hasProperty("layout")) {
      juce::var layout = layer["layout"];
      if (layout.hasProperty("text-field")) {
        m_TextField = layout["text-field"].toString();
        m_TextField = m_TextField.removeCharacters("{}");
      }

      if (layout.hasProperty("visibility")) {
        if (layout["visibility"].toString() == "visible")
          m_Visibility = true;
      }
    }

    // Lectures des informations FILTER
    if (layer.hasProperty("filter")) {
      juce::var filter = layer["filter"];
      if (filter.size() >= 3) {
        // Egalite
        if (filter[0].toString() == "==") {
          m_FilterType = equal;
          m_FilterAtt = filter[1].toString();
          m_FilterVal.push_back(filter[2].toString());
        }

        // Appartenance a un ensemble
        if (filter[0].toString() == "in") {
          m_FilterType = in;
          m_FilterAtt = filter[1].toString();
          for (int k = 2; k < filter.size(); k++)
            m_FilterVal.push_back(filter[k].toString());
        }
      }
    }
    else
      m_FilterType = noFilter;

    // Lecture des informations PAINT
    juce::Colour color;
    if (layer.hasProperty("paint")) {
      juce::var paint = layer["paint"];

      // Line Color
      if (paint.hasProperty("line-color")) {
        if (paint["line-color"].hasProperty("stops")) {
          juce::var stops = paint["line-color"]["stops"];
          for (int s = 0; s < stops.size(); s++) {
            m_LineStops.push_back((int)stops[s][0]);
            color = juce::Colour::fromString(stops[s][1].toString());
            color = color.withAlpha(1.f);
            m_LineColor.push_back(color);
          }
        }
        else {
          color = juce::Colour::fromString(paint["line-color"].toString());
          color = color.withAlpha(1.f);
          m_LineColor.push_back(color);
        }
      }

      // Outline Color
      if (paint.hasProperty("fill-outline-color")) {
        if (paint["fill-outline-color"].hasProperty("stops")) {
          juce::var stops = paint["fill-outline-color"]["stops"];
          for (int s = 0; s < stops.size(); s++) {
            m_OutlineStops.push_back((int)stops[s][0]);
            color = juce::Colour::fromString(stops[s][1].toString());
            color = color.withAlpha(1.f);
            m_OutlineColor.push_back(color);
          }
        }
        else {
          color = juce::Colour::fromString(paint["fill-outline-color"].toString());
          color = color.withAlpha(1.f);
          m_OutlineColor.push_back(color);
        }
      }

      // Text Color
      if (paint.hasProperty("text-color")) {
        if (paint["text-color"].hasProperty("stops")) {
          juce::var stops = paint["text-color"]["stops"];
          for (int s = 0; s < stops.size(); s++) {
            m_TextStops.push_back((int)stops[s][0]);
            color = juce::Colour::fromString(stops[s][1].toString());
            color = color.withAlpha(1.f);
            m_TextColor.push_back(color);
          }
        }
        else {
          color = juce::Colour::fromString(paint["text-color"].toString());
          color = color.withAlpha(1.f);
          m_TextColor.push_back(color);
        }
      }

      // Fill Color
      if (paint.hasProperty("fill-color")) {
        if (paint["fill-color"].hasProperty("stops")) {
          juce::var stops = paint["fill-color"]["stops"];
          for (int s = 0; s < stops.size(); s++) {
            m_FillStops.push_back((int)stops[s][0]);
            color = juce::Colour::fromString(stops[s][1].toString());
            color = color.withAlpha(1.f);
            m_FillType.push_back(juce::FillType(color));
          }
        }
        else {
          color = juce::Colour::fromString(paint["fill-color"].toString());
          color = color.withAlpha(1.f);
          m_FillType.push_back(juce::FillType(color));
        }
      }

      // Line Width
      if (paint.hasProperty("line-width")) {
        if (paint["line-width"].hasProperty("stops")) {
          juce::var stops = paint["line-width"]["stops"];
          for (int s = 0; s < stops.size(); s++) {
            m_WidthStops.push_back((int)stops[s][0]);
            m_Width.push_back(stops[s][1].toString().getFloatValue());
          }
        }
        else
          m_Width.push_back(paint["line_width"].toString().getFloatValue());
      }

      // Fill Opacity
      if (paint.hasProperty("fill-opacity")) {
        float opacity = paint["fill-opacity"].toString().getFloatValue();
        for (int i = 0; i < m_FillType.size(); i++)
          m_FillType[i].setOpacity(opacity);
      }
    }

    return true;
  }