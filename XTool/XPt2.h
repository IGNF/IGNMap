//-----------------------------------------------------------------------------
//								XPt2.h
//								======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/05/2003
//-----------------------------------------------------------------------------

#ifndef _XPT2_H
#define _XPT2_H

#include <iostream>
#include "XBase.h"
#include "XParserXML.h"

template<class T> class XPt3;

template<class T> class XPt2 {
public:
	T	X;
	T	Y;

	// Constructeurs
	XPt2(T x=0,T y=0): X(x), Y(y) {}

	// Operations
	inline XPt2& operator+=(XPt2D M){X+=M.X;Y+=M.Y; return *this;}
	inline XPt2& operator-=(XPt2D M){X-=M.X;Y-=M.Y; return *this;}
	inline XPt2& operator*=(double k){X*=k; Y*=k; return *this;}
	inline XPt2& operator/=(double k){X/=k; Y/=k; return *this;}

	// Operateur de conversion
	operator XPt3<T>() const;

	virtual bool XmlRead(XParserXML* parser, uint32 num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
template<class T> bool XPt2<T>::XmlRead(XParserXML* parser, uint32 num, XError* error)
{
	XParserXML sub;
	sub	= parser->FindSubParser("/pt2", num);
	if (sub.IsEmpty())
		return XErrorError(error, "XPt2::XmlRead", XError::eBadFormat);

	X = (T)sub.ReadNodeAsDouble("/pt2/x");
	Y = (T)sub.ReadNodeAsDouble("/pt2/y");
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
template<class T> bool XPt2<T>::XmlWrite(std::ostream* out)
{
	*out << "<pt2> " << std::endl;
	*out << "<x> " << X << " </x>" << std::endl;
	*out << "<y> " << Y << " </y>" << std::endl;
	*out << "</pt2>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Conversion en XPt3D
//-----------------------------------------------------------------------------
template<class T> XPt2<T>::operator XPt3<T>() const
{
	return XPt3<T>(X, Y, 0);
}

//-----------------------------------------------------------------------------
// Operateurs de calcul
//-----------------------------------------------------------------------------
template<class T> XPt2<T> operator+(XPt2<T> A, XPt2<T> B)
{
	XPt2<T> C = A;
	return C += B;
}

template<class T> XPt2<T> operator-(XPt2<T> A, XPt2<T> B)
{
	XPt2<T> C = A;
	return C -= B;
}

template<class T> XPt2<T> operator*(XPt2<T> A, double k)
{
	XPt2<T> B = A;
	return B *= k;
}

template<class T> XPt2<T> operator*(double k, XPt2<T> A)
{
	XPt2<T> B = A;
	return B *= k;
}

template<class T> XPt2<T> operator/(XPt2<T> A, double k)
{
	XPt2<T> B = A;
	return B /= k;
}

template<class T> XPt2D operator/(double k, XPt2<T> A)
{
	XPt2<T> B = A;
	return B /= k;
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
template<class T> bool operator==(XPt2<T> A, XPt2<T> B)
{
	return A.X==B.X && A.Y==B.Y;
}

template<class T> bool operator!=(XPt2<T> A, XPt2<T> B)
{
	return !(A==B);
}

//-----------------------------------------------------------------------------
// Fonctions de calcul de distances
//-----------------------------------------------------------------------------
template<class T> double dist(XPt2<T> A, XPt2<T> B)			// Distance
{
	return sqrt(dist2(A, B));
}

template<class T> double dist2(XPt2<T> A, XPt2<T> B)		// Distance au carre
{
	return (B.X - A.X) * (B.X - A.X) + (B.Y - A.Y) * (B.Y - A.Y);
}

//-----------------------------------------------------------------------------
// Fonctions vectorielles
//-----------------------------------------------------------------------------
template<class T> double prodScal(XPt2D A, XPt2D B)	// Produit Scalaire
{
	return A.X*B.X + A.Y*B.Y;
}

template<class T> double prodCross(XPt2<T> A, XPt2<T> B, XPt2<T> C)	// Produit en croix
{
	return (B.X - A.X)*(C.Y - A.Y) - (C.X - A.X)*(B.Y - A.Y);
}

#endif //_XPT2_H
