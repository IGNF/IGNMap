//-----------------------------------------------------------------------------
//								XStereoModel.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 4/3/2020
//-----------------------------------------------------------------------------

#include "XStereoModel.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XStereoModel::XStereoModel()
{
  m_Ima = XMat3D( XPt3D(1, 0, 0),
                  XPt3D(0, -1, 0),
                  XPt3D(0, 0, -1));
  m_dPixSize = 0.;
}

//-----------------------------------------------------------------------------
// Fixe les cliches
//-----------------------------------------------------------------------------
void XStereoModel::SetCli(bool first, XPt3D S, double omega, double phi, double kappa)
{
  if (first) {
    m_SomA = S;
    m_RotA = XMat3D(omega * XPI / 180., phi * XPI / 180., kappa * XPI / 180., XMat3D::OPK);
  } else {
    m_SomB = S;
    m_RotB = XMat3D(omega * XPI / 180., phi * XPI / 180., kappa * XPI / 180., XMat3D::OPK);
  }
}

//-----------------------------------------------------------------------------
// Calcul des points terrains a partir de 2 mesures sur les images
//-----------------------------------------------------------------------------
double XStereoModel::Image2Ground(XPt2D u1, XPt2D u2, XPt3D& U1, XPt3D& U2)
{
  XPt3D cliA, cliB;
  cliA.X = u1.X - m_Focal.X;
  cliA.Y = u1.Y - m_Focal.Y;
  cliA.Z = m_Focal.Z;
  cliA *= m_dPixSize;

  cliB.X = u2.X - m_Focal.X;
  cliB.Y = u2.Y - m_Focal.Y;
  cliB.Z = m_Focal.Z;
  cliB *= m_dPixSize;

  // Vecteurs de visee
  XPt3D VisA = m_RotA * m_Ima * cliA;
  XPt3D VisB = m_RotB * m_Ima * cliB;

  // Vecteur des sommets
  XPt3D AB = m_SomB - m_SomA;

  // Matrice denominateur
  XPt3D prod = prodVect(VisA, VisB);
  XMat3D denom(VisA, VisB, prod);

  // Matrices numerateurs
  XMat3D numA(AB, VisB, prod);
  XMat3D numB(AB, VisA, prod);

  double x = numA.Det() / denom.Det();
  double y = numB.Det() / denom.Det();

  // Points
  U1 = x * VisA + m_SomA;
  U2 = y * VisB + m_SomB;

  return dist(U1, U2);
}

//-----------------------------------------------------------------------------
// Calcul de l'intersection des faisceaux perspectifs avec un plan Z constant
//-----------------------------------------------------------------------------
double XStereoModel::Image2Plane(double Z0, XPt2D u1, XPt2D u2, XPt3D &U1, XPt3D &U2)
{
  XPt3D cliA, cliB;
  cliA.X = u1.X - m_Focal.X;
  cliA.Y = u1.Y - m_Focal.Y;
  cliA.Z = m_Focal.Z;
  cliA *= m_dPixSize;

  cliB.X = u2.X - m_Focal.X;
  cliB.Y = u2.Y - m_Focal.Y;
  cliB.Z = m_Focal.Z;
  cliB *= m_dPixSize;

  // Vecteurs de visee
  XPt3D VisA = m_RotA * m_Ima * cliA;
  XPt3D VisB = m_RotB * m_Ima * cliB;

  double kA = (Z0 - m_SomA.Z) / VisA.Z;
  double kB = (Z0 - m_SomB.Z) / VisB.Z;

  U1 = kA * VisA + m_SomA;
  U2 = kB * VisB + m_SomB;

  return dist(U1, U2);
}

//-----------------------------------------------------------------------------
// Calcul des points images a partir d'un point terrain
//-----------------------------------------------------------------------------
void XStereoModel::Ground2Image(XPt3D U, XPt2D &u1, XPt2D &u2)
{
  // Vecteurs de visee
  XPt3D VisA = m_RotA.Trn() * (U - m_SomA);
  XPt3D VisB = m_RotB.Trn() * (U - m_SomB);

  u1.X = m_Focal.X - m_Focal.Z * VisA.X / VisA.Z;
  u1.Y = m_Focal.Y + m_Focal.Z * VisA.Y / VisA.Z;

  u2.X = m_Focal.X - m_Focal.Z * VisB.X / VisB.Z;
  u2.Y = m_Focal.Y + m_Focal.Z * VisB.Y / VisB.Z;
}

void XStereoModel::Ground2Image(bool first, XPt3D U, XPt2D &u)
{
  if (first) {
    XPt3D VisA = m_RotA.Trn() * (U - m_SomA);
    u.X = m_Focal.X - m_Focal.Z * VisA.X / VisA.Z;
    u.Y = m_Focal.Y + m_Focal.Z * VisA.Y / VisA.Z;
  } else {
    XPt3D VisB = m_RotB.Trn() * (U - m_SomB);
    u.X = m_Focal.X - m_Focal.Z * VisB.X / VisB.Z;
    u.Y = m_Focal.Y + m_Focal.Z * VisB.Y / VisB.Z;
  }
}

