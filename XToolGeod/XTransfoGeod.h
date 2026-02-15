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

public:
  XTransfoGeodInterpol(XGeodConverter* geod) : XTransfoGeod(geod) { m_bInterpol = false; m_dStep = 0.; }
  bool AutoCalibration();
  virtual void Direct(double x, double y, double *u, double *v);
};

//-----------------------------------------------------------------------------
// Transformation avec calcul geodesique complet et facteur de zoom
//-----------------------------------------------------------------------------
class XTransfoGeodZoom : public XTransfo {
protected:
  XGeodConverter* m_Geod;
  XFrame						m_Fi; // Cadre initial
  XFrame						m_Ff; // Cadre final
  double						m_dResolI;  // Resolution initiale
  double            m_dResolF;  // Resolution finale

public:
  XTransfoGeodZoom(XGeodConverter* geod) { m_Geod = geod; m_dResolI = m_dResolF = -1.; }

  void SetStartFrame(const XFrame& f, double resol) { m_Fi = f; m_dResolI = resol; }	// Cadre de l'image de depart
  void SetEndFrame(const XFrame& f, double resol) { m_Ff = f; m_dResolF = resol; }		// Cadre de l'image d'arrivee

  virtual void Direct(double x, double y, double* u, double* v);
  virtual void Dimension(int w, int h, int* wout, int* hout);
  virtual bool SetGeoref(double* xmin, double* ymax, double* gsd, unsigned short* epsg);
};

//-----------------------------------------------------------------------------
// Transformation avec interpolation
//-----------------------------------------------------------------------------
class XTransfoGeodZoomInterpol : public XTransfoGeodZoom {
protected:
  bool			m_bInterpol;		// Indique l'interpolation a ete initialise
  double		m_dStep;				// Pas d'interpolation
  XPt2D			m_Ai;
  XPt2D			m_Bi;
  XPt2D			m_Af;
  XPt2D			m_Bf;
  XPt2D			m_P;						// Point courant

  double ErrorFrame(XFrame F);

public:
  XTransfoGeodZoomInterpol(XGeodConverter* geod) : XTransfoGeodZoom(geod) { m_bInterpol = false; m_dStep = 0.; }
  bool AutoCalibration();
  virtual void Direct(double x, double y, double* u, double* v);
};

#endif // XTRANSFOGEOD_H
