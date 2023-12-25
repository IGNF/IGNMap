//-----------------------------------------------------------------------------
//								XGeodConverter.h
//								================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/03/2008
//-----------------------------------------------------------------------------

#ifndef _XGEODCONVERTER_H
#define _XGEODCONVERTER_H

#include "XGeoProjection.h"

using namespace XGeoProjection;

class XGeodConverter {
protected:
		// Projection par defaut
	XProjCode	m_StartProjection;
	XProjCode	m_EndProjection;	

public:
	XGeodConverter() { m_StartProjection = m_EndProjection = RGF93;}

	virtual bool SetDefaultProjection(XProjCode start_proj, XProjCode end_proj) 
														{ m_StartProjection = start_proj; m_EndProjection = end_proj; return true;}

	XProjCode StartProjection() { return m_StartProjection;}
	XProjCode EndProjection() { return m_EndProjection;}

	// Conversion avec passage en degree decimaux
	virtual bool ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.) 
									{ Xf = Xi; Yf = Yi; return true;}
	// Conversion normale
	virtual bool Convert(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.)
									{ Xf = Xi; Yf = Yi; return true;}

  // Alteration lineaire
  virtual double AltLin(XProjCode start_proj, double Xi, double Yi) { return 0;}
};

#endif //_XGEODCONVERTER_H
