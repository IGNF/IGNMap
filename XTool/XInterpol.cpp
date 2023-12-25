//-----------------------------------------------------------------------------
//								XInterpol.cpp
//								=============
//
// Auteur : F.Becirspahic - MODSP
//
// 30/08/2010
//-----------------------------------------------------------------------------

#include "XInterpol.h"

//-----------------------------------------------------------------------------
// Interpolation au plus proche voisin
//-----------------------------------------------------------------------------
double XInterpol::Compute(double* value, double x, double dx)
{
  if (x <= dx * 0.5)
    return value[0];
  else
    return value[1];
}

//-----------------------------------------------------------------------------
// Interpolation sur deux directions
//-----------------------------------------------------------------------------
double XInterpol::BiCompute(double* value, double x, double y,
              double dx, double dy)
{
  double z;
  for (int i = 0; i < 2*m_nWin; i++)
    m_Tab[i] = Compute(&value[i*2*m_nWin], x, dx);

  z = Compute(m_Tab, y, dy);
  return z;
}

//-----------------------------------------------------------------------------
// Interpolation lineaire
//-----------------------------------------------------------------------------
double XInterLin::Compute(double* value, double x, double dx)
{
  double a;
  if (fabs(dx) < m_dEpsilon)
    return value[0];
  a = (value[1] - value[0]) / dx;
  return (a*x + value[0]);
}

//-----------------------------------------------------------------------------
// Interpolation cubique standard
//-----------------------------------------------------------------------------
double XInterCub::Compute(double* value, double x, double dx)
{
  double a, b, c, d = value[1];
  double dz1 = value[0] - d, dz2 = value[2] - d, dz3 = value[3] - d;

  if (fabs(dx) < m_dEpsilon)
    return value[1];

  a = (-1. * dz1 - 3. * dz2 + 1. * dz3) / (6. * dx * dx * dx);
  b = ( 3. * dz1 + 3. * dz2) / (6. * dx * dx);
  c = (-2. * dz1 + 6. * dz2 - 1. * dz3) / (6. * dx);
  return (a*x*x*x + b*x*x + c*x + d);
}

//-----------------------------------------------------------------------------
// Interpolation cubique Catmull-Rom
//-----------------------------------------------------------------------------
double XInterCubCatmull::Compute(double* value, double x, double dx)
{
  double a, b, c, d = value[1];
  if (fabs(dx) < m_dEpsilon)
    return value[1];

  c = -0.5 * value[0] + 0.5 * value[2];
  b = value[0] - 2.5 * value[1] + 2. * value[2] - 0.5 * value[3];
  a = -0.5 * value[0] + 1.5 * value[1] - 1.5 * value[2] + 0.5 * value[3];
  return (a*x*x*x + b*x*x + c*x + d);
}

//-----------------------------------------------------------------------------
// Interpolation cubique avec la formule GDAL (convolution)
//-----------------------------------------------------------------------------
double XInterCubConv::Compute(double* value, double x, double dx)
{
  double a, b, c, d = value[1];
  if (fabs(dx) < m_dEpsilon)
    return value[1];
  a = -value[0] + value[1] - value[2] + value[3];
  b = 2.0 * (value[0] - value[1]) + value[2] - value[3];
  c = -value[0] + value[2];
  return (a*x*x*x + b*x*x + c*x + d);
}

//-----------------------------------------------------------------------------
//		Interpolation en sin x / x
//-----------------------------------------------------------------------------
void XInterSin::TabSinXX()
{
  int L0, i, j;
  double xx, Pi100, tmp[50];

  Pi100 = 0.03141592654;

  // Allocation de la memoire pour le tableau
  m_dTab = new double[m_nWin*100 + 1];
  if (m_dTab == 0)
    return;

  L0 = 100 * m_nWin;
  for (i = 1; i <= L0; i++) {
    xx = Pi100 * (double)i;
    m_dTab[i] = sin(xx) / xx;
  }

  for (j = 1; j <= m_nWin; j++)
    m_dTab[100*j] = 0.0;

  m_dTab[0] = 1.0;

  // Amortissement de la troncature
  for (i = 50; i <= 99; i++) {
    xx = m_dTab[i];
    L0 = 100;
    for (j = 1; j <= m_nWin - 1; j++) {
      xx = xx + m_dTab[L0 - i] + m_dTab[L0 + i];
      L0+=100;
    }
    xx += m_dTab[L0-i];
    tmp[i-50] = (1.0 - xx)*0.5;
  } /* endfor i */

  // Repartition de l'erreur de troncature
  L0 = 100 * (m_nWin - 1);
  for (i = 50; i <= 99; i++)
    m_dTab[L0 + i] += tmp[i-50];
  for (i = 1; i <= 49; i++)
    m_dTab[L0 + i] += tmp[50-i];
}

//-----------------------------------------------------------------------------
// Interpolation en sin x / x
//-----------------------------------------------------------------------------
double XInterSin::Compute(double* value, double x, double dx)
{
  int k, n;
  double out, r;

  out = 0.0;
  r = (x + (double)m_nWin)*100.0;
  n = (int)r;
  for(k = 1 - m_nWin; k<= m_nWin;k++){
    n -=100;
    out += m_dTab[(int)abs(n)]*value[k+m_nWin-1];
  }
  return out;
}
