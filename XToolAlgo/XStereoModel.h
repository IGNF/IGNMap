//-----------------------------------------------------------------------------
//								XStereoModel.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 4/3/2020
//-----------------------------------------------------------------------------

#ifndef XSTEREOMODEL_H
#define XSTEREOMODEL_H

#include "../XTool/XMat3D.h"
#include "../XTool/XPt2D.h"

class XStereoModel {
protected:
  XMat3D  m_RotA; // Matrice rotation image -> Terrain
  XMat3D  m_RotB;
  XPt3D   m_SomA; // Sommet de prise de vues
  XPt3D   m_SomB;
  XMat3D  m_Ima;  // Matrice repere image -> cliche
  XPt3D   m_Focal;
  double  m_dPixSize;

public:
  XStereoModel();

  XPt3D SomL() { return m_SomA;}
  XPt3D SomR() { return m_SomB;}
  XPt3D Focal() { return m_Focal;}
  double PixSize() { return m_dPixSize;}

  void SetFocal(XPt3D F, double pix_size) { m_Focal = F; m_dPixSize = pix_size;}
  void SetCli(bool first, XPt3D S, double omega, double phi, double kappa);

  double Image2Ground(XPt2D u1, XPt2D u2, XPt3D &U1, XPt3D &U2);
  double Image2Plane(double Z0, XPt2D u1, XPt2D u2, XPt3D &U1, XPt3D &U2);
  void Ground2Image(XPt3D U, XPt2D &u1, XPt2D &u2);
  void Ground2Image(bool first, XPt3D U, XPt2D &u);
};

#endif // XSTEREOMODEL_H
