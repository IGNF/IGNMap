//-----------------------------------------------------------------------------
//								XWmts.h
//								=======
//
// Gestion des flux WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 15/11/2012
//-----------------------------------------------------------------------------

#ifndef XWMTS_H
#define XWMTS_H

#include "../XTool/XBase.h"
#include <vector>

class XParserXML;

//-----------------------------------------------------------------------------
// XTileMatrix
//-----------------------------------------------------------------------------
class XTileMatrix {
protected:
  std::string   m_strId;
  double        m_dScaleDenom;
  double        m_dXTL;         // X Top Left
  double        m_dYTL;         // Y Top Left
  uint32_t      m_nTileW;
  uint32_t      m_nTileH;
  uint32_t      m_nMatrixW;
  uint32_t      m_nMatrixH;

public:
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
  inline std::string Id() const { return m_strId; }
  inline uint32_t TileW() const { return m_nTileW; }
  inline uint32_t TileH() const { return m_nTileH; }
  inline uint32_t MatrixW() const { return m_nMatrixW; }
  inline uint32_t MatrixH() const { return m_nMatrixH; }
  inline double GSD() const { return m_dScaleDenom * 0.00028; }
  inline double XTL() const { return m_dXTL; }
  inline double YTL() const { return m_dYTL; }

  static bool predTileMatrix(const XTileMatrix& A, const XTileMatrix& B)
  {
    return (A.GSD() > B.GSD());
  }
};

//-----------------------------------------------------------------------------
// XTileMatrixSet
//-----------------------------------------------------------------------------
class XTileMatrixSet {
protected:
  std::string   m_strId;
  std::string   m_strCrs;
  std::vector<XTileMatrix> m_T;

public:
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
  std::string Id() { return m_strId; }
  std::string Crs() { return m_strCrs; }
  std::vector<double> GSD();
  int NbTile() { return (int)m_T.size(); }
  XTileMatrix Tile(int i) { return m_T[i]; }
  std::vector<XTileMatrix> TileMatrix() { return m_T; }
};

//-----------------------------------------------------------------------------
// XTileMatrixLimits
//-----------------------------------------------------------------------------
class XTileMatrixLimits {
protected:
  std::string m_strId;
  uint32_t    m_nMinTileRow = 0;
  uint32_t    m_nMaxTileRow = 0xFFFFFFFF;
  uint32_t    m_nMinTileCol = 0;
  uint32_t    m_nMaxTileCol = 0xFFFFFFFF;
public:
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
  inline uint32_t MinTileCol() const { return m_nMinTileCol; }
  inline uint32_t MaxTileCol() const { return m_nMaxTileCol; }
  inline uint32_t MinTileRow() const { return m_nMinTileRow; }
  inline uint32_t MaxTileRow() const { return m_nMaxTileRow; }
  inline std::string Id() const { return m_strId; }
  static bool predTileMatrixLimits(const XTileMatrixLimits& A, const XTileMatrixLimits& B)
  {
    return (A.Id() > B.Id());
  }
};

//-----------------------------------------------------------------------------
// XTileMatrixSetLink
//-----------------------------------------------------------------------------
class XTileMatrixSetLink {
protected:
  std::string   m_strTileMatrixSetId;
  std::vector<XTileMatrixLimits> m_L;

public:
  std::string Id() { return m_strTileMatrixSetId; }
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
  int NbLimits() { return (int)m_L.size(); }
  XTileMatrixLimits Limits(int i) { return m_L[i]; }
};

//-----------------------------------------------------------------------------
// XWmtsLayer
//-----------------------------------------------------------------------------
class XWmtsLayer {
  friend class XWmtsCapabilities;
protected:
  std::string   m_strTitle;
  std::string   m_strAbstract;
  std::string   m_strId;
  std::string   m_strFormat;
  std::vector<XTileMatrixSetLink> m_SetLink;

public:
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
};

//-----------------------------------------------------------------------------
// XWmtsLayerTMS
//-----------------------------------------------------------------------------
class XWmtsLayerTMS {
  friend class XWmtsCapabilities;
protected:
  std::string   m_strTitle;
  std::string   m_strAbstract;
  std::string   m_strId;
  std::string   m_strFormat;
  XTileMatrixSet m_TMS;
  std::vector<XTileMatrixLimits> m_Limits;

public:
  
};

//-----------------------------------------------------------------------------
// XWmtsCapabilities
//-----------------------------------------------------------------------------
class XWmtsCapabilities {
protected:
  std::vector<XWmtsLayer>       m_Layer;
  std::vector< XTileMatrixSet>  m_MatrixSet;

public:
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);

  bool SetLayerTMS(XWmtsLayerTMS* layer, std::string layerId, std::string tmsId);
};

#endif //XWMTS_H
