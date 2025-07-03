//-----------------------------------------------------------------------------
//								MvtLayer.h
//								==========
//
// Gestion des flux de donnees Mapbox Vector Tile
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/06/2025
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>
#include <string>
#include "GeoBase.h"
#include "../../XTool/XFrame.h"
#include "vtzero/vector_tile.hpp"

class XTransfo;

// Layer MVT
class MvtLayer : public GeoInternetImage {
protected:
  juce::String m_strServer;
  juce::String m_strFormat;
  uint32_t m_nTileW;
  uint32_t m_nTileH;
  uint32_t m_nMaxZoom;  // Niveau de zoom maximum de la pyramide
  uint32_t m_nLastZoom; // Dernier niveau de zoom utilise
  juce::var m_StyleLayers;

protected:
  juce::String LoadTile(int x, int y, int zoomlevel);

public:
  MvtLayer(std::string server, std::string format = "mvt", uint32_t tileW = 256,
    uint32_t tileH = 256, uint32_t max_zoom = 19);
  virtual ~MvtLayer() { ; }

  struct geom_handler;

  virtual	bool ReadAttributes(std::vector<std::string>& V);
  inline double Swath(uint32_t zoom_level) const { return 6378137. * 2 * XPI / pow(2, zoom_level);} // Largeur d'une dalle a l'Equateur
  inline double Resol(uint32_t zoom_level) const { return Swath(zoom_level) / m_nTileW; }
  inline virtual double Resolution() const { return Swath(m_nMaxZoom) / m_nTileW; } // Resolution max a l'Equateur

  bool LoadFrameProj(const XFrame& F, int zoomlevel);
  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
  bool LoadMvt(juce::String filename, double X0, double Y0, double GSD0);
  bool LoadStyle(juce::String server);
  bool FindStyle(juce::String layername, vtzero::feature* feature, juce::Colour* pen, juce::Colour* fill, float* line_width);
};
