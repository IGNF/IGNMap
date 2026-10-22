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

  static double ConvertLonToX(const double& lon) { return lon * 6378137.; }
  static double ConvertLatToY(const double& lat) { return 6378137. * log(tan(lat * 0.5 + XPI4));}
  static double ConvertXToLon(const double& X) { return X / 6378137.; }
  static double ConvertYToLat(const double& Y) { return 2. * (atan(exp(Y / 6378137.)) - XPI4);}
  static double ConvertLonToXDeg(const double& lon) { return lon * 111319.49079327357264771338267056; }
  static double ConvertLatToYDeg(const double& lat) { return 6378137. * log(tan(lat * 0.00872664625997164788461845384244 + XPI4)); }

};

#endif // XWEBMERCATOR_H
