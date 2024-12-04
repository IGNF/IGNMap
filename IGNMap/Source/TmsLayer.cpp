//-----------------------------------------------------------------------------
//								TmsLayer.cpp
//								============
//
// Gestion des flux TMS https://wiki.osgeo.org/wiki/Tile_Map_Service_Specification
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/11/2024
//-----------------------------------------------------------------------------

#include "TmsLayer.h"
#include <algorithm>
#include <limits>
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolGeod/XTransfoGeod.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
TmsLayer::TmsLayer()
{
	m_nW = m_nH = 256;
	m_dX0 = m_dY0 = 0.;
	m_ProjCode = XGeoProjection::Unknown;
}

//-----------------------------------------------------------------------------
// Lecture d'une URL contenant la description de la couche
//-----------------------------------------------------------------------------
bool TmsLayer::ReadServer(juce::String serverUrl)
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
	element = root.get()->getChildByName("SRS");
	if (element != nullptr)
		m_strSRS = element->getAllSubText();
	else return false;
	if (m_strSRS == "OSGEO:41001") m_strSRS = "EPSG:3857";	// Projection depreciee
	element = root.get()->getChildByName("BoundingBox");
	if (element != nullptr) {
		m_F.Xmin = element->getDoubleAttribute("minx");
		m_F.Xmax = element->getDoubleAttribute("maxx");
		m_F.Ymin = element->getDoubleAttribute("miny");
		m_F.Ymax = element->getDoubleAttribute("maxy");
	}
	else return false;
	element = root.get()->getChildByName("Origin");
	if (element != nullptr) {
		m_dX0 = element->getDoubleAttribute("x");
		m_dY0 = element->getDoubleAttribute("y");
	}
	else return false;
	element = root.get()->getChildByName("TileFormat");
	if (element != nullptr) {
		m_strFormat = element->getStringAttribute("extension");
		m_nW = (uint16_t)element->getIntAttribute("width");
		m_nH = (uint16_t)element->getIntAttribute("height");
	}
	else return false;

	// Lecture des TileSets
	element = root.get()->getChildByName("TileSets");
	if (element == nullptr) return false;
	m_TileSet.clear();
	for (int i = 0; i < element->getNumChildElements(); i++) {
		juce::XmlElement* tileSet = element->getChildElement(i);
		if (tileSet->getTagName() != "TileSet")
			continue;
		TileSet set;
		set.m_dGsd = tileSet->getDoubleAttribute("units-per-pixel");
		set.m_nMinRow = tileSet->getIntAttribute("minrow", 0);
		set.m_nMaxRow = tileSet->getIntAttribute("maxrow", std::numeric_limits<int>::max());
		set.m_nMinCol = tileSet->getIntAttribute("mincol", 0);
		set.m_nMaxCol = tileSet->getIntAttribute("maxcol", std::numeric_limits<int>::max());
		set.m_strHRef = tileSet->getStringAttribute("href");
		m_TileSet.push_back(set);
	}
	std::sort(m_TileSet.begin(), m_TileSet.end(), TileSet::predTileSet);
	if (m_TileSet.size() < 1)
		return false;

	// Recherche de la projection
	m_ProjCode = XGeoProjection::Unknown;
	for (int proj = XGeoProjection::RGF93; proj < XGeoProjection::NC_RGNC91_UTM59; proj++) {
		juce::String code = "EPSG:" + juce::String(XGeoProjection::EPSGCode((XGeoProjection::XProjCode)proj));
		if (m_strSRS == code) {
			m_ProjCode = (XGeoProjection::XProjCode)proj;
			break;
		}
	}
	if (m_ProjCode == XGeoProjection::Unknown)
		return false;
	XGeoPref pref;
	pref.ConvertDeg(m_ProjCode, pref.Projection(), m_F.Xmin, m_F.Ymin, m_Frame.Xmin, m_Frame.Ymin);
	pref.ConvertDeg(m_ProjCode, pref.Projection(), m_F.Xmax, m_F.Ymax, m_Frame.Xmax, m_Frame.Ymax);
	XFrame projF, geoF= XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, projF.Xmin, projF.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, projF.Xmax, projF.Ymax);

	if (projF.IsIn(m_Frame)||(m_ProjCode == XGeoProjection::WebMercator))
		m_Frame = projF;

	CreateCacheDir(m_strTitle);

	return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
