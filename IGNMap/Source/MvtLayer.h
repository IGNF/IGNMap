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

// Tile MVT
class MvtTile {
protected:
  std::string m_strFilename;
  char* m_Buffer;
  size_t m_nLength;
  bool m_bLoaded;
  double m_dX0;
  double m_dY0;
  double m_dGsd;
  vtzero::vector_tile* m_Tile;

public:
  MvtTile() {
    m_Buffer = nullptr; m_bLoaded = false; m_dX0 = m_dY0 = m_dGsd = 0.; m_nLength = 0; m_Tile = nullptr;}
  ~MvtTile() { Clear(); }
  void Clear();

  bool Load(juce::String filename, double X0, double Y0, double GSD0);

  inline bool IsLoaded() const { return m_bLoaded; }
  inline double X0() const { return m_dX0; }
  inline double Y0() const { return m_dY0; }
  inline double GSD() const { return m_dGsd; }
  inline size_t Length() const { return m_nLength; }
  inline const char* Buffer() const { return m_Buffer; }

  vtzero::vector_tile* Tile() { return m_Tile; }
};

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
  juce::Colour m_PenColor;
  juce::Colour m_FillColor;
  juce::String m_TextAtt;
  float m_LineWidth;
  bool m_Repres;

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
  bool FindStyle(juce::String layername, vtzero::feature* feature, juce::Colour* pen, juce::Colour* fill, float* line_width, juce::String* text);

  bool DrawWithStyle(const XFrame& F, int zoomlevel);
  bool LoadMvt(MvtTile* T, double X0, double Y0, double GSD0, juce::var& style);
  bool ReadStylePaint(juce::var& layer, juce::Colour* pen, juce::Colour* fill, float* line_width, juce::String* text) const;
  bool ReadStylePaint(juce::var& layer);
};
