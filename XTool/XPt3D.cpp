//-----------------------------------------------------------------------------
//								XPt3D.cpp
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 06/10/00
//-----------------------------------------------------------------------------

#include <cmath>
#include "XPt3D.h"
#include "XPt2D.h"

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPt3D::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML sub;
	sub	= parser->FindSubParser("/pt3d", num);
	if (sub.IsEmpty())
		return XErrorError(error, "XPt3D::XmlRead", XError::eBadFormat);

	X = sub.ReadNodeAsDouble("/pt3d/x");
	Y = sub.ReadNodeAsDouble("/pt3d/y");
	Z = sub.ReadNodeAsDouble("/pt3d/z");
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPt3D::XmlWrite(std::ostream* out)
{
	*out << "<pt3d> " << std::endl;
	*out << "<x> " << X << " </x>" << std::endl;
	*out << "<y> " << Y << " </y>" << std::endl;
	*out << "<z> " << Z << " </z>" << std::endl;
	*out << "</pt3d>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Conversion en XPt2D
//-----------------------------------------------------------------------------
XPt3D::operator XPt2D() const
{
	return XPt2D(X, Y);
}

//-----------------------------------------------------------------------------
// Norme
//-----------------------------------------------------------------------------
double XPt3D::norm()
{
  return sqrt(norm2());
}

//-----------------------------------------------------------------------------
// Egalite a un epsilon
//-----------------------------------------------------------------------------
bool XPt3D::egal(const XPt3D& M, const double& epsilon)
{
  if (fabs(X - M.X) > epsilon)
    return false;
  if (fabs(Y - M.Y) > epsilon)
    return false;
  return true;
  if (fabs(Z - M.Z) > epsilon)
    return false;
}

//-----------------------------------------------------------------------------
// Operateurs de calcul
//-----------------------------------------------------------------------------
XPt3D operator+(XPt3D A, XPt3D B)
{
	XPt3D C = A;
	return C += B;
}

XPt3D operator-(XPt3D A, XPt3D B)
{
	XPt3D C = A;
	return C -= B;
}

XPt3D operator*(XPt3D A, double k)
{
	XPt3D B = A;
	return B *= k;
}

XPt3D operator*(double k, XPt3D A)
{
	XPt3D B = A;
	return B *= k;
}

XPt3D operator/(XPt3D A, double k)
{
	XPt3D B = A;
	return B /= k;
}

XPt3D operator/(double k, XPt3D A)
{
	XPt3D B = A;
	return B /= k;
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
bool operator==(XPt3D A, XPt3D B)
{
	return A.X==B.X && A.Y==B.Y && A.Z ==B.Z; 
}

bool operator!=(XPt3D A, XPt3D B)
{
	return !(A==B);
}

//-----------------------------------------------------------------------------
// Fonctions de calcul de distances
//-----------------------------------------------------------------------------
double dist(XPt3D A, XPt3D B)			// Distance
{
	return sqrt(dist2(A, B));
}

double dist2(XPt3D A, XPt3D B)		// Distance au carre
{
	XPt3D C = A - B;
	return prodScal(C, C);
}

double dist_plani(XPt3D A, XPt3D B)	// Distance planimetrique
{
	return sqrt(dist_plani2(A, B));
}

double dist_plani2(XPt3D A, XPt3D B)	// Distance planimetrique au carre
{
	return (A.X-B.X)*(A.X-B.X) + ((A.Y-B.Y)*(A.Y-B.Y));
}

double dist_alti(XPt3D A, XPt3D B)	// Distance altimetrique
{
	return fabs(A.Z - B.Z);
}

double dist_polar(XPt3D A)			// Distance polaire en plani
{
	return sqrt(dist_polar2(A));
}

double dist_polar2(XPt3D A)			// Distance polaire carre en plani
{
	return A.X*A.X + A.Y*A.Y;
}

//-----------------------------------------------------------------------------
// Fonctions vectorielles
//-----------------------------------------------------------------------------
double prodScal(XPt3D A, XPt3D B)	// Produit Scalaire
{
	return A.X*B.X + A.Y*B.Y + A.Z*B.Z;
}

XPt3D prodVect(XPt3D A, XPt3D B)	// Produit Vectoriel
{
	double x = A.Y*B.Z - B.Y*A.Z;
	double y = B.X*A.Z - A.X*B.Z;
	double z = A.X*B.Y - B.X*A.Y;
	return XPt3D(x, y, z);
}

double prodMixt(XPt3D A, XPt3D B, XPt3D C)		// Produit Mixte
{
	return prodScal(A, prodVect(B, C));
}

//-----------------------------------------------------------------------------
// Fonctions d'entree / sortie
//-----------------------------------------------------------------------------
std::istream& operator>>(std::istream& s, XPt3D& M)
{
	s >> M.X >> M.Y >> M.Z;
	return s;
}

std::ostream& operator<<(std::ostream& s, XPt3D M)
{
	return s << M.X << "\t" << M.Y << "\t" << M.Z;
}
