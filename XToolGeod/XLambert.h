//-----------------------------------------------------------------------------
//								XLambert.h
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/03/2003
//-----------------------------------------------------------------------------

#ifndef _XLAMBERT_H
#define _XLAMBERT_H

#include "../XTool/XBase.h"
#include "../XTool/XPt3D.h"
#include "XGeodConverter.h"

class XLambert : public XGeodConverter {
protected:
	// Donnees de la grille
  XPt3*     m_Grid;
	double		m_dLatMin;
	double		m_dLatMax;
	double		m_dLongMin;
	double		m_dLongMax;
	double		m_dLatStep;
	double		m_dLongStep;
	uint32_t		m_nNbLat;
	uint32_t		m_nNbLong;

	// GRS 80
	double GRS80_a;		// 1/2 grand axe
	double GRS80_e2;	// Aplatissement

	// Clarke 1880
  double Clark_a;		// 1/2 grand axe
  double Clark_e2;	// Aplatissement

	// Parametres de transformation standard
  double Tx;
  double Ty;
  double Tz;
  double D;
  double Rx;
  double Ry;
  double Rz;

	//Lambert 2 Etendu
	double Lamb2E_x0;			// Coordonnees de l'origine
	double Lamb2E_y0;
	double Lamb2E_lambda0;
	double Lamb2E_phi0;
	double Lamb2E_k0;			// Facteur d'echelle

	double Lamb2E_lambdac;
	double Lamb2E_n;
	double Lamb2E_c;
	double Lamb2E_xs;
	double Lamb2E_ys;

	//Lambert 93
	XProjCode	m_Proj93;		// Indique quelle projection RGF93 est actuellement fixee
	double Lamb93_x0;			// Coordonnees de l'origine
	double Lamb93_y0;
	double Lamb93_lambda0; 
	double Lamb93_phi0;   // Latitude origine         
	double Lamb93_phi1;		// Paralleles d'echelle conservee             
	double Lamb93_phi2;  
	
	double Lamb93_lambdac;
	double Lamb93_n;
	double Lamb93_c;
	double Lamb93_xs;
	double Lamb93_ys;

  // LAEA
  double Laea_x0;       // Coordonnees de l'origine
  double Laea_y0;
  double Laea_lambda0;
  double Laea_phi0;     // Latitude origine
  double Laea_qp;
  double Laea_beta1;
  double Laea_Rq;
  double Laea_D;

  // UTM
  XProjCode	m_ProjUTM;	// Indique quelle projection UTM est actuellement fixee
  double Utm_x0;
  double Utm_y0;
  double Utm_lambda0;
  double Utm_phi0;
  double Utm_K0;

  double Utm_n;         // Parametres calculés de la projection
  double Utm_c;
  double Utm_lambdac;
  double Utm_xs;
  double Utm_ys;

	// Methodes de calcul internes
	bool Interpol(double lambda, double phi, double* Tx, double* Ty, double* Tz);

	bool SetParameterClarke(XProjCode proj, double& N, double& C, double& X, double& Y);
	bool ConvertClarke(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
														 double& Xf, double& Yf);

	void Convert2ETo93(double x2e, double y2e, double* x93, double* y93, double Z = 0, bool rgf93 = false);
	void Convert93To2E(double x93, double y93, double* x2e, double* y2e, double Z = 0, bool rgf93 = false);
	bool SetParametersGRS(XProjCode projection);
	bool ConvertGRS(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
														 double& Xf, double& Yf);
	bool ConvertRGF93(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
											 double& Xf, double& Yf, double Z = 0);

  bool ConvertLAEA(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                   double& Xf, double& Yf, double Z);

  bool SetParametersUTM(XProjCode projection);
  bool ConvertUTM(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                  double& Xf, double& Yf, double Z);

public:
	XLambert();
	virtual ~XLambert();

	bool LoadGrid(const char* filename, XError* error = NULL);

	bool WriteGrid(const char* filename, XError* error = NULL);
	bool ReadGrid(const char* filename, XError* error = NULL);


	bool Convert(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
											 double& Xf, double& Yf, double Z = 0.);
	bool ConvertDeg(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
											 double& Xf, double& Yf, double Z = 0.);

	virtual bool Convert(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.)
														{ return Convert(m_StartProjection, m_EndProjection, Xi, Yi, Xf, Yf, Z);}
	virtual bool ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.)
														{ return ConvertDeg(m_StartProjection, m_EndProjection, Xi, Yi, Xf, Yf, Z);}

  // Alteration lineaire
  virtual double AltLin(XProjCode start_proj, double Xi, double Yi);
};

#endif //_XLAMBERT_H
