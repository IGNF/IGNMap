//-----------------------------------------------------------------------------
//								XGpkgMap.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 14/12/2020
//-----------------------------------------------------------------------------

#ifndef XGPKGMAP_H
#define XGPKGMAP_H

#include "../Sqlite/sqlite3.h"
#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoPoly.h"

class XGeoBase;

class XGpkgVector;

class XGpkgMap : public XGeoMap {
protected:
  typedef struct {
    std::string m_strName;
    std::string m_strType;
    std::string m_strGeomName;
    std::string m_strGeomType;
    std::string m_strPrimaryKey;
    bool        m_bZ;
    bool        m_bM;
  } GpkgTable;

  typedef struct {
    uint8_t byteorder:1;
    uint8_t envelop:3;
    uint8_t empty:1;
    uint8_t type:1;
    uint8_t reserved:2;
  } GpkgFlags;

public:
  XGpkgMap();
  virtual ~XGpkgMap();

  bool OpenFile(const char* filename);

  bool ReadGpkgContents();
  bool ReadGpkgGeomCol();
  bool ReadPrimaryKey();

  bool Read(XGeoBase* base, XWait* wait = NULL);

  bool ReadAttributes(sqlite3_int64 id, std::vector<std::string>& V, XGeoClass* C);
  bool LoadGeom(sqlite3_int64 id, XGpkgVector* V);
  bool LoadGeom2D(sqlite3_int64 id, XGpkgVector* V);

  static bool ImportGpkg(XGeoBase* base, const char* path, XGeoMap* map = NULL);

protected:
  sqlite3*                m_DB;           // Base de donnees Sqlite
  std::string             m_strFilename;  // Nom du fichier
  std::vector<GpkgTable>  m_Table;
  XGeoClass*              m_Class;        // Classe par defaut
  uint32_t                  m_idxClass;     // Index de la classe par defaut
  std::vector<std::string>  m_Att;        // Attribut de la classe par defaut

  bool ReadVectors(uint32_t index, XGeoClass* C);
  bool ReadGeomHeader(uint8_t* geom, XFrame* F, double *zmin, double *zmax);
  int GetGeomHeaderSize(uint8_t* geom);
  bool SetClass(XGeoClass* C);
  bool SetAttClass(XGeoClass* C);
};

//-----------------------------------------------------------------------------
// Vecteur GeoPackage
//-----------------------------------------------------------------------------
class XGpkgVector {
protected:
  XGpkgMap*       m_Map;
  sqlite3_int64   m_Id;

public:
  XGpkgVector() { m_Map = NULL; m_Id = 0; }
  virtual inline XGeoClass* GeoClass() const { return NULL;}
  virtual void SetGeom(int /*nb*/, XPt* /*P*/, double* /*Z*/, int /*nbparts*/, int* /*parts*/,
                       double /*zmin*/, double /*zmax*/) { ; }
};

//-----------------------------------------------------------------------------
// Point 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgPoint2D : public XGeoPoint2D, public XGpkgVector {
public:
  XGpkgPoint2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F) { m_Map = map; m_Id = id; m_Frame = F;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual inline XGeoClass* GeoClass() const { return Class();}
};

//-----------------------------------------------------------------------------
// Point 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgPoint3D : public XGeoPoint3D, public XGpkgVector {
public:
  XGpkgPoint3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, double z)
          { m_Map = map; m_Id = id; m_Frame = F; m_Z = z;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual inline XGeoClass* GeoClass() const { return Class();}
};

//-----------------------------------------------------------------------------
// Multi-Point 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMPoint2D : public XGeoMPoint2D, public XGpkgVector {
public:
  XGpkgMPoint2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
                  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* , int , int*, double, double)
                        { m_nNumPoints = nb; m_Pt = P;}
};

//-----------------------------------------------------------------------------
// Multi-Point 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMPoint3D : public XGeoMPoint3D, public XGpkgVector {
public:
  XGpkgMPoint3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL; m_Z = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}
  virtual bool LoadGeom2D()
                        { if (m_Map!=NULL) return m_Map->LoadGeom2D(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* Z, int , int*, double zmin, double zmax)
                        { m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
};

//-----------------------------------------------------------------------------
// Ligne 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgLine2D : public XGeoLine2D, public XGpkgVector {
public:
  XGpkgLine2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
    { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* , int , int*, double, double )
                        { m_nNumPoints = nb; m_Pt = P;}
};

