//-----------------------------------------------------------------------------
//								XTransfoGeod.h
//								==============
//
// Auteur : F.Becirspahic - MODSP
//
// 30/08/2010
//-----------------------------------------------------------------------------
#ifndef XTRANSFOGEOD_H
#define XTRANSFOGEOD_H

#include "../XTool/XTransfo.h"
#include "XGeodConverter.h"
#include "../XTool/XFrame.h"

//-----------------------------------------------------------------------------
// Transformation avec calcul geodesique complet
//-----------------------------------------------------------------------------
class XTransfoGeod : public XTransfo {
protected:
  XGeodConverter*		m_Geod;
  XFrame						m_Fi;
  XFrame						m_Ff;
  double						m_dResol;

public:
  XTransfoGeod(XGeodConverter* geod) { m_Geod = geod; m_dResol = -1.;}

  void SetStartFrame(const XFrame& f) { m_Fi = f;}	// Cadre de l'image de depart
  void SetEndFrame(const XFrame& f) { m_Ff = f;}		// Cadre de l'image d'arrivee
  void SetResolution(double r) { m_dResol = r;}

  virtual void Direct(double x, double y, double *u, double *v);
  virtual void Dimension(int w, int h, int* wout, int* hout);
  virtual bool SetGeoref(double* xmin, double* ymax, double* gsd, unsigned short* epsg);
};

//-----------------------------------------------------------------------------
// Transformation avec interpolation
//-----------------------------------------------------------------------------
class XTransfoGeodInterpol : public XTransfoGeod {
protected:
  bool			m_bInterpol;		// Indique l'interpolation a ete initialise
  double		m_dStep;				// Pas d'interpolation
  XPt2D			m_Ai;
  XPt2D			m_Bi;
  XPt2D			m_Af;
  XPt2D			m_Bf;
  XPt2D			m_P;						// Point courant

  double ErrorFrame(XFrame F);

  void ComputeFrame(XFrame* F);

public:
  XTransfoGeodInterpol(XGeodConverter* geod) : XTransfoGeod(geod) {m_bInterpol = false;}

  bool AutoCalibration();

  virtual void Direct(double x, double y, double *u, double *v);

};

#endif // XTRANSFOGEOD_H
