//-----------------------------------------------------------------------------
//								XPolygone2D.h
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/05/2001
//-----------------------------------------------------------------------------

#ifndef _XPOLYGONE2D_H
#define _XPOLYGONE2D_H

#include <vector>
#include "XPt2D.h"
#include "XFrame.h"

class XPolygone2D {
protected:
	std::vector<XPt2D> P;		// Points du polygone

public:
  XPolygone2D() {;}
  virtual ~XPolygone2D() {;}

	// Ajout d'un point
	void AddPt(XPt2D M) { P.push_back(M);}
	void AddPt(double x, double y) { P.push_back(XPt2D(x, y));}

	// Recuperation d'un point
	uint32_t NbPt() { return (uint32_t)P.size();}
	XPt2D Pt(uint32_t num) { if (num < P.size()) return P[num]; else return XPt2D(0, 0);}

	// Suppression de tous les points
	void Empty() { P.clear();}

	// Suppression du dernier point
	void DelPt() { P.pop_back();}

	// Fermeture du polygone
	bool Close() { if (P.size() < 3) return false; P.push_back(P[0]); return true;}

	// Test si un polygone est ferme
	bool IsClosed() { if (P.size() < 3) return false; if (P[0] == P[NbPt()-1]) return true; return false;}

	// Retourne le rectangle englobant du polygone
	XFrame Frame() const;

	// Retourne le polygone convexe englobant le polygone
	XPolygone2D Convex() const;

	// Indique si un point est contenu dans le polygone
	bool IsIn(XPt2D& P) const;

	// Regarde si deux polygones s'intersectent
	bool Intersect(XPolygone2D& pol) const;

	// Renvoie le barycentre du polygone
	XPt2D Center() const;

	// Expansion du polygone
	XPolygone2D Expand(double d);

	// Surface du polygone
	double Area();

	// Longueur du polygone
	double Dist();

	// Retourne true si le polygone est decrit dans le sens trigonometrique, false sinon
	bool Direction();

  // Met a l'echelle et translate le polygone
  void Scale(double X0, double Y0, double scaleX, double scaleY);

	// Import/Export au format XML
	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

#endif //_XPOLYGONE2D_H

