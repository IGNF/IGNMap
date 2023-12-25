//-----------------------------------------------------------------------------
//								XPolygone2D.cpp
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/05/2001
//-----------------------------------------------------------------------------

#include "XPolygone2D.h"


//-----------------------------------------------------------------------------
// Retourne le rectangle englobant du polygone
//-----------------------------------------------------------------------------
XFrame  XPolygone2D::Frame() const
{
	if (P.size() < 1)
		return XFrame(0, 0, 0, 0);

	double xmin, xmax, ymin, ymax;
	xmin = xmax = P[0].X;
	ymin = ymax = P[0].Y;

	for (uint32_t i = 1; i < P.size(); i++) {
		xmin = XMin(xmin, P[i].X);
		ymin = XMin(ymin, P[i].Y);
		xmax = XMax(xmax, P[i].X);
		ymax = XMax(ymax, P[i].Y);
	}
	return XFrame(xmin, ymin, xmax, ymax);
}

//-----------------------------------------------------------------------------
// Retourne le polygone convexe englobant le polygone
//-----------------------------------------------------------------------------
XPolygone2D XPolygone2D::Convex() const
{
	return XPolygone2D();
}
	
//-----------------------------------------------------------------------------
// Indique si un point est contenu dans le polygone
//-----------------------------------------------------------------------------
bool XPolygone2D::IsIn(XPt2D& M) const
{
	if (P.size() < 3)
		return false;
	XFrame F = Frame();
	if (!F.IsIn(M))
		return false;

	double alpha;
	int nb_inter = 0;
	XPt2D A, B, C;
	for (int i = 0; i < P.size() - 1; i++) {
		A = P[i];
		B = P[i + 1];
		if ((A.X == M.X)&&(A.Y == M.Y)){
			nb_inter = 0;
			break;
		}
		alpha = ((A.X - A.Y) -  (M.X - M.Y)) / ((A.X - A.Y) -  (B.X - B.Y));
		if ((alpha <= 0.0)||(alpha > 1.0))
			continue;
		C = A + alpha * (B - A);
		C -= M;
		if ((C.X > 0.0)&&(C.Y > 0.0))
			nb_inter++;
	}
	if ((nb_inter % 2) == 0)	// Nombre pair d'intersection
		return false;
	else
		return true;
}

//-----------------------------------------------------------------------------
// Regarde si deux emprises s'intersectent
//-----------------------------------------------------------------------------
bool XPolygone2D::Intersect(XPolygone2D& pol) const
{
	XPt2D M;
	for (uint32_t i = 0; i < pol.NbPt(); i++) {
		M = pol.Pt(i);
		if (IsIn(M))
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Renvoie le barycentre du polygone
//-----------------------------------------------------------------------------
XPt2D XPolygone2D::Center() const
{
	XPt2D bary;
	for (uint32_t i = 0; i < P.size(); i++)
		bary += (P[i] / (double)P.size());
	return bary;
}

//-----------------------------------------------------------------------------
// Expansion du polygone
//-----------------------------------------------------------------------------
XPolygone2D XPolygone2D::Expand(double d)
{
	XPt2D bary = Center();
	XPolygone2D poly;

	for (uint32_t i = 0; i < P.size(); i++) {
		XPt2D M = P[i];
		poly.AddPt(((dist(bary, M) + d)/dist(bary, M))*(M - bary) + bary);
	}
	return poly;
}

//-----------------------------------------------------------------------------
// Surface du polygone
//-----------------------------------------------------------------------------
double XPolygone2D::Area()
{
	if (P.size() < 3)
		return 0.;
	double area = 0.;
  uint32_t i;
  for (i = 0; i < P.size() - 1; i++)
		area += ( (P[i+1].Y + P[i].Y) * (P[i+1].X - P[i].X) * 0.5);
	// Attention : il faut fermer sur le premier point du polygone
	i = (uint32_t)P.size() - 1;
	area += ( (P[0].Y + P[i].Y) * (P[0].X - P[i].X) * 0.5);
	return area;
}

//-----------------------------------------------------------------------------
// Perimetre du polygone
//-----------------------------------------------------------------------------
double XPolygone2D::Dist()
{
	double d = 0.;
	if (P.size() < 2)
		return 0;
	for (uint32_t i = 0; i < P.size() - 1; i++)
		d += dist2(P[i], P[i+1]);
	return sqrt(d);
}

//-----------------------------------------------------------------------------
// Retourne true si le polygone est decrit dans le sens trigonometrique, false sinon
//-----------------------------------------------------------------------------
bool XPolygone2D::Direction()
{
	if (P.size() < 3)
		return true;
	uint32_t nbPt = (uint32_t)P.size();
	if (IsClosed())
		nbPt -= 1;
	double xmin = P[0].X;
	uint32_t num = 0;
	for (uint32_t i = 1; i < nbPt; i++)
		if (P[i].X < xmin) {
			xmin = P[i].X;
			num = i;
		}

	XPt2D A, B = P[num], C;
	if (num == 0)
		A = P[nbPt - 1];
	else
		A = P[num - 1];
	if (num == nbPt - 1)
		C = P[0];
	else
		C = P[num + 1];

	if (prodMixt(A, B, C) > 0)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Met a l'echelle et translate le polygone
//-----------------------------------------------------------------------------
void XPolygone2D::Scale(double X0, double Y0, double scaleX, double scaleY)
{
  for (uint32_t i = 0; i < P.size(); i++) {
    P[i].X = X0 + P[i].X * scaleX;
    P[i].Y = Y0 + P[i].Y * scaleY;
  }
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPolygone2D::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML poly = parser->FindSubParser("/polygon2d", num);
	if (poly.IsEmpty())
		return XErrorError(error, "XPolygone2D::XmlRead", XError::eBadFormat);

	uint32_t nb_pt = poly.ReadNodeAsUInt32("/polygon2d/nb_pt");

	for (uint32_t i = 0; i < nb_pt; i++) {
		double x = poly.ReadNodeAsDouble("/polygon2d/x", i);
		double y = poly.ReadNodeAsDouble("/polygon2d/y", i);
		P.push_back(XPt2D(x, y));
	}

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XPolygone2D::XmlWrite(std::ostream* out)
{
	int prec = (int)out->precision(2);						// Sauvegarde des parametres du flux
	std::ios::fmtflags flags = out->setf(std::ios::fixed);

	*out << "<polygon2d>" << std::endl;
	*out << "<nb_pt> " << P.size() << " </nb_pt>" << std::endl;

	for (uint32_t i = 0; i < P.size(); i++) {
		*out << "<x> " << P[i].X << "</x> ";
		*out << "<y> " << P[i].Y << "</y> " << std::endl;
	}
	*out << "</polygon2d>" << std::endl;

	out->precision(prec);		// Restauration des parametres du flux
	out->unsetf(std::ios::fixed);
	out->setf(flags);
	return out->good();
}