juce::String TmsLayer::LoadTile(int x, int y, int zoomlevel)
{
	juce::String filename = m_Cache.getFullPathName() + juce::File::getSeparatorString() +
		juce::String(zoomlevel) + "_" + juce::String(x) + "_" + juce::String(y) + "." + m_strFormat;
	filename = juce::File::createLegalPathName(filename);
	juce::File cache(filename);
	if (cache.existsAsFile()) // Le fichier a deja ete telecharge
		return filename;

	// Telechargement du fichier
	m_strRequest = m_TileSet[zoomlevel].m_strHRef + "/" + juce::String(x) + "/" + juce::String(y) + "." + m_strFormat;
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
// Chargement d'un cadre en projection a un zoom donne
//-----------------------------------------------------------------------------
bool TmsLayer::LoadFrame(const XFrame& F, int zoomlevel)
{
	double gsd = m_TileSet[zoomlevel].m_dGsd;
	// Dalles a charger pour couvrir l'emprise
	double xmin = (F.Xmin - m_dX0) / (gsd * m_nW);
	int firstX = (int)floor(xmin);
	double xmax = (F.Xmax - m_dX0) / (gsd * m_nW);
	int lastX = (int)ceil(xmax);
	double ymin = (m_dY0 - F.Ymax) / (gsd * m_nH);
	int firstY = (int)floor(ymin);
	double ymax = (m_dY0 - F.Ymin) / (gsd * m_nH);
	int lastY = (int)ceil(ymax);

	int nb_tilex = lastX - firstX;
	int nb_tiley = lastY - firstY;
	m_SourceImage = juce::Image(juce::Image::PixelFormat::ARGB, nb_tilex * m_nW, nb_tiley * m_nH, true);
	juce::Graphics g(m_SourceImage);
	g.setOpacity(1.0f);

	for (int i = 0; i < nb_tiley; i++) {
		int y = firstY + i;
		if ((y < (int)m_TileSet[zoomlevel].m_nMinRow) || (y > (int)m_TileSet[zoomlevel].m_nMaxRow))
			continue;
		for (int j = 0; j < nb_tilex; j++) {
			int x = firstX + j;
			if ((x < (int)m_TileSet[zoomlevel].m_nMinCol) || (x > (int)m_TileSet[zoomlevel].m_nMaxCol))
				continue;
			juce::String filename = LoadTile(x, y, zoomlevel);
			juce::Image image = juce::ImageFileFormat::loadFrom(juce::File(filename));
			if (!image.isValid()) {
				juce::File badFile(filename); // Le fichier est peut-etre corrompu
				badFile.deleteFile();
				continue;
			}
			g.drawImageAt(image, j * m_nW, i * m_nH);
		}
	}

	int xcrop = XRint((xmin - firstX) * m_nW);
	int ycrop = XRint((ymin - firstY) * m_nH);
	int wcrop = XRint(F.Width() / gsd);
	int hcrop = XRint(F.Height() / gsd);
	m_SourceImage = m_SourceImage.getClippedImage(juce::Rectangle<int>(xcrop, ycrop, wcrop, hcrop));

	return true;
}

//-----------------------------------------------------------------------------
// Renvoie une image pour couvrir un cadre
//-----------------------------------------------------------------------------
juce::Image& TmsLayer::GetAreaImage(const XFrame& F, double gsd)
{
	if ((F == m_LastFrame) && (gsd == m_LastGsd))
		return m_ProjImage;
	m_LastFrame = F;
	m_LastGsd = gsd;
	uint32_t wout = (uint32_t)(F.Width() / gsd);
	uint32_t hout = (uint32_t)(F.Height() / gsd);

	// Recherche du niveau de zoom adapte
	int index = (int)m_TileSet.size() - 1;
	for (int i = 0; i < m_TileSet.size(); i++) {
		if (gsd >= m_TileSet[i].m_dGsd) {
			index = i;
			if (i > 0) {
				if (gsd > (m_TileSet[i].m_dGsd + m_TileSet[i-1].m_dGsd)*0.5)
					index = i - 1;
			}
			break;
		}
	}

	// Si on est dans la bonne projection
	XGeoPref pref;
	if (pref.Projection() == m_ProjCode) {
		LoadFrame(F, index);
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

	LoadFrame(FwebMerc, index);
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
// Attributs de l'objet TmsLayer
//-----------------------------------------------------------------------------
bool TmsLayer::ReadAttributes(std::vector<std::string>& V)
{
	V.clear();
	V.push_back("Title"); V.push_back(m_strTitle.toStdString());
	V.push_back("SRS"); V.push_back(m_strSRS.toStdString());
	V.push_back("Url"); V.push_back(m_strRequest.toStdString());
	V.push_back("TileW"); V.push_back(juce::String(m_nW).toStdString());
	V.push_back("TileH"); V.push_back(juce::String(m_nH).toStdString());
	V.push_back("GSD"); V.push_back(juce::String(m_LastGsd).toStdString());
	return true;
}
