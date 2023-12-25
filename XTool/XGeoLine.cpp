//-----------------------------------------------------------------------------
//								XGeoLine.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 04/07/2003
//-----------------------------------------------------------------------------

#include "XGeoLine.h"


//-----------------------------------------------------------------------------
// Recuperation d'un point de la geometrie
//-----------------------------------------------------------------------------
XPt2D XGeoLine2D::Pt(uint32_t i)
{ 
	if ((i < m_nNumPoints)&&(m_Pt != NULL)) 
		return XPt2D(m_Pt[i].X, m_Pt[i].Y);
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoLine2D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;

	double d2 = dist * dist;
	double d, am, ab, prod;
	double minx, maxx, miny, maxy;
	if (!LoadGeom())
		return false;
	for (uint32_t j = 0; j < m_nNumPoints - 1; j++) {
		minx = XMin(m_Pt[j].X, m_Pt[j + 1].X);
		if (P.X < minx - dist)
			continue;
		maxx = XMax(m_Pt[j].X, m_Pt[j + 1].X);
		if (P.X > maxx + dist)
			continue;
		miny = XMin(m_Pt[j].Y, m_Pt[j + 1].Y);
		if (P.Y < miny - dist)
			continue;
		maxy = XMax(m_Pt[j].Y, m_Pt[j + 1].Y);
		if (P.Y > maxy + dist)
			continue;
		
		prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
		am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
		ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) + 
					(m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
		if (ab == 0)
				continue;
		d = am - (prod * prod) / ab;
		if (d <= d2){
			Unload();
			return true;
		}
	}
	Unload();
	return false;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoLine3D::~XGeoLine3D()
{ 
	Unload();
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoLine3D::Unload()
{ 
	XGeoLine2D::Unload();
	if (m_Z != NULL)
		delete[] m_Z;
	m_Z = NULL;
	if (m_ZRange != NULL)
		delete[] m_ZRange;
	m_ZRange = NULL;
}

//-----------------------------------------------------------------------------
// Recuperation d'un Z de la geometrie
//-----------------------------------------------------------------------------
double XGeoLine3D::Z(uint32_t i)
{
	if ((i < m_nNumPoints)&&(m_Z != NULL)) 
		return m_Z[i];
	return XGEO_NO_DATA;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMLine2D::~XGeoMLine2D()
{ 
	Unload();
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMLine2D::Unload()
{ 
	XGeoLine2D::Unload();
	if (m_Parts != NULL)
		delete[] m_Parts;
	m_Parts = NULL;
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoMLine2D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;

	double d2 = dist * dist;
	double d, am, ab, prod;
	uint32_t i, j, k;
	double minx, maxx, miny, maxy;
	if (!LoadGeom())
		return false;
	for (i = 0; i < m_nNumParts; i++) {
		if (i == m_nNumParts - 1)
			k = m_nNumPoints;
		else
			k = m_Parts[i + 1];
		for (j = m_Parts[i]; j < k - 1; j++) {
			minx = XMin(m_Pt[j].X, m_Pt[j + 1].X);
			if (P.X < minx - dist)
				continue;
			maxx = XMax(m_Pt[j].X, m_Pt[j + 1].X);
			if (P.X > maxx + dist)
				continue;
			miny = XMin(m_Pt[j].Y, m_Pt[j + 1].Y);
			if (P.Y < miny - dist)
				continue;
			maxy = XMax(m_Pt[j].Y, m_Pt[j + 1].Y);
			if (P.Y > maxy + dist)
				continue;
			
			prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
			am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
			ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) + 
						(m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
			if (ab == 0)
				continue;
			d = am - (prod * prod) / ab;
			if (d < d2){
				Unload();
				return true;
				}
			}
		}
	Unload();
	return false;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoMLine3D::~XGeoMLine3D()
{
	Unload();
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMLine3D::Unload()
{ 
	XGeoMLine2D::Unload();
	if (m_Z != NULL)
		delete[] m_Z;
	m_Z = NULL;
	if (m_ZRange != NULL)
		delete[] m_ZRange;
	m_ZRange = NULL;
}

//-----------------------------------------------------------------------------
// Recuperation d'un Z de la geometrie
//-----------------------------------------------------------------------------
double XGeoMLine3D::Z(uint32_t i)
{
	if ((i < m_nNumPoints)&&(m_Z != NULL)) 
		return m_Z[i];
	return XGEO_NO_DATA;
}

