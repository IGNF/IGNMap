//-----------------------------------------------------------------------------
//								DtmTmsLayer.cpp
//								===============
//
// Gestion des flux MNT diffuses en TMS 
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/02/2026
//-----------------------------------------------------------------------------

#include "DtmTmsLayer.h"
#include "AppUtil.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolGeod/XTransfoGeod.h"
#include "../../XTool/XInterpol.h"
#include "../../XToolImage/XWebPImage.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
DtmTmsLayer::DtmTmsLayer(std::string server, std::string apikey, std::string format, std::string encoding,
	uint16_t tileW, uint16_t tileH, uint16_t max_zoom)
{
	m_strServer = server;
	m_strKey = apikey;
	m_strFormat = format;
	m_strEncoding = encoding;
	m_nTileW = tileW;
	m_nTileH = tileH;
	m_nMaxZoom = max_zoom;

	m_ProjCode = XGeoProjection::WebMercator;
	m_dX0 = m_dY0 = 0.;
	m_bFlip = true;

  m_SourceGrid = nullptr;
  m_SourceW = m_SourceH = 0;

  m_dZmin = -1000.;
  m_dZmax = 8900.;
  m_nNbNoZ = 0;

	CreateCacheDir("DTM_TMS");
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
DtmTmsLayer::~DtmTmsLayer()
{
  if (m_SourceGrid != nullptr)
    delete[] m_SourceGrid;
}

//-----------------------------------------------------------------------------
// Attributs
//-----------------------------------------------------------------------------
bool DtmTmsLayer::ReadAttributes(std::vector<std::string>& V)
{
  V.clear();
  V.push_back("Nom"); V.push_back(Name());
  V.push_back("Serveur"); V.push_back(m_strServer.toStdString());
  V.push_back("Requete"); V.push_back(m_strRequest.toStdString());
  V.push_back("Format"); V.push_back(m_strFormat.toStdString());
  V.push_back("Encodage"); V.push_back(m_strEncoding.toStdString());

	return false;
}

//-----------------------------------------------------------------------------
// Chargement d'une image WebP
//-----------------------------------------------------------------------------
juce::Image DtmTmsLayer::LoadWebp(juce::String filename)
{
  XWebPImage webp(filename.toStdString().c_str());
  if (!webp.IsValid())
    return juce::Image();
  juce::Image image(juce::Image::PixelFormat::RGB, webp.W(), webp.H(), true);
  juce::Image::BitmapData bitmap(image, juce::Image::BitmapData::readWrite);
  webp.GetArea(nullptr, 0, 0, webp.W(), webp.H(), bitmap.data);
  XBaseImage::SwitchRGB2BGR(bitmap.data, webp.W() * webp.H());
  return image;
}

//-----------------------------------------------------------------------------
// Chargement d'une dalle
//-----------------------------------------------------------------------------
juce::String DtmTmsLayer::LoadTile(int x, int y, int zoomlevel)
{
	juce::String filename = m_Cache.getFullPathName() + juce::File::getSeparatorString() +
		juce::String(zoomlevel) + "_" + juce::String(x) + "_" + juce::String(y) + "." + m_strFormat;
	juce::File cache(filename);
	if (cache.existsAsFile()) // Le fichier a deja ete telecharge
		return filename;

	// Telechargement du fichier
	m_strRequest = "https://" + m_strServer + "/" + juce::String(zoomlevel) + "/" + juce::String(x) + "/" + juce::String(y) + "." + m_strFormat;
  if (m_strKey.isNotEmpty())
    m_strRequest = m_strRequest + "?" + m_strKey;
	return AppUtil::DownloadFile(m_strRequest, filename, 20, 50);
}

//-----------------------------------------------------------------------------
// Conversion d'une dalle en grille altimetrique
//-----------------------------------------------------------------------------
bool DtmTmsLayer::ConvertTile(const juce::Image& image, float* grid)
{
  juce::Image::BitmapData data(image, juce::Image::BitmapData::readOnly);
  if ((data.height != m_nTileH) || (data.width != m_nTileW))
    return false;
  // Terrarium = (red * 256 + green + blue / 256) - 32768
  // TerrainRGB = -10000 + ((R * 256 * 256 + G * 256 + B) * 0.1)
  float* ptr_grid = grid;
  if (m_strEncoding == "Terrarium") {
    for (int i = 0; i < data.height; i++) {
      uint8_t* line = data.getLinePointer(i);
      for (int j = 0; j < data.width; j++) {
        *ptr_grid = (float)((line[2] * 256. + line[1] + line[0] / 256.) - 32768.);
        m_dZmin = XMin(m_dZmin, (double)*ptr_grid);
        m_dZmax = XMax(m_dZmax, (double)*ptr_grid);
        ptr_grid++;
        line += data.pixelStride;
      }
    }
  }

  if (m_strEncoding == "TerrainRGB") {
    for (int i = 0; i < data.height; i++) {
      uint8_t* line = data.getLinePointer(i);
      for (int j = 0; j < data.width; j++) {
        *ptr_grid = (float)((line[2] * 256. * 256. + line[1] * 256. + line[0]) * 0.1 - 10000.);
        m_dZmin = XMin(m_dZmin, (double)*ptr_grid);
        m_dZmax = XMax(m_dZmax, (double)*ptr_grid);
        ptr_grid++;
        line += data.pixelStride;
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'un cadre en WebMercator a un zoom donne
//-----------------------------------------------------------------------------
bool DtmTmsLayer::LoadFrame(const XFrame& F, int zoomlevel)
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
  m_SourceW = nb_tilex * m_nTileW;
  m_SourceH = nb_tiley * m_nTileH;
  m_SourceGrid = new(std::nothrow) float[m_SourceW * m_SourceH];
  if (m_SourceGrid == nullptr)
    return false;
  float* grid = new(std::nothrow) float[m_nTileW * m_nTileH];
  if (grid == nullptr) {
    delete[] m_SourceGrid;
    m_SourceGrid = nullptr;
    return false;
  }
  // On reinitialise le Zmin et le Zmax
  m_dZmin = std::numeric_limits<double>::max();
  m_dZmax = std::numeric_limits<double>::min();

  for (int i = 0; i < nb_tiley; i++) {
    for (int j = 0; j < nb_tilex; j++) {
      int x = firstX + j;
      int y = firstY + i;
      juce::Image image = FindCachedTile(x, y, zoomlevel);
      if (image.isNull()) {
        juce::String filename = LoadTile(x, y, zoomlevel);
        if (m_strFormat != "webp")
          image = juce::ImageFileFormat::loadFrom(juce::File(filename));
        else
          image = LoadWebp(filename);
        if (!image.isValid()) {
          juce::File badFile(filename); // Le fichier est peut etre corrompu
          badFile.deleteFile();
          continue;
        }
        AddCachedTile(x, y, zoomlevel, image);
      }
      if (!ConvertTile(image, grid))
        continue;
      XBaseImage::CopyArea((uint8_t*)grid, (uint8_t*)m_SourceGrid, m_nTileW * sizeof(float), m_nTileH, 
                            m_SourceW * sizeof(float), m_SourceH, j * m_nTileW * sizeof(float), i * m_nTileH);
    }
  }

  double osm_resol = Resol(zoomlevel);;
  int xcrop = XRint((xmin - firstX) * m_nTileW);
  int ycrop = XRint((ymin - firstY) * m_nTileH);
  int wcrop = XRint(F.Width() / osm_resol);
  int hcrop = XRint(F.Height() / osm_resol);
  if (XBaseImage::Crop((uint8_t*)m_SourceGrid, m_SourceW * sizeof(float), m_SourceH, wcrop * sizeof(float), hcrop,
    xcrop * sizeof(float), ycrop)) {
    m_SourceW = wcrop;
    m_SourceH = hcrop;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Chargement d'une zone
//-----------------------------------------------------------------------------
bool DtmTmsLayer::ComputeZGrid(float* grid, uint32_t w, uint32_t h, XFrame* F)
{
	// Recherche du niveau de zoom adapte
	double gsd = F->Width() / w;
  uint32_t zoom_level = m_nMaxZoom;
  for (uint32_t zoom = 0; zoom <= m_nMaxZoom; zoom++) {
    if (Resol(zoom) < gsd + gsd * 0.5) {
      zoom_level = zoom;
      break;
    }
  }

  // Si on est en WebMercator
  XGeoPref pref;
  if (pref.Projection() == WebMercator) {
    LoadFrame(*F, zoom_level);
    XBaseImage::FastZoomBil(m_SourceGrid, m_SourceW, m_SourceH, grid, w, h);
    delete[] m_SourceGrid;
    m_SourceW = m_SourceH = 0;
    m_SourceGrid = nullptr;
    return true;
  }

  // Reprojection sinon
  // Calcul de l'emprise en WebMercator
  XWebMercator geod;
  double x0, y0, x1, y1, x2, y2, x3, y3;
  geod.SetDefaultProjection(pref.Projection(), WebMercator);
  geod.Convert(F->Xmin, F->Ymin, x0, y0);
  geod.Convert(F->Xmin, F->Ymax, x1, y1);
  geod.Convert(F->Xmax, F->Ymax, x2, y2);
  geod.Convert(F->Xmax, F->Ymin, x3, y3);

  XFrame FwebMerc;
  FwebMerc.Xmin = XMin(x0, x1) - 5. * gsd;  // On ajoute un buffer pour eviter les pixels blancs
  FwebMerc.Xmax = XMax(x2, x3) + 5. * gsd;  // On ajoute un buffer pour eviter les pixels blancs
  FwebMerc.Ymin = XMin(y0, y3) - 5. * gsd;  // On ajoute un buffer pour eviter les pixels blancs
  FwebMerc.Ymax = XMax(y1, y2) + 5. * gsd;  // On ajoute un buffer pour eviter les pixels blancs

  LoadFrame(FwebMerc, zoom_level);

  // Reechantillonage dans la projection souhaitee
  XTransfoGeodZoomInterpol transfo(&geod);
  transfo.SetStartFrame(FwebMerc, FwebMerc.Width() / m_SourceW);
  transfo.SetEndFrame(*F, gsd);
  transfo.AutoCalibration();

  //XInterpol interpol;
  XInterCubCatmull interpol;
  XBaseImage::Resample(m_SourceGrid, grid, m_SourceW, m_SourceH, 1, 0, &transfo, &interpol, false);
  delete[] m_SourceGrid;
  m_SourceW = m_SourceH = 0;
  m_SourceGrid = nullptr;

	return true;
}

//-----------------------------------------------------------------------------
// DtmTmsComponent : constructeur
//-----------------------------------------------------------------------------
DtmTmsComponent::DtmTmsComponent(XGeoBase* base)
{
  m_Base = base;
  setSize(400, 300);

  m_cbxTitle.addItem("Mapterhorn", 1);
  m_cbxTitle.addItem("Mapzen", 2);
  m_cbxTitle.addItem("MapBox", 3);
  m_cbxTitle.addItem("MapTiler", 4);
  addAndMakeVisible(m_cbxTitle);
  m_cbxTitle.setBounds(100, 10, 200, 30);
  m_cbxTitle.addListener(this);

  m_lblServer.setText(juce::translate("Server :"), juce::NotificationType::dontSendNotification);
  m_lblServer.setBounds(10, 50, 80, 30);
  addAndMakeVisible(m_lblServer);
  m_edtServer.setBounds(100, 50, 290, 30);
  addAndMakeVisible(m_edtServer);
  m_lblKey.setText(juce::translate("API Key :"), juce::NotificationType::dontSendNotification);
  m_lblKey.setBounds(10, 85, 80, 30);
  addAndMakeVisible(m_lblKey);
  m_edtKey.setBounds(100, 85, 290, 30);
  addAndMakeVisible(m_edtKey);
  
  m_lblFormat.setText(juce::translate("Format :"), juce::NotificationType::dontSendNotification);
  m_lblFormat.setBounds(10, 120, 80, 30);
  addAndMakeVisible(m_lblFormat);
  m_cbxFormat.addItem("PNG", 1);
  m_cbxFormat.addItem("WEBP", 2);
  m_cbxFormat.setBounds(100, 120, 100, 30);
  addAndMakeVisible(m_cbxFormat);

  m_lblEncoding.setText(juce::translate("Encoding :"), juce::NotificationType::dontSendNotification);
  m_lblEncoding.setBounds(10, 155, 80, 30);
  addAndMakeVisible(m_lblEncoding);
  m_cbxEncoding.addItem("Terrarium", 1);
  m_cbxEncoding.addItem("TerrainRGB", 2);
  m_cbxEncoding.setBounds(100, 155, 100, 30);
  addAndMakeVisible(m_cbxEncoding);

  m_lblTileSize.setText(juce::translate("Tile size :"), juce::NotificationType::dontSendNotification);
  m_lblTileSize.setBounds(10, 190, 80, 30);
  addAndMakeVisible(m_lblTileSize);
  m_edtTileSize.setBounds(100, 190, 100, 30);
  addAndMakeVisible(m_edtTileSize);
  m_lblZoomMax.setText(juce::translate("Zoom max :"), juce::NotificationType::dontSendNotification);
  m_lblZoomMax.setBounds(10, 225, 80, 30);
  addAndMakeVisible(m_lblZoomMax);
  m_edtZoomMax.setBounds(100, 225, 100, 30);
  addAndMakeVisible(m_edtZoomMax);

  m_btnOK.setBounds(170, 260, 60, 30);
  m_btnOK.setButtonText("OK");
  addAndMakeVisible(m_btnOK);
  m_btnOK.addListener(this);
}

//-----------------------------------------------------------------------------
// DtmTmsComponent : changement des combo box
//-----------------------------------------------------------------------------
void DtmTmsComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) 
{ 
  if (comboBoxThatHasChanged != &m_cbxTitle)
    return;
  if (m_cbxTitle.getText() == "Mapterhorn") {
    m_edtServer.setText("tiles.mapterhorn.com", false);
    m_edtKey.setText("", false);
    m_edtTileSize.setText("512", false);
    m_edtZoomMax.setText("14", false);
    m_cbxEncoding.setSelectedId(1);
    m_cbxFormat.setSelectedId(2);
  }
  if (m_cbxTitle.getText() == "Mapzen") {
    m_edtServer.setText("s3.amazonaws.com/elevation-tiles-prod/terrarium", false);
    m_edtKey.setText("", false);
    m_edtTileSize.setText("256", false);
    m_edtZoomMax.setText("15", false);
    m_cbxEncoding.setSelectedId(1);
    m_cbxFormat.setSelectedId(1);
  }
  if (m_cbxTitle.getText() == "MapBox") {
    m_edtServer.setText("api.mapbox.com/v4/mapbox.terrain-rgb", false);
    m_edtKey.setText("", false);  // A remplir pour MapBox
    m_edtTileSize.setText("256", false);
    m_edtZoomMax.setText("14", false);
    m_cbxEncoding.setSelectedId(2);
    m_cbxFormat.setSelectedId(1);
  }
  if (m_cbxTitle.getText() == "MapTiler") {
    m_edtServer.setText("api.maptiler.com/tiles/terrain-rgb-v2", false);
    m_edtKey.setText("", false);  // A remplir pour MapTiler
    m_edtTileSize.setText("512", false);
    m_edtZoomMax.setText("12", false);
    m_cbxEncoding.setSelectedId(2);
    m_cbxFormat.setSelectedId(2);
  }
}

//-----------------------------------------------------------------------------
// DtmTmsComponent : reponse aux boutons
//-----------------------------------------------------------------------------
void DtmTmsComponent::buttonClicked(juce::Button* button)
{
  if ((button != &m_btnOK)||(m_Base == nullptr))
    return;
  juce::String server = m_edtServer.getText().toLowerCase();
  if (server.startsWith("https://"))
    server = server.replaceFirstOccurrenceOf("https://", "");
  if (server.endsWithChar('/'))
    server = server.dropLastCharacters(1);
  m_edtServer.setText(server);

  if ((!m_edtServer.isEmpty()) && (!m_edtTileSize.isEmpty()) && (!m_edtZoomMax.isEmpty())) {
    XGeoPref pref;
    XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
    pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
    pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

    DtmTmsLayer* dtm = new DtmTmsLayer(Server(), ApiKey(), Format(), Encoding(),
      (uint16_t)TileSize(), (uint16_t)TileSize(), (uint16_t)ZoomMax());

    dtm->SetFrame(F);
    if (!GeoTools::RegisterObject(m_Base, dtm, "Terrarium", "DTM", Server()))
      delete dtm;
    else 
      sendActionMessage("AddDtmLayer");
  }
  if (juce::DialogWindow* dw = findParentComponentOfClass<juce::DialogWindow>())
    dw->exitModalState(1);
}
