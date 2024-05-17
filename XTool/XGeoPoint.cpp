//-----------------------------------------------------------------------------
//								XGeoPoint.cpp
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/07/2003
//-----------------------------------------------------------------------------

#include "XGeoPoint.h"
#include <algorithm>

//-----------------------------------------------------------------------------
// Nom du ponctuel
//-----------------------------------------------------------------------------
std::string XGeoPoint2D::Name()
{
  /*
	std::string name = FindAttribute("nom", true);
	if (name.size() > 1)
		return name;
	name = FindAttribute("name", true);
  return name; */
  std::vector<std::string> V;
  if (!ReadAttributes(V))
    return "";
  for (uint32_t i = 0; i < V.size(); i+=2) {
    //std::transform(V[i].begin(), V[i].end(), V[i].begin(), tolower);
    std::transform(V[i].begin(), V[i].end(), V[i].begin(), [](char c) {return static_cast<char>(std::tolower(c));});
    if (V[i] == "toponyme")
      return V[i+1];
    if (V[i] == "nom")
      return V[i+1];
    if (V[i] == "name")
      return V[i+1];
  }
  return "";
}

//-----------------------------------------------------------------------------
// Importance du ponctuel
//-----------------------------------------------------------------------------
uint16_t XGeoPoint2D::Importance()
{
	std::string imp = FindAttribute("importance", true);
	if (imp.size() < 1)
		return 1;
	uint16_t importance;
	std::ignore = sscanf(imp.c_str(), "%hu", &importance);
	return importance;
}

//-----------------------------------------------------------------------------
// Intersection avec un cadre
//-----------------------------------------------------------------------------
bool XGeoPoint2D::IntersectFrame(XFrame& r) const
{ 
	double d = XMax(r.Width(), r.Height()) * 0.05;
	if (m_Frame.Xmin > r.Xmax + d) return false;
	if (m_Frame.Xmax < r.Xmin - d) return false;
	if (m_Frame.Ymin > r.Ymax + d) return false;
	if (m_Frame.Ymax < r.Ymin - d) return false;
	return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'un point de la geometrie
//-----------------------------------------------------------------------------
XPt2D XGeoMPoint2D::Pt(uint32_t i)
{ 
  if ((i < NbPt())&&(m_Pt != NULL))
		return XPt2D(m_Pt[i].X, m_Pt[i].Y);
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Test si la geometrie intersecte un cadre
//-----------------------------------------------------------------------------
bool XGeoMPoint2D::Intersect(const XFrame& F)
{
  // Le cadre peut contenir toute la geometrie
  if (m_Frame.IsIn(F))
    return true;

  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return false;
  }
  bool inter = false;
  for(uint32_t i = 0; i < NbPt(); i++) {
    if (m_Pt[i].X < F.Xmin) continue;
    if (m_Pt[i].X > F.Xmax) continue;
    if (m_Pt[i].Y < F.Ymin) continue;
    if (m_Pt[i].Y > F.Ymax) continue;
    inter = true;
    break;
  }

  if (!loaded)
    Unload();
  return inter;
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XGeoMPoint2D::IsNear2D(const XPt2D& P, double dist)
{
  if (!XGeoVector::IsNear2D(P, dist))
    return false;

  double d2 = dist * dist;
  if (!LoadGeom())
    return false;
  for (uint32_t j = 0; j < NbPt(); j++) {
    if (P.X < m_Pt[j].X - dist) continue;
    if (P.X > m_Pt[j].X + dist) continue;
    if (P.Y < m_Pt[j].Y - dist) continue;
    if (P.Y > m_Pt[j].Y + dist) continue;

    if (dist2(P, m_Pt[j]) < d2) {
      Unload();
      return true;
    }
  }
  Unload();
  return false;
}

//-----------------------------------------------------------------------------
// Dechargement de la geometrie
//-----------------------------------------------------------------------------
void XGeoMPoint3D::Unload()
{ 
	XGeoMPoint2D::Unload();
	if (m_Z != NULL)
		delete[] m_Z;
	m_Z = NULL;
  /*
	if (m_ZRange != NULL)
		delete[] m_ZRange;
	m_ZRange = NULL;
  */
}

//-----------------------------------------------------------------------------
// Recuperation d'un Z de la geometrie
//-----------------------------------------------------------------------------
double XGeoMPoint3D::Z(uint32_t i)
{
  if ((i < NbPt())&&(m_Z != NULL))
		return m_Z[i];
	return XGEO_NO_DATA;
}

