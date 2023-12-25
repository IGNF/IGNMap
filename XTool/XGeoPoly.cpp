//-----------------------------------------------------------------------------
//								XGeoPoly.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/07/2003
//-----------------------------------------------------------------------------

#include "XGeoPoly.h"


//-----------------------------------------------------------------------------
// Regarde si un point est contenu dans le polygone
//-----------------------------------------------------------------------------
/*
bool XGeoPoly2D::IsIn2D(const XPt2D& P)
{ 
	uint32_t j;
	double alpha;
	int nb_inter = 0;
	XPt A, B, C;
	bool loaded = true;
	if (m_Pt == NULL) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	for (j = 0; j < NbPt() - 1; j++) {
		A = m_Pt[j];
		B = m_Pt[j + 1];
		if ((A.X == P.X)&&(A.Y == P.Y)){
			nb_inter = 1;
			break;
		}
		alpha = ((A.X - A.Y) -  (P.X - P.Y)) / ((A.X - A.Y) -  (B.X - B.Y));
		if ((alpha <= 0.0)||(alpha > 1.0))
			continue;
		C.X = A.X + alpha * (B.X - A.X) - P.X;
		C.Y = A.Y + alpha * (B.Y - A.Y) - P.Y;
		if ((C.X > 0.0)&&(C.Y > 0.0))
			nb_inter++;
	}

	if ((nb_inter % 2) != 0) {	// Nombre pair d'intersection
		if (!loaded)
			Unload();
		return true;
	}
	if (!loaded)
		Unload();
	return false;
}
*/
/*
bool XGeoPoly2D::IsIn2D(const XPt2D& P)
{ 
	uint32_t j;
	double alpha, delta = 0.0;
	int nb_inter = 0;
	XPt A, B, C;
	bool loaded = true, inside = false;
	if (m_Pt == NULL) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	for (j = 0; j < NbPt() - 1; j++) {
		A = m_Pt[j];
		B = m_Pt[j + 1];
		if ((A.X == P.X)&&(A.Y == P.Y)){
			nb_inter = 1;
			break;
		}
		alpha = ((A.X - A.Y) -  (P.X + delta - P.Y)) / ((A.X - A.Y) -  (B.X - B.Y));
		if (alpha == 0.0) {
			delta += 0.05;
			j = nb_inter = 0;
			continue;
		}

		if ((alpha <= 0.0)||(alpha > 1.0))
			continue;
		C.X = A.X + alpha * (B.X - A.X) - P.X;
		C.Y = A.Y + alpha * (B.Y - A.Y) - P.Y;
		if ((C.X > 0.0)&&(C.Y > 0.0))
			nb_inter++;
	}

	if ((nb_inter % 2) != 0) 	// Nombre impair d'intersection
		inside = true;
	if (!loaded)
		Unload();
	return inside;
}
*/

//-----------------------------------------------------------------------------
// Test si un segment passe sur le polygone
//-----------------------------------------------------------------------------
/*
bool XGeoPoly2D::IsIn2D(const XPt2D& M, const XPt2D& P)
{
	XPt2D A, B;
	double prod1, prod2;
	bool loaded = true;
	if (m_Pt == NULL) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}
	for (uint32_t j = 0; j < NbPt() - 1; j++) {
		A = XPt2D(m_Pt[j].X, m_Pt[j].Y);
		B = XPt2D(m_Pt[j+1].X, m_Pt[j+1].Y);

		prod1 = prodCross(M, A, B);
		prod2 = prodCross(P, A, B);
		if (prod1 * prod2 > 0.)
			continue;
		prod1 = prodCross(A, M, P);
		prod2 = prodCross(B, M, P);
		if (prod1 * prod2 < 0.) {
			if (!loaded)
				Unload();
			return true;
		}
	}
	if (!loaded)
		Unload();
	return false;
}
*/

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoPoly2D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;
	return IsIn2D(P);
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoPoly3D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;
	return IsIn2D(P);
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoMPoly2D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;
	return IsIn2D(P);
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoMPoly3D::IsNear2D(const XPt2D& P, double dist)
{
	if (!XGeoVector::IsNear2D(P, dist))
		return false;
	return IsIn2D(P);
}
