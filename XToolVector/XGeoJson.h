//-----------------------------------------------------------------------------
//								XGeoJson.h
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 18/12/2017
//-----------------------------------------------------------------------------

#ifndef XGEOJSON_H
#define XGEOJSON_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoly.h"
#include "../XToolGeod/XGeoProjection.h"

class XGeoBase;

class XGeoJson : public XGeoMap {
protected:
  std::string                   m_strFilename;
  std::ifstream                 m_In;
  XGeoProjection::XProjCode     m_Projection;  // Projection

  std::vector<std::string>		m_Att;				// Attributs du feature en cours

  std::string ReadStringToken(std::istream* in);
  bool ReadKeyValue(std::istream* in, std::string& key, std::string& value);
  std::string ReadObject(std::istream* in);

  bool ReadPoint(std::istream* in, double &x, double &y, double &z);

  bool AnalyzeLineString(std::istream* in, XFrame* F, uint32_t* nbPt, XPt *P = NULL);
  bool AnalyzePolygon(std::istream* in, XFrame* F, uint32_t* nbPt, uint32_t* nbPart,
                      XPt *P = NULL, int *Parts = NULL);
  bool AnalyzeMultiPolygon(std::istream* in, XFrame* F, uint32_t* nbPt, uint32_t* nbPart,
                      XPt *P = NULL, int *Parts = NULL);
  bool AnalyzeGeometry(std::istream* in, std::string &type, XFrame *F, uint32_t *nbPt, uint32_t *nbPart,
                       XPt *P = NULL, int *Parts = NULL);
  bool AnalyzeFeature(std::istream* in);

  virtual bool ReadForeignMembers(std::string, std::string, std::vector<std::string>&) { return true;}

public:
  XGeoJson();
  virtual ~XGeoJson() {;}

  void Class(XGeoClass* C);
  XGeoClass* Class();

  bool Read(const char* filename, XError* error = NULL);

  inline std::ifstream* IStream() { return &m_In;}

  bool ReadFeatureGeom(uint32_t pos, uint32_t nbPt, XPt* P, uint32_t nbPart = 1, int *Parts = NULL);

  bool ReadAttributes(uint32_t pos, std::vector<std::string>& V);

  static XGeoClass* ImportGeoJson(XGeoBase* base, const char* path, XGeoMap* map = NULL);
};

//-----------------------------------------------------------------------------
// XGeoJsonVector
//-----------------------------------------------------------------------------
class XGeoJsonVector {
protected :
  XGeoJson*       m_File;
  uint32_t          m_Pos;						// Position de la geometrie dans le fichier

public :
  XGeoJsonVector() { m_File = NULL; m_Pos = 0; }
  XGeoJsonVector(XGeoJson* file, uint32_t pos) { m_File = file; m_Pos = pos;}

  inline XGeoJson* GeoJson() const { return m_File;}
  inline uint32_t Pos() const { return m_Pos;}
  void GeoJson(XGeoJson* geojson, uint32_t pos) { m_File = geojson; m_Pos = pos;}

  bool ReadGeoJsonAttributes(std::vector<std::string>& V);
  std::string FindGeoJsonAttribute(std::string att_name, bool no_case = false);
};


