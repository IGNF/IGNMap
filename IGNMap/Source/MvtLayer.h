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

//-----------------------------------------------------------------------------
// Style d'un layer MVT
//-----------------------------------------------------------------------------
class MvtStyleLayer {
public:
  enum eLayerType { noType, fill, line, symbol, circle };
  enum eFilterType { noFilter, equal, in, diff };

  MvtStyleLayer() { Clear(); }

  void Clear();
  bool Read(const juce::var& layer);
  bool ReadFilterArray(const juce::var& filter);
  bool ReadFilter(const juce::var& filter);
  bool TestZoomLevel(int zoomlevel) const;
  eLayerType Type() { return m_Type; }

  juce::String SourceLayer() { return m_SourceLayer; }
  int NbFilter() { return (int)m_Filter.size(); }
  eFilterType FilterType(int i) { if (i < NbFilter()) return m_Filter[i].Type; return noFilter; }
  juce::String FilterAtt(int i) { if (i < NbFilter()) return m_Filter[i].Att; return ""; }
  bool TestAtt(int num, juce::String val);
  juce::String TextField() const { return m_TextField; }
  int TextSize() const { return m_TextSize; }

  bool SetStyle(int zoomlevel, juce::Colour* pen, juce::FillType* fill, float* lineWidth);
  int NbLineDash() const { return m_LineDash.size(); }
  float* LineDash() { return m_LineDash.data(); }

protected:
  eLayerType     m_Type;
  juce::String  m_SourceLayer;
  int           m_MinZoom;
  int           m_MaxZoom;
  bool          m_Visibility;
  std::vector<juce::Colour>  m_LineColor;
  std::vector<int> m_LineStops;
  std::vector<juce::Colour>  m_TextColor;
  std::vector<int> m_TextStops;
  std::vector<juce::Colour>  m_OutlineColor;
  std::vector<int> m_OutlineStops;
  std::vector<juce::FillType>  m_FillType;
  std::vector<int> m_FillStops;
  std::vector<float> m_Width;
  std::vector<int> m_WidthStops;
  std::vector<float> m_LineDash;

  juce::String m_TextField;
  int m_TextSize;

  typedef struct {
    eFilterType  Type;
    juce::String Att;
    std::vector<juce::String> Val;
  } Filter;
  std::vector<Filter> m_Filter;

  static bool ReadExpression(const juce::var& expr, std::vector<float>& T, std::vector<int>& stops);
  
};

//-----------------------------------------------------------------------------
// Tile MVT
//-----------------------------------------------------------------------------
class MvtTile {
protected:
  std::string m_strFilename;
  char* m_Buffer;
  size_t m_nLength;
  bool m_bLoaded;
  bool m_bPrepared;
  double m_dX0;
  double m_dY0;
  double m_dGsd;
  vtzero::vector_tile* m_Tile;
  juce::Rectangle<int> m_ClipR;

public:
  MvtTile() {
    m_Buffer = nullptr; m_bLoaded = m_bPrepared = false; m_dX0 = m_dY0 = m_dGsd = 0.; m_nLength = 0; m_Tile = nullptr;}
  ~MvtTile() { Clear(); }
  void Clear();

  bool Load(juce::String filename, double X0, double Y0, double GSD0);
  bool PrepareForDrawing(const XFrame& F, const double& gsd, const uint32_t& tileW, const uint32_t& tileH);

  inline bool IsLoaded() const { return m_bLoaded; }
  inline bool IsPrepared() const { return m_bPrepared; }
  inline double X0() const { return m_dX0; }
  inline double Y0() const { return m_dY0; }
  inline double GSD() const { return m_dGsd; }
  inline size_t Length() const { return m_nLength; }
  inline const char* Buffer() const { return m_Buffer; }
  juce::Rectangle<int> ClipR() const { return m_ClipR; }

  vtzero::vector_tile* Tile() { return m_Tile; }
};

//-----------------------------------------------------------------------------
// Layer MVT
//-----------------------------------------------------------------------------
class MvtLayer : public GeoInternetImage {
protected:
  juce::String m_strServer;
  juce::String m_strFormat;
  uint32_t m_nTileW;
  uint32_t m_nTileH;
  uint32_t m_nMaxZoom;  // Niveau de zoom maximum de la pyramide
  uint32_t m_nLastZoom; // Dernier niveau de zoom utilise
  juce::var m_StyleLayers;
  juce::Colour m_LineColor;
  juce::Colour m_FillOutlineColor;
  juce::Colour m_FillColor;
  juce::Colour m_TextColor;
  juce::String m_TextAtt;
  float m_LineWidth;
  bool m_bPaintProperties;
  std::vector< MvtStyleLayer> m_Layer;

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

  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
  bool LoadStyle(juce::String server);
  
  bool DrawWithStyle(const XFrame& F, int zoomlevel);

  bool DrawMvt(MvtTile* T, MvtStyleLayer* layer);
};



