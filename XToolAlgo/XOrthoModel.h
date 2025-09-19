//-----------------------------------------------------------------------------
//								XOrthoModel.h
//								=============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 8/9/2025
//-----------------------------------------------------------------------------

#ifndef XORTHOMODEL_H
#define XORTHOMODEL_H

#include "../XTool/XMat3D.h"
#include "../XTool/XPt2D.h"

class XOrthoModel {
protected:
  XPt3D   m_SomA; // Sommet de prise de vues
  XPt3D   m_SomB;
  XPt3D   m_OriA; // Origine de l'ortho
  XPt3D   m_OriB;
  double  m_dGsdA;
  double  m_dGsdB;

public:
  XOrthoModel() { m_dGsdA = m_dGsdB = 1.; }

  XPt3D SomL() const { return m_SomA; }
  XPt3D SomR() const { return m_SomB; }
  XPt3D OriL() const { return m_OriA; }
  XPt3D OriR() const { return m_OriB; }
  double GsdL() const { return m_dGsdA; }
  double GsdR() const { return m_dGsdB; }

  void SetCli(bool first, const XPt3D& S, const XPt3D& Ori, const double& gsd);

  double Image2Ground(XPt2D u1, XPt2D u2, XPt3D& U1, XPt3D& U2);
  //double Image2Plane(double Z0, XPt2D u1, XPt2D u2, XPt3D& U1, XPt3D& U2);
  void Ground2Image(XPt3D U, XPt2D& u1, XPt2D& u2);
  void Ground2Image(bool first, XPt3D U, XPt2D& u);
};


#endif //XORTHOMODEL_H
