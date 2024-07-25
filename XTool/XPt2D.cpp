//-----------------------------------------------------------------------------
//								XPt2D.cpp
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 10/10/00
//-----------------------------------------------------------------------------

#include <cmath>
#include "XPt2D.h"
#include "XPt3D.h"
#include "XParserXML.h"

//-----------------------------------------------------------------------------
// Egalite a un epsilon
//-----------------------------------------------------------------------------
bool XPt2D::egal(const XPt2D& M, const double& epsilon)
{
  if (fabs(X - M.X) > epsilon)
    return false;
  if (fabs(Y - M.Y) > epsilon)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Norme
//-----------------------------------------------------------------------------
double XPt2D::norm()
{
  return sqrt(norm2());
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPt2D::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML sub;
	sub	= parser->FindSubParser("/pt2d", num);
	if (sub.IsEmpty())
		return XErrorError(error, "XPt2D::XmlRead", XError::eBadFormat);

	X = sub.ReadNodeAsDouble("/pt2d/x");
	Y = sub.ReadNodeAsDouble("/pt2d/y");
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPt2D::XmlWrite(std::ostream* out)
{
	*out << "<pt2d> " << std::endl;
	*out << "<x> " << X << " </x>" << std::endl;
	*out << "<y> " << Y << " </y>" << std::endl;
	*out << "</pt2d>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Conversion en XPt3D
//-----------------------------------------------------------------------------
XPt2D::operator XPt3D() const
{
	return XPt3D(X, Y, 0);
}

//-----------------------------------------------------------------------------
// Operateurs de calcul
//-----------------------------------------------------------------------------
XPt2D operator+(XPt2D A, XPt2D B)
{
	XPt2D C = A;
	return C += B;
}

XPt2D operator-(XPt2D A, XPt2D B)
{
	XPt2D C = A;
	return C -= B;
}

XPt2D operator*(XPt2D A, double k)
{
	XPt2D B = A;
	return B *= k;
}

XPt2D operator*(double k, XPt2D A)
{
	XPt2D B = A;
	return B *= k;
}

XPt2D operator/(XPt2D A, double k)
{
	XPt2D B = A;
	return B /= k;
}

XPt2D operator/(double k, XPt2D A)
{
	XPt2D B = A;
	return B /= k;
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
bool operator==(XPt2D A, XPt2D B)
{
	return A.X==B.X && A.Y==B.Y; 
}

bool operator!=(XPt2D A, XPt2D B)
{
	return !(A==B);
}

//-----------------------------------------------------------------------------
// Fonctions de calcul de distances
//-----------------------------------------------------------------------------
double dist(XPt2D A, XPt2D B)			// Distance
{
	return sqrt(dist2(A, B));
}

double dist2(XPt2D A, XPt2D B)		// Distance au carre
{
	return (B.X - A.X) * (B.X - A.X) + (B.Y - A.Y) * (B.Y - A.Y);
}

//-----------------------------------------------------------------------------
// Fonctions vectorielles
//-----------------------------------------------------------------------------
double prodScal(XPt2D A, XPt2D B)	// Produit Scalaire
{
	return A.X*B.X + A.Y*B.Y;
}

double prodCross(XPt2D A, XPt2D B, XPt2D C)	// Produit en croix
{
	return (B.X - A.X)*(C.Y - A.Y) - (C.X - A.X)*(B.Y - A.Y);
}

double area(XPt2D A, XPt2D B, XPt2D C) // Aire du triangle
{
  return 0.5 * fabs(A.X*C.Y - A.X*B.Y + B.X*A.Y - B.X*C.Y + C.X*B.Y - C.X*A.Y);
}

double cap(XPt2D A, XPt2D B, XPt2D C) // Cap de la bissectrice
{
  XPt2D bissec = (A-B)/dist(A,B) + (C-B)/dist(C,B);
  if (bissec.Y >= 0.)
    return acos(bissec.X / bissec.norm());
  return 2*XPI - acos(bissec.X / bissec.norm());
}

//-----------------------------------------------------------------------------
// Equation de la droite ax + by + c = 0 passant par deux points
//-----------------------------------------------------------------------------
void droite(XPt2D M, XPt2D P, double& a, double& b, double& c)
{
  a = P.Y - M.Y;
  b = M.X - P.X;
  c = (-1.)*(a * M.X + b * M.Y);
}

//-----------------------------------------------------------------------------
// Distance du point M a la droite AB
//-----------------------------------------------------------------------------
double dist_droite(XPt2D A, XPt2D B, XPt2D M)
{
  double a, b, c;
  droite(A, B, a, b, c);
  return fabs(a * M.X + b * M.Y + c) / sqrt(a*a + b*b);
}

//-----------------------------------------------------------------------------
// Indique si la projection de M sur le segment [AB] est dans le segment
//-----------------------------------------------------------------------------
bool proj_in_seg(XPt2D A, XPt2D B, XPt2D M)
{
  if (prodScal((B - A),(M - A)) < 0.)
    return false;
  if (dist2(A, M) > dist2(A, B))
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Donne la projection de M sur le segment [AB]
//-----------------------------------------------------------------------------
XPt2D proj_seg(XPt2D A, XPt2D B, XPt2D M)
{
  return A + (prodScal((B - A),(M - A)) / dist2(A, B)) * (B - A);
}
