//-----------------------------------------------------------------------------
//								XDomGeod.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/02/2008
//
// Geodesie pour les DOM (Martinique, Gaudeloupe, Guyane, Reunion)
//-----------------------------------------------------------------------------

#ifndef _XDOMGEOD_H
#define _XDOMGEOD_H

#include "XGeodConverter.h"

class XDomGeod : public XGeodConverter {
protected:

	typedef struct {
		double ElgA;
		double ElgE2;

		// Parametres de la projection
	  double X0;
		double Y0;
		double Lambda0;
		double Phi0;
		double K0;

		// Parametres calculés de la projection
		double N;
		double N2;
		double C;
		double LambdaC;
		double PhiC;
		double Xs;
		double Ys;

	} DataProjection;

	typedef struct {
		double Tx;			// Paramètres de transformation du premier vers le second
		double Ty;
		double Tz;
		double D;
		double Rx;
		double Ry;
		double Rz;
	} Transformation;	// Transformation a 7 parametres

	// Parametres des transformation
	DataProjection	m_L;		// Projection locale
	DataProjection	m_W;		// Projection mondiale

  double m_dElg1a;	// Ellipsoide de départ
  double m_dElg1e2;

  double m_dElg2a;	// Ellipsoide d'arrivée
  double m_dElg2e2;

	Transformation	m_TrLToW;	// Transformation Local vers Mondial
	Transformation	m_TrWToL;	// Transformation Mondial vers Local

  double m_dX0;
  double m_dY0;
  double m_dLambda0;
  double m_dPhi0;
  double m_dK0;

  double m_dN1;			// Parametres calculés de la projection
  double m_dC1;
  double m_dLambdaC1;
  double m_dXs1;
  double m_dYs1;

	double m_dN2;			// Parametres calculés de la projection
  double m_dC2;
  double m_dLambdaC2;
  double m_dXs2;
  double m_dYs2;

public:
	XDomGeod() {;}
	virtual ~XDomGeod() {;}

	virtual bool SetDefaultProjection(XProjCode start_proj, XProjCode end_proj);

  // RRAF
  void SetParamRRAFRGAF09();
	// Fort Desaix
	void SetParamDesaixRRAF();
  void SetParamDesaixRGAF09();
	// Sainte Anne
	void SetParamSteAnneRRAF();
  void SetParamSteAnneRGAF09();
  // Fort Marigot
	void SetParamMarigotRRAF();
  void SetParamMarigotRGAF09();
  // Guyane UTM 21
	void SetParamCSG21RGFG95();
	// Guyane UTM 22
	void SetParamCSG22RGFG95();
	// Gauss Laborde Reunion
	void SetParamReunionRGR92();
	// Saint Pierre et Miquelon 1950
	void SetParamSPMiquelonRGSPM06();
	// Mayotte Combani 1950
	void SetParamCombaniRGM04();
	// Mayotte Cadastre1997
	void SetParamCadastreRGM04();

	void ConvertUTMLocalToWorld(double xL, double yL, double* xW, double* yW, double Z = 0.);
	void ConvertUTMWorldToLocal(double xW, double yW, double* xL, double* yL, double Z = 0.);
  void ConvertUTMLocalToWGS(double xL, double yL, double* xW, double* yW, double Z = 0.);
  void ConvertUTMWorldToWGS(double xW, double yW, double* xL, double* yL, double Z = 0.);
  void ConvertWGSToUTMWorld(double lambda, double phi, double* x93, double* y93, double Z = 0.);
  void ConvertWGSToUTMLocal(double lambda, double phi, double* x93, double* y93, double Z = 0.);

  void ConvertGaussLocalToWorld(double xL, double yL, double* xW, double* yW, double Z = 0.);
  void ConvertGaussWorldToLocal(double xW, double yW, double* xL, double* yL, double Z = 0.);
  void ConvertGaussLocalToWGS(double xL, double yL, double* xW, double* yW, double Z = 0.);
  void ConvertWGSToGaussLocal(double xW, double yW, double* xL, double* yL, double Z = 0.);

  void ConvertRRAFToRGAF09(double xW, double yW, double* xL, double* yL, double Z = 0.);
  void ConvertRGAF09ToRRAF(double xW, double yW, double* xL, double* yL, double Z = 0.);
  void SetZoneRGAF09(double phi);

	virtual bool ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);
	virtual bool Convert(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);

  // Alteration lineaire
  virtual double AltLin(XProjCode start_proj, double Xi, double Yi);
};

#endif //_XDOMGEOD_H
