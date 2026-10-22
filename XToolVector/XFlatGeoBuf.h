//-----------------------------------------------------------------------------
//								XFlatGeoBuf.h
//								=============
//
// Gestion du format Flatgeobuf
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/10/2026
//-----------------------------------------------------------------------------

#ifndef XFLATGEOBUF_H
#define XFLATGEOBUF_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoPoly.h"
#include "../XTool/XFile.h"

#include "../XToolGeod/XGeoProjection.h"

#include "../flatgeobuf/feature_generated.h"
#include "../flatgeobuf/header_generated.h"
#include "../flatgeobuf/packedrtree.h"

class XGeoBase;
class XFlatGeoBufVector;

class XFlatGeoBufMap : public XGeoMap {
protected :
  XFile m_In;
	uint8_t* m_HeaderData = nullptr;
  uint8_t* m_FeatureData = nullptr;
	XGeoProjection::XProjCode m_ProjCode;
  uint64_t m_FeatureOffset = 0;
	
public :
	XFlatGeoBufMap();
	virtual ~XFlatGeoBufMap();

	bool OpenFile(const char* filename);
  bool Read(XGeoBase* base, XWait* wait = nullptr);

  bool ReadAttributes(uint64_t offset, std::vector<std::string>& V, XGeoClass* C);
  bool LoadFlatGeoBufGeom(uint64_t offset, XFlatGeoBufVector* V, bool needZ);
  bool LoadGeom(uint64_t offset, XFlatGeoBufVector* V) { return LoadFlatGeoBufGeom(offset, V, true); }
  bool LoadGeom2D(uint64_t offset, XFlatGeoBufVector* V) { return LoadFlatGeoBufGeom(offset, V, false); }

  void Class(XGeoClass* C);
  XGeoClass* Class();

  static XGeoClass* ImportFlatGeoBuf(XGeoBase* base, const char* path, XGeoMap* map = nullptr);

protected :
  bool AnalyzeGeometry3D(const FlatGeobuf::Geometry* geometry, XFrame* F, double* Zmin, double* Zmax,
                        uint32_t* nbPoint, uint32_t* nbPart) const;
  bool AnalyzeGeometry2D(const FlatGeobuf::Geometry* geometry, XFrame* F, uint32_t* nbPoint, uint32_t* nbPart) const;
};

//-----------------------------------------------------------------------------
// Vecteur FlatGeoBuf
//-----------------------------------------------------------------------------
class XFlatGeoBufVector {
protected:
	XFlatGeoBufMap* m_Map;
	uint64_t   m_Offset;

public:
	XFlatGeoBufVector() { m_Map = nullptr; m_Offset = 0; }
	virtual ~XFlatGeoBufVector() { ; }
	virtual inline XGeoClass* GeoClass() const { return nullptr; }
	virtual void SetGeom(int /*nb*/, XPt* /*P*/, double* /*Z*/, int /*nbparts*/, int* /*parts*/,
		double /*zmin*/, double /*zmax*/) {
		;
	}
};

//-----------------------------------------------------------------------------
// Point 2D GeoPackage
//-----------------------------------------------------------------------------
class XFlatGeoBufPoint2D : public XGeoPoint2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufPoint2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F) { m_Map = map; m_Offset = offset; m_Frame = F; }

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
class XFlatGeoBufPoint3D : public XGeoPoint3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufPoint3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, double z)
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
class XFlatGeoBufMPoint2D : public XGeoMPoint2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMPoint2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufMPoint3D : public XGeoMPoint3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMPoint3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufLine2D : public XGeoLine2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufLine2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufLine3D : public XGeoLine3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufLine3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufMLine2D : public XGeoMLine2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMLine2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
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
class XFlatGeoBufMLine3D : public XGeoMLine3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMLine3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
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
class XFlatGeoBufPoly2D : public XGeoPoly2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufPoly2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufPoly3D : public XGeoPoly3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufPoly3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt)
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
class XFlatGeoBufMPoly2D : public XGeoMPoly2D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMPoly2D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
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
class XFlatGeoBufMPoly3D : public XGeoMPoly3D, public XFlatGeoBufVector {
public:
  XFlatGeoBufMPoly3D(XFlatGeoBufMap* map, uint64_t offset, XFrame& F, uint32_t nbpt, uint32_t nbparts)
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
#endif
