//-----------------------------------------------------------------------------
//														XAlgoGeod.h
//														===========
//
// Auteur : J.P. Papelard - Projet Camera Numerique
//
// Date : 5/09/2000
//
// Bibliotheque de calcul geodesique basee sur les travaux du SGN
//-----------------------------------------------------------------------------

#ifndef _XALGOGEOD_H
#define _XALGOGEOD_H

#include <complex>
#include <cmath>

namespace XAlgoGeod {

	enum TYP_PROJ { COURB = 1, BITAN = 2, EQUA = 3, HOTINE = 4, POL_NORD = 5, POL_SUD = 6};
	enum TYP_DEGRE { DEGRE = 4} ;

	inline double Pi() { return 3.14159265358979323846;}
	inline double Pi2(){ return 1.57079632679489661923;}
	inline double Pi4(){ return 0.785398163397448309116;}
	inline double Inf(){ return 1.79769313e308;}
/*
	static double M_PI = 3.14159265358979323846;	// extrait de tc\include\math.h
	static double M_PI_2 =	1.57079632679489661923;
	static double M_PI_4	=	0.785398163397448309116;
*/
#ifndef M_PI_2
  static double M_PI_2 =	1.57079632679489661923;
#endif

  //static double INF = 1.79769313e308;  //max sur 8 bits

	// alg0001
	double LatIso(double phi, double e2);
	// alg0002
	double LatIsoInv(double latiso, double e2, double epsilon_phi);
	// alg0003
	void GeoLambert(double e2, double n, double c, double lambdac, double xs,
		double ys,double lambda, double phi, double *x,double *y);
	// alg0004
	void LambertGeo(double e2, double n, double c, double lambdac, double epsilon_phi,
		double xs, double ys, double *lambda, double *phi,
		double x, double y);
	// alg0009
	void CartGeo10(double x, double y, double z,
		double a, double e2,double epsilon_phi,
		double *lambda, double *phi, double *h);
	// alg0010
	void GeoCart(double a, double e2,
		double lambda, double phi, double h,
		double *x, double *y, double *z);
	// alg0011
	void CartGeo11(double x, double y, double z,
		double a, double e2,double epsilon_phi,
		double *lambda, double *phi, double *h);
	// alg0012
	void CartGeo12(double x, double y, double z,
		double a, double e2,double epsilon_phi,
		double *lambda, double *phi, double *h);
	// alg0013
	void Tr7ParRc(double tx, double ty, double tz, double d,
		double rx, double ry, double rz,
		double x1, double y1, double z1,
		double *x2, double *y2, double *z2);
	// alg0014
	void RotAxe(char axe, double angle,	double ux,double uy,double uz,
				double *vx,double *vy,double *vz);
	// alg0015
	void SpherCart(double lambda, double phi, double *x, double *y, double *z);
	// alg0016
	void CartSpher(double x, double y, double z, double *lambda, double *phi);
	// alg0019
	void CoefProjLambTan(double a, double e2, double lambda0, double phi0,
		double k0, double x0, double y0,
		double *lambdac, double *n, double *c,
		double *xs, double *ys);
	// alg0021
	double GrandeNormale(double phi, double a, double e2);
	// fonction nécessaire aux algos 25, 28, 29
	double poly ( double x, int degre, double coef[]);
	// alg0025
	void CoeffArcMer(double e2, double c[DEGRE+1]);
	// alg0026
	double DevArcMer(double phi, double e2);
	// alg0027
	double LatArcMer(double beta, double e2, double epsilon_phi);
	// alg0028
	void CoefProjMercTrDir(double e2, double c[DEGRE+1]);
	// alg0029
	void CoefProjMercTrInv(double e2, double c[DEGRE+1]);
	// alg0030
	void GeoMercTr(double lambda0, double n, double xs, double ys,
		double e2, double lambda, double phi, double *x, double *y);
	// alg0031
	void MercTrGeo(double lambda0, double n, double xs, double ys, double e2,
		double *lambda, double *phi, double x, double y, double epsilon_phi);
	// alg0032
	void GeoMerc(double n, double xs, double ys, double e2,
		double lambda, double phi, double *x, double *y);
	// alg0033
	void MercGeo(double n, double xs, double ys, double e2,
		double *lambda, double *phi, double x, double y, double epsilon_phi);
	// alg0034
	void GeoGaussLab(double c, double n1, double n2, double xs, double ys,
		double e2, double lambdac,double lambda, double phi,
		double *x, double *y);
	// alg0035
	void GaussLabGeo(double c, double n1, double n2, double xs, double ys,
		double e2, double lambdac, double *lambda, double *phi,
		double x, double y, double epsilon_phi);
	// alg0036
	void GeoMercObl(double azc,double c,double n1,double n2,double xs,double ys,
		double e2,double lambdac,double phic,double lambda,double phi,
		double *x,double *y, double epsilon_phi);
	// alg0037
	void MercOblGeo(double azc,double n1,double n2,double xs,double ys,
		double e2, double c,double lambdac,double phic,
		double *lambda,double *phi,double x,double y, double epsilon_phi);
	// alg0038
	void GeoSterObl(double c,double n1,double n2,double xs,double ys,
		double e2,double lambdac,double phic, double epsilon_phi,
		double lambda, double phi,double *x,double *y);
	// alg0039
	void SterOblGeo(double n1,double n2,double xs,double ys,
		double e2, double c,double lambdac,double phic, double epsilon_phi,
		double *lambda,double *phi,double x,double y);
	// alg0040
	void GeoLabMad(double azc, double c, double n1, double n2, double xs, double ys,
		double e2, double lambdac, double phic, double lambda, double phi,
		double *x, double *y, double epsilon_phi);
	// alg0041
	void LabMadGeo(double azc, double n1, double n2, double xs, double ys,
		double e2, double c, double lambdac, double phic,
		double *lambda, double *phi, double x, double y, double epsilon_phi);
	// alg0042
	void CoefProjMercObl(int choix, double azc, double a, double e2,
		double lambda0, double phi0, double k0, double x0,
		double y0, double *lambdac, double *phic, double *c,
		double epsilon_phi, double *n1,double *n2,
		double *xs, double *ys);
	// alg0043
	void CoefProjSterObl(char choix,double a,double e2,double lambda0,double phi0,
		double k0,double x0,double y0,double *lambdac, double *phic,
		double *c,double *n1,double *n2, double *xs, double *ys,
		double epsilon_phi);
	// alg0044
	void GeoPolycon(double n, double xs, double ys, double e2, double lambdac,
		double lambda, double phi, double *x, double *y);
	// alg0045
	void PolyconGeo(double n, double xs, double ys, double e2, double lambdac,
		double *lambda, double *phi, double x, double y, double epsilon_phi);
	// alg0046
	void CoefProjGaussLab( TYP_PROJ choix, double a, double e2,
		double lambda0, double phi0, double k0, double x0, double y0,
		double *lambdac, double *phic, double *c, double *n1, double *n2,
		double *xs, double *ys, double epsilon_phi);
	// alg0047
	void CoefProjPolycon(double a, double e2, double lambda0, double phi0, double k0,
		double x0, double y0, double *n, double *lambdac, double *xs,
		double *ys);
	// alg0048
	void GeoBonne(double n, double xs, double ys, double e2, double c,
		double lambdac, double lambda, double phi,
		double *x, double *y);
	// alg0049
	void BonneGeo(double n, double xs, double ys, double e2, double c,
		double lambdac, double epsilon_phi,
		double *lambda, double *phi, double x, double y);

