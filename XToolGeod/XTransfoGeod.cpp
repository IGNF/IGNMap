//-----------------------------------------------------------------------------
//								XTransfoGeod.cpp
//								================
//
// Auteur : F.Becirspahic - MODSP
//
// 30/08/2010
//-----------------------------------------------------------------------------
#include "XTransfoGeod.h"

//-----------------------------------------------------------------------------
// Transformation image d'arrivee -> image de depart
//-----------------------------------------------------------------------------
void XTransfoGeod::Direct(double x, double y, double *u, double *v)
{
  double xi, yi, xf, yf;

  xi = x * m_dResol + m_Ff.Xmin;
  yi = m_Ff.Ymax - y * m_dResol;

  m_Geod->Convert(xi, yi, xf, yf);

  *u = (xf - m_Fi.Xmin) / m_dResol;
  *v = (m_Fi.Ymax - yf) / m_dResol;
  return;
}

//-----------------------------------------------------------------------------
// Dimension de l'image de sortie
//-----------------------------------------------------------------------------
void XTransfoGeod::Dimension(int w, int h, int* wout, int* hout)
{
  *wout = XRint(m_Ff.Width() / m_dResol);
  *hout = XRint(m_Ff.Height() / m_dResol);
}

//-----------------------------------------------------------------------------
// Georefencement de l'image resultante
//-----------------------------------------------------------------------------
bool XTransfoGeod::SetGeoref(double* xmin, double* ymax, double* gsd, unsigned short* epsg)
{
  *xmin = m_Ff.Xmin;
  *ymax = m_Ff.Ymax;
  *gsd = m_dResol;
  *epsg = XGeoProjection::EPSGCode(m_Geod->StartProjection());
  return true;
}

//-----------------------------------------------------------------------------
// Auto-calibration pour trouver le pas de la grille d'interpolation
//-----------------------------------------------------------------------------
bool XTransfoGeodInterpol::AutoCalibration()
{
  if (m_Ff.IsEmpty())
    return false;
  if (m_dResol <= 0.)
    return false;

  m_bInterpol = false;
  double error;
  XFrame F = m_Ff;
  while(true) {
    error = ErrorFrame(F);
    if (error < (m_dResol * 0.1))
      break;
    F *= (-0.1);
    if ((F.Xmin > F.Xmax)||(F.Ymin > F.Ymax))
      return false;
  }
  m_dStep = XMin(F.Width(), F.Height());

  m_Ai.X = m_Ff.Xmin;
  m_Ai.Y = m_Ff.Ymax;
  m_Bi.X = XMin(m_Ai.X + m_dStep, m_Ff.Xmax);
  m_Bi.Y = m_Ai.Y;
  m_Geod->Convert(m_Ai.X, m_Ai.Y, m_Af.X, m_Af.Y);
  m_Geod->Convert(m_Bi.X, m_Bi.Y, m_Bf.X, m_Bf.Y);

  return true;
}

//-----------------------------------------------------------------------------
// Calcul de l'erreur pour un rectangle
//-----------------------------------------------------------------------------
double XTransfoGeodInterpol::ErrorFrame(XFrame F)
{
  double xa, ya, xb, yb, xc, yc, xd, yd, xp, yp;

  m_Geod->Convert(F.Xmin, F.Ymax, xa, ya);
  m_Geod->Convert(F.Xmax, F.Ymax, xb, yb);
  m_Geod->Convert(F.Xmin, F.Ymin, xc, yc);
  m_Geod->Convert(F.Xmax, F.Ymin, xd, yd);

  m_Geod->Convert((F.Xmin + F.Xmax)*0.5, (F.Ymin + F.Ymax)*0.5, xp, yp);

  double dx = fabs(xp - (xa + xb + xc + xd) * 0.25);
  double dy = fabs(yp - (ya + yb + yc + yd) * 0.25);
  return XMax(dx, dy);
}

//-----------------------------------------------------------------------------
// Transformation image d'arrivee -> image de depart
//-----------------------------------------------------------------------------
void XTransfoGeodInterpol::Direct(double x, double y, double *u, double *v)
{
  double xf, yf, epsilon = m_dResol * 0.01;
  m_P.X = x * m_dResol + m_Ff.Xmin;
  m_P.Y = m_Ff.Ymax - y * m_dResol;

  double ratio;
  if (fabs(m_P.Y - m_Ai.Y) < m_dResol - epsilon) {
    if ((m_P.X >= m_Ai.X)&&(m_P.X <= m_Bi.X)) {	// P entre A et B
      ratio = (m_P.X - m_Ai.X) / (m_Bi.X - m_Ai.X);
      xf = m_Af.X + ratio * (m_Bf.X - m_Af.X);
      yf = m_Af.Y + ratio * (m_Bf.Y - m_Af.Y);
      *u = (xf - m_Fi.Xmin) / m_dResol;
      *v = (m_Fi.Ymax - yf) / m_dResol;
      return;
    }
    m_Ai = m_Bi;
    m_Bi.X = XMin(m_Ai.X + m_dStep, m_Ff.Xmax);
    m_Bi.Y = m_Ai.Y;

    m_Af = m_Bf;
    m_Geod->Convert(m_Bi.X, m_Bi.Y, m_Bf.X, m_Bf.Y);
    ratio = (m_P.X - m_Ai.X) / (m_Bi.X - m_Ai.X);
    xf = m_Af.X + ratio * (m_Bf.X - m_Af.X);
    yf = m_Af.Y + ratio * (m_Bf.Y - m_Af.Y);

    *u = (xf - m_Fi.Xmin) / m_dResol;
    *v = (m_Fi.Ymax - yf) / m_dResol;

    return;

  } else {	// Changement de ligne
    m_Ai.X = m_Ff.Xmin;
    m_Ai.Y = m_Ai.Y - m_dResol;
    m_Bi.X = XMin(m_Ai.X + m_dStep, m_Ff.Xmax);
    m_Bi.Y = m_Ai.Y;
    m_Geod->Convert(m_Ai.X, m_Ai.Y, m_Af.X, m_Af.Y);
    m_Geod->Convert(m_Bi.X, m_Bi.Y, m_Bf.X, m_Bf.Y);
    Direct(x, y, u, v);
  }

}
