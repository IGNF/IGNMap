//-----------------------------------------------------------------------------
//								XGeodGrid.h
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 18/11/2013
//-----------------------------------------------------------------------------

#ifndef XGEODGRID_H
#define XGEODGRID_H

#include "../XTool/XBase.h"

// Grille Geodesique pour les transformations grille
class XGeodGrid {
protected:
  XPt3*     m_Grid;
  double		m_dLatMin;
  double		m_dLatMax;
  double		m_dLongMin;
  double		m_dLongMax;
  double		m_dLatStep;
  double		m_dLongStep;
  uint32_t		m_nNbLat;
  uint32_t		m_nNbLong;

public:
  XGeodGrid() {m_Grid = NULL;}
  virtual ~XGeodGrid() { Unload();}

  void Unload() { if (m_Grid != NULL) delete[] m_Grid; m_Grid = NULL;}
  bool IsLoaded() { if (m_Grid != NULL) return true; return false;}

  bool LoadGrid(const char* filename, XError* error = NULL);
  bool WriteGrid(const char* filename, XError* error = NULL);
  bool ReadGrid(const char* filename, XError* error = NULL);

  bool Interpol(double lambda, double phi, double* Tx, double* Ty, double* Tz);
};

#endif // XGEODGRID_H
