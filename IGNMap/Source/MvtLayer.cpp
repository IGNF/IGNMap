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
	m_bLoaded = m_bPrepared = false;
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
// Chargement d'une dalle
//-----------------------------------------------------------------------------
bool MvtTile::PrepareForDrawing(const XFrame& F, const double& gsd, const uint32_t& tileW, const uint32_t& tileH)
{
	XGeoPref pref;
	// Emprise de la dalle dans l'image
	double x0, y0, x1, y1, x2, y2, x3, y3;
	pref.Convert(XGeoProjection::WebMercator, pref.Projection(), m_dX0, m_dY0, x0, y0);
	pref.Convert(XGeoProjection::WebMercator, pref.Projection(), m_dX0 + tileW * m_dGsd, m_dY0, x1, y1);
	pref.Convert(XGeoProjection::WebMercator, pref.Projection(), m_dX0 + tileW * m_dGsd, m_dY0 - tileH * m_dGsd, x2, y2);
	pref.Convert(XGeoProjection::WebMercator, pref.Projection(), m_dX0, m_dY0 - tileH * m_dGsd, x3, y3);
	x0 = (x0 - F.Xmin) / gsd;
	y0 = (F.Ymax - y0) / gsd;
	x1 = (x1 - F.Xmin) / gsd;
	y1 = (F.Ymax - y1) / gsd;
	x2 = (x2 - F.Xmin) / gsd;
	y2 = (F.Ymax - y2) / gsd;
	x3 = (x3 - F.Xmin) / gsd;
	y3 = (F.Ymax - y3) / gsd;
	int xmin = (int)XMin(x0, x3);
	int xmax = (int)XMax(x1, x2);
	int ymin = (int)XMin(y0, y1);
	int ymax = (int)XMax(y2, y3);
	m_ClipR = juce::Rectangle<int>(xmin, ymin, xmax - xmin, ymax - ymin);

	juce::Rectangle<int> R(0, 0, (int)ceil(F.Width() / gsd), (int)ceil(F.Height() / gsd));
	m_bPrepared = R.intersects(m_ClipR);

	return m_bPrepared;
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

	CreateCacheDir("MVT");
}

