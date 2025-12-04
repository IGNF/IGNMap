//-----------------------------------------------------------------------------
//								XFrame.h
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/10/2000
//-----------------------------------------------------------------------------

#ifndef _XFRAME_H
#define _XFRAME_H

#include <iostream>
#include "XPt2D.h"
#include "XPt3D.h"

class XParserXML;
class XError;

class XFrame {
public:
	double	Xmin;
	double	Ymin;
	double	Xmax;
	double	Ymax;

	XFrame(double xmin=0, double ymin=0, double xmax=0, double ymax=0) :
		Xmin(xmin), Ymin(ymin), Xmax(xmax), Ymax(ymax) {
		;
	}

	// Operations
	XFrame& operator+=(XFrame r);
	XFrame& operator+=(XPt2D P);
	XFrame& operator+=(XPt3D P) { return *this += XPt2D(P.X, P.Y);}
  XFrame& operator+=(double d);
	XFrame& operator*=(double k);

  bool Intersect(const XFrame &r) const;
	bool IsIn(const XPt2D& P) const;
	bool IsIn(const XFrame& r) const;
	inline bool IsEmpty() const { return ((Xmin == 0)&&(Ymin == 0)&&(Xmax == 0)&&(Ymax == 0));}
	inline double Width() const { return Xmax - Xmin;}
	inline double Height() const { return Ymax - Ymin;}
  inline double Size() const { return (Xmax - Xmin + Ymax - Ymin);}
	XPt2D Center() const { return XPt2D((Xmax + Xmin)*0.5, (Ymax + Ymin)*0.5);}
  void Round(double unit = 1.0);

	bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	bool XmlWrite(std::ostream* out);
};

// Operateurs logiques
bool operator==(XFrame, XFrame);
bool operator!=(XFrame, XFrame);

// Fonctions de calcul de distances
double dist(XFrame, XFrame);			// Distance

#endif //_XFRAME_H
