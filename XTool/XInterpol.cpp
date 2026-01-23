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
  for (int i = 0; i < 2*m_nWin; i++)
    m_Tab[i] = Compute(&value[i*2*m_nWin], x, dx);

  return Compute(m_Tab, y, dy);
}

//-----------------------------------------------------------------------------
// Interpolation lineaire
//-----------------------------------------------------------------------------
double XInterLin::Compute(double* value, double x, double dx)
{
  if (fabs(dx) < m_dEpsilon)
    return value[0];
  double a = (value[1] - value[0]) / dx;
  return (a*x + value[0]);
}

//-----------------------------------------------------------------------------
// Interpolation cubique standard
//-----------------------------------------------------------------------------
double XInterCub::Compute(double* value, double x, double dx)
{
  if (fabs(dx) < m_dEpsilon)
    return value[1];

  double d = value[1];
  double dz1 = value[0] - d, dz2 = value[2] - d, dz3 = value[3] - d;

  double a = (-1. * dz1 - 3. * dz2 + 1. * dz3) / (6. * dx * dx * dx);
  double b = ( 3. * dz1 + 3. * dz2) / (6. * dx * dx);
  double c = (-2. * dz1 + 6. * dz2 - 1. * dz3) / (6. * dx);
  return (a*x*x*x + b*x*x + c*x + d);
}

//-----------------------------------------------------------------------------
// Interpolation cubique Catmull-Rom
//-----------------------------------------------------------------------------
double XInterCubCatmull::Compute(double* value, double x, double dx)
{
  if (fabs(dx) < m_dEpsilon)
    return value[1];
  double d = value[1];
  double c = -0.5 * value[0] + 0.5 * value[2];
  double b = value[0] - 2.5 * value[1] + 2. * value[2] - 0.5 * value[3];
  double a = -0.5 * value[0] + 1.5 * value[1] - 1.5 * value[2] + 0.5 * value[3];
  return (a*x*x*x + b*x*x + c*x + d);
}

//-----------------------------------------------------------------------------
// Interpolation cubique avec la formule GDAL (convolution)
//-----------------------------------------------------------------------------
double XInterCubConv::Compute(double* value, double x, double dx)
{
  if (fabs(dx) < m_dEpsilon)
    return value[1];
  double d = value[1];
  double a = -value[0] + value[1] - value[2] + value[3];
  double b = 2.0 * (value[0] - value[1]) + value[2] - value[3];
  double c = -value[0] + value[2];
  return (a*x*x*x + b*x*x + c*x + d);
}