	// alg0050
	void CoefProjLabMad(double a,double e2,double lambda0,double phi0,double k0,
		double x0,double y0,double az0,double *azc,double *lambdac,double *phic,double *c,
		double *n1,double *n2,double *xs,double *ys);
	// alg0051
	void CoefProjBonne(double a, double e2, double lambda0, double phi0,
		double k0, double x0, double y0, double *lambdac,double *c,
		double *n, double *xs,double *ys);
	// alg0052
	void CoefProjMercTr(double a, double e2, double lambda0, double phi0,
		double k0, double x0, double y0, double *lambdac, double *n,
		double *xs, double *ys);
	// alg0053
	void CoefProjMerc(double a, double e2, double lambda0, double phi0, double k0,
		double x0, double y0, double *n, double *xs, double *ys);
	// alg0054
	void CoefProjLambSec(double a, double e2, double lambda0, double phi0,
		double phi1, double phi2, double x0, double y0,
		double *lambdac, double *n, double *c, double *xs, double *ys);
	// alg0055
	void CoefProjLambSecParam(double a, double e2, double n, double lambda0,double y0 ,
		double xs, double ys, double c, double epsilon_phi,
		double *lambdac, double *phi0, double *phi1, double *phi2,
		double *x0);
	// alg0056
	void CartGeo56(double x, double y, double z, double a, double e2,
		double *lambda, double *phi, double *h);
	// alg0057
	void ConstLambTgt(double a, double e2, double phi1, double phi2,
		double *phit, double *k0, double *n, double *c);
	// alg0058
	void ConstLambSec(double a,double e2,double phit,double k0,double *phi1,
		double *phi2, double *n, double *c, double epsilon_phi);
	// alg0059
	void ModConvSterObl ( double a, double lambda, double phi, double e2, double c, double n1,
		double n2,double lambdac,double phic,
		double *conv, double *modlin, double *altlin, double epsilon_phi);
	// alg0060
	void ModConvLamb(double lambda, double phi, double a, double e2, double n,
		double c, double lambdac,  double *conv, double *modlin, double *K, double *alter );
	// alg0061
	void ModConvMercTr(double lambda, double phi, double e2,
		double k0, double lambdac, double *conv, double *modlin);
	// alg0062
	void CartLoc  (double x0, double y0, double z0,
		double x1, double y1, double z1,
		double a, double e2,
		double *x2, double *y2, double *z2);
	// alg0063
	void Tr7ParCR(double tx, double ty, double tz, double d,
		double rx, double ry, double rz,
		double x1, double y1, double z1,
		double *x2, double *y2, double *z2);

	// alg0069 (ex 38bis)
	void GeoSterPolSudSec(double e2, double lambdac, double n2, double Xs, double Ys, double lon,
		double lat, double epsilon, double *X,  double *Y);
	// alg0070 (ex 39bis)
	void SterPolSudSecGeo(double e2, double lambdac, double n2, double Xs, double Ys, double X,
		double Y, double epsilon, double *lon,  double *lat);

  // alg0072 Transformation de coordonnées géographiques en coordonnées planes
  // dans la projection "Lambert Azimuthal Equal Area".
  double q_(double e2, double phi);
  double m_(double e2, double phi);
  void CoefProjLAEA(double a, double e2, double phi0,
           double *qp, double *beta1, double *Rq, double *D);
  void GeoLAEA(double a, double e2, double lambda0, double phi0, double qp,
         double Rq, double beta1, double D, double x0, double y0,
         double lambda, double phi, double *x, double *y);
  // alg0073 Transformation de coordonnées planes dans la projection
  // "Lambert Azimutal Equal Area", en coordonnées géographiques.
  void LAEAgeo(double a, double e2, double lambda0, double phi0, double qp,
         double Rq,	double beta1, double D, double x0, double y0,
         double *lambda, double *phi, double x, double y);

}

#endif //_XALGOGEOD_H