//-----------------------------------------------------------------------------
// Point
//-----------------------------------------------------------------------------
class XGeoJsonPoint2D : public XGeoPoint2D, public XGeoJsonVector {
public:
  XGeoJsonPoint2D() { m_File = NULL;}
  XGeoJsonPoint2D(XGeoJson* file, uint32_t pos, XFrame* F) : XGeoJsonVector(file, pos) { m_Frame = *F;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
};
/*
class XGeoJsonPoint3D : public XGeoPoint3D, public XGeoJsonVector {
public:
  XGeoJsonPoint3D() { m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  bool Read(std::ifstream* in, XError* error = NULL);

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
};*/

//-----------------------------------------------------------------------------
// Multi-Point
//-----------------------------------------------------------------------------
class XGeoJsonMPoint2D : public XGeoMPoint2D, public XGeoJsonVector {
public:
  XGeoJsonMPoint2D() { m_File = NULL;}
  XGeoJsonMPoint2D(XGeoJson* file, uint32_t pos, XFrame* F, uint32_t *nbPt) : XGeoJsonVector(file, pos)
                  { m_Frame = *F; m_nNumPoints = *nbPt;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
};
/*
class XGeoJsonMPoint3D : public XGeoMPoint3D, public XGeoJsonVector {
public:
  XGeoJsonMPoint3D() { m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  bool Read(std::ifstream* in, XError* error = NULL);

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
  virtual bool LoadGeom2D();
};
*/
//-----------------------------------------------------------------------------
// Mutli-Ligne 2D
//-----------------------------------------------------------------------------
class XGeoJsonMLine2D : public XGeoMLine2D, public XGeoJsonVector {
public:
  XGeoJsonMLine2D(XGeoJson* file, uint32_t pos, XFrame* F, uint32_t *nbPt, uint32_t *nbPart) : XGeoJsonVector(file, pos)
            { m_Frame = *F; m_nNumPoints = *nbPt; m_nNumParts = *nbPart;}
  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Ligne 2D
//-----------------------------------------------------------------------------
class XGeoJsonLine2D : public XGeoLine2D, public XGeoJsonVector {
public:
  XGeoJsonLine2D() { m_File = NULL;}
  XGeoJsonLine2D(XGeoJson* file, uint32_t pos, XFrame* F, uint32_t *nbPt) : XGeoJsonVector(file, pos)
    { m_Frame = *F; m_nNumPoints = *nbPt;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
};
/*
//-----------------------------------------------------------------------------
// Multi-Polyligne 3D
//-----------------------------------------------------------------------------
class XGeoJsonMLine3D : public XGeoMLine3D, public XGeoJsonVector {
public:
  XGeoJsonMLine3D() { m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  bool Read(std::ifstream* in, XError* error = NULL);

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
  virtual bool LoadGeom2D();
};
*/
/*
//-----------------------------------------------------------------------------
// Polyligne 3D
//-----------------------------------------------------------------------------
class XGeoJsonLine3D : public XGeoLine3D, public XGeoJsonVector {
public:
  XGeoJsonLine3D() { m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
  virtual bool LoadGeom2D();
};
*/
//-----------------------------------------------------------------------------
// Multi-Polygone 2D
//-----------------------------------------------------------------------------
class XGeoJsonMPoly2D : public XGeoMPoly2D, public XGeoJsonVector {
public:
  XGeoJsonMPoly2D() { m_File = NULL;}
  XGeoJsonMPoly2D(XGeoJson* file, uint32_t pos, XFrame* F, uint32_t *nbPt, uint32_t *nbPart) : XGeoJsonVector(file, pos)
    { m_Frame = *F; m_nNumPoints = *nbPt; m_nNumParts = *nbPart;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Polygone 2D
//-----------------------------------------------------------------------------
class XGeoJsonPoly2D : public XGeoPoly2D, public XGeoJsonVector {
public:
  XGeoJsonPoly2D() { m_File = NULL;}
  XGeoJsonPoly2D(XGeoJson* file, uint32_t pos, XFrame* F, uint32_t *nbPt) : XGeoJsonVector(file, pos)
    { m_Frame = *F; m_nNumPoints = *nbPt;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
};

/*
//-----------------------------------------------------------------------------
// Multi-Polygone 3D
//-----------------------------------------------------------------------------
class XGeoJsonMPoly3D : public XGeoMPoly3D, public XGeoJsonVector {
public:
  XGeoJsonMPoly3D() { m_Pos = 0; m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  bool Read(std::ifstream* in, XError* error = NULL);
  bool Write(std::ofstream* out, XError* error = NULL);

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
  virtual bool LoadGeom2D();
};
*/
/*
//-----------------------------------------------------------------------------
// Polygone 3D
//-----------------------------------------------------------------------------
class XGeoJsonPoly3D : public XGeoPoly3D, public XGeoJsonVector {
public:
  XGeoJsonPoly3D() { m_Pos = 0; m_File = NULL;}

  virtual inline XGeoMap* Map() const { return m_File;}

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadGeoJsonAttributes(V);}
  virtual std::string FindAttribute(std::string att_name, bool no_case = false)
                                                    { return FindGeoJsonAttribute(att_name, no_case);}
  virtual bool LoadGeom();
  virtual bool LoadGeom2D();
};
*/


#endif // XGEOJSON_H
