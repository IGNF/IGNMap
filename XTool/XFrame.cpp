//-----------------------------------------------------------------------------
//								XFrame.cpp
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/10/2000
//-----------------------------------------------------------------------------

#include "XFrame.h"
#include "XParserXML.h"
#include "XBase.h"

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------
XFrame& XFrame::operator+=(XFrame r)
{
	if (r.IsEmpty())
		return *this;
	if (IsEmpty()) {
		*this = r;
		return *this;
	}

	Xmin = XMin(Xmin, r.Xmin);
	Ymin = XMin(Ymin, r.Ymin);
	Xmax = XMax(Xmax, r.Xmax);
	Ymax = XMax(Ymax, r.Ymax);
	return *this;
}

XFrame& XFrame::operator+=(XPt2D P)
{
	if (IsEmpty()) {
		Xmin = Xmax = P.X;
		Ymin = Ymax = P.Y;
		return *this;
	}

	Xmin = XMin(Xmin, P.X);
	Ymin = XMin(Ymin, P.Y);
	Xmax = XMax(Xmax, P.X);
	Ymax = XMax(Ymax, P.Y);
	return *this;
}

XFrame& XFrame::operator+=(double d)
{
  if (IsEmpty())
    return *this;
  Xmin -= d;
  Ymin -= d;
  Xmax += d;
  Ymax += d;
  return *this;
}

XFrame& XFrame::operator*=(double k)
{
	Xmin -= k*Width();
	Ymin -= k*Height();
	Xmax += k*Width();
	Ymax += k*Height();
	return *this;
}

//-----------------------------------------------------------------------------
// Intersection
//-----------------------------------------------------------------------------
bool XFrame::Intersect(const XFrame& r) const
{
	if ((Xmin > r.Xmax)||(Xmax < r.Xmin))
		return false;
	if ((Ymin > r.Ymax)||(Ymax < r.Ymin))
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Teste si un point est dans le cadre
//-----------------------------------------------------------------------------
bool XFrame::IsIn(const XPt2D& P) const
{
	if ((P.X < Xmin)||(P.Y < Ymin))
		return false;
	if ((P.X > Xmax)||(P.Y > Ymax))
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Teste si le cadre est completement inclus dans un cadre
//-----------------------------------------------------------------------------
bool XFrame::IsIn(const XFrame& r) const
{
	if ((Xmin < r.Xmin)||(Ymin < r.Ymin))
		return false;
	if ((Xmax > r.Xmax)||(Ymax > r.Ymax))
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
bool operator==(XFrame A, XFrame B)
{
	return A.Xmin==B.Xmin && A.Ymin==B.Ymin && A.Xmax==B.Xmax && A.Ymax==B.Ymax;
}

bool operator!=(XFrame A, XFrame B)
{
	return !(A==B);
}

//-----------------------------------------------------------------------------
// Fonctions de calcul de distances
//-----------------------------------------------------------------------------
double dist(XFrame A, XFrame B)
{
  return  dist(XPt2D(A.Xmin, A.Ymin), XPt2D(B.Xmin, B.Ymin)) +
          dist(XPt2D(A.Xmax, A.Ymin), XPt2D(B.Xmax, B.Ymin)) +
          dist(XPt2D(A.Xmax, A.Ymax), XPt2D(B.Xmax, B.Ymax)) +
          dist(XPt2D(A.Xmin, A.Ymax), XPt2D(B.Xmin, B.Ymax));
}

//-----------------------------------------------------------------------------
// Arrondi du cadre
//-----------------------------------------------------------------------------
void XFrame::Round(double unit)
{
  Xmin = floor(Xmin / unit) * unit;
  Xmax = ceil(Xmax / unit) * unit;
  Ymin = floor(Ymin / unit) * unit;
  Ymax = ceil(Ymax / unit) * unit;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XFrame::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML sub;
	sub	= parser->FindSubParser("/frame", num);
	if (sub.IsEmpty())
		return XErrorError(error, "XFrame::XmlRead", XError::eBadFormat);

  Xmin = sub.ReadNodeAsDouble("/frame/xmin");
  Ymin = sub.ReadNodeAsDouble("/frame/ymin");
  Xmax = sub.ReadNodeAsDouble("/frame/xmax");
  Ymax = sub.ReadNodeAsDouble("/frame/ymax");
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XFrame::XmlWrite(std::ostream* out)
{
	*out << "<frame> " << std::endl;
	*out << "<xmin> " << Xmin << " </xmin>" << std::endl;
	*out << "<ymin> " << Ymin << " </ymin>" << std::endl;
	*out << "<xmax> " << Xmax << " </xmax>" << std::endl;
	*out << "<ymax> " << Ymax << " </ymax>" << std::endl;
	*out << "</frame>" << std::endl;
	return out->good();
}
