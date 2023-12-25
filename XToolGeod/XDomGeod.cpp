//-----------------------------------------------------------------------------
//								XDomGeod.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/02/2008
//-----------------------------------------------------------------------------

#include "XDomGeod.h"
#include "XAlgoGeod.h"

//-----------------------------------------------------------------------------
// Choix des projections par defaut
//-----------------------------------------------------------------------------
bool XDomGeod::SetDefaultProjection(XProjCode start_proj, XProjCode end_proj)
{ 
	m_StartProjection = start_proj;
	m_EndProjection = end_proj;

  if (m_StartProjection == RGF93) {
    switch(m_EndProjection) {
    case FortDesaix : SetParamDesaixRRAF(); return true;
    case SainteAnne : SetParamSteAnneRRAF(); return true;
    case FortMarigot : SetParamMarigotRRAF(); return true;
    case CSG1967_UTM21 : SetParamCSG21RGFG95(); return true;
    case CSG1967_UTM22 : SetParamCSG22RGFG95(); return true;
    case PitonNeiges : SetParamReunionRGR92(); return true;
    case SPMiquelon1950 : SetParamSPMiquelonRGSPM06(); return true;
    case Combani1950 : SetParamCombaniRGM04(); return true;
    case Cadastre1997 : SetParamCadastreRGM04(); return true;
    // Systemes mondiaux vers locaux
    case RRAF : SetParamDesaixRRAF(); return true;
    case RGAF09 : SetParamDesaixRRAF(); return true;
    case RGFG95 : SetParamCSG22RGFG95(); return true;
    case RGR92 : SetParamReunionRGR92(); return true;
    case RGSPM06 : SetParamSPMiquelonRGSPM06(); return true;
    case RGM04 : SetParamCombaniRGM04(); return true;
    }
  }
	
	switch (m_StartProjection) {
		// Systemes locaux vers systemes mondiaux
		case FortDesaix : 
      if (m_EndProjection == RGAF09) { SetParamDesaixRGAF09(); return true;}
      if ((m_EndProjection != RRAF)&&(m_EndProjection != RGF93)) return false;
			SetParamDesaixRRAF();
			return true;
		case SainteAnne : 
      if (m_EndProjection == RGAF09) { SetParamSteAnneRGAF09(); return true;}
      if ((m_EndProjection != RRAF)&&(m_EndProjection != RGF93)) return false;
			SetParamSteAnneRRAF();
			return true;
		case FortMarigot : 
      if (m_EndProjection == RGAF09) { SetParamMarigotRGAF09(); return true;}
      if ((m_EndProjection != RRAF)&&(m_EndProjection != RGF93)) return false;
			SetParamMarigotRRAF();
			return true;
		case CSG1967_UTM21 : 
      if ((m_EndProjection != RGFG95)&&(m_EndProjection != RGF93)) return false;
			SetParamCSG21RGFG95();
			return true;
		case CSG1967_UTM22 : 
      if ((m_EndProjection != RGFG95)&&(m_EndProjection != RGF93)) return false;
			SetParamCSG22RGFG95();
			return true;
		case PitonNeiges : 
      if ((m_EndProjection != RGR92)&&(m_EndProjection != RGF93)) return false;
			SetParamReunionRGR92();
			return true;
		case SPMiquelon1950 :
      if ((m_EndProjection != RGSPM06)&&(m_EndProjection != RGF93)) return false;
			SetParamSPMiquelonRGSPM06();
			return true;
		case Combani1950 :
      if ((m_EndProjection != RGM04)&&(m_EndProjection != RGF93)) return false;
			SetParamCombaniRGM04();
			return true;
		case Cadastre1997 :
      if ((m_EndProjection != RGM04)&&(m_EndProjection != RGF93)) return false;
			SetParamCadastreRGM04();
			return true;

		// Systemes mondiaux vers locaux
		case RRAF :
      if (m_EndProjection == RGF93) { SetParamDesaixRRAF(); return true; }
      if (m_EndProjection == RGAF09) { SetParamRRAFRGAF09(); return true; }
      if ((m_EndProjection < FortDesaix)||(m_EndProjection > FortMarigot)) {
				m_EndProjection = FortDesaix;
				SetParamDesaixRRAF();
				return false;
			}
			if (m_EndProjection == FortDesaix) SetParamDesaixRRAF();
			if (m_EndProjection == SainteAnne) SetParamSteAnneRRAF();
			if (m_EndProjection == FortMarigot) SetParamMarigotRRAF();
			return true;
    case RGAF09 :
      if (m_EndProjection == RGF93) { SetParamDesaixRGAF09(); return true; }
      if (m_EndProjection == RRAF) { SetParamRRAFRGAF09(); return true; }
      if ((m_EndProjection < FortDesaix)||(m_EndProjection > FortMarigot)) {
        m_EndProjection = FortDesaix;
        SetParamDesaixRGAF09();
        return false;
      }
      if (m_EndProjection == FortDesaix) SetParamDesaixRGAF09();
      if (m_EndProjection == SainteAnne) SetParamSteAnneRGAF09();
      if (m_EndProjection == FortMarigot) SetParamMarigotRGAF09();
      return true;
    case RGFG95 :
      if (m_EndProjection == RGF93) { SetParamCSG22RGFG95(); return true; }
      if ((m_EndProjection != CSG1967_UTM21)&&(m_EndProjection != CSG1967_UTM22)) {
				m_EndProjection = CSG1967_UTM22;
				SetParamCSG22RGFG95();
				return false;
			}
			if (m_EndProjection == CSG1967_UTM21) SetParamCSG21RGFG95();
			if (m_EndProjection == CSG1967_UTM22) SetParamCSG22RGFG95();
			return true;
		case RGR92 :
      if (m_EndProjection == RGF93) { SetParamReunionRGR92(); return true; }
      if (m_EndProjection != PitonNeiges) {
				m_EndProjection = PitonNeiges;
				SetParamReunionRGR92();
				return false;
			}
			SetParamReunionRGR92();
			return true;
		case RGSPM06 :
      if (m_EndProjection == RGF93) { SetParamSPMiquelonRGSPM06(); return true; }
      if (m_EndProjection != SPMiquelon1950) {
				m_EndProjection = SPMiquelon1950;
				SetParamSPMiquelonRGSPM06();
				return false;
			}
			SetParamSPMiquelonRGSPM06();
			return true;
		case RGM04 :
      if (m_EndProjection == RGF93) { SetParamCombaniRGM04(); return true; }
      if ((m_EndProjection != Combani1950)&&(m_EndProjection != Cadastre1997)) {
				m_EndProjection = Combani1950;
				SetParamCombaniRGM04();
				return false;
			}
			if (m_EndProjection == Combani1950) SetParamCombaniRGM04();
			if (m_EndProjection == Cadastre1997) SetParamCadastreRGM04();
      return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Conversion en fonction des projections par defaut
//-----------------------------------------------------------------------------
bool XDomGeod::Convert(double Xi, double Yi, double& Xf, double& Yf, double Z)
{
  if (m_StartProjection == RGF93) {
    switch(m_EndProjection){
      // Systemes locaux
      case FortDesaix :
      case SainteAnne :
      case FortMarigot :
      case CSG1967_UTM21 :
      case CSG1967_UTM22 :
      case SPMiquelon1950 :
      case Combani1950 :
      case Cadastre1997 :
        ConvertWGSToUTMLocal(Xi, Yi, &Xf, &Yf, Z);
        return true;
      case PitonNeiges :
        ConvertWGSToGaussLocal(Xi, Yi, &Xf, &Yf, Z);
        return true;

      // Systemes mondiaux
      case RRAF :
      case RGAF09 :
      case RGFG95 :
      case RGSPM06 :
      case RGM04 :
      case RGR92 :
        ConvertWGSToUTMWorld(Xi, Yi, &Xf, &Yf, Z);
        return true;
    }
  }

  // Cas specifique du RRAF
  if ((m_StartProjection == RRAF)&&(m_EndProjection == RGAF09)) {
    ConvertRRAFToRGAF09(Xi, Yi, &Xf, &Yf, Z);
    return true;
  }
  if ((m_StartProjection == RGAF09)&&(m_EndProjection == RRAF)) {
    ConvertRGAF09ToRRAF(Xi, Yi, &Xf, &Yf, Z);
    return true;
  }

	switch (m_EndProjection) {
		// Systemes mondiaux vers systemes locaux
		case FortDesaix : 
		case SainteAnne : 
		case FortMarigot : 
		case CSG1967_UTM21 : 
		case CSG1967_UTM22 : 
		case SPMiquelon1950 :
		case Combani1950 :
		case Cadastre1997 :
			ConvertUTMWorldToLocal(Xi, Yi, &Xf, &Yf, Z);
			return true;
		case PitonNeiges : 
			ConvertGaussWorldToLocal(Xi, Yi, &Xf, &Yf, Z);
			return true;

		// Systemes locaux vers systemes mondiaux
		case RRAF :
    case RGAF09 :
    case RGFG95 :
		case RGSPM06 :
		case RGM04 :
			ConvertUTMLocalToWorld(Xi, Yi, &Xf, &Yf, Z);
			return true;
		case RGR92 :
			ConvertGaussLocalToWorld(Xi, Yi, &Xf, &Yf, Z);
      return true;
	}
  if (m_EndProjection == RGF93) {
    switch(m_StartProjection){
      // Systemes locaux
      case FortDesaix :
      case SainteAnne :
      case FortMarigot :
      case CSG1967_UTM21 :
      case CSG1967_UTM22 :
      case SPMiquelon1950 :
      case Combani1950 :
      case Cadastre1997 :
        ConvertUTMLocalToWGS(Xi, Yi, &Xf, &Yf, Z);
        return true;
      case PitonNeiges :
        ConvertGaussLocalToWGS(Xi, Yi, &Xf, &Yf, Z);
        return true;

      // Systemes mondiaux
      case RRAF :
      case RGAF09 :
      case RGFG95 :
      case RGSPM06 :
      case RGM04 :
      case RGR92 :
        ConvertUTMWorldToWGS(Xi, Yi, &Xf, &Yf, Z);
        return true;
    }
  }
	return false;
}

//-----------------------------------------------------------------------------
// Conversion avec passage en degree decimaux
//-----------------------------------------------------------------------------
bool XDomGeod::ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z)
{
  if ((m_StartProjection != RGF93)&&(m_EndProjection != RGF93))
    return Convert(Xi, Yi, Xf, Yf, Z);

  double xi = Xi, yi = Yi;
  if (m_StartProjection == RGF93) {
    xi *= (XPI / 180.);
    yi *= (XPI / 180.);
  }
  bool flag = Convert(xi, yi, Xf, Yf, Z);
  if (m_EndProjection == RGF93) {
    Xf *= (180. / XPI);
    Yf *= (180. / XPI);
  }
  return flag;
}

//-----------------------------------------------------------------------------
// Parametres RRAF vers RGAF09
//-----------------------------------------------------------------------------
void XDomGeod::SetParamRRAFRGAF09()
{
  // Ellipsoide GRS 1980
  m_L.ElgA = 6378137.0;
  m_L.ElgE2 = 0.0066943800229;

  // Ellipsoide GRS 1980
  m_W.ElgA = 6378137.0;
  m_W.ElgE2 = 0.0066943800229;

  // UTM Nord fuseau 20
  m_L.X0 = 500000.;
  m_L.Y0 = 0.;
  m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_L.Phi0 = 0.;
  m_L.K0 = 0.9996;

  // UTM Nord fuseau 20
  m_W.X0 = 500000.;
  m_W.Y0 = 0.;
  m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_W.Phi0 = 0.;
  m_W.K0 = 0.9996;

  // Transformation a 7 parametres
  m_TrLToW.Tx = 1.2239;
  m_TrLToW.Ty = 2.4156;
  m_TrLToW.Tz = -1.7598;
  m_TrLToW.D = 0.2387e-6;
  m_TrLToW.Rx = 0.03800 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.16101 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -0.04925 * XAlgoGeod::Pi() / (3600. * 180.);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
    m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
    m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Fort Desaix vers RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetParamDesaixRRAF()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 20
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 20
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = 126.926;			
  m_TrLToW.Ty = 547.939;
  m_TrLToW.Tz = 130.409;
  m_TrLToW.D = 13.82265e-6;
  m_TrLToW.Rx = -2.78670 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = 5.16124 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -0.85844 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Fort Desaix vers RGAF09
//-----------------------------------------------------------------------------
void XDomGeod::SetParamDesaixRGAF09()
{
  // Ellipsoide International (Hayford 1909)
  m_L.ElgA = 6378388.0;
  m_L.ElgE2 = 0.006722670022;

  // Ellipsoide GRS 1980
  m_W.ElgA = 6378137.0;
  m_W.ElgE2 = 0.0066943800229;

  // UTM Nord fuseau 20
  m_L.X0 = 500000.;
  m_L.Y0 = 0.;
  m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_L.Phi0 = 0.;
  m_L.K0 = 0.9996;

  // UTM Nord fuseau 20
  m_W.X0 = 500000.;
  m_W.Y0 = 0.;
  m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_W.Phi0 = 0.;
  m_W.K0 = 0.9996;

  // Transformation a 7 parametres
  m_TrLToW.Tx = 127.744;
  m_TrLToW.Ty = 547.069;
  m_TrLToW.Tz = 118.359;
  m_TrLToW.D = 14.1012e-6;
  m_TrLToW.Rx = -3.1116 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = 4.9509 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -0.8837 * XAlgoGeod::Pi() / (3600. * 180.);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
    m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
    m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Sainte Anne vers RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetParamSteAnneRRAF()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 20
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 20
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -472.29;			
  m_TrLToW.Ty = -5.63;
  m_TrLToW.Tz = -304.12;
  m_TrLToW.D = 1.8984e-6;
  m_TrLToW.Rx = 0.4362 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.8374 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = 0.2563 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Sainte Anne vers RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetParamSteAnneRGAF09()
{
  // Ellipsoide International (Hayford 1909)
  m_L.ElgA = 6378388.0;
  m_L.ElgE2 = 0.006722670022;

  // Ellipsoide GRS 1980
  m_W.ElgA = 6378137.0;
  m_W.ElgE2 = 0.0066943800229;

  // UTM Nord fuseau 20
  m_L.X0 = 500000.;
  m_L.Y0 = 0.;
  m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_L.Phi0 = 0.;
  m_L.K0 = 0.9996;

  // UTM Nord fuseau 20
  m_W.X0 = 500000.;
  m_W.Y0 = 0.;
  m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_W.Phi0 = 0.;
  m_W.K0 = 0.9996;

  // Transformation a 7 parametres
  m_TrLToW.Tx = -471.060;
  m_TrLToW.Ty = -3.212;
  m_TrLToW.Tz = -305.843;
  m_TrLToW.D = 2.1353e-6;
  m_TrLToW.Rx = 0.4752 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.9978 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = 0.2068 * XAlgoGeod::Pi() / (3600. * 180.);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
    m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
    m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Fort Marigot vers RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetParamMarigotRRAF()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 20
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 20
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = 136.596;			
  m_TrLToW.Ty = 248.148;
  m_TrLToW.Tz = -429.789;
  m_TrLToW.D = 0.;
  m_TrLToW.Rx = 0.;
  m_TrLToW.Ry = 0.;
  m_TrLToW.Rz = 0.;

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Fort Marigot vers RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetParamMarigotRGAF09()
{
  // Ellipsoide International (Hayford 1909)
  m_L.ElgA = 6378388.0;
  m_L.ElgE2 = 0.006722670022;

  // Ellipsoide GRS 1980
  m_W.ElgA = 6378137.0;
  m_W.ElgE2 = 0.0066943800229;

  // UTM Nord fuseau 20
  m_L.X0 = 500000.;
  m_L.Y0 = 0.;
  m_L.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_L.Phi0 = 0.;
  m_L.K0 = 0.9996;

  // UTM Nord fuseau 20
  m_W.X0 = 500000.;
  m_W.Y0 = 0.;
  m_W.Lambda0 = (-63.)*XAlgoGeod::Pi() / 180.;
  m_W.Phi0 = 0.;
  m_W.K0 = 0.9996;

  // Transformation a 7 parametres
  m_TrLToW.Tx = 151.613;
  m_TrLToW.Ty = 253.832;
  m_TrLToW.Tz = -429.084;
  m_TrLToW.D = -0.3971e-6;
  m_TrLToW.Rx = -0.0506 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = 0.0958 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -0.5974 * XAlgoGeod::Pi() / (3600. * 180.);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
    m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
    m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Guyane CSG1967 UTM 21 vers RGFG95 (UTM 22)
//-----------------------------------------------------------------------------
void XDomGeod::SetParamCSG21RGFG95()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 21
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-57.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 22
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-51.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -193.066;			
  m_TrLToW.Ty = 236.993;
  m_TrLToW.Tz = 105.447;
  m_TrLToW.D = 1.5649e-6;
  m_TrLToW.Rx = 0.4814 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.8074 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = 0.1276 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Guyane CSG1967 UTM 22 vers RGFG95 (UTM 22)
//-----------------------------------------------------------------------------
void XDomGeod::SetParamCSG22RGFG95()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 22
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-51.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 22
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-51.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -193.066;			
  m_TrLToW.Ty = 236.993;
  m_TrLToW.Tz = 105.447;
  m_TrLToW.D = 1.5649e-6;
  m_TrLToW.Rx = 0.4814 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.8074 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = 0.1276 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Saint Pierre et Miquelon 1950 vers RGSPM06
//-----------------------------------------------------------------------------
void XDomGeod::SetParamSPMiquelonRGSPM06()
{
	// Ellipsoide Clarke 1866
	m_L.ElgA = 6378206.4;
	m_L.ElgE2 = 0.006768657997;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Nord fuseau 21
	m_L.X0 = 500000.;
	m_L.Y0 = 0.;
	m_L.Lambda0 = (-57.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Nord fuseau 21
	m_W.X0 = 500000.;
	m_W.Y0 = 0.;
	m_W.Lambda0 = (-57.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -95.593;			
  m_TrLToW.Ty = 573.763;
  m_TrLToW.Tz = 173.442;
  m_TrLToW.D = 42.6265e-6;
  m_TrLToW.Rx = -0.9602 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = 1.2510 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -1.3918 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Mayotte Combani 1950 vers RGM04
//-----------------------------------------------------------------------------
void XDomGeod::SetParamCombaniRGM04()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Sud fuseau 38
	m_L.X0 = 500000.;
	m_L.Y0 = 10000000.;
	m_L.Lambda0 = 45.*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Sud fuseau 38
	m_W.X0 = 500000.;
	m_W.Y0 = 10000000.;
	m_W.Lambda0 = 45.*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -599.928;			
  m_TrLToW.Ty = -275.552;
  m_TrLToW.Tz = -195.665;
  m_TrLToW.D = 49.2814e-6;
  m_TrLToW.Rx = -0.0835 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.4715 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = 0.0602 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Parametres Mayotte Cadastre 1997 vers RGM04
//-----------------------------------------------------------------------------
void XDomGeod::SetParamCadastreRGM04()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// UTM Sud fuseau 38
	m_L.X0 = 500000.;
	m_L.Y0 = 10000000.;
	m_L.Lambda0 = 45.*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = 0.;
	m_L.K0 = 0.9996;

	// UTM Sud fuseau 38
	m_W.X0 = 500000.;
	m_W.Y0 = 10000000.;
	m_W.Lambda0 = 45.*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres
	m_TrLToW.Tx = -381.788;			
  m_TrLToW.Ty = -57.501;
  m_TrLToW.Tz = -256.673;
  m_TrLToW.D = 0.;
  m_TrLToW.Rx = 0.;
  m_TrLToW.Ry = 0.;
  m_TrLToW.Rz = 0.;

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_L.ElgA, m_L.ElgE2, m_L.Lambda0, m_L.Phi0,
		m_L.K0, m_L.X0, m_L.Y0, &m_L.LambdaC, &m_L.N, &m_L.Xs, &m_L.Ys);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Conversion systemes locaux (Fort Desaix, Sainte Anne, Fort Marigot, CSG1967)
// vers les systemes mondiaux (RRAF, RGFG95)
//-----------------------------------------------------------------------------
void XDomGeod::ConvertUTMLocalToWorld(double x2e, double y2e, double* x93, double* y93, double Z)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;	


	// Transformation UTM -> Geographique 
	XAlgoGeod::MercTrGeo(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys, m_L.ElgE2,
		&lambda, &phi, x2e, y2e, epsilon_phi);

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(m_L.ElgA, m_L.ElgE2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParRc(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D, 
											m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 m_W.ElgA, m_W.ElgE2, epsilon_phi, 
											 &lambda, &phi, &h);


	// Transformation Geographique -> UTM
	XAlgoGeod::GeoMercTr(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, 
											 m_W.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Conversion systemes mondiaux (RRAF, RGFG95)
// vers les systemes locaux (Fort Desaix, Sainte Anne, Fort Marigot, CSG1967)
//-----------------------------------------------------------------------------
void XDomGeod::ConvertUTMWorldToLocal(double x2e, double y2e, double* x93, double* y93, double Z)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;	


	// Transformation UTM -> Geographique 
	XAlgoGeod::MercTrGeo(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, m_W.ElgE2,
		&lambda, &phi, x2e, y2e, epsilon_phi);

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(m_W.ElgA, m_W.ElgE2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParCR(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D, 
											m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 m_L.ElgA, m_L.ElgE2, epsilon_phi, 
											 &lambda, &phi, &h);


	// Transformation Geographique -> UTM
	XAlgoGeod::GeoMercTr(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys, 
											 m_L.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Conversion systemes locaux (Fort Desaix, Sainte Anne, Fort Marigot, CSG1967)
// vers le WGS84
//-----------------------------------------------------------------------------
void XDomGeod::ConvertUTMLocalToWGS(double x2e, double y2e, double* x93, double* y93, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;


  // Transformation UTM -> Geographique
  XAlgoGeod::MercTrGeo(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys, m_L.ElgE2,
    &lambda, &phi, x2e, y2e, epsilon_phi);

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_L.ElgA, m_L.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParRc(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D,
                      m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_W.ElgA, m_W.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);
  *x93 = lambda;
  *y93 = phi;
}

//-----------------------------------------------------------------------------
// Conversion systemes mondiaux (RRAF, RGFG95)
// vers le WGS84
//-----------------------------------------------------------------------------
void XDomGeod::ConvertUTMWorldToWGS(double x2e, double y2e, double* x93, double* y93, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  // Transformation UTM -> Geographique
  XAlgoGeod::MercTrGeo(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, m_W.ElgE2,
    &lambda, &phi, x2e, y2e, epsilon_phi);
  *x93 = lambda;
  *y93 = phi;
}

//-----------------------------------------------------------------------------
// Conversion du WGS84 vers les systemes mondiaux (RRAF, RGFG95)
//-----------------------------------------------------------------------------
void XDomGeod::ConvertWGSToUTMWorld(double lambda, double phi, double* x93, double* y93, double Z)
{
  // Transformation Geographique -> UTM
  XAlgoGeod::GeoMercTr(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys,
                       m_W.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Conversion du WGS84
// vers les systemes locaux (Fort Desaix, Sainte Anne, Fort Marigot, CSG1967)
//-----------------------------------------------------------------------------
void XDomGeod::ConvertWGSToUTMLocal(double x2e, double y2e, double* x93, double* y93, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  lambda = x2e;
  phi = y2e;

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_W.ElgA, m_W.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParCR(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D,
                      m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_L.ElgA, m_L.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);


  // Transformation Geographique -> UTM
  XAlgoGeod::GeoMercTr(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys,
                       m_L.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Parametres Gauss Laborde Reunion vers RGR92
//-----------------------------------------------------------------------------
void XDomGeod::SetParamReunionRGR92()
{
	// Ellipsoide International (Hayford 1909)
	m_L.ElgA = 6378388.0;
	m_L.ElgE2 = 0.006722670022;

	// Ellipsoide GRS 1980
	m_W.ElgA = 6378137.0;
	m_W.ElgE2 = 0.0066943800229;

	// Gauss Laborde Reunion
	m_L.X0 = 160000.;
	m_L.Y0 = 50000.;
	m_L.Lambda0 = (55 + 32. / 60.)*XAlgoGeod::Pi() / 180.;
	m_L.Phi0 = -(21 + 7. / 60.)*XAlgoGeod::Pi() / 180.;
	m_L.K0 = 1.0;

	// UTM Sud fuseau 40
	m_W.X0 = 500000.;
	m_W.Y0 = 10000000.;
	m_W.Lambda0 = (57.)*XAlgoGeod::Pi() / 180.;
	m_W.Phi0 = 0.;
	m_W.K0 = 0.9996;

	// Transformation a 7 parametres Local vers Monde
	m_TrLToW.Tx = 789.524;			
  m_TrLToW.Ty = -626.486;
  m_TrLToW.Tz = -89.904;
  m_TrLToW.D = -32.3241e-6;
  m_TrLToW.Rx = 0.6006 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = 76.7946 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -10.5788 * XAlgoGeod::Pi() / (3600. * 180.);

	// Transformation a 7 parametres Monde vers Local
	m_TrWToL.Tx = -789.990;			
  m_TrWToL.Ty = 627.333;
  m_TrWToL.Tz = 89.685;
  m_TrWToL.D = 32.2083e-6;
  m_TrWToL.Rx = -0.6072 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrWToL.Ry = -76.8019 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrWToL.Rz = 10.5680 * XAlgoGeod::Pi() / (3600. * 180.);

	// Calcul des paramètres de la projection indépendants de la position
	double epsilon_phi = 1e-11;	
	XAlgoGeod::CoefProjGaussLab( XAlgoGeod::COURB, m_L.ElgA, m_L.ElgE2,
															m_L.Lambda0, m_L.Phi0, m_L.K0, m_L.X0, m_L.Y0, 
															&m_L.LambdaC, &m_L.PhiC, &m_L.C, &m_L.N, &m_L.N2, 
															&m_L.Xs, &m_L.Ys, epsilon_phi);

	// Calcul des paramètres de la projection indépendants de la position
	XAlgoGeod::CoefProjMercTr(m_W.ElgA, m_W.ElgE2, m_W.Lambda0, m_W.Phi0,
		m_W.K0, m_W.X0, m_W.Y0, &m_W.LambdaC, &m_W.N, &m_W.Xs, &m_W.Ys);
}

//-----------------------------------------------------------------------------
// Conversion systemes locaux (Reunion Piton des Neiges) en Gauss Laborde
// vers les systemes mondiaux (RGR92)
//-----------------------------------------------------------------------------
void XDomGeod::ConvertGaussLocalToWorld(double xL, double yL, double* xW, double* yW, double Z)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;	

	// Transformation Gauus Laborde -> Geographique 
	XAlgoGeod::GaussLabGeo( m_L.C, m_L.N, m_L.N2, m_L.Xs, m_L.Ys, 
													m_L.ElgE2, m_L.LambdaC, 
													&lambda, &phi, 
													xL, yL, epsilon_phi);

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(m_L.ElgA, m_L.ElgE2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParRc(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D, 
											m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 m_W.ElgA, m_W.ElgE2, epsilon_phi, 
											 &lambda, &phi, &h);

	// Transformation Geographique -> UTM
	XAlgoGeod::GeoMercTr(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, 
											 m_W.ElgE2, lambda, phi, xW, yW);
}

//-----------------------------------------------------------------------------
// Conversion systemes mondiaux (RGR92)
// vers les systemes locaux (Reunion Piton des Neiges) en Gauss Laborde
//-----------------------------------------------------------------------------
void XDomGeod::ConvertGaussWorldToLocal(double xW, double yW, double* xL, double* yL, double Z)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;	

	// Transformation UTM -> Geographique 
	XAlgoGeod::MercTrGeo(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, m_W.ElgE2,
		&lambda, &phi, xW, yW, epsilon_phi);

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(m_W.ElgA, m_W.ElgE2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParRc(m_TrWToL.Tx, m_TrWToL.Ty, m_TrWToL.Tz, m_TrWToL.D, 
											m_TrWToL.Rx, m_TrWToL.Ry, m_TrWToL.Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 m_L.ElgA, m_L.ElgE2, epsilon_phi, 
											 &lambda, &phi, &h);

	// Transformation Geographique -> Gauss Laborde
	XAlgoGeod::GeoGaussLab(	m_L.C, m_L.N, m_L.N2, m_L.Xs, m_L.Ys, 
													m_L.ElgE2, m_L.LambdaC,
													lambda, phi, 
													xL, yL);
}

//-----------------------------------------------------------------------------
// Conversion systemes locaux (Reunion Piton des Neiges) en Gauss Laborde
// vers le WGS84
//-----------------------------------------------------------------------------
void XDomGeod::ConvertGaussLocalToWGS(double xL, double yL, double* xW, double* yW, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  // Transformation Gauss Laborde -> Geographique
  XAlgoGeod::GaussLabGeo( m_L.C, m_L.N, m_L.N2, m_L.Xs, m_L.Ys,
                          m_L.ElgE2, m_L.LambdaC,
                          &lambda, &phi,
                          xL, yL, epsilon_phi);

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_L.ElgA, m_L.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParRc(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D,
                      m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_W.ElgA, m_W.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);
  *xW = lambda;
  *yW = phi;
}

//-----------------------------------------------------------------------------
// Conversion du WGS84 verl les
// systemes locaux (Reunion Piton des Neiges) en Gauss Laborde
//-----------------------------------------------------------------------------
void XDomGeod::ConvertWGSToGaussLocal(double xW, double yW, double* xL, double* yL, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  lambda = xW;
  phi = yW;

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_W.ElgA, m_W.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParRc(m_TrWToL.Tx, m_TrWToL.Ty, m_TrWToL.Tz, m_TrWToL.D,
                      m_TrWToL.Rx, m_TrWToL.Ry, m_TrWToL.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_L.ElgA, m_L.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);

  // Transformation Geographique -> Gauss Laborde
  XAlgoGeod::GeoGaussLab(	m_L.C, m_L.N, m_L.N2, m_L.Xs, m_L.Ys,
                          m_L.ElgE2, m_L.LambdaC,
                          lambda, phi,
                          xL, yL);
}

//-----------------------------------------------------------------------------
// Alteration lineaire
//-----------------------------------------------------------------------------
double XDomGeod::AltLin(XProjCode start_proj, double Xi, double Yi)
{
  if (start_proj == RGF93)
    return 0;
  if ((start_proj != RGR92)&&(start_proj != RRAF)&&(start_proj != RGFG95)&&
      (start_proj != RGSPM06)&&(start_proj != RGM04))
    return 0;
  double lambda, phi;
  double conv, modlin, alter;

  SetDefaultProjection(start_proj, RGF93);
  Convert(Xi, Yi, lambda, phi);

  XAlgoGeod::ModConvMercTr(lambda, phi, m_W.ElgE2, m_W.K0, m_W.LambdaC, &conv, &modlin);
  alter = (modlin -1)*1e5;

  return alter;
}

//-----------------------------------------------------------------------------
// Conversion RRAF - > RGAF09
//-----------------------------------------------------------------------------
void XDomGeod::ConvertRRAFToRGAF09(double x2e, double y2e, double* x93, double* y93, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  // Transformation UTM -> Geographique
  XAlgoGeod::MercTrGeo(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys, m_L.ElgE2,
    &lambda, &phi, x2e, y2e, epsilon_phi);

  // Detection de la zone
  SetZoneRGAF09(phi);

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_L.ElgA, m_L.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParRc(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D,
                      m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_W.ElgA, m_W.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);


  // Transformation Geographique -> UTM
  XAlgoGeod::GeoMercTr(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys,
                       m_W.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Conversion RGAF09 -> RRAF
//-----------------------------------------------------------------------------
void XDomGeod::ConvertRGAF09ToRRAF(double x2e, double y2e, double* x93, double* y93, double Z)
{
  double lambda, phi, h = Z;
  double epsilon_phi = 0.000000001;

  // Transformation UTM -> Geographique
  XAlgoGeod::MercTrGeo(m_W.Lambda0, m_W.N, m_W.Xs, m_W.Ys, m_W.ElgE2,
    &lambda, &phi, x2e, y2e, epsilon_phi);

  // Detection de la zone
  SetZoneRGAF09(phi);

  // Transformation Geographique -> Cartesienne
  double x1, y1, z1;
  XAlgoGeod::GeoCart(m_W.ElgA, m_W.ElgE2,
                     lambda, phi, h,
                     &x1, &y1, &z1);

  // Changement de systeme geodesique
  double x2, y2, z2;
  XAlgoGeod::Tr7ParCR(m_TrLToW.Tx, m_TrLToW.Ty, m_TrLToW.Tz, m_TrLToW.D,
                      m_TrLToW.Rx, m_TrLToW.Ry, m_TrLToW.Rz,
                      x1, y1, z1,
                      &x2, &y2, &z2);

  // Transformation Cartesienne -> Geographique
  XAlgoGeod::CartGeo12(x2, y2, z2,
                       m_L.ElgA, m_L.ElgE2, epsilon_phi,
                       &lambda, &phi, &h);


  // Transformation Geographique -> UTM
  XAlgoGeod::GeoMercTr(m_L.Lambda0, m_L.N, m_L.Xs, m_L.Ys,
                       m_L.ElgE2, lambda, phi, x93, y93);
}

//-----------------------------------------------------------------------------
// Choix de la transformation a 7 parametres RGAF09 <-> RRAF
//-----------------------------------------------------------------------------
void XDomGeod::SetZoneRGAF09(double phi)
{
  // Transformation a 7 parametres
  m_TrLToW.Tx = 1.2239;
  m_TrLToW.Ty = 2.4156;
  m_TrLToW.Tz = -1.7598;
  m_TrLToW.D = 0.2387e-6;
  m_TrLToW.Rx = 0.03800 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Ry = -0.16101 * XAlgoGeod::Pi() / (3600. * 180.);
  m_TrLToW.Rz = -0.04925 * XAlgoGeod::Pi() / (3600. * 180.);

  // Detection de la zone
  if ((phi >= 14.274 * XAlgoGeod::Pi()/180.)&&(phi <= 15.001 * XAlgoGeod::Pi()/180.)) {
    // Fort Desaix
    m_TrLToW.Tx = 0.7696;
    m_TrLToW.Ty = -0.8692;
    m_TrLToW.Tz = -12.0631;
    m_TrLToW.D = 0.2529e-6;
    m_TrLToW.Rx = -0.32511 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Ry = -0.21041 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Rz = -0.02390 * XAlgoGeod::Pi() / (3600. * 180.);
  }
  if ((phi >= 15.824 * XAlgoGeod::Pi()/180.)&&(phi <= 16.601 * XAlgoGeod::Pi()/180.)) {
    // Saint-Anne
    m_TrLToW.Tx = 1.2239;
    m_TrLToW.Ty = 2.4156;
    m_TrLToW.Tz = -1.7598;
    m_TrLToW.D = 0.2387e-6;
    m_TrLToW.Rx = 0.03800 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Ry = -0.16101 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Rz = -0.04925 * XAlgoGeod::Pi() / (3600. * 180.);
  }
  if (phi >= 17.824 * XAlgoGeod::Pi()/180.) {
    // Fort Marigot
    m_TrLToW.Tx = 14.6642;
    m_TrLToW.Ty = 5.2493;
    m_TrLToW.Tz = 0.1981;
    m_TrLToW.D = -0.4067e-6;
    m_TrLToW.Rx = -0.06838 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Ry = 0.09141 * XAlgoGeod::Pi() / (3600. * 180.);
    m_TrLToW.Rz = -0.58131 * XAlgoGeod::Pi() / (3600. * 180.);
  }
}
