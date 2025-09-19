//-----------------------------------------------------------------------------
//								XOrthoModel.h
//								=============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 8/9/2025
//-----------------------------------------------------------------------------

#include "XOrthoModel.h"


//-----------------------------------------------------------------------------
// Fixe les cliches
//-----------------------------------------------------------------------------
void XOrthoModel::SetCli(bool first, const XPt3D& S, const XPt3D& Ori, const double& gsd)
{
  if (first) {
    m_SomA = S;
    m_OriA = Ori;
    m_dGsdA = gsd;
  }
  else {
    m_SomB = S;
    m_OriB = Ori;
    m_dGsdB = gsd;
  }
}

//-----------------------------------------------------------------------------
// Calcul des points terrains a partir de 2 mesures sur les images
//-----------------------------------------------------------------------------
double XOrthoModel::Image2Ground(XPt2D u1, XPt2D u2, XPt3D& U1, XPt3D& U2)
{
  XPt3D cliA, cliB;
  cliA.X = u1.X * m_dGsdA + m_OriA.X;
  cliA.Y = m_OriA.Y - u1.Y * m_dGsdA;
  cliA.Z = m_OriA.Z;

  cliB.X = u2.X * m_dGsdB + m_OriB.X;
  cliB.Y = m_OriB.Y - u2.Y * m_dGsdB;
  cliB.Z = m_OriB.Z;

  // Vecteurs de visee
  XPt3D VisA = cliA - m_SomA;
  XPt3D VisB = cliB - m_SomB;

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
// Calcul des points images a partir d'un point terrain
//-----------------------------------------------------------------------------
void XOrthoModel::Ground2Image(XPt3D U, XPt2D& u1, XPt2D& u2)
{
  double k = (m_OriA.Z - m_SomA.Z) / (U.Z - m_SomA.Z);
  double xT = m_SomA.X + (U.X - m_SomA.X) * k;
  double yT = m_SomA.Y + (U.Y - m_SomA.Y) * k;
  u1.X = (xT - m_OriA.X) / m_dGsdA;
  u1.Y = (m_OriA.Y - yT) / m_dGsdA;

  k = (m_OriB.Z - m_SomB.Z) / (U.Z - m_SomB.Z);
  xT = m_SomB.X + (U.X - m_SomB.X) * k;
  yT = m_SomB.Y + (U.Y - m_SomB.Y) * k;
  u2.X = (xT - m_OriB.X) / m_dGsdB;
  u2.Y = (m_OriB.Y - yT) / m_dGsdB;
}

void XOrthoModel::Ground2Image(bool first, XPt3D U, XPt2D& u)
{
  double k, xT, yT;
  
  if (first) {
    k = (m_OriA.Z - m_SomA.Z) / (U.Z - m_SomA.Z);
    xT = m_SomA.X + (U.X - m_SomA.X) * k;
    yT = m_SomA.Y + (U.Y - m_SomA.Y) * k;
    u.X = (xT - m_OriA.X) / m_dGsdA;
    u.Y = (m_OriA.Y - yT) / m_dGsdA;
  }
  else {
    k = (m_OriB.Z - m_SomB.Z) / (U.Z - m_SomB.Z);
    xT = m_SomB.X + (U.X - m_SomB.X) * k;
    yT = m_SomB.Y + (U.Y - m_SomB.Y) * k;
    u.X = (xT - m_OriB.X) / m_dGsdB;
    u.Y = (m_OriB.Y - yT) / m_dGsdB;
  }
}
