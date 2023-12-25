//-----------------------------------------------------------------------------
//								XWKBGeom.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 27/06/2006
//-----------------------------------------------------------------------------

#ifndef _XWKBGEOM_H
#define _XWKBGEOM_H

#include "../XTool/XBase.h"

class XFrame;

class XWKBGeom {
public:
  enum eType { Null = 0,
               wkbPoint = 1, wkbLineString = 2, wkbPolygon = 3, wkbMultiPoint = 4,
               wkbMultiLineString = 5, wkbMultiPolygon = 6, wkbGeometryCollection = 7,
               wkbPolyhedralSurface = 15, wkbTIN = 16, wkbTriangle = 17,
               wkbPointZ = 1001, wkbLineStringZ = 1002, wkbPolygonZ = 1003, wkbMultiPointZ = 1004,
               wkbMultiLineStringZ = 1005, wkbMultiPolygonZ = 1006, wkbGeometryCollectionZ = 1007,
               wkbPolyhedralSurfaceZ = 1015, wkbTINZ = 1016, wkbTriangleZ = 1017,
               wkbPointM = 2001, wkbLineStringM = 2002, wkbPolygonM = 2003, wkbMultiPointM = 2004,
               wkbMultiLineStringM = 2005, wkbMultiPolygonM = 2006, wkbGeometryCollectionM = 2007,
               wkbPolyhedralSurfaceM = 2015, wkbTINM = 2016, wkbTriangleM = 2017,
               wkbPointZM = 3001, wkbLineStringZM = 3002, wkbPolygonZM = 3003, wkbMultiPointZM = 3004,
               wkbMultiLineStringZM = 3005, wkbMultiPolygonZM = 3006, wkbGeometryCollectionZM = 3007,
               wkbPolyhedralSurfaceZM = 3015, wkbTINZM = 3016, wkbTriangleZM = 3017
             };
protected:
  bool									m_bAutoDelete;
	eType									m_Type;
	uint32_t								m_nNumPoints;
	XPt*									m_Pt;
  double*               m_Z;
	uint32_t								m_nNumParts;
	uint32_t*								m_Parts;

	bool ReadLineString(uint8_t* geom);
	bool ReadPolygon(uint8_t* geom);
	bool ReadMultiPoint(uint8_t* geom);
	bool ReadMultiLineString(uint8_t* geom);
	bool ReadMultiPolygon(uint8_t* geom);

  bool ReadLineStringZ(uint8_t* geom);
  bool ReadPolygonZ(uint8_t* geom);
  bool ReadMultiPointZ(uint8_t* geom);
  bool ReadMultiLineStringZ(uint8_t* geom);
  bool ReadMultiPolygonZ(uint8_t* geom);

  bool ReadLineStringZM(uint8_t* geom);
  bool ReadPolygonZM(uint8_t* geom);
  bool ReadMultiPointZM(uint8_t* geom);
  bool ReadMultiLineStringZM(uint8_t* geom);
  bool ReadMultiPolygonZM(uint8_t* geom);

public:
  XWKBGeom(bool autodelete = true);
	virtual ~XWKBGeom();

	inline eType Type() { return m_Type;}

	inline uint32_t NbPt() { return m_nNumPoints;}
	inline uint32_t NbPart() { return m_nNumParts;}
	inline XPt* Pt() { return m_Pt;}
	inline uint32_t* Parts() { return m_Parts;}
  inline double* Z() { return m_Z;}

	bool Read(uint8_t* geom);
  bool Frame(XFrame* F);
};

#endif //_XWKBGEOM_H
