//-----------------------------------------------------------------------------
//								XInterpol.h
//								===========
//
// Auteur : F.Becirspahic - MODSP
//
// 30/08/2010
//-----------------------------------------------------------------------------

#ifndef XINTERPOL_H
#define XINTERPOL_H

#include "XBase.h"

class XInterpol {
protected:
  double  m_dEpsilon;	// Plus petite valeur non nulle
  int			m_nWin;			// Taille de la fenetre
  double*	m_Tab;			// Tableau pour l'interpolation en 2 dimensions
public:
  XInterpol(double e = 1e-6) { m_dEpsilon = e; m_nWin = 1; m_Tab = nullptr; AllocTab(); }
  virtual ~XInterpol() { delete[] m_Tab; m_Tab = nullptr; }

  void AllocTab() { if (m_Tab != nullptr) delete[] m_Tab; m_Tab = new double[2 * m_nWin];}
  inline int Win() const { return m_nWin;}
  virtual double Compute(double* value, double x, double dx = 1.0);
  double BiCompute(double* value, double x, double y, double dx = 1.0, double dy = 1.0);
};

class XInterLin : public XInterpol {
public :
  XInterLin(double e = 1e-6) { m_dEpsilon = e; m_nWin = 1; AllocTab();}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCub : public XInterpol {
public :
  XInterCub(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; AllocTab();}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCubCatmull : public XInterpol {
public :
  XInterCubCatmull(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; AllocTab();}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCubConv : public XInterpol {
public :
  XInterCubConv(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; AllocTab();}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

#endif // XINTERPOL_H
