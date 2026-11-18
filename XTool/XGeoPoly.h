//-----------------------------------------------------------------------------
//								XGeoPoly.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 04/07/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOPOLY_H
#define _XGEOPOLY_H

#include "XGeoLine.h"

//-----------------------------------------------------------------------------
// Polygone simple 2D
//-----------------------------------------------------------------------------
class XGeoPoly2D : public XGeoLine2D { 
public :
	XGeoPoly2D() : XGeoLine2D() { XGeoObject::IsClosed(true); }
	virtual ~XGeoPoly2D() {;}

	virtual eTypeVector TypeVector () const { return Poly;}

	virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Polygone simple 3D
//-----------------------------------------------------------------------------
class XGeoPoly3D : public XGeoLine3D { 
public :
	XGeoPoly3D() : XGeoLine3D() { XGeoObject::IsClosed(true); }
	virtual ~XGeoPoly3D() {;}

	virtual eTypeVector TypeVector () const { return PolyZ;}
	
	virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Polygone multiple 2D
//-----------------------------------------------------------------------------
class XGeoMPoly2D : public XGeoMLine2D { 
public:
	XGeoMPoly2D() : XGeoMLine2D() { XGeoObject::IsClosed(true); }
	virtual ~XGeoMPoly2D() {;}

	virtual eTypeVector TypeVector () const { return MPoly;}

	virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Polygone multiple 3D
//-----------------------------------------------------------------------------
class XGeoMPoly3D : public XGeoMLine3D { 
public:
	XGeoMPoly3D() : XGeoMLine3D() { XGeoObject::IsClosed(true); }
	virtual ~XGeoMPoly3D() {;}

	virtual eTypeVector TypeVector () const { return MPolyZ;}

	virtual bool IsNear2D(const XPt2D& P, double dist);
};


#endif //_XGEOPOLY_H
