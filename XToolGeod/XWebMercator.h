//-----------------------------------------------------------------------------
//								XWebMercator.h
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 19/11/2012
//-----------------------------------------------------------------------------

#ifndef XWEBMERCATOR_H
#define XWEBMERCATOR_H

#include "XGeodConverter.h"

class XWebMercator : public XGeodConverter {
public:
  XWebMercator() : XGeodConverter() {;}

  virtual bool SetDefaultProjection(XProjCode start_proj, XProjCode end_proj);

  // Conversion avec passage en degree decimaux
  virtual bool ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);

  // Conversion normale
  virtual bool Convert(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);
};

#endif // XWEBMERCATOR_H
