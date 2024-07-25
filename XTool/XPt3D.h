//-----------------------------------------------------------------------------
//								XPt3D.h
//								=======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 06/10/00
//-----------------------------------------------------------------------------

#ifndef _XPT3D_H
#define _XPT3D_H

#include <iostream>
#include "XBase.h"

class XPt2D;
class XParserXML;

class XPt3D {
public:
	double	X;
	double	Y;
	double	Z;

	// Constructeurs
	XPt3D(double x=0,double y=0,double z=0): X(x), Y(y), Z(z) {}
  XPt3D(const XPt3& A) : X(A.X), Y(A.Y), Z(A.Z) {;}

	// Operations
	inline XPt3D& operator+=(XPt3D M){X+=M.X;Y+=M.Y;Z+=M.Z; return *this;}
	inline XPt3D& operator-=(XPt3D M){X-=M.X;Y-=M.Y;Z-=M.Z; return *this;}
	inline XPt3D& operator*=(double k){X*=k; Y*=k; Z*=k; return *this;}
	inline XPt3D& operator/=(double k){X/=k; Y/=k; Z/=k; return *this;}

  // Norme
  inline double norm2() { return (X*X + Y*Y + Z*Z);}
  double norm();

	// Operateur de conversion
	operator XPt2D() const;

  // Tests
  bool egal(const XPt3D &M, const double &epsilon);

	bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	bool XmlWrite(std::ostream* out);
};

// Fonctions d'aide

// Operateurs de calcul
XPt3D operator+(XPt3D, XPt3D);
XPt3D operator-(XPt3D, XPt3D);

XPt3D operator*(XPt3D, double);
XPt3D operator*(double, XPt3D);
XPt3D operator/(XPt3D, double);
XPt3D operator/(double, XPt3D);

// Operateurs logiques
bool operator==(XPt3D, XPt3D);
bool operator!=(XPt3D, XPt3D);

// Fonctions de calcul de distances
double dist(XPt3D, XPt3D);			// Distance
double dist2(XPt3D, XPt3D);			// Distance au carre
double dist_plani(XPt3D, XPt3D);	// Distance planimetrique
double dist_plani2(XPt3D, XPt3D);	// Distance planimetrique au carre
double dist_alti(XPt3D, XPt3D);		// Distance altimetrique
double dist_polar(XPt3D);			// Distance polaire en plani
double dist_polar2(XPt3D);			// Distance polaire carre en plani

// Fonctions vectorielles
double prodScal(XPt3D, XPt3D);		// Produit Scalaire
XPt3D prodVect(XPt3D, XPt3D);			// Produit Vectoriel
double prodMixt(XPt3D, XPt3D, XPt3D);	// Produit Mixte

// Fonctions d'entree / sortie
std::istream& operator>>(std::istream&, XPt3D&);	// entree
std::ostream& operator<<(std::ostream&, XPt3D);	// sortie

#endif //_XPT3D_H
