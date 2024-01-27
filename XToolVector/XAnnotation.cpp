//-----------------------------------------------------------------------------
//								XAnnotation.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP
//
// 09/02/2010
//-----------------------------------------------------------------------------

#include "XAnnotation.h"
#include <sstream>
#include "../XToolGeod/XGeoPref.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XAnnotation::XAnnotation()
{
  m_Primitive = pNull;
  m_Repres = new XGeoRepres;
  //m_Repres->Size(3);
  m_T = NULL;
}

//-----------------------------------------------------------------------------
// Nettoyage
//-----------------------------------------------------------------------------
void XAnnotation::Clear()
{
  m_Primitive = pNull;
  m_Pt.clear();
  m_Frame = XFrame();
  Unload();
}

//-----------------------------------------------------------------------------
// Nombre de points de la geometrie
//-----------------------------------------------------------------------------
uint32_t XAnnotation::NbPt() const
{
  if ((m_Primitive >= pPolyline) && (m_Primitive <= pPolygon))
    return m_Pt.size();
  if ((m_Primitive == pEllipse) || (m_Primitive == pRect))
    return 4;
  if (m_Primitive == pText)
    return 1;
  return 0;
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie
//-----------------------------------------------------------------------------
bool XAnnotation::LoadGeom()
{
  if (m_T != NULL) delete[] m_T;
  m_T = new XPt[NbPt()];
  if (m_T == NULL) return false;
  if ((m_Primitive >= pPolyline) && (m_Primitive <= pPolygon)) {
    for (uint32_t i = 0; i < m_Pt.size(); i++) {
      m_T[i].X = m_Pt[i].X;
      m_T[i].Y = m_Pt[i].Y;
    }
  }
  if ((m_Primitive == pEllipse) || (m_Primitive == pRect)) {
    m_T[0].X = m_T[3].X = m_Frame.Xmin;
    m_T[1].X = m_T[2].X = m_Frame.Xmax;
    m_T[0].Y = m_T[1].Y = m_Frame.Ymax;
    m_T[2].Y = m_T[3].Y = m_Frame.Ymin;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XAnnotation::Unload()
{
  if (m_T != NULL) delete[] m_T;
  m_T = NULL;
}

//-----------------------------------------------------------------------------
// Indique si un vecteur est proche d'un point terrain
//-----------------------------------------------------------------------------
bool XAnnotation::IsNear2D(const XPt2D& P, double dist)
{
  if ((m_Primitive == pPolygon)||(m_Primitive == pEllipse)||
      (m_Primitive == pRect))
    return IsIn2D(P);

  if (m_Primitive == pText) {
    if (dist2(P, m_Frame.Center()) < dist*dist)
      return true;
    else
      return false;
  }

  if ((m_Primitive != pPolyline)&&(m_Primitive != pArrow)&&
      (m_Primitive != pCurve))
    return false;

  double d2 = dist * dist;
  double d, am, ab, prod;
  double minx, maxx, miny, maxy;
  if (!LoadGeom())
    return false;
  for (uint32_t j = 0; j < NbPt() - 1; j++) {
    minx = XMin(m_Pt[j].X, m_Pt[j + 1].X);
    if (P.X < minx - dist)
      continue;
    maxx = XMax(m_Pt[j].X, m_Pt[j + 1].X);
    if (P.X > maxx + dist)
      continue;
    miny = XMin(m_Pt[j].Y, m_Pt[j + 1].Y);
    if (P.Y < miny - dist)
      continue;
    maxy = XMax(m_Pt[j].Y, m_Pt[j + 1].Y);
    if (P.Y > maxy + dist)
      continue;

    prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
    am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
    ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) +
          (m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
    if (ab == 0)
        continue;
    d = am - (prod * prod) / ab;
    if (d <= d2){
      Unload();
      return true;
    }
  }
  Unload();
  return false;
}

//-----------------------------------------------------------------------------
// Geometrie fermee
//-----------------------------------------------------------------------------
bool XAnnotation::IsClosed() const
{
  if ((m_Primitive >= pPolyline)&&(m_Primitive < pPolygon))return false;
  return true;
}

//-----------------------------------------------------------------------------
// Indique si le tableau de points est ferme
//-----------------------------------------------------------------------------
bool XAnnotation::IsPtClosed()
{
  if(m_Pt.size() < 2)
    return false;
  XPt P = m_Pt[0];
  XPt M = m_Pt[m_Pt.size() - 1];
 if ((P.X == M.X)&&(P.Y == M.Y))
   return true;
 return false;
}

//-----------------------------------------------------------------------------
// Type vecteur de l'annotation
//-----------------------------------------------------------------------------
XGeoVector::eTypeVector XAnnotation::TypeVector() const
{
  switch(m_Primitive) {
  case pPolyline : return XGeoVector::Line;
  case pArrow : return XGeoVector::Line;
  case pCurve : return XGeoVector::Line;
  case pPolygon : return XGeoVector::Poly;
  case pEllipse : return XGeoVector::Poly;
  case pRect : return XGeoVector::Poly;
  case pText : return XGeoVector::Point;
  }
  return XGeoVector::Null;
}

//-----------------------------------------------------------------------------
// Nom de l'annotation : son type ou son texte
//-----------------------------------------------------------------------------
std::string XAnnotation::Name()
{
  if (m_strText.size() > 1)
    return m_strText;
  switch(m_Primitive) {
  case pPolyline : return "Polyligne";
  case pArrow : return "Flèche";
  case pCurve : return "Courbe";
  case pPolygon : return "Polygone";
  case pEllipse : return "Ellipse";
  case pRect : return "Rectangle";
  case pText : return m_strText;
  }
  return "";
}

//-----------------------------------------------------------------------------
// Calcul du rectangle englobant
//-----------------------------------------------------------------------------
void XAnnotation::ComputeFrame()
{
  m_Frame = XFrame();
  if (m_Pt.size() < 1)
    return;
  m_Frame.Xmax = m_Frame.Xmin = m_Pt[0].X;
  m_Frame.Ymax = m_Frame.Ymin = m_Pt[0].Y;
  for (uint32_t i = 0; i < m_Pt.size(); i++) {
    m_Frame.Xmax = XMax(m_Frame.Xmax, m_Pt[i].X);
    m_Frame.Xmin = XMin(m_Frame.Xmin, m_Pt[i].X);
    m_Frame.Ymax = XMax(m_Frame.Ymax, m_Pt[i].Y);
    m_Frame.Ymin = XMin(m_Frame.Ymin, m_Pt[i].Y);
  }
}

//-----------------------------------------------------------------------------
// Ferme une annotation si necessaire
//-----------------------------------------------------------------------------
bool XAnnotation::Close()
{
  if (m_Pt.size() < 1)
    return false;
  std::vector<XPt> T;
  XPt A = m_Pt[0];
  T.push_back(A);
  for (uint32_t i = 1; i < m_Pt.size(); i++) {
    if ((A.X == m_Pt[i].X)&&(A.Y == m_Pt[i].Y))
      continue;
    A = m_Pt[i];
    T.push_back(A);
  }
  m_Pt = T;
  if (m_Primitive != pPolygon)
    return true;
  if (m_Pt.size() < 3)
    return false;
  if (m_Pt[0] != m_Pt[m_Pt.size() - 1])
    m_Pt.push_back(m_Pt[0]);
  return true;
}

//-----------------------------------------------------------------------------
// Ajout d'un point
//-----------------------------------------------------------------------------
void XAnnotation::AddPt(double x, double y)
{
  XPt P;
  P.X = x;
  P.Y = y;
  m_Pt.push_back(P);
  m_Frame += XPt2D(x, y);
}

//-----------------------------------------------------------------------------
// Modification du dernier point ou ajout s'il n'y a qu'un point
//-----------------------------------------------------------------------------
void XAnnotation::ModPt(double x, double y)
{
  XPt P;
  P.X = x;
  P.Y = y;
  if (m_Pt.size() > 1)
    m_Pt.pop_back();
  m_Pt.push_back(P);
  ComputeFrame();
}

//-----------------------------------------------------------------------------
// Deplacement d'un point de la geometrie
//-----------------------------------------------------------------------------
void XAnnotation::MovePt(int& index, double x, double y)
{
  if (index < 0)
    return;
  if ((m_Primitive >= pPolyline) && (m_Primitive <= pPolygon)){
    if (index < m_Pt.size()) {
      m_Pt[index].X = x;
      m_Pt[index].Y = y;
    }
    return;
  }
  if ((m_Primitive == pRect) || (m_Primitive == pEllipse)){
    double opp_x, opp_y;
    if (index == 0) {
      opp_x = m_Frame.Xmax; opp_y = m_Frame.Ymin;
    }
    if (index == 1) {
      opp_x = m_Frame.Xmin; opp_y = m_Frame.Ymin;
    }
    if (index == 2) {
      opp_x = m_Frame.Xmin; opp_y = m_Frame.Ymax;
    }
    if (index == 3) {
      opp_x = m_Frame.Xmax; opp_y = m_Frame.Ymax;
    }
    m_Frame = XFrame(XMin(x, opp_x), XMin(y, opp_y), XMax(x, opp_x), XMax(y, opp_y));
    if (x == m_Frame.Xmin) {
      if (y == m_Frame.Ymax) index = 0; else index = 3;
    }
    if (x == m_Frame.Xmax) {
      if (y == m_Frame.Ymax) index = 1; else index = 2;
    }
    m_Pt.clear();
    XPt P;
    P.X = m_Frame.Xmin;
    P.Y = m_Frame.Ymax;
    m_Pt.push_back(P);
    P.X = m_Frame.Xmax;
    P.Y = m_Frame.Ymin;
    m_Pt.push_back(P);
    return;
  }
  if (m_Primitive == pText)
    if (index == 0) {
      m_Frame.Xmin = m_Frame.Xmax = x;
      m_Frame.Ymax = m_Frame.Ymin = y;
      if (m_Pt.size() > 0) {
          m_Pt[0].X = x;
          m_Pt[0].Y = y;
      }
    }

}

//-----------------------------------------------------------------------------
// Deplacement de l'ensemble de la geometrie
//-----------------------------------------------------------------------------
void XAnnotation::Move(double x, double y)
{
  for (uint32_t i = 0; i < m_Pt.size(); i++) {
    m_Pt[i].X += x;
    m_Pt[i].Y += y;
  }
  ComputeFrame();
}

//-----------------------------------------------------------------------------
// Destruction du dernier point
//-----------------------------------------------------------------------------
void XAnnotation::RemovePt()
{
  m_Pt.pop_back();
  ComputeFrame();
}

//-----------------------------------------------------------------------------
// Destruction du ieme point
//-----------------------------------------------------------------------------
void XAnnotation::DeletePt(int index)
{
  if ((m_Primitive < pPolyline) && (m_Primitive > pPolygon))
    return;
  if (index >= m_Pt.size())
    return;
  std::vector<XPt>::iterator iter = m_Pt.begin();
  iter += index;
  m_Pt.erase(iter);
  ComputeFrame();
  Close();
}

//-----------------------------------------------------------------------------
// Renvoi un point de construction
//-----------------------------------------------------------------------------
XPt2D XAnnotation::Pt(uint32_t i)
{
  if ((m_Primitive >= pPolyline) && (m_Primitive <= pPolygon)){
    if (i < m_Pt.size())
      return XPt2D(m_Pt[i].X, m_Pt[i].Y);
    return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
  }
  if ((m_Primitive == pRect) || (m_Primitive == pEllipse)){
    if (i == 0) return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
    if (i == 1) return XPt2D(m_Frame.Xmax, m_Frame.Ymax);
    if (i == 2) return XPt2D(m_Frame.Xmax, m_Frame.Ymin);
    if (i == 3) return XPt2D(m_Frame.Xmin, m_Frame.Ymin);
  }
  if (m_Primitive == pText)
    return XPt2D(m_Frame.Xmin, m_Frame.Ymax);

  return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Fixe la primitive
//-----------------------------------------------------------------------------
void XAnnotation::Primitive(ePrimitive prim)
{
  if ( m_Primitive == prim)
    return;
  Clear();
  m_Primitive = prim;
}

//-----------------------------------------------------------------------------
// Trouve le point d'edition le plus proche
//-----------------------------------------------------------------------------
int XAnnotation::FindClosestEditPoint(XPt2D P, double distance)
{
  double d2 = distance * distance;
  if ((m_Primitive >= pPolyline) && (m_Primitive <= pPolygon)){
    for (uint32_t i = 0; i < m_Pt.size(); i++) {
      if (dist2(XPt2D(m_Pt[i].X, m_Pt[i].Y), P) < d2)
        return i;
    }
    return -1;
  }
  if ((m_Primitive == pRect) || (m_Primitive == pEllipse)){
    if (dist2(XPt2D(m_Frame.Xmin, m_Frame.Ymax), P) < d2)
      return 0;
    if (dist2(XPt2D(m_Frame.Xmax, m_Frame.Ymax), P) < d2)
      return 1;
    if (dist2(XPt2D(m_Frame.Xmax, m_Frame.Ymin), P) < d2)
      return 2;
    if (dist2(XPt2D(m_Frame.Xmin, m_Frame.Ymin), P) < d2)
      return 3;
    return -1;
  }
  if (m_Primitive == pText)
    if (dist2(XPt2D(m_Frame.Xmin, m_Frame.Ymax), P) < d2)
      return 0;
  return -1;
}

//-----------------------------------------------------------------------------
// Insertion d'un point
//-----------------------------------------------------------------------------
bool XAnnotation::InsertPt(double x, double y, double dist)
{
  XPt P;
  P.X = x; P.Y = y;
  if ((m_Primitive < pPolyline) && (m_Primitive > pPolygon))
    return false;

  if (m_Pt.size() < 2)
    return false;
  if (!IsNear2D(XPt2D(x, y), dist))
    return false;

  if (m_Primitive == pPolygon)
    if (!IsPtClosed())
      m_Pt.push_back(m_Pt[0]);

  double d2 = dist * dist;
  double d, am, ab, prod;
  double minx, maxx, miny, maxy;
  for (uint32_t j = 0; j < m_Pt.size() - 1; j++) {
    minx = XMin(m_Pt[j].X, m_Pt[j + 1].X);
    if (P.X < minx - dist)
      continue;
    maxx = XMax(m_Pt[j].X, m_Pt[j + 1].X);
    if (P.X > maxx + dist)
      continue;
    miny = XMin(m_Pt[j].Y, m_Pt[j + 1].Y);
    if (P.Y < miny - dist)
      continue;
    maxy = XMax(m_Pt[j].Y, m_Pt[j + 1].Y);
    if (P.Y > maxy + dist)
      continue;

    prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
    am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
    ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) +
          (m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
    if (ab == 0)
        continue;
    d = am - (prod * prod) / ab;
    if (d <= d2){
      std::vector<XPt>::iterator iter = m_Pt.begin();
      iter += (j + 1);
      m_Pt.insert(iter, P);
//      if (m_Primitive == pPolygon)
//        if (IsPtClosed())
//          m_Pt.pop_back();
      ComputeFrame();
      return true;
    }
  }
  if (m_Primitive == pPolygon)
    if (IsPtClosed())
      m_Pt.pop_back();

  return false;
}

//-----------------------------------------------------------------------------
// Ajout des points d'une annotation
//-----------------------------------------------------------------------------
void XAnnotation::Add(XAnnotation* A)
{
  if ((m_Primitive < pPolyline) && (m_Primitive > pPolygon))
    return;
  for (uint32_t i = 0; i < A->NbPt(); i++) {
    if (A->m_Pt[i] != m_Pt[m_Pt.size()-1])
      m_Pt.push_back(A->m_Pt[i]);
  }
  ComputeFrame();
}

//-----------------------------------------------------------------------------
// Lecture en XML
//-----------------------------------------------------------------------------
bool XAnnotation::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
  XParserXML sub;
  sub	= parser->FindSubParser("/xannotation", num);
  if (sub.IsEmpty())
    return XErrorError(error, "XAnnotation::XmlRead", XError::eBadFormat);

  m_Primitive = (ePrimitive)sub.ReadNodeAsUInt32("/xannotation/primitive");
  m_strText = sub.ReadNode("/xannotation/text");
  std::string points = sub.ReadNode("/xannotation/points");
  std::istringstream in(points);
  XGeoPref Pref;
  XPt P;
  double lon, lat;
  while(in.good()) {
    in >> lon >> lat;
    Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
    m_Pt.push_back(P);
  }
  ComputeFrame();
  // Lecture de la representation
  XParserXML rep;
  rep	= sub.FindSubParser("/xannotation/xgeorepres");
  m_Repres->XmlRead(&rep);
  // Lecture des attributs
  m_Att.clear();
  uint32_t numAtt = 0;
  do {
    XParserXML attribut = sub.FindSubParser("/xannotation/attribut", numAtt);
    if (attribut.IsEmpty())
      break;
    numAtt++;
    m_Att.push_back(attribut.ReadNode("/attribut/name"));
    m_Att.push_back(attribut.ReadNode("/attribut/value"));
  } while(true);
  return true;
}

//-----------------------------------------------------------------------------
// Ecriture en XML
//-----------------------------------------------------------------------------
bool XAnnotation::XmlWrite(std::ostream* out)
{
  XGeoPref Pref;
  double lon, lat;
  *out << "<xannotation>" << std::endl;
  *out << "\t<primitive> " << m_Primitive << " </primitive>" << std::endl;
  *out << "\t<text> " << m_strText << " </text>" << std::endl;
  *out << "\t<points>" << std::endl;
  for(uint32_t i = 0; i < m_Pt.size(); i++) {
    Pref.ConvertDeg(Pref.Projection(), XGeoProjection::RGF93, m_Pt[i].X, m_Pt[i].Y, lon, lat);
    *out << "\t\t" << lon << " " << lat << std::endl;
  }
  *out << "\t</points>" << std::endl;
  m_Repres->XmlWrite(out);
  if (m_Att.size() > 0) {
    for (uint32_t i = 0; i < m_Att.size() / 2; i++) {
      *out << "\t<attribut>" << std::endl
           << "\t\t<name> " << m_Att[2*i] << " </name>" << std::endl
           << "\t\t<value> " << m_Att[2*i+1] << " </value>" << std::endl
           << "\t</attribut>" << std::endl;
    }
  }
  *out << "</xannotation>" << std::endl;

  return out->good();
}

//-----------------------------------------------------------------------------
// Lecture en XML
//-----------------------------------------------------------------------------
bool XAnnotation::KmlRead(XParserXML* parser, uint32_t num, XError* error)
{
  XParserXML sub;
  sub	= parser->FindSubParser("/Placemark", num);
  if (sub.IsEmpty())
    return XErrorError(error, "XAnnotation::KmlRead", XError::eBadFormat);

  XGeoPref Pref;
  XPt P;
  double lon, lat, z;
  char sep;

  // Polygone
  XParserXML poly = sub.FindSubParser("/Placemark/Polygon");
  if (!poly.IsEmpty()) {
    XParserXML outer	= poly.FindSubParser("/Polygon/outerBoundaryIs");
    if (outer.IsEmpty())
      return false;
    XParserXML ring = outer.FindSubParser("/outerBoundaryIs/LinearRing");
    if (ring.IsEmpty())
      return false;
    std::string coord = ring.ReadNode("/LinearRing/coordinates");
    std::istringstream in(coord);
    while(in.good()) {
      in >> lon >> sep >> lat;
      if (in.peek() == sep)
        in >> sep >> z;
      Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
      m_Pt.push_back(P);
    }
    m_Primitive = pPolygon;
    ComputeFrame();
    return true;
  }

  // Polyligne
  XParserXML line = sub.FindSubParser("/Placemark/LineString");
  if (!line.IsEmpty()) {
    std::string coord = line.ReadNode("/LineString/coordinates");
    std::istringstream in(coord);
    while(in.good()) {
      in >> lon >> sep >> lat;
      if (in.peek() == sep)
        in >> sep >> z;
      Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
      m_Pt.push_back(P);
    }
    m_Primitive = pPolyline;
    ComputeFrame();
    return true;
  }

  // Ponctuel
  XParserXML point = sub.FindSubParser("/Placemark/Point");
  if (!point.IsEmpty()) {
    std::string coord = point.ReadNode("/Point/coordinates");
    std::istringstream in(coord);
    in >> lon >> sep >> lat;
    if (in.peek() == sep)
      in >> sep >> z;
    Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, P.X, P.Y);
    m_Pt.push_back(P);
    m_Primitive = pText;
    m_strText = sub.ReadNode("/Placemark/name");
    ComputeFrame();
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Ecriture en MIF/MID
//-----------------------------------------------------------------------------
bool XAnnotation::WriteMifMid(std::ostream* mif, std::ostream* mid)
{
  // Ecriture de la geometrie
  switch(m_Primitive) {
  case pPolyline:
    *mif << "PLINE" << std::endl;
    *mif << NbPt() << std::endl;
    for (uint32_t i = 0; i < m_Pt.size(); i++)
      *mif << m_Pt[i].X << " " << m_Pt[i].Y << std::endl;
    break;
  case pArrow:
    *mif << "PLINE" << std::endl;
    *mif << NbPt() << std::endl;
    for (uint32_t i = 0; i < m_Pt.size(); i++)
      *mif << m_Pt[i].X << " " << m_Pt[i].Y << std::endl;
    break;
  case pCurve:
    *mif << "PLINE" << std::endl;
    *mif << NbPt() << std::endl;
    for (uint32_t i = 0; i < m_Pt.size(); i++)
      *mif << m_Pt[i].X << " " << m_Pt[i].Y << std::endl;
    *mif << "SMOOTH" << std::endl;
    break;
  case pPolygon:
    *mif << "REGION 1" << std::endl;
    *mif << NbPt() << std::endl;
    for (uint32_t i = 0; i < m_Pt.size(); i++)
      *mif << m_Pt[i].X << " " << m_Pt[i].Y << std::endl;
    break;
  case pEllipse:
    *mif << "ELLIPSE " << m_Frame.Xmin << " " << m_Frame.Ymin << " "
        << m_Frame.Xmax << " " << m_Frame.Ymax << std::endl;
    break;
  case pRect:
    *mif << "RECT " << m_Frame.Xmin << " " << m_Frame.Ymin << " "
        << m_Frame.Xmax << " " << m_Frame.Ymax << std::endl;
    break;
  case pText:
    *mif << "POINT " << m_Frame.Xmin << " " << m_Frame.Ymin << std::endl;
    break;
  default : return false;
  }

  // Ecriture des attributs
  *mid << "\"" << m_strText << "\"" << std::endl;

  // Ecriture de la representation
  XGeoRepres* repres = Repres();
  if (repres == NULL)
    return true;
  uint32_t pen_color = repres->Color();
  uint32_t fill_color = repres->FillColor();

  switch(m_Primitive) {
  case pPolyline:
  case pArrow:
  case pCurve:
    *mif << "PEN (" << (int)repres->Size() << "," <<  2 <<  "," << pen_color << ")" << std::endl;
    break;
  case pPolygon:
  case pEllipse:
  case pRect:
    *mif << "PEN (" << (int)repres->Size() << "," <<  2 <<  "," << pen_color << ")" << std::endl;
    *mif << "BRUSH (" << 2 << "," << fill_color << "," << pen_color << ")" << std::endl;
    break;
  case pText:
    *mif << "SYMBOL (" << 35 << "," <<  pen_color << "," << (int)repres->Size() << ")" <<  std::endl;
    break;
  default : return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Ecriture au format CSV
//-----------------------------------------------------------------------------
bool XAnnotation::WriteCsv(std::ostream* csv)
{
  XPt2D P = m_Frame.Center();
  *csv << Name() << "\t" << P.X << "\t" << P.Y << std::endl;
  return true;
}

//-----------------------------------------------------------------------------
// Ecriture des informations sur l'objet
//-----------------------------------------------------------------------------
bool XAnnotation::WriteHtml(std::ostream* out)
{
  *out << "<HTML> <BODY>";
  out->setf(std::ios::fixed);
  if ((m_Frame.Xmax < 180)&&(m_Frame.Ymax < 180))
    out->precision(6);
  else
    out->precision(2);

  //*out << "<hr style=\"width: 100%; height: 2px;\">";
  if (m_strText.size() > 0) {
    *out << "<hr style=\"width: 100%; height: 2px;\">";
    *out << "<TABLE BORDER=\"0\" CELLPADDING=\"1\" CELLSPACING=\"0\"> <TBODY> ";
    *out << "<TR><TD><I>" << "Nom" << " : </I><BR></TD> <TD>" << m_strText << "<BR></TD></TR>";
    *out << "</TBODY></TABLE>";
  }

  // Affichage des attributs
  if (m_Att.size() > 0) {
    *out << "<hr style=\"width: 100%; height: 2px;\">";
    *out << "<TABLE BORDER=\"0\" CELLPADDING=\"1\" CELLSPACING=\"0\"> <TBODY> ";
    for(uint32_t i = 0; i < m_Att.size() / 2; i++)
      *out << "<TR><TD><I>" << m_Att[2 * i] << " : </I><BR></TD> <TD>" << m_Att[2*i + 1] << "<BR></TD></TR>";
    *out << "</TBODY></TABLE>";
  }

  // Chargement de l'objet
  if (!LoadGeom()) {
    *out << "Impossible de charger l'objet !";
    *out << "</BODY> </HTML>";
    return true;
  }

  // Calcul de la longueur, de la surface, des pentes
  double d = Length();
  if (d > 0.) {
    *out << "<hr style=\"width: 100%; height: 2px;\">";
    *out << "<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"0\"> <TBODY> ";
    *out << "<TR><TD><I>Longueur : </I><BR></TD> <TD><B>" << d << " m</B><BR></TD></TR>";

    d = LengthAltLin();
    *out << "<TR><TD><I>Longueur sans altération : </I><BR></TD> <TD><B>" << d << " m</B><BR></TD></TR>";
    d = LengthOrtho();
    *out << "<TR><TD><I>Distance orthodromique : </I><BR></TD> <TD><B>" << d << " m</B><BR></TD></TR>";

    d = Area();
    if (d > 0.) {
      *out << "<TR><TD><I>Surface : </I><BR></TD> <TD><B>" << d << " m2</B><BR></TD></TR>";
      *out << "<TR><TD><I>Surface : </I><BR></TD> <TD><B>" << d * 0.0001 << " ha</B><BR></TD></TR>";
    }
    *out << "</TBODY></TABLE>";
  }

  // Calcul du rectangle equivalent
  /*
  double p = Length();
  double s = Area();
  if (s > 0.) {
    double disc = p * p / 4. - 4 * s;
    double L = (p * 0.5 + sqrt(disc)) * 0.5;
    double l = (p * 0.5 - sqrt(disc)) * 0.5;
    *out << "<hr style=\"width: 100%; height: 2px;\">";
    *out << "<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"0\"> <TBODY> ";
    *out << "<TR><TD><I>Longueur equivalente : </I><BR></TD> <TD><B>" << L << " m</B><BR></TD></TR>";
    *out << "<TR><TD><I>Largueur equivalente : </I><BR></TD> <TD><B>" << l << " m</B><BR></TD></TR>";
    *out << "</TBODY></TABLE>";
  }
  */

  // Affichage de la geometrie
  *out << "<hr style=\"width: 100%; height: 2px;\">";
  *out << "<I>Type</I> : <B>" << TypeVectorString() << "</B><BR>";
  *out << "<I>Nombre de points</I> : " << NbPt() << "<BR>";

  *out << "<I>Emprise</I> : (" << m_Frame.Xmin << " ; " << m_Frame.Ymin << ") - (" <<
            m_Frame.Xmax << " ; " << m_Frame.Ymax << ")<BR>";

  *out << "<hr style=\"width: 100%; height: 2px;\">";
  XPt2D P;
  double z;
  uint32_t part = 1;
  for (uint32_t i = 0; i < NbPt(); i++) {
    P = Pt(i);
    for (uint32_t j = part; j < NbPart(); j++)
      if (i == Part(j)) {
        part ++;
        *out << "<hr style=\"width: 100%; height: 2px;\">";
      }
      *out << P.X << " ; " << P.Y << "<BR>";
  }
  Unload();

  *out << "</BODY> </HTML>";

  return true;
}

//-----------------------------------------------------------------------------
// Calcul de la longueur corrigee de l'alteration lineaire
//-----------------------------------------------------------------------------
double XAnnotation::LengthAltLin()
{
  if (NbPt() < 2)
    return 0;
  if (!LoadGeom())
    return 0;
  XPt2D A, B;
  double altLinA, altLinB, altlin, d = 0.;
  XGeoPref pref;
  for (uint32_t i = 0; i < NbPt() - 1; i++) {
    A = Pt(i);
    B = Pt(i+1);
    altLinA = pref.AltLin(pref.Projection(), A.X, A.Y);
    altLinB = pref.AltLin(pref.Projection(), B.X, B.Y);
    altlin = ((altLinA + altLinB) * 0.5) * 0.00001;
    d += dist(A, B) / (altlin + 1);
  }
  Unload();
  return d;
}

//-----------------------------------------------------------------------------
// Calcul de la distance Orthodromique
//-----------------------------------------------------------------------------
double XAnnotation::LengthOrtho()
{
  if (NbPt() < 2)
    return 0;
  if (!LoadGeom())
    return 0;
  XPt2D A, B;
  double d = 0., distAB;
  double latA, latB, lonA, lonB;
  double rayon = 6371010;
  XGeoPref pref;
  for (uint32_t i = 0; i < NbPt() - 1; i++) {
    A = Pt(i);
    B = Pt(i+1);
    pref.Convert(pref.Projection(), XGeoProjection::RGF93, A.X, A.Y, lonA, latA);
    pref.Convert(pref.Projection(), XGeoProjection::RGF93, B.X, B.Y, lonB, latB);

    //Problème numérique pour les très petites distances
    if (abs(latA - latB) < 1e-7 && abs(lonA - lonB) < 1e-7) // ~1 mètre
      //Assimilation à un plan
      distAB = rayon*sqrt(pow(cos(latA)*(lonB-lonA),2)+pow(latB-latA,2));
    else
      //Formule de la distance orthodromique
      distAB = rayon * acos(sin(latA)*sin(latB) + cos(latA)*cos(latB)*cos(lonB-lonA));

    d += distAB;
  }
  Unload();
  return d;
}

