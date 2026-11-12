//-----------------------------------------------------------------------------
//								XGeoParquet.h
//								=============
//
// Gestion du format GeoParquet
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/11/2026
//-----------------------------------------------------------------------------

#ifndef XGEOPARQUET_H
#define XGEOPARQUET_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoPoly.h"
#include "../XTool/XFile.h"

#include "../XToolGeod/XGeoProjection.h"

#include <carquet/carquet.h>

class XGeoBase;
class XGeoParquetVector;

class XGeoParquetMap : public XGeoMap {
protected:
  std::string m_strFilename;
  XGeoProjection::XProjCode m_ProjCode;
  uint64_t m_FeatureOffset = 0;
  carquet_reader_t* m_Reader = nullptr;
  int64_t m_nTotalRows = 0;
  int32_t m_nNumCols = 0;
  int32_t m_nNumRgs = 0;
  std::string m_strGeomCol;
  int32_t m_nGeomCol = -1;
  int32_t m_nXminCol = -1;
  int32_t m_nXmaxCol = -1;
  int32_t m_nYminCol = -1;
  int32_t m_nYmaxCol = -1;
  std::vector<std::string> m_Attribut;
  int32_t m_nActiveRowGroup = -1;
  int64_t m_nNbGeom = 0;
  carquet_column_reader_t* m_GeomColReader = nullptr;
  carquet_byte_array_t* m_GeometryBuf = nullptr;

public:
  XGeoParquetMap() { m_ProjCode = XGeoProjection::WebMercator; }
  virtual ~XGeoParquetMap();
  void ClearGeomReader();

  bool OpenFile(const char* filename);
  bool Read(XGeoBase* base, XWait* wait = nullptr);

  bool ReadAttributes(int64_t offset, std::vector<std::string>& V, XGeoClass* C);
  bool LoadGeoParquetGeom(int64_t offset, XGeoParquetVector* V, bool needZ);
  bool LoadGeom(int64_t offset, XGeoParquetVector* V) { return LoadGeoParquetGeom(offset, V, true); }
  bool LoadGeom2D(int64_t offset, XGeoParquetVector* V) { return LoadGeoParquetGeom(offset, V, false); }

  void Class(XGeoClass* C);
  XGeoClass* Class();

  static XGeoClass* ImportGeoParquet(XGeoBase* base, const char* path, XGeoMap* map = nullptr);
};

//-----------------------------------------------------------------------------
// Vecteur GeoParquet
//-----------------------------------------------------------------------------
class XGeoParquetVector {
protected:
  XGeoParquetMap* m_Map;
  int64_t   m_Offset;

public:
  XGeoParquetVector() { m_Map = nullptr; m_Offset = 0; }
  virtual ~XGeoParquetVector() { ; }
  virtual inline XGeoClass* GeoClass() const { return nullptr; }
  virtual void SetGeom(int /*nb*/, XPt* /*P*/, double* /*Z*/, int /*nbparts*/, int* /*parts*/,
    double /*zmin*/, double /*zmax*/) {
    ;
  }
};

//-----------------------------------------------------------------------------
// Point 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetPoint2D : public XGeoPoint2D, public XGeoParquetVector {
public:
  XGeoParquetPoint2D(XGeoParquetMap* map, int64_t offset, XFrame& F) { m_Map = map; m_Offset = offset; m_Frame = F; }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual inline XGeoClass* GeoClass() const { return Class(); }
};

//-----------------------------------------------------------------------------
// Point 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetPoint3D : public XGeoPoint3D, public XGeoParquetVector {
public:
  XGeoParquetPoint3D(XGeoParquetMap* map, int64_t offset, XFrame& F, double z)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_Z = z;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual inline XGeoClass* GeoClass() const { return Class(); }
};

//-----------------------------------------------------------------------------
// Multi-Point 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMPoint2D : public XGeoMPoint2D, public XGeoParquetVector {
public:
  XGeoParquetMPoint2D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double*, int, int*, double, double)
  {
    m_nNumPoints = nb; m_Pt = P;
  }
};

//-----------------------------------------------------------------------------
// Multi-Point 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMPoint3D : public XGeoMPoint3D, public XGeoParquetVector {
public:
  XGeoParquetMPoint3D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map; m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr; m_Z = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* Z, int, int*, double zmin, double zmax)
  {
    m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;
  }
};

//-----------------------------------------------------------------------------
// Ligne 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetLine2D : public XGeoLine2D, public XGeoParquetVector {
public:
  XGeoParquetLine2D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map; m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double*, int, int*, double, double)
  {
    m_nNumPoints = nb; m_Pt = P;
  }
};

//-----------------------------------------------------------------------------
// Ligne 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetLine3D : public XGeoLine3D, public XGeoParquetVector {
public:
  XGeoParquetLine3D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr; m_Z = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* Z, int, int*, double zmin, double zmax)
  {
    m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;
  }
};

//-----------------------------------------------------------------------------
// Multi-Ligne 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMLine2D : public XGeoMLine2D, public XGeoParquetVector {
public:
  XGeoParquetMLine2D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = nbparts; m_Pt = nullptr; m_Parts = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* /*Z*/, int nbparts, int* parts, double, double)
  {
    m_nNumPoints = nb; m_Pt = P; m_nNumParts = nbparts; m_Parts = parts;
  }
};

//-----------------------------------------------------------------------------
// Multi-Ligne 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMLine3D : public XGeoMLine3D, public XGeoParquetVector {
public:
  XGeoParquetMLine3D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = nbparts;
    m_Pt = nullptr; m_Parts = nullptr; m_Z = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* Z, int nbparts, int* parts, double zmin, double zmax)
  {
    m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_nNumParts = nbparts; m_Parts = parts; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;
  }
};

//-----------------------------------------------------------------------------
// Polygone 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetPoly2D : public XGeoPoly2D, public XGeoParquetVector {
public:
  XGeoParquetPoly2D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double*, int, int*, double, double)
  {
    m_nNumPoints = nb; m_Pt = P;
  }
};

//-----------------------------------------------------------------------------
// Polygone 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetPoly3D : public XGeoPoly3D, public XGeoParquetVector {
public:
  XGeoParquetPoly3D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_Pt = nullptr; m_Z = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* Z, int, int*, double zmin, double zmax)
  {
    m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;
  }
};

//-----------------------------------------------------------------------------
// Multi-Polygon 2D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMPoly2D : public XGeoMPoly2D, public XGeoParquetVector {
public:
  XGeoParquetMPoly2D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
  {
    m_Map = map;  m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = nbparts;
    m_Pt = nullptr; m_Parts = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* /*Z*/, int nbparts, int* parts, double, double)
  {
    m_nNumPoints = nb; m_Pt = P; m_nNumParts = nbparts; m_Parts = parts;
  }
};

//-----------------------------------------------------------------------------
// Multi-Polygon 3D GeoPackage
//-----------------------------------------------------------------------------
class XGeoParquetMPoly3D : public XGeoMPoly3D, public XGeoParquetVector {
public:
  XGeoParquetMPoly3D(XGeoParquetMap* map, int64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
  {
    m_Map = map; m_Offset = offset; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = nbparts;
    m_Pt = nullptr; m_Parts = nullptr; m_Z = nullptr;
  }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != NULL) return m_Map->ReadAttributes(m_Offset, V, m_Class); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != NULL) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != NULL) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }

  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(int nb, XPt* P, double* Z, int nbparts, int* parts, double zmin, double zmax)
  {
    m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_nNumParts = nbparts; m_Parts = parts; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;
  }
};

#endif //XGEOPARQUET_H
