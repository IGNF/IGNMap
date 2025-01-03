//-----------------------------------------------------------------------------
//								WmtsLayer.h
//								===========
//
// Gestion des flux WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/11/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include <string>
#include "GeoBase.h"
#include "../../XTool/XParserXML.h"
#include "../../XTool/XFrame.h"
#include "../../XToolAlgo/XWmts.h"
#include "../../XToolGeod/XGeoProjection.h"

// Layer WMTS en WebMercator
class WmtsLayerWebMerc : public GeoInternetImage {
protected:
  juce::String m_strServer;
  juce::String m_strLayer;
  juce::String m_strFormat;
  juce::String m_strTileMatrixSet;
  juce::String m_strApiKey;
  uint32_t m_nTileW;
  uint32_t m_nTileH;
  uint32_t m_nMaxZoom;  // Niveau de zoom maximum de la pyramide

protected:
  juce::String LoadTile(int x, int y, int zoomlevel);

public:
  WmtsLayerWebMerc(std::string server, std::string layer, std::string TMS, std::string format = "png", uint32_t tileW = 256,
    uint32_t tileH = 256, uint32_t max_zoom = 19, std::string apikey = "");

  virtual	bool ReadAttributes(std::vector<std::string>& V);
  inline virtual double Resolution() const { return 6378137. * 2 * XPI / pow(2, (m_nMaxZoom + 8)); } // resolution max a l'Equateur

  bool LoadFrame(const XFrame& F, int zoomlevel);
  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
};

// Layer WMTS + Pyramide associee
class WmtsLayerTMS : public GeoInternetImage, public XWmtsLayerTMS {
protected:
  juce::String m_strServer;
  juce::String m_strApiKey;
  XGeoProjection::XProjCode m_ProjCode;

  juce::String LoadTile(uint32_t x, uint32_t y, std::string idLevel);

public:
  WmtsLayerTMS(juce::String server) { m_strServer = server; m_ProjCode = XGeoProjection::Unknown; }

  virtual std::string Name() { return m_strId; }
  virtual	bool ReadAttributes(std::vector<std::string>& V);
  virtual double Resolution() const { return XWmtsLayerTMS::Resolution(); }
  bool FindProjection();
  bool LoadFrame(const XFrame& F, int numMatrix);
  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
};