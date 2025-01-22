//-----------------------------------------------------------------------------
//								GeoBase.h
//								=========
//
// Encapsulation des classes XTool pour la gestion de donnees
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/01/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

#include "../../XTool/XFrame.h"
#include "../../XToolImage/XFileImage.h"
#include "../../XTool/XGeoVector.h"
#include "../../XTool/XGeoFDtm.h"
#include "../../XToolAlgo/XLasFile.h"
#include "../../XToolAlgo/XSentinelScene.h"

class XGeoBase;
class XGeoMap;

//==============================================================================
// Classe GeoImage : pour la gestion des images importees
//==============================================================================
class GeoImage : public XGeoVector {
protected:
  GeoImage() { ; }
public:
  virtual eTypeVector TypeVector() const { return XGeoVector::Raster; }
  virtual uint32_t NbPt() const { return 5; }
  virtual uint32_t NbPart() const { return 1; }
  virtual bool IsClosed() const { return true; }
  virtual XPt2D Pt(uint32_t i);
  virtual bool Intersect(const XFrame& F) { return m_Frame.Intersect(F); }
};

//==============================================================================
// Classe GeoFileImage : image liee a un fichier image (TIF, COG, JP2)
//==============================================================================
class GeoFileImage : public GeoImage, public XFileImage {
public:
  GeoFileImage() { ; }

  virtual bool AnalyzeImage(std::string path);
	virtual	bool ReadAttributes(std::vector<std::string>& V);
	virtual inline double Resolution() const { if (m_Image == nullptr) return 10.; return m_Image->GSD(); }
};

//==============================================================================
// Classe GeoInternetImage : image liee a un flux internet (WMTS)
//==============================================================================
class GeoInternetImage : public GeoImage {
protected :
  juce::File m_Cache;
  XFrame m_LastFrame;
  double m_LastGsd;
  juce::Image m_SourceImage;  // Image en WebMercator
  juce::Image m_ProjImage;    // Image en projection
  juce::String m_strRequest;

  void CreateCacheDir(juce::String name);
  bool SaveSourceImage(juce::String filename = "");
  virtual bool Resample(XTransfo* transfo);

public:
  GeoInternetImage() { m_LastGsd = 0.; }
  virtual ~GeoInternetImage() { m_Cache.deleteRecursively(); }
  void SetFrame(const XFrame& F) { m_Frame = F; }
  void SetDirty() { m_LastGsd = -1.; }  // On force le reaffichage de l'image

  virtual juce::Image& GetAreaImage(const XFrame& F, double scale) = 0;
};

//==============================================================================
// Classe GeoSentinelImage : image liee a une scene Sentinel
//==============================================================================
class GeoSentinelImage : public GeoImage, public XSentinelScene {
public:
  GeoSentinelImage() { ; }

  virtual std::string Name() { return m_strName; }
  bool ComputeFrame();
  virtual	bool ReadAttributes(std::vector<std::string>& V) { return SentinelAttributes(V); }
  virtual inline double Resolution() const { return GetActiveResolution(); }
  juce::Image GetQuickView();
};

//==============================================================================
// Classe GeoDTM : pour la gestion des MNT importes
//==============================================================================
class GeoDTM : public XGeoFDtm {
public:
  GeoDTM() : XGeoFDtm() { ; }
  virtual ~GeoDTM() { Close(); }
  virtual void Close();

  virtual bool StreamReady();
  virtual bool ReadLine(float* line, uint32_t numLine);
  virtual bool ReadNode(float* node, uint32_t x, uint32_t y);
  virtual bool ReadAll(float* area);

  virtual bool ImportTif(std::string file_tif, std::string file_bin);

protected:
  XFileImage     m_Image;
};

//==============================================================================
// Classe GeoLAS : pour la gestion des LAS importes
//==============================================================================
class GeoLAS : public XGeoVector, public XLasFile {
public:
  GeoLAS() {
    m_ZRange[0] = m_ZRange[1] = XGEO_NO_DATA;
  }
  bool Open(std::string filename);

  virtual eTypeVector TypeVector() const { return XGeoVector::LAS; }
  virtual	bool ReadAttributes(std::vector<std::string>&);
  virtual std::string Filename() { return m_strFilename; }
  virtual inline double* ZRange() { return m_ZRange; }
  virtual inline double Zmin() const { return m_ZRange[0]; }
  virtual inline double Zmax() const { return m_ZRange[1]; }

protected:
  double m_ZRange[2];
};

//==============================================================================
// Fonctions de recherche de fichiers de georeferencement
//==============================================================================
namespace GeoTools {
  bool FindGeorefTab(std::string filename, double* Xmin, double* Ymax, double* GSD);
  bool FindGeorefTfw(std::string filename, double* Xmin, double* Ymax, double* GSD);
}

//==============================================================================
// Fonctions d'import des donnees vectorielles en fonction des formats
//==============================================================================
namespace GeoTools {
  bool ImportVectorFolder(juce::String folderName, XGeoBase* base, int& nb_total, int& nb_imported);
  bool ImportShapefile(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
  bool ImportGeoPackage(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
  bool ImportMifMid(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
  bool ImportDxf(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
  bool ImportGeoJson(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
  bool ImportTA(juce::String fileName, XGeoBase* base, XGeoMap* map = nullptr);
	XGeoClass* ImportDataFolder(juce::String folderName, XGeoBase* base, XGeoVector::eTypeVector type);
}

//==============================================================================
// Fonctions utilitaires
//==============================================================================
namespace GeoTools {
  bool RegisterObject(XGeoBase* base, XGeoVector* V, std::string mapName, std::string layerName, std::string className,
                      int transparency = 0, uint32_t color = 0xFFFFFFFF, uint32_t fill = 0xFFFFFFFF, uint32_t zorder = 0, uint8_t size = 1);
  void ColorizeClasses(XGeoBase* base);
  juce::File CreateCacheDir(juce::String name);
  void UpdateProjection(XGeoBase* base);
  bool ComputeZGrid(XGeoBase* base, float* grid, uint32_t w, uint32_t h, XFrame* F);
  bool AddImageInObect(XGeoBase* base, int index);
  bool AddRotationImage(XGeoBase* base, juce::String filename, double rot = 0., double Xc = 0., double Yc = 0., double gsd = 1.);
}

//==============================================================================
// Traitement long
//==============================================================================
class GeoTask : public juce::ThreadWithProgressWindow, public XWait {
public:
	GeoTask() : ThreadWithProgressWindow("busy...", true, true) {;}
	
	virtual void SetRange(int min, int max) { m_nMin = min ; m_nMax = max;}
	virtual void SetStep(int step) { m_nStep = step;}
	virtual void StepIt() { m_nStep++; setProgress((double)(m_nMax - m_nMin) / m_nStep);}
	virtual void SetStatus(const char* s) {setStatusMessage(s);}
	virtual void Cancel() { m_bCancel = true; stopThread(100);}
	virtual bool CheckCancel() { return threadShouldExit();}
};
