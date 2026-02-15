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

  m_dZmin = 0.;
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
	return false;
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
  //(red * 256 + green + blue / 256) - 32768
  float* ptr_grid = grid;
  for (int i = 0; i < data.height; i++) {
    uint8_t* line = data.getLinePointer(i);
    for (int j = 0; j < data.width; j++) {
      *ptr_grid = (float)((line[2] * 256. + line[1] + line[0] / 256.) - 32768.);
      ptr_grid++;
      line += data.pixelStride;
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

  for (int i = 0; i < nb_tiley; i++) {
    for (int j = 0; j < nb_tilex; j++) {
      int x = firstX + j;
      int y = firstY + i;
      juce::Image image = FindCachedTile(x, y, zoomlevel);
      if (image.isNull()) {
        juce::String filename = LoadTile(x, y, zoomlevel);
        image = juce::ImageFileFormat::loadFrom(juce::File(filename));
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
  FwebMerc.Xmin = XMin(x0, x1);
  FwebMerc.Xmax = XMax(x2, x3);
  FwebMerc.Ymin = XMin(y0, y3);
  FwebMerc.Ymax = XMax(y1, y2);

  /*
  LoadFrame(FwebMerc, zoom_level);
  int wscaled = (int)(FwebMerc.Width() / gsd), hscaled = (int)(FwebMerc.Height() / gsd);
  float* scaledBuf = new float[wscaled * hscaled];
  XBaseImage::FastZoomBil(m_SourceGrid, m_SourceW, m_SourceH, scaledBuf, wscaled, hscaled);
  delete[] m_SourceGrid;
  m_SourceW = m_SourceH = 0;
  m_SourceGrid = nullptr;

  // Reechantillonage dans la projection souhaitee
  XTransfoGeodInterpol transfo(&geod);
  transfo.SetStartFrame(FwebMerc);
  transfo.SetEndFrame(*F);
  transfo.SetResolution(gsd);
  transfo.AutoCalibration();
  
  XInterpol interpol;
  XBaseImage::Resample(scaledBuf, grid, wscaled, hscaled, 1, 0, &transfo, &interpol, false);
  delete[] scaledBuf;
  */

  LoadFrame(FwebMerc, zoom_level);

  // Reechantillonage dans la projection souhaitee
  XTransfoGeodZoomInterpol transfo(&geod);
  transfo.SetStartFrame(FwebMerc, FwebMerc.Width() / m_SourceW);
  transfo.SetEndFrame(*F, gsd);
  transfo.AutoCalibration();

  XInterpol interpol;
  XBaseImage::Resample(m_SourceGrid, grid, m_SourceW, m_SourceH, 1, 0, &transfo, &interpol, false);
  delete[] m_SourceGrid;
  m_SourceW = m_SourceH = 0;
  m_SourceGrid = nullptr;

	return true;
}
