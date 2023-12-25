//-----------------------------------------------------------------------------
//								XGeoLine.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 05/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOLINE_H
#define _XGEOLINE_H

#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Ligne simple 2D
//-----------------------------------------------------------------------------
class XGeoLine2D : public XGeoVector {
protected :
	int			m_nNumPoints;
	XPt*		m_Pt;

public :
	XGeoLine2D() : XGeoVector() { m_Pt = NULL; m_nNumPoints = 0;}
	virtual ~XGeoLine2D() { if(m_Pt != NULL) delete[] m_Pt; m_Pt = NULL;}

	virtual eTypeVector TypeVector () const { return Line;}
	virtual uint32_t NbPt() const { return m_nNumPoints;}
	virtual XPt2D Pt(uint32_t i);
	virtual uint32_t NbPart() const { return 1;}

	virtual inline XPt* Pt() { return m_Pt;}
	virtual inline int* Parts() { return NULL;}

	virtual bool LoadGeom() { return false;}
	virtual void Unload() { if (m_Pt != NULL) delete[] m_Pt; m_Pt = NULL;}
	virtual bool IsLoaded() { if (m_Pt != NULL) return true; return false;}

	virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Ligne simple 3D
//-----------------------------------------------------------------------------
class XGeoLine3D : public XGeoLine2D {
protected :
	double*	m_Z;
	double*	m_ZRange;

public :
	XGeoLine3D() : XGeoLine2D() { m_Z = NULL; m_ZRange = NULL;}
	virtual ~XGeoLine3D();

	virtual eTypeVector TypeVector () const { return LineZ;}
	virtual double Z(uint32_t i);
	virtual inline double* Z() { return m_Z;}
	virtual inline double* ZRange() { return m_ZRange;}
	virtual bool Is3D() const { return true;}

	virtual inline double Zmin() const { if (m_ZRange != NULL) return m_ZRange[0]; return XGEO_NO_DATA;}
	virtual inline double Zmax() const { if (m_ZRange != NULL) return m_ZRange[1]; return XGEO_NO_DATA;}

	virtual bool LoadGeom() { return false;}
	virtual void Unload();
};

//-----------------------------------------------------------------------------
// Ligne multiple 2D
//-----------------------------------------------------------------------------
class XGeoMLine2D : public XGeoLine2D {
protected :
	int			m_nNumParts;
	int*		m_Parts;

public:
	XGeoMLine2D() : XGeoLine2D() { m_nNumParts = 0; m_Parts = NULL;}
	virtual ~XGeoMLine2D();

	virtual eTypeVector TypeVector () const { return MLine;}

	virtual uint32_t NbPart() const { return m_nNumParts;}
	virtual uint32_t Part(uint32_t i) { if (((int)i < m_nNumParts)&&(m_Parts != NULL)) return m_Parts[i]; return 0;}

	virtual inline int* Parts() { return m_Parts;}

	virtual bool LoadGeom() { return false;}
	virtual void Unload();

	virtual bool IsNear2D(const XPt2D& P, double dist);
};

//-----------------------------------------------------------------------------
// Ligne multiple 3D
//-----------------------------------------------------------------------------
class XGeoMLine3D : public XGeoMLine2D {
protected :
	double*	m_Z;
	double*	m_ZRange;

public:
	XGeoMLine3D() : XGeoMLine2D() { m_Z = NULL; m_ZRange = NULL;}
	virtual ~XGeoMLine3D();

	virtual eTypeVector TypeVector () const { return MLineZ;}
	virtual double Z(uint32_t i);
	virtual inline double* Z() { return m_Z;}
	virtual inline double* ZRange() { return m_ZRange;}
	virtual bool Is3D() const { return true;}

	virtual inline double Zmin() const { if (m_ZRange != NULL) return m_ZRange[0]; return XGEO_NO_DATA;}
	virtual inline double Zmax() const { if (m_ZRange != NULL) return m_ZRange[1]; return XGEO_NO_DATA;}

	virtual bool LoadGeom() { return false;}
	virtual void Unload();
};

#endif //_XGEOLINE_H
