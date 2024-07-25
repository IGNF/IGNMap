//-----------------------------------------------------------------------------
//								GeoBase.cpp
//								===========
//
// Encapsulation des classes XTool pour la gestion de donnees
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/01/2023
//-----------------------------------------------------------------------------

#include "GeoBase.h"
#include "AppUtil.h"

#include "../../XTool/XGeoBase.h"
#include "../../XTool/XPath.h"
#include "../../XTool/XParserXML.h"
#include "../../XToolVector/XShapefile.h"
#include "../../XToolVector/XGpkgMap.h"
#include "../../XToolVector/XMifMid.h"
#include "../../XToolVector/XDxf.h"
#include "../../XToolVector/XGeoJson.h"
#include "../../XToolVector/XTAChantier.h"
#include "../../XTool/XInterpol.h"
#include "../../XTool/XTransfo.h"
#include "../../XToolGeod/XGeoPref.h"
#include "AffineImage.h"

//-----------------------------------------------------------------------------
// Points du cadre de l'image
//-----------------------------------------------------------------------------
XPt2D GeoImage::Pt(uint32_t i)
{
	switch (i) {
	case 0: return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
	case 1: return XPt2D(m_Frame.Xmax, m_Frame.Ymax);
	case 2: return XPt2D(m_Frame.Xmax, m_Frame.Ymin);
	case 3: return XPt2D(m_Frame.Xmin, m_Frame.Ymin);
	case 4: return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
	}
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool GeoFileImage::ReadAttributes(std::vector<std::string>& V)
{
	char buf[64];
	V.clear();
	V.push_back("Nom");
	V.push_back(m_strFilename);

	double gsd = 0., xmin = 0., ymax = 0.;
	GetGeoref(&xmin, &ymax, &gsd);

	V.push_back("Resolution");
	snprintf(buf, 64, "%.2lf", gsd);
	V.push_back(buf);
	V.push_back("Largeur");
	snprintf(buf, 64, "%u", XFileImage::Width());
	V.push_back(buf);
	V.push_back("Hauteur");
	snprintf(buf, 64, "%u", XFileImage::Height());
	V.push_back(buf);
	V.push_back("Nb. bits");
	snprintf(buf, 64, "%u", NbBits());
	V.push_back(buf);
	V.push_back("Xmin");
	snprintf(buf, 64, "%.2lf", m_Frame.Xmin);
	V.push_back(buf);
	V.push_back("Ymin");
	snprintf(buf, 64, "%.2lf", m_Frame.Ymin);
	V.push_back(buf);
	V.push_back("Xmax");
	snprintf(buf, 64, "%.2lf", m_Frame.Xmax);
	V.push_back(buf);
	V.push_back("Ymax");
	snprintf(buf, 64, "%.2lf", m_Frame.Ymax);
	V.push_back(buf);
	V.push_back("Metadata");
	V.push_back(GetMetadata());
	V.push_back("XmlMetadata");
	V.push_back(GetXmlMetadata());
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool GeoFileImage::AnalyzeImage(std::string path)
{
	if (!XFileImage::AnalyzeImage(path))
		return false;
	double xmin = 0., ymax = 0., gsd = 1.;
	GetGeoref(&xmin, &ymax, &gsd);
	if ((xmin == 0.) && (ymax == 0.)) {
		if (GeoTools::FindGeorefTab(path, &xmin, &ymax, &gsd)) {
			XFileImage::SetGeoref(xmin, ymax, gsd);
		}
		else {
			if (GeoTools::FindGeorefTfw(path, &xmin, &ymax, &gsd)) {
				XFileImage::SetGeoref(xmin, ymax, gsd);
			}
			else {
				XFileImage::SetGeoref(0., 0., 1.);
				gsd = 1.0;
			}
		}
	}
	m_Frame.Xmin = xmin;
	m_Frame.Xmax = xmin + gsd * XFileImage::Width();
	m_Frame.Ymax = ymax;
	m_Frame.Ymin = ymax - gsd * XFileImage::Height();
	return true;
}

//-----------------------------------------------------------------------------
// Creation du repertoire cache temporaire pour stocker les imagettes
//-----------------------------------------------------------------------------
void GeoInternetImage::CreateCacheDir(juce::String name)
{ 
	m_Cache = GeoTools::CreateCacheDir(name);
}

//-----------------------------------------------------------------------------
// Sauve l'image source
//-----------------------------------------------------------------------------
bool GeoInternetImage::SaveSourceImage(juce::String filename)
{
	juce::File tmpFile;
	if (filename.isEmpty())
		tmpFile = juce::File::createTempFile("png");
	else
		tmpFile = juce::File(filename);
	juce::FileOutputStream out(tmpFile);
	juce::PNGImageFormat pngWriter;
	return pngWriter.writeImageToStream(m_SourceImage, out);
}

//-----------------------------------------------------------------------------
// Reechantillonnage rapide
//-----------------------------------------------------------------------------
bool GeoInternetImage::Resample(XTransfo* transfo)
{
	int Wproj = 0, Hproj = 0, u, v, Wsource = m_SourceImage.getWidth(), Hsource = m_SourceImage.getHeight();
	double xi = 0., yi = 0.;
	transfo->Dimension(Wsource, Hsource, &Wproj, &Hproj);

	m_ProjImage = juce::Image(juce::Image::PixelFormat::ARGB, Wproj, Hproj, true);

	juce::Image::BitmapData sourceData(m_SourceImage, juce::Image::BitmapData::readOnly);
	juce::Image::BitmapData projData(m_ProjImage, juce::Image::BitmapData::readWrite);
	if ((sourceData.pixelStride != 4) || (projData.pixelStride != 4))
		return false; // Les images doivent etre ARGB
	int sourceLineW = sourceData.lineStride;

	XInterCubCatmull interpol;
	int win = interpol.Win(), win_size = (interpol.Win() * 2);
	double* value = new double[win_size * win_size];
	int result[4];
	juce::uint8* pix = new juce::uint8[win_size * win_size * 4];
	for (int i = 0; i < Hproj; i++) {
		juce::uint8* line_out = projData.getLinePointer(i);
		for (int j = 0; j < Wproj; j++) {
			juce::uint8* pix_out = line_out + j * 4;
			transfo->Direct(j, i, &xi, &yi);
			u = (int)xi;
			v = (int)yi;
			if (u >= Wsource - win) continue;
			if (v >= Hsource - win) continue;
			if (u < win) continue;
			if (v < win) continue;
			for (int k = 0; k < win_size; k++)
				::memcpy(&pix[k * win_size * 4], &sourceData.data[(v - win + k) * sourceLineW + (u - win) * 4], win_size * 4);

			for (int k = 0; k < 4; k++) {
				uint8_t* ptr = (uint8_t*)pix + k;
				for (int p = 0; p < win_size * win_size; p++) {
					value[p] = *ptr;
					ptr += 4;
				}
				result[k] = (int)interpol.BiCompute(value, xi - u, yi - v);
				if (result[k] < 0) result[k] = 0;
				if (result[k] > 255) result[k] = 255;
			}

			pix_out[0] = (juce::uint8)result[0];
			pix_out[1] = (juce::uint8)result[1];
			pix_out[2] = (juce::uint8)result[2];
			pix_out[3] = (juce::uint8)result[3];
		}
	}
	delete[] value;
	delete[] pix;

	return true;
}

//-----------------------------------------------------------------------------
// Fermeture du MNT
//-----------------------------------------------------------------------------
void GeoDTM::Close()
{
	m_In.Close();
	if (m_bTmpFile) {
		juce::File file(m_strImageName.c_str());
		file.deleteFile();
	}
	XGeoFDtm::Close();
}

//-----------------------------------------------------------------------------
// Indique si la lecture est possible
//-----------------------------------------------------------------------------
bool GeoDTM::StreamReady()
{
	if (m_bTmpFile)
		return XGeoFDtm::StreamReady();
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne de noeuds
//-----------------------------------------------------------------------------
bool GeoDTM::ReadLine(float* line, uint32_t numLine)
{
	if (m_bTmpFile)
		return XGeoFDtm::ReadLine(line, numLine);
	uint32_t nb_sample;
	return m_Image.GetRawArea(0, numLine, m_nW, 1, line, &nb_sample);
}

//-----------------------------------------------------------------------------
// Lecture d'un noeud
//-----------------------------------------------------------------------------
bool GeoDTM::ReadNode(float* node, uint32_t x, uint32_t y)
{
	if (m_bTmpFile)
		return XGeoFDtm::ReadNode(node, x, y);
	uint32_t nb_sample;
	return m_Image.GetRawArea(x, y, 1, 1, node, &nb_sample);
}

//-----------------------------------------------------------------------------
// Lecture de tout le MNT
//-----------------------------------------------------------------------------
bool GeoDTM::ReadAll(float* area)
{
	if (m_bTmpFile)
		return XGeoFDtm::ReadAll(area);
	uint32_t nb_sample;
	return m_Image.GetRawArea(0, 0, m_nW, m_nH, area, &nb_sample);
}

//-----------------------------------------------------------------------------
// Ouverture du MNT Tiff
//-----------------------------------------------------------------------------
bool GeoDTM::ImportTif(std::string file_tif, std::string file_bin)
{
	if (!m_Image.AnalyzeImage(file_tif))
		return false;
	if (m_Image.NbSample() > 1)
		return false;
	m_nW = m_Image.Width();
	m_nH = m_Image.Height();
	m_dNoData = -9999;
	m_strFilename = file_tif;
	XPath P;
	m_strPath = P.Path(file_tif.c_str());
	m_strName = P.Name(file_tif.c_str());
	m_strImageName = m_strFilename;

	double x0, y0;
	m_Image.GetGeoref(&x0, &y0, &m_dGSD);
	m_Frame.Xmin = x0 + m_dGSD * 0.5;
	m_Frame.Ymax = y0 - m_dGSD * 0.5;
	m_Frame.Xmax = m_Frame.Xmin + m_dGSD * (m_nW - 1);
	m_Frame.Ymin = m_Frame.Ymax - m_dGSD * (m_nH - 1);

	// Si le fichier n'est pas un GEOTIFF, on cherche un fichier XML
	bool xml_file = false;
	if (m_dGSD <= 0.) {
		std::string xmlfile = P.PathName(file_tif.c_str()) + ".xml";
		XParserXML parser;
		if (parser.Parse(xmlfile))
			xml_file = XmlRead(&parser);
		if (!xml_file) { // On cherche un TFW
			if (!GeoTools::FindGeorefTfw(file_tif.c_str(), &x0, &y0, &m_dGSD))
				GeoTools::FindGeorefTab(file_tif.c_str(), &x0, &y0, &m_dGSD);
			if (m_dGSD <= 0.)
				return false;
			m_Frame.Xmin = x0 + m_dGSD * 0.5;
			m_Frame.Ymax = y0 - m_dGSD * 0.5;
			m_Frame.Xmax = m_Frame.Xmin + m_dGSD * (m_nW - 1);
			m_Frame.Ymin = m_Frame.Ymax - m_dGSD * (m_nH - 1);
		}
	}

	if (!xml_file) {
		// Calcul des statistiques
		if (m_Image.NbSample() < 5) {
			double minVal[4], maxVal[4], meanVal[4];
			uint32_t noData[4];
			m_dNoData = -9999.;
			m_Image.GetStat(minVal, maxVal, meanVal, noData, m_dNoData);
			m_dZmin = minVal[0];
			m_dZmax = maxVal[0];
			m_nNbNoZ = noData[0];
		}
	}
	m_bValid = true;
	return m_bValid;
}

//==============================================================================
// Classe GeoLAS : ouverture d'un fichier LAS
//==============================================================================
bool GeoLAS::Open(std::string filename)
{
	if (!XLasFile::Open(filename))
		return false;
	m_Frame = XFrame(m_Header->min_x, m_Header->min_y, m_Header->max_x, m_Header->max_y);
	m_ZRange[0] = m_Header->min_z;
	m_ZRange[1] = m_Header->max_z;
	return true;
}

//==============================================================================
// Classe GeoLAS : lecture des attributs du fichier LAS
//==============================================================================
bool GeoLAS::ReadAttributes(std::vector<std::string>& Att)
{
	Att.clear();
	if (!ReOpen())
		return false;
	Att.push_back("Filename");  Att.push_back(Filename());
	Att.push_back("Software");  Att.push_back(m_Header->generating_software);
	Att.push_back("Version"); Att.push_back(std::to_string(m_Header->version_major) + "." + std::to_string(m_Header->version_minor));
	Att.push_back("Point format"); Att.push_back(std::to_string(m_Header->point_data_format));
	Att.push_back("Global encoding"); Att.push_back(std::to_string(m_Header->global_encoding));
	Att.push_back("Date"); Att.push_back(std::to_string(m_Header->file_creation_day) + "/" + std::to_string(m_Header->file_creation_year));
	Att.push_back("Nb Points"); Att.push_back(std::to_string(NbLasPoints()));
	Att.push_back("Zmin"); Att.push_back(std::to_string(Zmin()));
	Att.push_back("Zmax"); Att.push_back(std::to_string(Zmax()));
	if (m_bCopc) { // Fichier COPC
		Att.push_back("COPC spacing"); Att.push_back(std::to_string(m_CopcReader.m_dSpacing));
		Att.push_back("COPC level"); Att.push_back(std::to_string(m_CopcReader.MaxLevel()));
	}
	
	CloseIfNeeded();
	return true;
}

//-----------------------------------------------------------------------------
// Recherche d'un geo-referencement TAB
//-----------------------------------------------------------------------------
bool GeoTools::FindGeorefTab(std::string filename, double* Xmin, double* Ymax, double* GSD)
{
	std::string grfFile, path = filename;
	grfFile = path.substr(0, path.rfind('.')) + ".tab";
	*GSD = 0.;

	// Ouverture du fichier en entree
	std::ifstream file_in;
	file_in.open(grfFile.c_str());
	if (!file_in.good())
		return false;

	std::string chaine;
	double x1 = 0., y1 = 0., x2 = 0., y2 = 0., gsd = 0.;
	int  u1 = 0, v1 = 0, u2 = 0, v2 = 0;
	char buf[256];
	while (!file_in.eof()) {
		file_in.getline(buf, 256);
		chaine = buf;
		if (chaine == "Type \"RASTER\"") {
			file_in.getline(buf, 256);
			if (sscanf(buf, "(%lf,%lf) (%d,%d)", &x1, &y1, &u1, &v1) != 4)
				return false;
			file_in.getline(buf, 256);
			if (sscanf(buf, "(%lf,%lf) (%d,%d)", &x2, &y2, &u2, &v2) != 4)
				return false;
			break;
		}
		if (!file_in.good())
			break;
	}

	if (abs(u1 - u2) > 0) {
		gsd = fabs(x1 - x2) / abs(u1 - u2);
	}
	else {
		if (abs(v1 - v2) > 0) {
			gsd = fabs(y1 - y2) / abs(v1 - v2);
		}
	}

	if (gsd <= 0.00001)
		return false;

	*Xmin = x1 - u1 * gsd;
	*Ymax = y1 + v1 * gsd;
	*GSD = gsd;
	return true;
}

//-----------------------------------------------------------------------------
// Recherche d'un geo-referencement TFW
//-----------------------------------------------------------------------------
bool GeoTools::FindGeorefTfw(std::string filename, double* Xmin, double* Ymax, double* GSD)
{
	std::string tfwFile, path = filename;
	tfwFile = path.substr(0, path.rfind('.')) + ".tfw";
	*GSD = 0.;

	// Ouverture du fichier en entree
	std::ifstream file_in;
	file_in.open(tfwFile.c_str());
	if (!file_in.good())
		return false;

	double x, y, gsd, data1, data2, data3;
	file_in >> gsd >> data1 >> data2 >> data3 >> x >> y;

	if (gsd <= 0.00001)
		return false;

	*Xmin = x - (gsd * 0.5);
	*Ymax = y + (gsd * 0.5);
	*GSD = gsd;
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un repertoire de donnees vectorielles
//-----------------------------------------------------------------------------
bool GeoTools::ImportVectorFolder(juce::String folderName, XGeoBase* base, int& nb_total, int& nb_imported)
{
	juce::File folder = folderName;
	nb_imported = 0;
	nb_total = 0;

	// Format ShapeFile
	juce::Array<juce::File> T = folder.findChildFiles(juce::File::findFiles, false, "*.shp");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportShapefile(T[i].getFullPathName(), base);

	// Format GeoPackage
	T = folder.findChildFiles(juce::File::findFiles, false, "*.gpkg");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportGeoPackage(T[i].getFullPathName(), base);

	// Format MIF/MID
	T = folder.findChildFiles(juce::File::findFiles, false, "*.mif");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportMifMid(T[i].getFullPathName(), base);

	// Format DXF
	T = folder.findChildFiles(juce::File::findFiles, false, "*.dxf");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportDxf(T[i].getFullPathName(), base);

	// Format GEOJSON
	T = folder.findChildFiles(juce::File::findFiles, false, "*.json");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportGeoJson(T[i].getFullPathName(), base);

	// Format TA XML
	T = folder.findChildFiles(juce::File::findFiles, false, "*.xml");
	nb_total += T.size();
	for (int i = 0; i < T.size(); i++)
		ImportTA(T[i].getFullPathName(), base);

	ColorizeClasses(base);

	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier Shapefile
//-----------------------------------------------------------------------------
bool GeoTools::ImportShapefile(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	XGeoClass* C = XShapefile::ImportShapefile(base, AppUtil::GetStringFilename(fileName).c_str(), map);
	if (C == NULL)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier GeoPackage
//-----------------------------------------------------------------------------
bool GeoTools::ImportGeoPackage(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	if (!XGpkgMap::ImportGpkg(base, AppUtil::GetStringFilename(fileName).c_str(), map))
		return false;
	base->UpdateFrame();
	base->SortClass();
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier MIF/MID
//-----------------------------------------------------------------------------
bool GeoTools::ImportMifMid(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	XGeoClass* C = XMifMid::ImportMifMid(base, AppUtil::GetStringFilename(fileName).c_str(), map);
	if (C == NULL)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier DXF
//-----------------------------------------------------------------------------
bool GeoTools::ImportDxf(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	if (!XDxf::ImportDxf(base, AppUtil::GetStringFilename(fileName).c_str(), map))
		return false;
	base->UpdateFrame();
	base->SortClass();
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier GEOJSON
//-----------------------------------------------------------------------------
bool GeoTools::ImportGeoJson(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	XGeoClass* C = XGeoJson::ImportGeoJson(base, AppUtil::GetStringFilename(fileName).c_str(), map);
	if (C == NULL)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un fichier Tableau d'Assemblage IGN
//-----------------------------------------------------------------------------
bool GeoTools::ImportTA(juce::String fileName, XGeoBase* base, XGeoMap* map)
{
	if (fileName.isEmpty())
		return false;
	XGeoClass* C = XTAChantier::ImportTA(base, AppUtil::GetStringFilename(fileName).c_str(), map);
	if (C == NULL)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Colorisation des classes
//-----------------------------------------------------------------------------
void GeoTools::ColorizeClasses(XGeoBase* base)
{
	XGeoRepres defaut_repres;
	for (uint32_t i = 0; i < base->NbLayer(); i++) {
		XGeoLayer* layer = base->Layer(i);
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			XGeoClass* C = layer->Class(j);
			XGeoRepres* repres = C->Repres();
			if (repres == nullptr)
				continue;
			if (*repres == defaut_repres) {
				auto& rd = juce::Random::getSystemRandom();
				repres->Color(juce::Colour((juce::uint8)rd.nextInt(256), (juce::uint8)rd.nextInt(256), (juce::uint8)rd.nextInt(256)).getARGB());
				repres->FillColor(juce::Colour((juce::uint8)rd.nextInt(256), (juce::uint8)rd.nextInt(256), (juce::uint8)rd.nextInt(256)).getARGB());
			}

		}
	}
}

//-----------------------------------------------------------------------------
// Enregistrement d'un objet dans une map et une classe
//-----------------------------------------------------------------------------
bool GeoTools::RegisterObject(XGeoBase* base, XGeoVector* V, std::string mapName, std::string layerName, std::string className,
														 int transparency, uint32_t color, uint32_t fill, uint32_t zorder, uint8_t size)
{
	XGeoMap* map = new XGeoMap(mapName);
	XGeoClass* raster_class = base->AddClass(layerName.c_str(), className.c_str());
	if (raster_class == nullptr)
		return false;
	raster_class->Repres()->Transparency((uint8_t)transparency);
	raster_class->Repres()->Color(color);
	raster_class->Repres()->FillColor(fill);
	raster_class->Repres()->ZOrder(zorder);
	raster_class->Repres()->Size(size);
	raster_class->Vector(V);
	V->Class(raster_class);
	map->AddObject(V);

	base->AddMap(map);
	base->SortClass();
	return true;
}

//-----------------------------------------------------------------------------
// Creation du repertoire cache temporaire
//-----------------------------------------------------------------------------
juce::File GeoTools::CreateCacheDir(juce::String name)
{
	juce::File tmpDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory);
	juce::File cache = tmpDir.getNonexistentChildFile(name, "");
	cache.createDirectory();
	return cache;
}

//-----------------------------------------------------------------------------
// Changement de projection de la base
//-----------------------------------------------------------------------------
void GeoTools::UpdateProjection(XGeoBase* base)
{
	// On regarde s'il y a des donnees non-Internet
	bool no_data = true;
	for (uint32_t k = 0; k < base->NbMap(); k++) {
		XGeoMap* map = base->Map(k);
		if ((map->Name() != "OSM") && (map->Name() != "WMTS")) {
			no_data = false;
			continue;
		}
	}
	XFrame newFrame = base->Frame();
	if (no_data) {
		XGeoPref pref;
		XFrame geoF = XGeoProjection::FrameGeo(pref.Projection());
		pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, newFrame.Xmin, newFrame.Ymin);
		pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, newFrame.Xmax, newFrame.Ymax);
	}

	// Changement de projection des couches raster Internet
	for (uint32_t k = 0; k < base->NbMap(); k++) {
		XGeoMap* map = base->Map(k);
		if ((map->Name() != "OSM") && (map->Name() != "WMTS"))
			continue;
		for (uint32_t i = 0; i < map->NbObject(); i++) {
			GeoInternetImage* raster = (GeoInternetImage*)map->Object(i);
			raster->SetFrame(newFrame);
			XGeoClass* classe = raster->Class();
			if (classe != NULL) {
				classe->RemoveAllVectors();
				classe->Vector(raster);
			}
			classe->UpdateFrame();
		}
		map->UpdateFrame();
	}
	base->UpdateFrame();
}

//-----------------------------------------------------------------------------
// Calcul d'une grille d'altitude a partir des MNT charges
//-----------------------------------------------------------------------------
bool GeoTools::ComputeZGrid(XGeoBase* base, float* grid, uint32_t w, uint32_t h, XFrame* F)
{
	for (uint32_t i = 0; i < base->NbClass(); i++) {
		XGeoClass* C = base->Class(i);
		if (C == nullptr)
			continue;
		if ((!C->IsDTM()) || (!C->Visible()))
			continue;
		for (uint32_t j = 0; j < C->NbVector(); j++) {
			GeoDTM* dtm = (GeoDTM*)C->Vector(j);
			if (!dtm->Visible())
				continue;
			if (!F->Intersect(dtm->Frame()))
				continue;
			XFileImage image;
			if (!image.AnalyzeImage(dtm->ImageName()))
				continue;
			if (image.NbSample() != 1)
				continue;
			double gsd = dtm->Resolution();
			image.SetGeoref(dtm->Frame().Xmin - gsd * 0.5, dtm->Frame().Ymax + gsd * 0.5, gsd);
			int U0, V0, win, hin, R0, S0, wout, hout, nbBand;
			if (!image.PrepareRasterDraw(F, F->Width() / w, U0, V0, win, hin, nbBand, R0, S0, wout, hout))
				continue;

			int factor = win / wout;
			if (factor < 1)
				factor = 1;
			int wtmp = win / factor, htmp = hin / factor;
			if ((wtmp == 0) || (htmp == 0))
				continue;

			float* area = new float[wtmp * htmp];
			float* data = new float[wout * hout];
			if ((area == nullptr)||(data == nullptr))
				continue;
			uint32_t nb_sample;
			image.GetRawArea(U0, V0, win, hin, area, &nb_sample, factor);
			XBaseImage::FastZoomBil(area, wtmp, htmp, data, wout, hout);
			XBaseImage::CopyArea((uint8_t*)data, (uint8_t*)grid, wout * sizeof(float), hout, w * sizeof(float), h, R0 * sizeof(float), S0);
			delete[] area;
			delete[] data;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Ajout d'une image dans le cadre d'un objet
//-----------------------------------------------------------------------------
bool GeoTools::AddImageInObect(XGeoBase* base, int index)
{
	XGeoVector* V = base->Selection(index);
	if (V == nullptr)
		return false;
	juce::String filename;
	filename = AppUtil::OpenFile("RasterPath");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();
	RotationImage* affine = new RotationImage();
	if (!affine->AnalyzeImage(filename.toStdString())) {
		delete affine;
		return false;
	}

	XTACliche* cliche = dynamic_cast<XTACliche*>(V);
	if (cliche != nullptr) {	// Cas particulier des cliches aeriens
		XPt2D S = cliche->Centroide();
		double gsd = cliche->Resol();
		XPt2D C = XPt2D(affine->GetImageW() * 0.5, affine->GetImageH() * 0.5);
		if (!cliche->IsDigital()) {
			gsd = (cliche->Resol() * 1000 * 0.24) / XMin(affine->GetImageW(), affine->GetImageH());
			C = XPt2D(affine->GetImageW() * 0.5, affine->GetImageH() - affine->GetImageW() * 0.5);
		}
		affine->SetPosition(S.X, S.Y, gsd);
		affine->SetImageCenter(C.X, C.Y);
		affine->SetRotation(XPI - (cliche->Cap() / 180.) * XPI);
	}
	else {	// Cas general : on colle l'image dans le cadre
		double gsd = 1.;
		if ((V->FrameW() > 1.) && (V->FrameH() > 1.))
			gsd = (V->FrameW() / affine->GetImageW() + V->FrameH() / affine->GetImageH()) * 0.5;
		XPt2D S = V->Frame().Center();
		affine->SetPosition(S.X, S.Y, gsd);
	}

	if (!GeoTools::RegisterObject(base, affine, "ROTATION", "Raster", name.toStdString().c_str())) {
		delete affine;
		return false;
	}
	return true;
}