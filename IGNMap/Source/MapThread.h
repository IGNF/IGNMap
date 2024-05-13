//-----------------------------------------------------------------------------
//								MapThread.h
//								===========
//
// Thread de dessin des objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 06/01/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "GeoBase.h"

class XGeoBase;
class XGeoClass;
class XGeoVector;

class MapThread : public juce::Thread {
public:
  MapThread(const juce::String& threadName, size_t threadStackSize = 0);
  virtual ~MapThread() { ; }

  void SetWorld(const double& X0, const double& Y0, const double& gsd, const int& W, const int& H, bool force_vector);
  void SetGeoBase(XGeoBase* base) { m_GeoBase = base; }
  void SetUpdate(bool overlay, bool raster, bool dtm, bool vector, bool las);
  bool NeedUpdate() { return m_bRaster; }

  juce::int64 NumObjects() { return m_nNumObjects; }
  XFrame Frame() { return m_Frame; }
  float GetZ(int u, int v);
  uint32_t ImageWidth() { return m_Raster.getWidth(); }
  uint32_t ImageHeight() { return m_Raster.getHeight(); }

  virtual void 	run() override;
  bool Draw(juce::Graphics& g, int x0 = 0, int y0 = 0);

private:
  juce::Image m_Raster;
  juce::Image m_Vector;
  juce::Image m_Overlay;
  juce::Image m_Dtm;
  juce::Image m_RawDtm;
  juce::Image m_Las;
  XGeoBase* m_GeoBase;
  double        m_dX0, m_dY0, m_dGsd; // Transformation terrain -> pixel
  bool          m_bRaster, m_bVector, m_bOverlay, m_bDtm, m_bLas; // Couches a dessiner
  bool          m_bRasterDone, m_bFirstRaster;
  juce::Path    m_Path;
  bool          m_bFill;        // Indique que le path doit etre rempli
  juce::int64   m_nNumObjects;  // Nombre d'objets affiches dans la vue
  XFrame        m_Frame;
  juce::Rectangle<int>  m_ClipVector;
  juce::Rectangle<int>  m_ClipRaster;
  juce::Rectangle<int>  m_ClipLas;

  bool AllocPoints(int numPt);
  void SetDimension(const int& w, const int& h);
  void PrepareImages(bool totalUpdate, int dX = 0, int dY = 0);

  void DrawVectorClass(XGeoClass* C);
  bool DrawGeometry(XGeoVector* V);
  bool DrawText(juce::Graphics* g, XGeoVector* V);
  bool DrawCentroide(XGeoVector* G);
  bool DrawPoint(XGeoVector* G);
  bool DrawPolyline(XGeoVector* G);
  bool DrawPolygon(XGeoVector* G);
  bool DrawMultiLine(XGeoVector* G);
  bool DrawMultiPolygon(XGeoVector* G);
  bool DrawMultiPoint(XGeoVector* G);
  
  bool DrawRasterClass(XGeoClass* C);
  bool DrawFileRaster(GeoFileImage* image);
  bool DrawInternetRaster(GeoInternetImage* image);
  bool DrawDtmClass(XGeoClass* C);
  bool DrawDtm(GeoDTM* poDataset);
  bool PrepareRasterDraw(XFileImage* image, int& U0, int& V0, int& win, int& hin, int& nbBand,
    int& R0, int& S0, int& wout, int& hout);

  bool DrawLasClass(XGeoClass* C);
  bool DrawLas(GeoLAS* las);

  void DrawSelection();
};