//-----------------------------------------------------------------------------
// Ligne 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgLine3D : public XGeoLine3D, public XGpkgVector {
public:
  XGpkgLine3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL; m_Z = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}
  virtual bool LoadGeom2D()
                        { if (m_Map!=NULL) return m_Map->LoadGeom2D(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* Z, int , int*, double zmin, double zmax)
              { m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
};

//-----------------------------------------------------------------------------
// Multi-Ligne 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMLine2D : public XGeoMLine2D, public XGpkgVector {
public:
  XGpkgMLine2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = 0; m_Pt = NULL; m_Parts = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* /*Z*/, int nbparts, int* parts, double, double)
                        { m_nNumPoints = nb; m_Pt = P; m_nNumParts = nbparts; m_Parts = parts;}
};

//-----------------------------------------------------------------------------
// Multi-Ligne 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMLine3D : public XGeoMLine3D, public XGpkgVector {
public:
  XGpkgMLine3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = 0; m_Pt = NULL; m_Parts = NULL; m_Z = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}
  virtual bool LoadGeom2D()
                        { if (m_Map!=NULL) return m_Map->LoadGeom2D(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* Z, int nbparts, int* parts, double zmin, double zmax)
      { m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_nNumParts = nbparts; m_Parts = parts; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
};

//-----------------------------------------------------------------------------
// Polygone 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgPoly2D : public XGeoPoly2D, public XGpkgVector {
public:
  XGpkgPoly2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* , int , int*, double, double)
                        { m_nNumPoints = nb; m_Pt = P;}
};

//-----------------------------------------------------------------------------
// Polygone 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgPoly3D : public XGeoPoly3D, public XGpkgVector {
public:
  XGpkgPoly3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_Pt = NULL; m_Z = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}
  virtual bool LoadGeom2D()
                        { if (m_Map!=NULL) return m_Map->LoadGeom2D(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* Z, int , int*, double zmin, double zmax)
                        { m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
};

//-----------------------------------------------------------------------------
// Multi-Polygon 2D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMPoly2D : public XGeoMPoly2D, public XGpkgVector {
public:
  XGpkgMPoly2D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = 0; m_Pt = NULL; m_Parts = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* /*Z*/, int nbparts, int* parts, double, double)
                        { m_nNumPoints = nb; m_Pt = P; m_nNumParts = nbparts; m_Parts = parts;}
};

//-----------------------------------------------------------------------------
// Multi-Polygon 3D GeoPackage
//-----------------------------------------------------------------------------
class XGpkgMPoly3D : public XGeoMPoly3D, public XGpkgVector {
public:
  XGpkgMPoly3D(XGpkgMap* map, sqlite3_int64 id, XFrame& F, uint32_t nbpt = 0)
  { m_Map = map; m_Id = id; m_Frame = F; m_nNumPoints = nbpt; m_nNumParts = 0; m_Pt = NULL; m_Parts = NULL; m_Z = NULL;}

  virtual inline XGeoMap* Map() const { return m_Map;}
  virtual bool ReadAttributes(std::vector<std::string>& V)
                        { if (m_Map!=NULL) return m_Map->ReadAttributes(m_Id, V, m_Class); return false;}
  virtual bool LoadGeom()
                        { if (m_Map!=NULL) return m_Map->LoadGeom(m_Id, this); return false;}
  virtual bool LoadGeom2D()
                        { if (m_Map!=NULL) return m_Map->LoadGeom2D(m_Id, this); return false;}

  virtual inline XGeoClass* GeoClass() const { return Class();}
  virtual void SetGeom(int nb, XPt* P, double* Z, int nbparts, int* parts, double zmin, double zmax)
    { m_nNumPoints = nb; m_Pt = P; m_Z = Z; m_nNumParts = nbparts; m_Parts = parts; m_ZRange = new double[2]; m_ZRange[0] = zmin; m_ZRange[1] = zmax;}
};

#endif // XGPKGMAP_H
