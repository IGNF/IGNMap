//-----------------------------------------------------------------------------
//								XMat3D.h
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 25/10/2000
//-----------------------------------------------------------------------------

#ifndef _XMAT3D_H
#define _XMAT3D_H

#include "XPt3D.h"

class XMat3D {
public:
	enum TypeRotation { KapaPhiOmega, KapaOmegaPhi, KPO, OPK};
protected:
	XPt3D	A;	// 1er ligne de la matrice
	XPt3D	B;	// 2em ligne de la matrice
	XPt3D	C;	// 3em ligne de la matrice
	
public:	
	// Constructeurs
	XMat3D(void) : A(), B(), C() { }
	XMat3D(XPt3D a, XPt3D b, XPt3D c) : A(a), B(b), C(c) { }
	XMat3D(XPt3D);	// Initialisation d'un axiateur
	XMat3D(double omega, double phi, double kapa, TypeRotation type = KPO);	// Matrice rotation
  XMat3D(double alpha, XPt3D axe);  // Matrice rotation en sur un axe
	
	// Acces au donnees membres
	XPt3D lig(int n) const;	// Renvoie la ligne n
	XPt3D col(int n) const;	// Renvoie la colonne n
	
	// Operations
	XMat3D& operator+=(XMat3D);
	XMat3D& operator-=(XMat3D);
	XMat3D& operator*=(XMat3D);
	XMat3D& operator*=(double);
	XMat3D& operator/=(double);
	
	double Det() const;	// Determinant
	XMat3D  Trn() const;	// Transposee
	
	// Calcul des angles d'une matrice rotation
	double Omega(TypeRotation = KPO) const;
	double Phi(TypeRotation = KPO) const;
	double Kapa(TypeRotation = KPO) const;
};

XMat3D operator+(XMat3D, XMat3D);
XMat3D operator-(XMat3D, XMat3D);

XMat3D operator*(XMat3D, XMat3D);
XMat3D operator*(XMat3D, double);
XMat3D operator*(double, XMat3D);
XPt3D operator*(XMat3D, XPt3D);
XPt3D operator*(XPt3D, XMat3D);

bool operator==(XMat3D, XMat3D);
bool operator!=(XMat3D, XMat3D);

std::istream& operator>>(std::istream&, XMat3D&);
std::ostream& operator<<(std::ostream&, XMat3D);


#endif //_XMAT3D_H
