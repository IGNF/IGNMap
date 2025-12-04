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
#include "../../XToolAlgo/XAffinity2D.h"
#include "vtzero/vector_tile.hpp"

class XTransfo;

//-----------------------------------------------------------------------------
// Sprite MVT
//-----------------------------------------------------------------------------
typedef struct { // Sprite MVT
  juce::String  m_Name;
  juce::Image m_Image;
} MvtSprite;

//-----------------------------------------------------------------------------
// Style d'un layer MVT
//-----------------------------------------------------------------------------
class MvtStyleLayer {
public:
  enum eLayerType { noType, fill, line, symbol, circle };
  enum eFilterType { noFilter, equal, in, diff };

  MvtStyleLayer() { Clear(); }

  void Clear();
  bool Read(const juce::var& layer, std::vector<MvtSprite>* sprite);
  bool ReadFilterArray(const juce::var& filter);
  bool ReadFilter(const juce::var& filter);
  int MinZoomLevel() const { return m_MinZoom; }
  int MaxZoomLevel() const { return m_MaxZoom; }
  eLayerType Type() { return m_Type; }

  juce::String SourceLayer() { return m_SourceLayer; }
  int NbFilter() { return (int)m_Filter.size(); }
  eFilterType FilterType(int i) { if (i < NbFilter()) return m_Filter[i].Type; return noFilter; }
  juce::String FilterAtt(int i) { if (i < NbFilter()) return m_Filter[i].Att; return ""; }
  bool TestAtt(int num, juce::String val);
  juce::String TextField() const { return m_TextField; }
  juce::Image Icon() const { return m_Icon; }

  bool SetStyle(int zoomlevel, juce::Colour* pen, juce::FillType* fill, float* lineWidth, float* iconSize, float* radius,
                float* textSize, juce::Colour* halo, float* opacity);
  int NbLineDash() const { return (int)m_LineDash.size(); }
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
  std::vector<juce::Colour>  m_TextHaloColor;
  std::vector<int> m_TextHaloStops;
  std::vector<juce::Colour>  m_OutlineColor;
  std::vector<int> m_OutlineStops;
  std::vector<juce::FillType>  m_FillType;
  std::vector<int> m_FillStops;
  std::vector<float> m_Width;
  std::vector<int> m_WidthStops;
  std::vector<float> m_Opacity;
  std::vector<int> m_OpacityStops;
  std::vector<float> m_LineDash;

  juce::String m_TextField;
  std::vector<float> m_TextSize;
  std::vector<int> m_TextSizeStops;
  juce::Image m_Icon;
  std::vector<float> m_IconSize;
  std::vector<int> m_IconSizeStops;
  std::vector<float> m_CircleRadius;
  std::vector<int> m_CircleRadiusStops;

  typedef struct {
    eFilterType  Type;
    juce::String Att;
    std::vector<juce::String> Val;
  } Filter;
  std::vector<Filter> m_Filter;

  bool ReadColorValue(const juce::var& paint, std::vector<juce::Colour>& Value, std::vector<int>& Stops, const char* field);
  bool ReadFillValue(const juce::var& paint, std::vector<juce::FillType>& Value, std::vector<int>& Stops, const char* field);
  bool ReadFloatValue(const juce::var& paint, std::vector<float>& Value, std::vector<int>& Stops, const char* field);

  static bool ReadExpression(const juce::var& expr, std::vector<float>& T, std::vector<int>& stops);
  static juce::Colour ReadColour(const juce::String& str);

  template<typename T> static T ReadStopVal(int zoomlevel, const std::vector<T>& Val, const std::vector<int>& Stops, T default_value);
  template<typename T> static T ReadStopValInterpol(int zoomlevel, const std::vector<T>& Val, const std::vector<int>& Stops, T default_value);
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
  XAffinity2D m_Affinity;

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
  void Tile2Ground(const double& x, const double& y, double& u, double& v) { m_Affinity.Direct(XPt2D(x, y), u, v); }

  vtzero::vector_tile* Tile() { return m_Tile; }
};

//-----------------------------------------------------------------------------
// Layer MVT
//-----------------------------------------------------------------------------
class MvtLayer : public GeoInternetImage {
protected:
  juce::String m_strTitle;
  juce::String m_strServer;
  juce::String m_strFormat;
  XFrame m_F;		// Cadre dans la projection native TMS
  bool m_bTrueProjection; // Indique que l'on fait une vraie reprojection geodesique
  uint32_t m_nTileW;
  uint32_t m_nTileH;
  uint32_t m_nMinZoom;  // Niveau de zoom minimum de la pyramide
  uint32_t m_nMaxZoom;  // Niveau de zoom maximum de la pyramide
  uint32_t m_nLastZoom; // Dernier niveau de zoom utilise
  std::vector<juce::String> m_StyleFiles;
  juce::var m_StyleLayers;
  
  std::vector<MvtStyleLayer> m_Layer;
  std::vector<MvtSprite> m_Sprite;
  juce::Image m_SpriteImage;

protected:
  juce::String LoadTile(int x, int y, int zoomlevel);

public:
  MvtLayer() { m_nTileW = m_nTileH = 256; m_nMinZoom = m_nMaxZoom = m_nLastZoom = 0; m_bTrueProjection = false; }
  MvtLayer(std::string server, std::string format = "mvt", uint32_t tileW = 256,
    uint32_t tileH = 256, uint32_t max_zoom = 19);
  virtual ~MvtLayer() { ; }

  bool ReadServer(juce::String serverUrl);

  struct geom_handler;

  virtual	bool ReadAttributes(std::vector<std::string>& V);
  inline double Swath(uint32_t zoom_level) const { return 6378137. * 2 * XPI / pow(2, zoom_level);} // Largeur d'une dalle a l'Equateur
  inline double Resol(uint32_t zoom_level) const { return Swath(zoom_level) / m_nTileW; }
  inline virtual double Resolution() const { return Swath(m_nMaxZoom) / m_nTileW; } // Resolution max a l'Equateur

  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
  bool DrawWithStyle(const XFrame& F, int zoomlevel);
  bool Draw(const XFrame& F, int zoomlevel);
  bool DrawMvt(MvtTile* T, MvtStyleLayer* layer);
  bool DrawMvt(MvtTile* T);

  bool LoadStyle(juce::String server);
  bool ReadSprite(juce::String server);
  void GetStyleFiles(juce::StringArray& A) { for (int i = 0; i < m_StyleFiles.size(); i++) A.add(m_StyleFiles[i]); }
};



