//-----------------------------------------------------------------------------
//								XPt3.h
//								======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/05/2003
//-----------------------------------------------------------------------------

#ifndef _XPT3_H
#define _XPT3_H

#include <iostream>
#include <cmath>

#include "XBase.h"
#include "XParserXML.h"

template<class T> class XPt2;

template<class T> class XPt3 {
public:
	T	X;
	T	Y;
	T	Z;

	// Constructeurs
	XPt3(T x=0,T y=0,T z=0): X(x), Y(y), Z(z) {}

	// Operations
	inline XPt3D& operator+=(XPt3D M){X+=M.X;Y+=M.Y;Z+=M.Z; return *this;}
	inline XPt3D& operator-=(XPt3D M){X-=M.X;Y-=M.Y;Z-=M.Z; return *this;}
	inline XPt3D& operator*=(double k){X*=k; Y*=k; Z*=k; return *this;}
	inline XPt3D& operator/=(double k){X/=k; Y/=k; Z/=k; return *this;}

	// Operateur de conversion
	operator XPt2<T>() const;

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
template<class T> bool XPt3<T>::XmlRead(XParserXML* parser, uint32 num, XError* error)
{
	XParserXML sub;
	sub	= parser->FindSubParser("/pt3", num);
	if (sub.IsEmpty())
		return XErrorError(error, "XPt3::XmlRead", XError::eBadFormat);

	X = (T)sub.ReadNodeAsDouble("/pt3/x");
	Y = (T)sub.ReadNodeAsDouble("/pt3/y");
	Z = (T)sub.ReadNodeAsDouble("/pt3/z");
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
template<class T> bool XPt3<T>::XmlWrite(std::ostream* out)
{
	*out << "<pt3> " << std::endl;
	*out << "<x> " << X << " </x>" << std::endl;
	*out << "<y> " << Y << " </y>" << std::endl;
	*out << "<z> " << Z << " </z>" << std::endl;
	*out << "</pt3>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Conversion en XPt2<T>
//-----------------------------------------------------------------------------
template<class T> XPt3<T>::operator XPt2<T>() const
{
	return XPt2<T>(X, Y);
}

//-----------------------------------------------------------------------------
// Operateurs de calcul
//-----------------------------------------------------------------------------
template<class T> XPt3<T> operator+(XPt3<T> A, XPt3<T> B)
{
	XPt3<T> C = A;
	return C += B;
}

template<class T> XPt3<T> operator-(XPt3<T> A, XPt3<T> B)
{
	XPt3<T> C = A;
	return C -= B;
}

template<class T> XPt3D operator*(XPt3<T> A, double k)
{
	XPt3<T> B = A;
	return B *= k;
}

template<class T> XPt3<T> operator*(double k, XPt3<T> A)
{
	XPt3<T> B = A;
	return B *= k;
}

template<class T> XPt3D operator/(XPt3<T> A, double k)
{
	XPt3<T> B = A;
	return B /= k;
}

template<class T> XPt3D operator/(double k, XPt3<T> A)
{
	XPt3<T> B = A;
	return B /= k;
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
template<class T> bool operator==(XPt3<T> A, XPt3<T> B)
{
	return A.X==B.X && A.Y==B.Y && A.Z ==B.Z;
}

template<class T> bool operator!=(XPt3<T> A, XPt3<T> B)
{
	return !(A==B);
}

//-----------------------------------------------------------------------------
// Fonctions de calcul de distances
//-----------------------------------------------------------------------------
template<class T> double dist(XPt3<T> A, XPt3<T> B)			// Distance
{
	return sqrt(dist2(A, B));
}

template<class T> double dist2(XPt3<T> A, XPt3<T> B)		// Distance au carre
{
	XPt3<T> C = A - B;
	return prodScal(C, C);
}

template<class T> double dist_plani(XPt3<T> A, XPt3<T> B)	// Distance planimetrique
{
	return sqrt(dist_plani2(A, B));
}

template<class T> double dist_plani2(XPt3<T> A, XPt3<T> B)	// Distance planimetrique au carre
{
	return (A.X-B.X)*(A.X-B.X) + ((A.Y-B.Y)*(A.Y-B.Y));
}

template<class T> double dist_alti(XPt3<T> A, XPt3<T> B)	// Distance altimetrique
{
	return fabs(A.Z - B.Z);
}

template<class T> double dist_polar(XPt3<T> A)			// Distance polaire en plani
{
	return sqrt(dist_polar2(A));
}

template<class T> double dist_polar2(XPt3<T> A)			// Distance polaire carre en plani
{
	return A.X*A.X + A.Y*A.Y;
}

//-----------------------------------------------------------------------------
// Fonctions vectorielles
//-----------------------------------------------------------------------------
template<class T> double prodScal(XPt3<T> A, XPt3<T> B)	// Produit Scalaire
{
	return A.X*B.X + A.Y*B.Y + A.Z*B.Z;
}

template<class T> XPt3<T> prodVect(XPt3<T> A, XPt3<T> B)	// Produit Vectoriel
{
	double x = A.Y*B.Z - B.Y*A.Z;
	double y = B.X*A.Z - A.X*B.Z;
	double z = A.X*B.Y - B.X*A.Y;
	return XPt3<T>(x, y, z);
}

template<class T> double prodMixt(XPt3<T> A, XPt3<T> B, XPt3<T> C)		// Produit Mixte
{
	return prodScal(A, prodVect(B, C));
}

//-----------------------------------------------------------------------------
// Fonctions d'entree / sortie
//-----------------------------------------------------------------------------
template<class T> std::istream& operator>>(std::istream& s, XPt3<T> M)
{
	s >> M.X >> M.Y >> M.Z;
	return s;
}

template<class T> std::ostream& operator<<(std::ostream& s, XPt3<T> M)
{
	return s << M.X << "\t" << M.Y << "\t" << M.Z;
}

#endif //_XPT3_H
