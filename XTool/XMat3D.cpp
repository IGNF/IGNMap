//-----------------------------------------------------------------------------
//								XMat3D.cpp
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 25/10/2000
//-----------------------------------------------------------------------------

#include <cmath>
#include "XMat3D.h"


//-----------------------------------------------------------------------------
// Constructeurs
//-----------------------------------------------------------------------------
XMat3D::XMat3D(XPt3D axiateur)
{
	A = XPt3D(0, -axiateur.Z, axiateur.Y);
	B = XPt3D(axiateur.Z, 0, -axiateur.X);
	C = XPt3D(-axiateur.Y, axiateur.X, 0);
}

XMat3D::XMat3D(double omega,double phi,double kapa, TypeRotation type)
{
	double sin_kapa = sin(kapa);
	double sin_omega= sin(omega);
	double sin_phi	= sin(phi);
	double cos_kapa = cos(kapa);
	double cos_omega= cos(omega);
	double cos_phi	= cos(phi);
	
	switch(type) {
		case KapaPhiOmega:
			A=XPt3D(	cos_phi * cos_kapa,
				cos_omega * sin_kapa + sin_phi * sin_omega * cos_kapa,
				sin_omega * sin_kapa - sin_phi * cos_omega * cos_kapa);
			B=XPt3D(	cos_phi * sin_kapa * (-1.0),
				cos_omega * cos_kapa - sin_phi * sin_omega * sin_kapa,
				sin_omega * cos_kapa + sin_phi * cos_omega * sin_kapa);
			C=XPt3D(	sin_phi,
				cos_phi * sin_omega * (-1.0),
				cos_phi * cos_omega);
			break;
		case KapaOmegaPhi:
			A=XPt3D(	cos_phi * cos_kapa + sin_phi * sin_omega * sin_kapa,
				cos_omega * sin_kapa,
				sin_omega * cos_phi * sin_kapa - sin_phi * cos_kapa);
			B=XPt3D(	sin_phi * sin_omega * cos_kapa - cos_phi * sin_kapa,
				cos_omega * cos_kapa,
				sin_phi * sin_kapa + sin_omega * cos_phi * cos_kapa);
			C=XPt3D(	sin_phi * cos_omega,
				sin_omega * (-1.0),
				cos_phi * cos_omega);
			break;
		case OPK:
			A=XPt3D(	cos_phi * cos_kapa,
				cos_phi * sin_kapa * (-1.0),
				sin_phi);
			B=XPt3D(	cos_omega * sin_kapa + sin_phi * sin_omega * cos_kapa,
				cos_omega * cos_kapa - sin_phi * sin_omega * sin_kapa,
				cos_phi * sin_omega * (-1.0));
			C=XPt3D(	sin_omega * sin_kapa - sin_phi * cos_omega * cos_kapa,
				sin_omega * cos_kapa + sin_phi * cos_omega * sin_kapa,
				cos_phi * cos_omega);			
			break;
		case KPO:
			A=XPt3D( cos_kapa * cos_phi,
				cos_kapa * sin_phi * sin_omega - sin_kapa * cos_omega,
				sin_kapa * sin_omega + cos_kapa * sin_phi * cos_omega);
			B=XPt3D( sin_kapa * cos_phi,
				cos_kapa * cos_omega + sin_kapa * sin_phi * sin_omega,
				sin_kapa * sin_phi * cos_omega - cos_kapa * sin_omega);
			C=XPt3D( sin_phi * (-1.0),
				cos_phi * sin_omega,
				cos_phi * cos_omega);
			break;
		}
}

//-----------------------------------------------------------------------------
// Matrice rotation en sur un axe
//-----------------------------------------------------------------------------
XMat3D::XMat3D(double alpha, XPt3D axe)
{
  if (axe == XPt3D(1, 0, 0)) {
    A = XPt3D(1, 0, 0);
    B = XPt3D(0, cos(alpha), -sin(alpha));
    C = XPt3D(0, sin(alpha), cos(alpha));
  }
  if (axe == XPt3D(0, 1, 0)) {
    A = XPt3D(cos(alpha), 0, sin(alpha));
    B = XPt3D(0, 1, 0);
    C = XPt3D(-sin(alpha), 0, cos(alpha));
  }
  if (axe == XPt3D(0, 0, 1)) {
    A = XPt3D(cos(alpha), -sin(alpha), 0 );
    B = XPt3D(sin(alpha), cos(alpha), 0);
    C = XPt3D(0, 0, 1);
  }
}

//-----------------------------------------------------------------------------
// Recuperation des lignes et des colonnes
//-----------------------------------------------------------------------------
XPt3D XMat3D::lig(int n) const	// n : numero de ligne de 1 a 3
{
	switch(n) {
		case 1:
			return A;
		case 2:
			return B;
		case 3:
			return C;
		}
	return XPt3D(0,0,0);
}

XPt3D XMat3D::col(int n) const	// n : numero de colonne de 1 a 3
{
	switch(n) {
		case 1:
			return XPt3D(A.X, B.X, C.X);
		case 2:
			return XPt3D(A.Y, B.Y, C.Y);
		case 3:
			return XPt3D(A.Z, B.Z, C.Z);
		}
	return XPt3D(0,0,0);
}

