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
  virtual bool ConvertDeg(const double& Xi, const double& Yi, double& Xf, double& Yf, double Z = 0.) override;

  // Conversion normale
  virtual bool Convert(const double& Xi, const double& Yi, double& Xf, double& Yf, double Z = 0.) override;
};

#endif // XWEBMERCATOR_H
