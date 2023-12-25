//-----------------------------------------------------------------------------
//								XPt2D.h
//								=======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 10/10/00
//-----------------------------------------------------------------------------

#ifndef _XPT2D_H
#define _XPT2D_H

#include <iostream>
#include "XBase.h"
#include "XParserXML.h"

class XPt3D;

class XPt2D {
public:
	double	X;
	double	Y;

	// Constructeurs
	XPt2D(double x=0,double y=0): X(x), Y(y) {}
  XPt2D(const XPt& A) : X(A.X), Y(A.Y) {;}

	// Operations
	inline XPt2D& operator+=(XPt2D M){X+=M.X;Y+=M.Y; return *this;}
	inline XPt2D& operator-=(XPt2D M){X-=M.X;Y-=M.Y; return *this;}
	inline XPt2D& operator*=(double k){X*=k; Y*=k; return *this;}
	inline XPt2D& operator/=(double k){X/=k; Y/=k; return *this;}

  // Norme
  inline double norm2() { return (X*X + Y*Y);}
  double norm();

	// Operateur de conversion
	operator XPt3D() const;

  // Tests
  bool egal(const XPt2D &M, const double &epsilon);

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

// Fonctions d'aide

// Operateurs de calcul
XPt2D operator+(XPt2D, XPt2D);
XPt2D operator-(XPt2D, XPt2D);

XPt2D operator*(XPt2D, double);
XPt2D operator*(double, XPt2D);
XPt2D operator/(XPt2D, double);
XPt2D operator/(double, XPt2D);

// Operateurs logiques
bool operator==(XPt2D, XPt2D);
bool operator!=(XPt2D, XPt2D);

// Fonctions de calcul de distances
double dist(XPt2D, XPt2D);			// Distance
double dist2(XPt2D, XPt2D);			// Distance au carre

// Fonctions vectorielles
double prodScal(XPt2D, XPt2D);					// Produit Scalaire
double prodCross(XPt2D, XPt2D, XPt2D);	// Produit en croix
double area(XPt2D, XPt2D, XPt2D);       // Aire du triangle
double cap(XPt2D, XPt2D, XPt2D);        // Cap de la bissectrice

// Fonctions sur les droites
void droite(XPt2D, XPt2D, double& a, double& b, double& c);
double dist_droite(XPt2D, XPt2D, XPt2D);

// Fonctions sur les segments
bool proj_in_seg(XPt2D A, XPt2D B, XPt2D M);
XPt2D proj_seg(XPt2D A, XPt2D B, XPt2D M);

#endif //_XPT2D_H