//-----------------------------------------------------------------------------
// Operateurs de calcul
//-----------------------------------------------------------------------------
XMat3D& XMat3D::operator+=(XMat3D M)
{
	A += M.A;	B += M.B;	C += M.C;
	return *this;
}

XMat3D& XMat3D::operator-=(XMat3D M)
{
	A -= M.A;	B -= M.B;	C -= M.C;
	return *this;
}

XMat3D& XMat3D::operator*=(XMat3D M)
{
	XMat3D N = M.Trn();
	A = XPt3D(prodScal(A, N.A),prodScal(A, N.B),prodScal(A, N.C));
	B = XPt3D(prodScal(B, N.A),prodScal(B, N.B),prodScal(B, N.C));
	C = XPt3D(prodScal(C, N.A),prodScal(C, N.B),prodScal(C, N.C));
	return *this;
}

XMat3D& XMat3D::operator*=(double k)
{
	A *= k;		B *= k;		C *= k;
	return *this;
}

XMat3D& XMat3D::operator/=(double k)
{
	A /= k;		B /= k;		C /= k;
	return *this;
}

double XMat3D::Det() const		// Determinant
{
	return prodMixt(A,B,C);
}

XMat3D XMat3D::Trn()	const		// Transposee
{
	return XMat3D(XPt3D(A.X,B.X,C.X),XPt3D(A.Y,B.Y,C.Y),
				XPt3D(A.Z,B.Z,C.Z));	
}

//-----------------------------------------------------------------------------
// Calcul des angles d'une matrice rotation
//-----------------------------------------------------------------------------
double XMat3D::Omega(TypeRotation type) const
{
	switch(type) {
		case KapaPhiOmega:
			return asin(C.Y * (-1.0) / cos(Phi(type)));
		case KapaOmegaPhi:
			return asin((-1.0) * C.Y);
		case OPK:
			return asin(B.Z * (-1.0) / cos(Phi(type)));
		case KPO:
			return asin(C.Y / cos(Phi(type)));
	}
	return 0.0;
}

double XMat3D::Phi(TypeRotation type) const
{
	switch(type) {
		case KapaPhiOmega:
			return asin(C.X);
		case KapaOmegaPhi:
			return asin(C.X / cos(Omega(KapaOmegaPhi)));
		case OPK:
			return asin(A.Z);
		case KPO:
			return asin(C.X*(-1.0));
	}
	return 0.0;
}

double XMat3D::Kapa(TypeRotation type) const
{
	double phi, omega;
	switch(type) {
		case KapaPhiOmega:
			phi = Phi(type);
			if (asin((-1.0)*B.X / cos(phi)) > 0)
				return acos(A.X / cos(phi));
			else
				return (-1.0) * acos(A.X / cos(phi));
		case KapaOmegaPhi:
			omega = Omega(KapaOmegaPhi);
			if (asin(A.Y / cos(omega)) > 0)
				return acos(B.Y / cos(omega));
			else
				return (-1.0) * acos(B.Y / cos(omega));
		case OPK:
			phi = Phi(type);
			if (asin((-1.0)*A.Y / cos(phi)) > 0)
				return acos(A.X / cos(phi));
			else
				return (-1.0) * acos(A.X / cos(phi));
		case KPO:
			phi = Phi(type);
			if (asin(A.Y / cos(phi)) > 0)
				return acos(A.X / cos(phi));
			else
				return (-1.0) * acos(A.X / cos(phi));			
	}
	return 0.0;
}

//-----------------------------------------------------------------------------
// Operations entre matrices
//-----------------------------------------------------------------------------
XMat3D operator+(XMat3D M, XMat3D P)
{
	XMat3D Q = M;
	return Q += P;
}

XMat3D operator-(XMat3D M, XMat3D P)
{
	XMat3D Q = M;
	return Q -= P;
}

XMat3D operator*(XMat3D M, XMat3D P)
{
	XMat3D Q = M;
	return M*= P;
}

XMat3D operator*(XMat3D M, double k)
{
	XMat3D Q = M;
	return Q *= k;
}

XMat3D operator*(double k, XMat3D M)
{
	XMat3D Q = M;
	return Q *= k;
}

XPt3D operator*(XMat3D M, XPt3D P)
{
	return XPt3D(prodScal(M.lig(1),P),prodScal(M.lig(2),P),prodScal(M.lig(3),P));
}

XPt3D operator*(XPt3D P, XMat3D M)
{
	return XPt3D(prodScal(M.col(1),P),prodScal(M.col(2),P),prodScal(M.col(3),P));
}

bool operator==(XMat3D M, XMat3D P)
{
	return M.lig(1)==P.lig(1)&& M.lig(2)==P.lig(2)&& M.lig(3)==P.lig(3);
}

bool operator!=(XMat3D M, XMat3D P)
{
	return !(M==P);
}

//-----------------------------------------------------------------------------
// Operateurs d'entree/sortie
//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& s, XMat3D& M)
{
	XPt3D A, B, C;
	s >> A >> B >> C;
	M = XMat3D(A, B, C);
	return s;
}

std::ostream& operator<<(std::ostream& s, XMat3D M)
{
	s << M.lig(1) << std::endl << M.lig(2) << std::endl << M.lig(3);
	return s;
}

