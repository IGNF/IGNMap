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
  XInterpol(double e = 1e-6) { m_dEpsilon = e; m_nWin = 1; m_Tab = new double[2*m_nWin];}
  virtual ~XInterpol() {delete[] m_Tab;}

  inline int Win() const { return m_nWin;}
  virtual double Compute(double* value, double x, double dx = 1.0);
  virtual double BiCompute(double* value, double x, double y, double dx = 1.0, double dy = 1.0);
};

class XInterLin : public XInterpol {
public :
  XInterLin(double e = 1e-6) { m_dEpsilon = e; m_nWin = 1; m_Tab = new double[2*m_nWin];}

  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCub : public XInterpol {
public :
  XInterCub(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; m_Tab = new double[2*m_nWin];}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCubCatmull : public XInterpol {
public :
  XInterCubCatmull(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; m_Tab = new double[2*m_nWin];}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterCubConv : public XInterpol {
public :
  XInterCubConv(double e = 1e-6) { m_dEpsilon = e; m_nWin = 2; m_Tab = new double[2*m_nWin];}
  virtual double Compute(double* value, double x, double dx = 1.0);
};

class XInterSin : public XInterpol {
protected :
  double *m_dTab;		// Tabulation (pour l'interpolatione en sin x / x)
  void TabSinXX();	// Tabulation de la fonction sin / x
public :
  XInterSin(int win = 3) { m_nWin = win; m_Tab = new double[2*m_nWin]; TabSinXX();}
  virtual ~XInterSin() { if (m_dTab != 0) delete[] m_dTab;}
  virtual double Compute(double* value, double x, double dx = 1.0);
};
#endif // XINTERPOL_H
