//==============================================================================
//    OsmLayer.h
//    Created: 10 Nov 2023 8:21:26am
//    Author:  FBecirspahic
//    Gestion des flux de donnees OSM raster
//==============================================================================


#pragma once
#include <JuceHeader.h>
#include <string>
#include "GeoBase.h"
#include "../../XTool/XParserXML.h"
#include "../../XTool/XFrame.h"

class XTransfo;

// Gestion des serveurs OSM
class OsmServer {
protected:
  std::string   m_strName;
  std::string   m_strKey;
  std::string   m_strFormat;
  uint16_t    m_nW;
  uint16_t    m_nH;
  uint16_t    m_nZoom;

public:
  OsmServer(const char* name)
  {
    m_strName = name; m_strFormat = "png"; m_nW = m_nH = 256; m_nZoom = 19;
  }

  inline std::string Name() const { return m_strName; }
  inline std::string Key() const { return m_strKey; }
  inline std::string Format() const { return m_strFormat; }
  inline uint16_t W() const { return m_nW; }
  inline uint16_t H() const { return m_nH; }
  inline uint16_t Zoom() const { return m_nZoom; }

  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
};

// Layer OSM
class OsmLayer : public GeoInternetImage {
protected:
  juce::String m_strServer;
  juce::String m_strKey;
  juce::String m_strFormat;
  uint32_t m_nTileW;
  uint32_t m_nTileH;
  uint32_t m_nMaxZoom;  // Niveau de zoom maximum de la pyramide
 
protected:
  juce::String LoadTile(int x, int y, int zoomlevel);

public:
  OsmLayer(std::string server, std::string apikey = "", std::string format = "png", uint32_t tileW = 256,
    uint32_t tileH = 256, uint32_t max_zoom = 19);
  virtual ~OsmLayer() { ; }

  void SetFrame(const XFrame& F) { m_Frame = F; }
  virtual	bool ReadAttributes(std::vector<std::string>& V);
  inline virtual double Resolution() const { return 6378137. * 2 * XPI / pow(2, (m_nMaxZoom + 8)); } // resolution max a l'Equateur

  bool LoadFrame(const XFrame& F, int zoomlevel);
  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
};