//-----------------------------------------------------------------------------
// Lecture d'une URL contenant la description de la couche
//-----------------------------------------------------------------------------
bool MvtLayer::ReadServer(juce::String serverUrl)
{
	juce::URL url(serverUrl);

	std::unique_ptr<juce::XmlElement> root = url.readEntireXmlStream();
	if (root.get()->getTagName() != "TileMap")
		return false;

	// Lecture des attributs
	juce::XmlElement* element = root.get()->getChildByName("Title");
	if (element != nullptr)
		m_strTitle = element->getAllSubText();
	else return false;

	// Lecture des metadata -> fichier de style
	m_StyleFiles.clear();
	for (int i = 0; i < root->getNumChildElements(); i++) {
		juce::XmlElement* meta = root->getChildElement(i);
		if (meta->getTagName() != "Metadata")
			continue;
		juce::String href = meta->getStringAttribute("href");
		if (href.isNotEmpty())
			m_StyleFiles.push_back(href);
	}
	if (m_StyleFiles.size() < 1) return false;
	if (!LoadStyle(m_StyleFiles[0])) return false;
	
	element = root.get()->getChildByName("BoundingBox");
	if (element != nullptr) {
		m_F.Xmin = element->getDoubleAttribute("minx");
		m_F.Xmax = element->getDoubleAttribute("maxx");
		m_F.Ymin = element->getDoubleAttribute("miny");
		m_F.Ymax = element->getDoubleAttribute("maxy");
	}
	else return false;
	
	element = root.get()->getChildByName("TileFormat");
	if (element != nullptr) {
		m_strFormat = element->getStringAttribute("extension");
		m_nTileW = (uint16_t)element->getIntAttribute("width");
		m_nTileH = (uint16_t)element->getIntAttribute("height");
	}
	else return false;

	// Lecture des TileSets
	element = root.get()->getChildByName("TileSets");
	if (element == nullptr) return false;
	int min_zoom = 100, max_zoom = -1;
	for (int i = 0; i < element->getNumChildElements(); i++) {
		juce::XmlElement* tileSet = element->getChildElement(i);
		if (tileSet->getTagName() != "TileSet")
			continue;
		double resol =  tileSet->getDoubleAttribute("units-per-pixel");
		if (resol <= 0.) continue;
		int zoom = XRint(log(40075016.686 / resol) / log(2.)) - 8;
		if (zoom > max_zoom) max_zoom = zoom;
		if (zoom < min_zoom) min_zoom = zoom;
	}
	if (max_zoom < 0) return false;
	m_nMaxZoom = (uint32_t)max_zoom;
	m_nMinZoom = (uint32_t)min_zoom;

	// Definition de l'emprise
	XGeoPref pref;
	XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);
	SetFrame(F);

	CreateCacheDir(m_strTitle);
	m_strServer = serverUrl;

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
	m_strRequest = m_strServer + "/" + juce::String(zoomlevel) + "/" + juce::String(x) + "/" + juce::String(y) + "." + m_strFormat;
	juce::URL url(m_strRequest);
	juce::URL::DownloadTaskOptions options;
	std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(filename, options);
	if (task.get() == nullptr)
		return filename;
	int count = 0;
	while (task.get()->isFinished() == false)
	{
		juce::Thread::sleep(10);
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
	osm_zoom += m_ZoomCorrection;
	if (osm_zoom < 0) osm_zoom = 0;
	if (osm_zoom > (int)m_nMaxZoom) osm_zoom = (int)m_nMaxZoom;
	m_nLastZoom = osm_zoom;
	m_ProjImage = juce::Image(juce::Image::ARGB, (int)wout, (int)hout, true, juce::SoftwareImageType());

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
// Attributs de l'objet MvtLayer
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

	if (parsedJSON.hasProperty("sprite"))
		ReadSprite(parsedJSON["sprite"].toString());

	if (!parsedJSON.hasProperty("layers"))
		return false;
	m_StyleLayers = parsedJSON["layers"];
	if (m_StyleLayers.size() < 1)
		return false;

	// Lecture des layers
	m_Layer.clear();
	for (int cmpt = 0; cmpt < m_StyleLayers.size(); cmpt++) {
		juce::var layer = m_StyleLayers[cmpt];
		MvtStyleLayer styleLayer;
		if (styleLayer.Read(layer, &m_Sprite))
			m_Layer.push_back(styleLayer);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Lecture des sprites
//-----------------------------------------------------------------------------
bool MvtLayer::ReadSprite(juce::String server)
{
	juce::URL json_url(server+".json");
	juce::WebInputStream web(json_url, false);
	juce::String content = web.readEntireStreamAsString();
	juce::var parsedJSON = juce::JSON::parse(content);
	if (!parsedJSON.isObject())
		return false;

	juce::URL image_url(server + ".png");
	juce::WebInputStream image_web(image_url, false);
	juce::MemoryBlock block;	// Pour une raison obscure, il faut passer par un MemoryBlock plutot que faire un loadFrom sur le WebInputStream
	size_t count = image_web.readIntoMemoryBlock(block);
	m_SpriteImage = juce::PNGImageFormat::loadFrom(block.getData(), block.getSize());
	if (m_SpriteImage.isNull())
		return false;

	m_Sprite.clear();
	juce::DynamicObject* object = parsedJSON.getDynamicObject();
	juce::NamedValueSet set = object->getProperties();
	for (int i = 0; i < set.size(); i++) {
		MvtSprite sprite;
		sprite.m_Name = set.getName(i).toString();
		juce::var sub = parsedJSON[set.getName(i)];
		int x = sub["x"];
		int y = sub["y"];
		int width = sub["width"];
		int height = sub["height"];
		int pixelRatio = sub["pixelRatio"];
		sprite.m_Image = m_SpriteImage.getClippedImage(juce::Rectangle<int>(x, y, width, height));
		m_Sprite.push_back(sprite);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Affichage dans l'ordre du fichier de style
//-----------------------------------------------------------------------------
bool MvtLayer::DrawWithStyle(const XFrame& F, int zoomlevel)
{
	// Dalles a charger pour couvrir l'emprise
	double XYori = -(6378137 * XPI);  // Origine du referentiel des dalles
	double xmin = (F.Xmin - XYori) / Swath(zoomlevel);
	int firstX = (int)floor(xmin);
	double xmax = (F.Xmax - XYori) / Swath(zoomlevel);
	int lastX = (int)ceil(xmax);
	double ymin = (-XYori - F.Ymax) / Swath(zoomlevel);
	int firstY = (int)floor(ymin);
	double ymax = (-XYori - F.Ymin) / Swath(zoomlevel);
	int lastY = (int)ceil(ymax);

	int nb_tilex = lastX - firstX;
	int nb_tiley = lastY - firstY;

	std::vector<MvtTile> T;
	T.resize(nb_tiley * nb_tilex);

	// Lecture des styles dans l'ordre
	for (int cmpt = 0; cmpt < m_Layer.size(); cmpt++) {
		//if (cmpt != 119)  // Bati Quelconque
		//  continue;
		//if (cmpt != 146) // Sentier
		//  continue;
		//if (cmpt != 149) // Sentier
		//  continue;
		//if (cmpt != 282) // Eolienne
		//	continue;
		//if (cmpt != 379) // Toponyme
		//  continue;
		//if (cmpt != 410) // "toponyme localite n0 typoA5etA6 commune"
		//	continue;
		//if (cmpt != 411)
		//	continue;
		MvtStyleLayer layer = m_Layer[cmpt];
		// Niveaux de zoom
		if (zoomlevel < layer.MinZoomLevel())
			continue;
		if (zoomlevel >= layer.MaxZoomLevel())
			continue;

		// Lecture des tiles
		int index = 0;
		for (int i = 0; i < nb_tiley; i++) {
			for (int j = 0; j < nb_tilex; j++) {
				int x = firstX + j;
				int y = firstY + i;
				double GSD0 = Resol(zoomlevel);
				double X0 = x * Swath(zoomlevel) + XYori;
				double Y0 = -XYori - y * Swath(zoomlevel);
				if (!T[index].IsLoaded()) {
					juce::String filename = LoadTile(x, y, zoomlevel);
					bool flag = T[index].Load(filename, X0, Y0, GSD0);
					if (!flag) {
						juce::File badFile(filename); // Le fichier est peut etre corrompu
						badFile.deleteFile();
						continue;
					}
					T[index].PrepareForDrawing(m_LastFrame, m_LastGsd, m_nTileW, m_nTileH);
				}
				DrawMvt(&T[index], &layer);
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
bool MvtLayer::DrawMvt(MvtTile* T, MvtStyleLayer* style)
{
	if (!T->IsPrepared())
		return false;
	XGeoPref pref;
	juce::Graphics g(m_ProjImage);
	g.setOpacity(1.f);

	if (style->Type() == MvtStyleLayer::fill)
		g.reduceClipRegion(T->ClipR());

	auto layer = T->Tile()->get_layer_by_name(style->SourceLayer().toStdString());
	if (layer.empty())
		return false;

	float factor = layer.extent() / m_nTileW, gsd_factor = (float)(m_LastGsd / T->GSD());
	gsd_factor = 1.;

	float line_width = 1.f, iconSize = 1.f, radius = 1.f, textSize = 10.f, opacity = 1.f;
	juce::String text_field = style->TextField();
	juce::Path path;

	juce::Colour pen, halo;
	juce::FillType fillType;

	// Zoom level du style a appliquer
	int styleZoomLevel = (int)m_nLastZoom;
	styleZoomLevel =	XRint(log(40075016.686 / m_LastGsd) / log(2.)) - 8;

	style->SetStyle(styleZoomLevel, &pen, &fillType, &line_width, &iconSize, &radius, &textSize, &halo, &opacity);
	if (opacity <= 0.f)
		return false;
	if ((style->Type() == MvtStyleLayer::line) && (line_width <= 0.f))
		return false;
	if ((style->Type() == MvtStyleLayer::circle) && (radius <= 0.5f))
		return false;
	g.setColour(pen);
	if ((style->Type() == MvtStyleLayer::fill) || (style->Type() == MvtStyleLayer::circle))
		g.setFillType(fillType);
	g.setFont(textSize);

	//juce::PathStrokeType stroke(line_width, juce::PathStrokeType::beveled);
	juce::PathStrokeType stroke(line_width * gsd_factor, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded);

	int nbLineDash = style->NbLineDash();
	float* dash = nullptr;
	if (nbLineDash > 0) {
		stroke.setEndStyle(juce::PathStrokeType::butt);
		stroke.setJointStyle(juce::PathStrokeType::mitered);
		stroke.setStrokeThickness(line_width * gsd_factor / 2.f); // Il faut diviser par 2 ...
		dash = new float[nbLineDash];
		for (int i = 0; i < nbLineDash; i++)
			dash[i] = style->LineDash()[i] * line_width * gsd_factor;
	}

	double X0 = T->X0(), Y0 = T->Y0(), GSD0 = T->GSD();

	while (auto feature = layer.next_feature()) {

		bool filtering = true;
		for (int i = 0; i < style->NbFilter(); i++) {
			switch (style->FilterType(i)) {
			case MvtStyleLayer::equal:
			case MvtStyleLayer::in:
			case MvtStyleLayer::diff:
				feature.reset_property();
				while (auto property = feature.next_property()) {
					if (property.key().to_string() == style->FilterAtt(i)) {
						if ((int)property.value().type() == 1)
							filtering = style->TestAtt(i, property.value().string_value().to_string());
						break;
					}
				}
				break;
			default:;
			}
			if (!filtering)
				break;
		}

		if (!filtering)
			continue;

		// Lecture de la geometrie
		vtzero::geometry geom = feature.geometry();
		if (geom.type() == vtzero::GeomType::UNKNOWN)
			continue;

		geom_handler handler;
		vtzero::decode_geometry(geom, handler);

		double Xima = 0., Yima = 0., Xter = 0., Yter = 0., Xlast = 0., Ylast = 0., d = 2.;

		// Dessin des points et multi-points
		if ((geom.type() == vtzero::GeomType::POINT) && ((style->Type() == MvtStyleLayer::symbol) || (style->Type() == MvtStyleLayer::circle))) {
			for (int i = 0; i < handler.points.size() / 2; i++) {
				Xima = X0 + (handler.points[2 * i] / factor) * GSD0;
				Yima = Y0 - (handler.points[2 * i + 1] / factor) * GSD0;
				pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
				if (!m_LastFrame.IsIn(XPt2D(Xter, Yter)))
					continue;
				Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
				Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;

				// Symbol
				if (style->Type() == MvtStyleLayer::symbol) {
					if (!style->Icon().isNull()) {
						g.setOpacity(1.f);
						int wout = style->Icon().getWidth(), hout = style->Icon().getHeight();
						if (fabs(iconSize - 1.) < 0.001)
							g.drawImageAt(style->Icon(), Xter - wout / 2, Yter - hout / 2);
						else {
							wout *= iconSize;
							hout *= iconSize;
							g.drawImageWithin(style->Icon(), Xter - wout / 2, Yter - hout / 2, wout, hout, juce::RectanglePlacement());
						}
					}
				}

				// Circle
				if (style->Type() == MvtStyleLayer::circle) {
					g.fillEllipse(Xter - radius, Yter - radius, 2.f * radius, 2.f * radius);
					if (line_width > 0.5f)
						g.drawEllipse(Xter - radius, Yter - radius, 2.f * radius, 2.f * radius, line_width);
				}

				// Text
				if (!text_field.isEmpty()) {
					feature.reset_property();
					while (auto property = feature.next_property()) {
						if (property.key().to_string() == text_field) {
							if ((int)property.value().type() == 1) {
								juce::String label = property.value().string_value().to_string();
								if (!halo.isTransparent()) {
									g.setOpacity(1.f);
									if (textSize >= 1) {
										g.setColour(halo);
										g.drawSingleLineText(label, Xter - 1, Yter);
										g.drawSingleLineText(label, Xter + 1, Yter);
										g.drawSingleLineText(label, Xter, Yter - 1);
										g.drawSingleLineText(label, Xter, Yter + 1);
									}
									else {
										g.setFillType(juce::FillType(halo));
										juce::Rectangle<float> R = juce::GlyphArrangement::getStringBounds(g.getCurrentFont(), label);
										R.setPosition(Xter, Yter - R.getHeight() + 2);
										g.fillRect(R);
									}
								}
								if (!pen.isTransparent())
									g.setOpacity(1.f);
									g.setColour(pen);
									g.drawSingleLineText(label, Xter, Yter);
							}
							break;
						}
					}
				}
			}
			continue;
		}

		path.clear();
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
			Xlast = Xter;
			Ylast = Yter;
			for (int i = 1; i < handler.parts[parts]; i++) {
				index++;
				Xima = X0 + (handler.points[2 * index] / factor) * GSD0;
				Yima = Y0 - (handler.points[2 * index + 1] / factor) * GSD0;
				pref.Convert(XGeoProjection::WebMercator, pref.Projection(), Xima, Yima, Xter, Yter);
				Xter = (Xter - m_LastFrame.Xmin) / m_LastGsd;
				Yter = (m_LastFrame.Ymax - Yter) / m_LastGsd;
				if ((fabs(Xter - Xlast) >= 1.) || (fabs(Yter - Ylast) >= 1.)) {
					path.lineTo(Xter, Yter);
					Xlast = Xter;
					Ylast = Yter;
				}
			}
			if (geom.type() == vtzero::GeomType::POLYGON)
				path.closeSubPath();
			index++;
		}

		//if (geom.type() == vtzero::GeomType::LINESTRING) {
		if (style->Type() == MvtStyleLayer::line) {
			if (nbLineDash > 0)
				stroke.createDashedStroke(path, path, dash, nbLineDash);
			g.strokePath(path, stroke);
			continue;
		}

		if ((geom.type() == vtzero::GeomType::POLYGON) && (style->Type() == MvtStyleLayer::fill)) {
			g.setFillType(fillType);
			g.fillPath(path);
			g.setColour(pen);
			if (nbLineDash > 0)
				stroke.createDashedStroke(path, path, dash, nbLineDash);
			g.strokePath(path, stroke);;
			continue;
		}

	} // feature

	if (dash != nullptr) delete[] dash;

	return true;
}

//-----------------------------------------------------------------------------
// Nettoyage d'un style
//-----------------------------------------------------------------------------
void MvtStyleLayer::Clear()
{
	m_Type = noType;
	m_SourceLayer = "";
	m_MinZoom = 0;	// Valeurs par defaut https://docs.mapbox.com/style-spec/reference/layers/
	m_MaxZoom = 24;	// At zoom levels equal to or greater than the maxzoom, the layer will be hidden
	m_Visibility = false;
	m_LineColor.clear();
	m_LineStops.clear();
	m_TextColor.clear();
	m_TextStops.clear();
	m_TextHaloColor.clear();
	m_TextHaloStops.clear();
	m_OutlineColor.clear();
	m_OutlineStops.clear();
	m_FillType.clear();
	m_FillStops.clear();
	m_Filter.clear();
	m_TextField = "";
	m_Width.clear();
	m_WidthStops.clear();
	m_Opacity.clear();
	m_OpacityStops.clear();
	m_LineDash.clear();
	m_Icon = juce::Image();
	m_IconSize.clear();
	m_IconSizeStops.clear();
	m_TextSize.clear();
	m_TextSizeStops.clear();
}

//-----------------------------------------------------------------------------
// Fixe le style pour le dessin
//-----------------------------------------------------------------------------
bool MvtStyleLayer::SetStyle(int zoomlevel, juce::Colour* pen, juce::FillType* fillType, float* lineWidth, float* iconSize, 
														 float* radius,float* textSize, juce::Colour* halo, float* opacity)
{
	if (m_Type == noType)
		return false;

	*lineWidth = ReadStopValInterpol<float>(zoomlevel, m_Width, m_WidthStops, 1.f);
	*opacity = ReadStopValInterpol<float>(zoomlevel, m_Opacity, m_OpacityStops, 1.f);

	if (m_Type == line) {
		*pen = ReadStopVal<juce::Colour>(zoomlevel, m_LineColor, m_LineStops, juce::Colour());
		return true;
	}

	if (m_Type == fill) {
		*pen = ReadStopVal<juce::Colour>(zoomlevel, m_OutlineColor, m_OutlineStops, juce::Colour());
		*fillType = ReadStopVal<juce::FillType>(zoomlevel, m_FillType, m_FillStops, juce::FillType());

		float opacity = ReadStopValInterpol<float>(zoomlevel, m_Opacity, m_OpacityStops, 1.f);
		fillType->setOpacity(opacity);

		return true;
	}

	if (m_Type == symbol) {
		*iconSize = ReadStopValInterpol<float>(zoomlevel, m_IconSize, m_IconSizeStops, 1.f);
		*textSize = ReadStopValInterpol<float>(zoomlevel, m_TextSize, m_TextSizeStops, 10.f);

		*pen = ReadStopVal<juce::Colour>(zoomlevel, m_TextColor, m_TextStops, juce::Colours::transparentWhite);
		*halo = ReadStopVal<juce::Colour>(zoomlevel, m_TextHaloColor, m_TextHaloStops, juce::Colours::transparentWhite);

		return true;
	}

	if (m_Type == circle) {
		*pen = ReadStopVal<juce::Colour>(zoomlevel, m_LineColor, m_LineStops, juce::Colour());
		*radius = ReadStopValInterpol<float>(zoomlevel, m_CircleRadius, m_CircleRadiusStops, 10.f);
		*fillType = ReadStopVal<juce::FillType>(zoomlevel, m_FillType, m_FillStops, juce::FillType());
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
// Lecture d'un style
//-----------------------------------------------------------------------------
bool MvtStyleLayer::Read(const juce::var& layer, std::vector<MvtSprite>* sprite)
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
		if (type == "circle") m_Type = circle;
	}

	// Lecture des informations LAYOUT
	if (layer.hasProperty("layout")) {
		juce::var layout = layer["layout"];
		if (layout.hasProperty("text-field")) {
			m_TextField = layout["text-field"].toString();
			m_TextField = m_TextField.removeCharacters("{}");
		}

		ReadFloatValue(layout, m_TextSize, m_TextSizeStops, "text-size");
		
		if (layout.hasProperty("visibility")) {
			if (layout["visibility"].toString() == "visible")
				m_Visibility = true;
		}
		if (layout.hasProperty("icon-image")) {
			juce::String icon_image = layout["icon-image"].toString();
			for (int i = 0; i < sprite->size(); i++) {
				if ((*sprite)[i].m_Name == icon_image) {
					m_Icon = ((*sprite)[i].m_Image).createCopy();
					break;
				}
			}
		}

		ReadFloatValue(layout, m_IconSize, m_IconSizeStops, "icon-size");
	}

	// Lectures des informations FILTER
	if (layer.hasProperty("filter")) {
		juce::var filter = layer["filter"];
		m_Filter.clear();
		ReadFilterArray(filter);
	}

	// Lecture des informations PAINT
	juce::Colour color;
	if (layer.hasProperty("paint")) {
		juce::var paint = layer["paint"];

		// Line Color
		ReadColorValue(paint, m_LineColor, m_LineStops, "line-color");
		if (m_Type == circle)
			ReadColorValue(paint, m_LineColor, m_LineStops, "circle-stroke-color");
		ReadColorValue(paint, m_OutlineColor, m_OutlineStops, "fill-outline-color");
		ReadColorValue(paint, m_TextColor, m_TextStops, "text-color");
		ReadColorValue(paint, m_TextHaloColor, m_TextHaloStops, "text-halo-color");

		// Fill Color
		ReadFillValue(paint, m_FillType, m_FillStops, "fill-color");
		if (m_Type == circle)
			ReadFillValue(paint, m_FillType, m_FillStops, "circle-color");

		// Fill Pattern
		if (paint.hasProperty("fill-pattern")) {
			juce::String pattern = paint["fill-pattern"].toString();
			for (int i = 0; i < sprite->size(); i++) {
				if ((*sprite)[i].m_Name == pattern) {
					m_FillType.push_back(juce::FillType((*sprite)[i].m_Image, juce::AffineTransform()));
					break;
				}
			}
		}

		// Line Width
		ReadFloatValue(paint, m_Width, m_WidthStops, "line-width");
		if (m_Type == circle) {
			ReadFloatValue(paint, m_Width, m_WidthStops, "circle-stroke-width");
			ReadFloatValue(paint, m_CircleRadius, m_CircleRadiusStops, "circle-radius");
		}
		// Fill Opacity
		ReadFloatValue(paint, m_Opacity, m_OpacityStops, "fill-opacity");
		if (m_Type == circle)
			ReadFloatValue(paint, m_Opacity, m_OpacityStops, "circle-opacity");

		// Line Dash
		if (paint.hasProperty("line-dasharray")) {
			juce::var dash = paint["line-dasharray"];
			for (int i = 0; i < dash.size(); i++)
				m_LineDash.push_back(dash[i].toString().getFloatValue());
		}

	}

	return true;
}

//-----------------------------------------------------------------------------
// Lecture des filtres
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadFilterArray(const juce::var& filter)
{
	for (int i = 0; i < filter.size(); i++) {
		if (filter[i].isString()) {
			if (filter[i] == "all")
				continue;
			return ReadFilter(filter);
		}

		if (filter[i].isArray()) {
			ReadFilter(filter[i]);
			continue;
		}
	}
	return true;
}


//-----------------------------------------------------------------------------
// Lecture des filtres
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadFilter(const juce::var& filter)
{
	if (filter.size() < 3)
		return false;

	Filter F;

	// Egalite
	if (filter[0].toString() == "==") {
		F.Type = equal;
		F.Att = filter[1].toString();
		F.Val.push_back(filter[2].toString());
		m_Filter.push_back(F);
		return true;
	}

	// Difference
	if (filter[0].toString() == "!=") {
		F.Type = diff;
		F.Att = filter[1].toString();
		F.Val.push_back(filter[2].toString());
		m_Filter.push_back(F);
		return true;
	}

	// Appartenance a un ensemble
	if (filter[0].toString() == "in") {
		F.Type = in;
		F.Att = filter[1].toString();
		for (int k = 2; k < filter.size(); k++)
			F.Val.push_back(filter[k].toString());
		m_Filter.push_back(F);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Test les valeurs d'attributs dans un filtre
//-----------------------------------------------------------------------------
bool MvtStyleLayer::TestAtt(int num, juce::String val)
{ 
	if (num >= m_Filter.size())
		return false;
	if ((m_Filter[num].Type == in) || (m_Filter[num].Type == equal)) {
		for (int i = 0; i < m_Filter[num].Val.size(); i++)
			if (m_Filter[num].Val[i] == val)
				return true;
	}
	if ((m_Filter[num].Type == diff)) {
		for (int i = 0; i < m_Filter[num].Val.size(); i++) {
			if (m_Filter[num].Val[i] == val)
				return false;
		}
		return true;
	}
	return false; 
}

//-----------------------------------------------------------------------------
// Lecture des expressions
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadExpression(const juce::var& expr, std::vector<float>& T, std::vector<int>& stops)
{
	if (expr.size() < 7)
		return false;
	if ((expr.size() - 3) % 2 != 0)
		return false;
	if (!expr[0].isString())
		return false;
	if (expr[0].toString() != "interpolate")
		return false;
	if (!expr[1].isArray())
		return false;
	if (!expr[1][0].isString())
		return false;
	if (expr[1][0].toString() != "linear")
		return false;
	if (!expr[2].isArray())
		return false;
	if (!expr[2][0].isString())
		return false;
	if (expr[2][0].toString() != "zoom")
		return false;
	stops.clear();
	T.clear();
	for (int i = 0; i < (expr.size() - 3) / 2; i++) {
		stops.push_back((int)expr[3 + 2 * i]);
		T.push_back((float)expr[3 + 2 * i + 1]);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des couleurs
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadColorValue(const juce::var& paint, std::vector<juce::Colour>& Value, std::vector<int>& Stops, const char* field)
{
	if (!paint.hasProperty(field))
		return false;
	juce::Colour color;
	if (paint[field].hasProperty("stops")) {
		juce::var stops = paint[field]["stops"];
		for (int s = 0; s < stops.size(); s++) {
			Stops.push_back((int)stops[s][0]);
			color = ReadColour(stops[s][1].toString());
			Value.push_back(color);
		}
	}
	else {
		color = ReadColour(paint[field].toString());
		Value.push_back(color);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des remplissages
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadFillValue(const juce::var& paint, std::vector<juce::FillType>& Value, std::vector<int>& Stops, const char* field)
{
	if (!paint.hasProperty(field))
		return false;
	juce::Colour color;
	if (paint[field].hasProperty("stops")) {
		juce::var stops = paint[field]["stops"];
		for (int s = 0; s < stops.size(); s++) {
			Stops.push_back((int)stops[s][0]);
			color = ReadColour(stops[s][1].toString());
			Value.push_back(juce::FillType(color));
		}
	}
	else {
		color = ReadColour(paint[field].toString());
		Value.push_back(juce::FillType(color));
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des valeurs Float
//-----------------------------------------------------------------------------
bool MvtStyleLayer::ReadFloatValue(const juce::var& paint, std::vector<float>& Value, std::vector<int>& Stops, const char* field)
{
	if (!paint.hasProperty(field))
		return false;

	if (!ReadExpression(paint[field], Value, Stops)) {
		if (paint[field].hasProperty("stops")) {
			juce::var stops = paint[field]["stops"];
			for (int s = 0; s < stops.size(); s++) {
				Stops.push_back((int)stops[s][0]);
				Value.push_back(stops[s][1].toString().getFloatValue());
			}
		}
		else
			Value.push_back(paint[field].toString().getFloatValue());
		}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une couleur
//-----------------------------------------------------------------------------
juce::Colour MvtStyleLayer::ReadColour(const juce::String& str)
{
	juce::Colour color;

	if (str.startsWithChar('#')) {
		color = juce::Colour::fromString(str);
		color = color.withAlpha(1.f);
		return color;
	}
	if (str.startsWith("rgba")) {
		juce::String valstr = str.substring(4);
		valstr = valstr.removeCharacters("()");
		juce::StringArray T;
		T.addTokens(valstr, ",", "");
		if (T.size() == 4) {
			color = juce::Colour((juce::uint8)T[0].getIntValue(), (juce::uint8)T[1].getIntValue(), (juce::uint8)T[2].getIntValue());
			color = color.withAlpha((float)T[3].getDoubleValue());
			return color;
		}
	}

	return color;
}

//-----------------------------------------------------------------------------
// Lecture d'une valeur en fonction des stops
//-----------------------------------------------------------------------------
template<typename T> T MvtStyleLayer::ReadStopVal(int zoomlevel, const std::vector<T>& Val, const std::vector<int>& Stops, T default_value)
{
	if (Val.size() < 1)
		return default_value;
	if (Stops.size() < 1)
		return Val[0];
	if (Stops.size() != Val.size())	// Cela ne devrait pas arriver ...
		return default_value;
	if (zoomlevel <= Stops[0])
		return Val[0];
	if (zoomlevel >= Stops[Stops.size() - 1])
		return Val[Val.size() - 1];
	T value = Val[0];
	for (int i = 0; i < Stops.size(); i++) {
		if (zoomlevel >= Stops[i])
				value = Val[i];
		}
	return value;
}

//-----------------------------------------------------------------------------
// Lecture d'une valeur en fonction des stops avec interpolation
//-----------------------------------------------------------------------------
template<typename T> T MvtStyleLayer::ReadStopValInterpol(int zoomlevel, const std::vector<T>& Val, const std::vector<int>& Stops, T default_value)
{
	if (Val.size() < 1)
		return default_value;
	if (Stops.size() < 1)
		return Val[0];
	if (Stops.size() != Val.size())	// Cela ne devrait pas arriver ...
		return default_value;
	if (zoomlevel <= Stops[0])
		return Val[0];
	if (zoomlevel >= Stops[Stops.size() - 1])
		return Val[Val.size() - 1];
	int index = 0;
	for (int i = 0; i < Stops.size(); i++) {
		if (zoomlevel == Stops[i])
			return Val[i];
		if (zoomlevel > Stops[i])
			index = i;
	}
	
	return Val[index] + (Val[index + 1] - Val[index]) * (zoomlevel - Stops[index]) / (Stops[index + 1] - Stops[index]);
}
