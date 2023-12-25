//-----------------------------------------------------------------------------
//								XGeoPoint.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 04/07/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOPOINT_H
#define _XGEOPOINT_H

#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Point simple 2D
//-----------------------------------------------------------------------------
class XGeoPoint2D : public XGeoVector {

public:
	XGeoPoint2D() : XGeoVector() {;}

	virtual eTypeVector TypeVector () const { return Point;}

	virtual uint32_t NbPt() const { return 1;}
	virtual XPt2D Pt(uint32_t) { return XPt2D(m_Frame.Xmin, m_Frame.Ymax);}

	virtual std::string Name();
	virtual uint16_t Importance();
	virtual uint16_t Rotation() { return 0;}
	virtual bool HasResidual() { return false;}
	virtual XPt3D Residual() { return XPt3D();}

	virtual bool IntersectFrame(XFrame& r) const;

	virtual bool LoadGeom() { return true;}
	virtual bool IsLoaded() { return true;}
};

//-----------------------------------------------------------------------------
// Point simple 3D
//-----------------------------------------------------------------------------
class XGeoPoint3D : public XGeoPoint2D {
protected:
	double m_Z;

public:
	XGeoPoint3D() : XGeoPoint2D() { m_Z = XGEO_NO_DATA;}

	virtual eTypeVector TypeVector () const { return PointZ;}

	virtual double Z(uint32_t i) { return m_Z;}
	virtual bool Is3D() const { return true;}

	virtual inline double Zmin() const { return m_Z;}
	virtual inline double Zmax() const { return m_Z;}
};

//-----------------------------------------------------------------------------
// Point multiple 2D
//-----------------------------------------------------------------------------
class XGeoMPoint2D : public XGeoPoint2D {
protected:
	int			m_nNumPoints;
	XPt*		m_Pt;
	
public:
	XGeoMPoint2D() : XGeoPoint2D() { m_nNumPoints = 0; m_Pt = NULL;}
	virtual ~XGeoMPoint2D() { Unload();}

	virtual eTypeVector TypeVector () const { return MPoint;}

	virtual uint32_t NbPt() const { return m_nNumPoints;}
	inline XPt* Pt() { return m_Pt;}
	virtual XPt2D Pt(uint32_t i);

	virtual bool LoadGeom() { return false;}
	virtual void Unload() { if (m_Pt != NULL) delete[] m_Pt; m_Pt = NULL;}
	virtual bool IsLoaded() { if (m_Pt != NULL) return true; return false;}

  virtual bool Intersect(const XFrame& F);
  virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Point multiple 3D
//-----------------------------------------------------------------------------
class XGeoMPoint3D : public XGeoMPoint2D {
protected:
	double*	m_Z;
	double*	m_ZRange;

public:
	XGeoMPoint3D() : XGeoMPoint2D() { m_Z = NULL; m_ZRange = NULL;}
	virtual ~XGeoMPoint3D() { Unload();}

	virtual eTypeVector TypeVector () const { return MPointZ;}

	virtual bool Is3D() const { return true;}

	virtual double Z(uint32_t i);
	virtual inline double* Z() { return m_Z;}
	virtual inline double* ZRange() { return m_ZRange;}

	virtual inline double Zmin() const { if (m_ZRange != NULL) return m_ZRange[0]; return XGEO_NO_DATA;}
	virtual inline double Zmax() const { if (m_ZRange != NULL) return m_ZRange[1]; return XGEO_NO_DATA;}

	virtual bool LoadGeom() { return false;}
	virtual void Unload();
};


#endif //_XGEOPOINT_H
