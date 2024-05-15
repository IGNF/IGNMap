//-----------------------------------------------------------------------------
//														XAlgoGeod.cpp
//														=============
//
// Auteur : J.P. Papelard - Projet Camera Numerique
//
// Date : 5/09/2000
//
// Bibliotheque de calcul geodesique basee sur les travaux du SGN
//-----------------------------------------------------------------------------

#include "XAlgoGeod.h"
#include "../XTool/XBase.h"

// calcul d'un polynome
double XAlgoGeod :: poly ( double x, int degre, double coef[])
{
	double  val = 0;
	for (int i = 0 ; i <= degre  ; i++)
	{
		val += coef[i]*pow( x , i ) ;  
	}
	return val;
}

//////////////////////////////////////////////////////////////////////////////
// Algo de 1 à 64
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/*alg0001----------latitude isométrique-------------------------------------*/
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod :: LatIso(double phi, double e2)
  {
  double li;
  double e = sqrt(e2);

  li = log(tan(XPI4+phi/2)) + e*log( (1-e*sin(phi))/(1+e*sin(phi)) )/2;
  return li;
  }
//////////////////////////////////////////////////////////////////////////////
/*alg0002---Latitude à partir de la latitude isométrique--------------------*/
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod::LatIsoInv(double latiso, double e2, double epsilon_phi)
{
  int LatIsoInv_niter;
  double l0, l1;
  double e = sqrt(e2);

  l1 = 2*atan(exp(latiso)) - XPI2;
  l0 = 100.;
  for (LatIsoInv_niter=0; fabs(l1-l0)>epsilon_phi; LatIsoInv_niter++)
    {
    l0 = l1;
    l1 = 2 * atan( exp(e*log((1+e*sin(l0))/(1-e*sin(l0)))/2) * exp(latiso) )
	 - XPI2;
    }
  return l1;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0003-transfo de coord géo en coord en proj conique conforme de lambert-*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod :: GeoLambert(double e2, double n, double c, double lambdac,
	double xs, double ys,
	double lambda, double phi, double *x,double *y)
{
  double ex, dl;
  ex = exp(-n*LatIso(phi,e2));
  dl = n*(lambda-lambdac);
  *x = xs + c * ex * sin(dl);
  *y = ys - c * ex * cos(dl);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0004-transfo de coord en proj conique conforme de lambert en coord géo -*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::LambertGeo(double e2, double n, double c, double lambdac, double epsilon_phi, 
					   double xs, double ys, double *lambda, double *phi, 
					   double x, double y)
{
  double r, gamma, latiso;

  r = sqrt((x-xs)*(x-xs) + (y-ys)*(y-ys));
  gamma = atan((x-xs)/(ys-y));
  *lambda = lambdac+gamma/n;
  latiso = -log(fabs(r/c))/n;
  *phi = LatIsoInv(latiso, e2, epsilon_phi);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod::GrandeNormale(double phi, double a, double e2)
{
  return a/sqrt(1 - e2*sin(phi)*sin(phi));

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoCart(double a, double e2, double lambda, double phi, 
					double h, double *x, double *y, double *z)
{
  double gn;
  gn = GrandeNormale(phi, a, e2);
  *x = (gn+h)*cos(phi)*cos(lambda);
  *y = (gn+h)*cos(phi)*sin(lambda);
  *z = (gn*(1-e2)+h)*sin(phi);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartGeo10(double x, double y, double z, 
					  double a, double e2, double epsilon_phi, 
					  double *lambda, double *phi, double *h)
{
  double phi0, phi1, r;
  int niter;

  *lambda = atan(y/x);
  r = sqrt(x*x+y*y);
  phi0 = 0;
  phi1 = atan(z/(r*(1-e2)));
  for (niter=0; fabs(phi1-phi0)>epsilon_phi; niter++)
    {
    phi0 = phi1;
    phi1 = atan(z/r + a*e2*sin(phi0)/(r*sqrt(1-e2*sin(phi0)*sin(phi0))));
    }
  *phi = phi1;
  *h = r/cos(*phi) - a/sqrt(1-e2*sin(*phi)*sin(*phi));

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartGeo11(double x, double y, double z, 
					  double a, double e2, double epsilon_phi, 
					  double *lambda, double *phi, double *h)
{
  double phi0, phi1, r;
  int niter;

  *lambda = atan(y/x);
  r = sqrt(x*x+y*y);
  phi0 = 0;
  phi1 = atan(z/(r*(1-e2)));
  for (niter=0; fabs(phi1-phi0)>epsilon_phi; niter++)
    {
    phi0 = phi1;
    phi1 = atan((z/r)/(1-a*e2*cos(phi0)/(r*sqrt(1-e2*sin(phi0)*sin(phi0)))));
    }
  *phi = phi1;
  *h = r/cos(*phi) - a/sqrt(1-e2*sin(*phi)*sin(*phi));

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartGeo12(double x, double y, double z, 
					  double a, double e2, double epsilon_phi, 
					  double *lambda, double *phi, double *h)
{
  double phi0, phi1, r;
  int niter;

  *lambda = atan(y/x);
  r = sqrt(x*x+y*y);
  phi0 = 0;
  phi1 = atan(z/(r*(1-a*e2/sqrt(x*x+y*y+z*z))));
  for (niter=0; fabs(phi1-phi0)>epsilon_phi; niter++)
    {
    phi0 = phi1;
    phi1 = atan((z/r)/(1-a*e2*cos(phi0)/(r*sqrt(1-e2*sin(phi0)*sin(phi0)))));
    }
  *phi = phi1;
  *h = r/cos(*phi) - a/sqrt(1-e2*sin(*phi)*sin(*phi));

}
//////////////////////////////////////////////////////////////////////////////
/*alg0013 Transfo à 7 paramètres (référence -> choisi)-----------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::Tr7ParRc(double tx, double ty, double tz, double d, 
					 double rx, double ry, double rz, 
					 double x1, double y1, double z1, 
					 double *x2, double *y2, double *z2)
{

/* emploi de var. intermédiaires pour autoriser x2 … pointer sur x1, etc... */
  double x,y,z;
  x = tx + x1*(1+d) + z1*ry - y1*rz;
  y = ty + y1*(1+d) + x1*rz - z1*rx;
  z = tz + z1*(1+d) + y1*rx - x1*ry;
  *x2 = x;
  *y2 = y;
  *z2 = z;
 
}
//////////////////////////////////////////////////////////////////////////////
/*alg0014--Rotation autour d'un axe-----------------------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::RotAxe(char axe, double angle,
		double ux,double uy,double uz,
		double *vx,double *vy,double *vz)
{
	switch (axe) {
	case 'x':
	case 'X':
		*vx=ux;
		*vy=uy*cos(angle)+uz*sin(angle);
		*vz=uz*cos(angle)-uy*sin(angle);
		break;
	case 'y':
	case 'Y':
		*vx=ux*cos(angle)-uz*sin(angle);
		*vy=uy;
		*vz=uz*cos(angle)+ux*sin(angle);
		break;
	case 'z':
	case 'Z':
		*vx=ux*cos(angle)+uy*sin(angle);
		*vy=uy*cos(angle)-ux*sin(angle);
		*vz=uz;
		break;
	default : break;
	}/*end switch*/
}

//////////////////////////////////////////////////////////////////////////////
/*alg0015 Transfo. sphériques en cartésiennes-------------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::SpherCart(double lambda, double phi, double *x, double *y, double *z)
{
	*x = cos(phi)*cos(lambda);
	*y = cos(phi)*sin(lambda);
	*z = sin(phi);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0016 Transfo. cartésiennes en sphériques-------------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartSpher(double x, double y, double z, double *lambda, double *phi)
{
	double r;
	r = sqrt(x*x+y*y);
	if (r)
    {
		*lambda=2*atan(y/(x+r));
		*phi=atan(z/r);
    }
	else
    {
		*lambda=0;
		*phi=XPI2*z/fabs(z);
    }
	
}

//////////////////////////////////////////////////////////////////////////////
/*alg0019 Paramètres de projection Lambert conique conforme tangent---------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjLambTan(double a, double e2, double lambda0, double phi0, 
							double k0, double x0, double y0, double *lambdac, 
							double *n, double *c, double *xs, double *ys)
{
  /* ici, on dit (abusivement) que phi0 est la latitude de tangence */
  double r = k0 * GrandeNormale(phi0,a,e2) / tan(phi0);
  *lambdac = lambda0;
  *n = sin(phi0);
  *c = r * exp(*n * LatIso(phi0,e2));
  *xs = x0;
  *ys = y0 + r;

}

//////////////////////////////////////////////////////////////////////////////
/*alg0025 calcul des coeff pour arc de méridien-----------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoeffArcMer(double e2, double c[DEGRE+1])
{
	double coef[DEGRE+1][DEGRE+1]={
		{1, -1./4,  -3./64,   -5./256, -175/16384.},
		{0, -3./8,  -3./32, -45./1024,  -105./4096},
		{0,     0, 15./256,  45/1024.,   525/16384.},
		{0,     0,       0, -35/3072.,  -175/12288.},
		{0,     0,       0,         0,  315/131072.}};
		
		for (int i=0; i<=DEGRE; i++)
			c[i] = poly(e2, DEGRE, coef[i]);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0026--Calcul de développement d'un arc de méridien---------------------*/
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod::DevArcMer(double phi, double e2)
{
	double beta, coef[5];
	int k;
	
	CoeffArcMer(e2, coef);
	beta = coef[0]*phi;
	for (k=1; k<5; k++)
		beta += coef[k]*sin(2*k*phi);
	return beta;
}

//////////////////////////////////////////////////////////////////////////////
/*alg0027-Calcul de latitude en fonction d'un arc de méridien---------------*/
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod::LatArcMer(double beta, double e2, double epsilon_phi)
{
  double phi0, phi1, b1, coef[5];
  int k;

  CoeffArcMer(e2, coef);
  phi1 = beta/coef[0];
  do
    {
    phi0=phi1;
    for (b1=0, k=1; k<5; k++)
      b1 += coef[k]*sin(2*k*phi0);
    phi1 = (beta-b1)/coef[0];
    } while (fabs(DevArcMer(phi1, e2) - beta) > epsilon_phi);
  return phi1;
}

//////////////////////////////////////////////////////////////////////////////
/*alg0028-Calcul des coefficients de la projection de Mercator sens direct--*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjMercTrDir(double e2, double c[])
{
  double coef[DEGRE+1][DEGRE+1]={
	{1, -1./4,  -3./64,   -5./256,      -175/16384.},
	{0,  1./8,  -1./96,  -9./1024,     -901/184320.},
	{0,     0, 13./768,  17/5120.,     -311/737280.},
	{0,     0,       0, 61/15360.,      899/430080.},
	{0,     0,       0,         0, 49561./41287680.}};

  for (int i=0; i<=DEGRE; i++)
    c[i] = poly(e2, DEGRE, coef[i]);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0029-Calcul des coefficients de la projection de Mercator sens inverse */
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjMercTrInv(double e2, double c[])
{
  double coef[DEGRE+1][DEGRE+1]={
	{1, -1./4, -3./64,   -5./256,    -175/16384.},
	{0,  1./8,  1./48,   7./2048,       1/61440.},
	{0,     0, 1./768,   3./1280,    559/368640.},
	{0,     0,      0, 17/30720.,    283/430080.},
	{0,     0,      0,         0, 4397/41287680.}};

  for (int i=0; i<=DEGRE; i++)
    c[i] = poly(e2, DEGRE, coef[i]);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0030--transfo de géographiques en Mercator transverse------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoMercTr(double lambda0, double n, double xs, double ys, 
					  double e2, double lambda, double phi, double *x, double *y)
{
	double phi1, lambda1, l, ls, c[5];
#ifndef __cplusplus
	struct
#endif
	int i;
	
	CoefProjMercTrDir(e2, c);
	l = LatIso(phi, e2);
	lambda1 = XPI2;
	if (l == atof("+INF"))
		phi1 = XPI2;
	else if (l == atof("-INF"))
		phi1 = -XPI2;
	else
    {
		phi1 = asin(sin(lambda-lambda0)/cosh(l));
		lambda1 = atan(sinh(l)/cos(lambda-lambda0));
    }
	ls = LatIso(phi1, 0.);
#ifdef __cplusplus
	//  z = complex(lambda1, ls);
	std::complex <double>z(lambda1,ls);
	std::complex <double> zz;
	zz = n*c[0]*z;
	for (i=1; i<5; i++)
		//    zz += n*c[i]*sin(2*i*z);
		zz +=  n * c[i] * std::sin( (2.0*i*z) );
	*x = xs+std::imag(zz);
	*y = ys+std::real(zz);
#else
	z.x = lambda1;
	z.y = ls;
	zz.x = n*c[0]*z.x;
	zz.y = n*c[0]*z.y;
	for (i=1; i<5; i++)
    {
		/* sin(z) = sin(x) ch(y) + i cos(x) sh(y) */
		zz.x += n*c[i]* sin(2*i*z.x) * cosh(2*i*z.y);
		zz.y += n*c[i]* cos(2*i*z.x) * sinh(2*i*z.y);
    }
	*x = xs + zz.y;
	*y = ys + zz.x;
#endif
}
//////////////////////////////////////////////////////////////////////////////
/*alg0031-Transfo de Mercator Transverse en géographiques-------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::MercTrGeo(double lambda0, double n, double xs, double ys, double e2,
					  double *lambda, double *phi, double x, double y, double epsilon_phi)
{
	double l, ls, phi1, c[5];
#ifndef __cplusplus
	struct
#endif
	int i;
	
	CoefProjMercTrInv(e2, c);
#ifdef __cplusplus
//	complex z, zp;
//	z = zp = complex((y-ys)/(n*c[0]), (x-xs)/(n*c[0]));
	std::complex <double>z ( (y-ys)/(n*c[0]), (x-xs)/(n*c[0]) ) ;
	std::complex <double>zp ( (y-ys)/(n*c[0]), (x-xs)/(n*c[0]) ) ;
	
	for (i=1; i<5; i++)
//		z -= c[i]*sin(2*i*zp);
			z -= c[i] * std::sin( (2.0*i*zp) );
	l = std::real(z);
	ls = std::imag(z);
#else
	z.x = zp.x = (y-ys)/(n*c[0]);
	z.y = zp.y = (x-xs)/(n*c[0]);
	for (i=1; i<5; i++)
    {
		/* sin(z) = sin(x) ch(y) + i cos(x) sh(y) */
		z.x -= c[i] * sin(2*i*zp.x) * cosh(2*i*zp.y);
		z.y -= c[i] * cos(2*i*zp.x) * sinh(2*i*zp.y);
    }
	l = z.x;
	ls = z.y;
#endif
	*lambda = lambda0 + atan(sinh(ls)/cos(l));
	phi1 = asin(sin(l)/cosh(ls));
	l = LatIso(phi1, 0.);
	*phi = LatIsoInv(l, e2, epsilon_phi);
	
}
//////////////////////////////////////////////////////////////////////////////
/*alg0032-passage de géographiques en Mercator Directe---------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoMerc(double n, double xs, double ys, double e2, 
					double lambda, double phi, double *x, double *y)
{
	*x = xs + n*lambda;
	*y = ys + n*LatIso(phi,e2);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0033-transfo de Mercator directe en géographiques----------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::MercGeo(double n, double xs, double ys, double e2, 
					double *lambda, double *phi, double x, double y, double epsilon_phi)
{
	*lambda = (x-xs)/n;
	*phi = LatIsoInv((y-ys)/n, e2, epsilon_phi);

}

//////////////////////////////////////////////////////////////////////////////
/*alg0034-transfo de g‚ographiques en Gauss-Laborde-------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoGaussLab(double c, double n1, double n2, double xs, double ys, 
						double e2, double lambdac, double lambda, double phi,
						double *x, double *y)
{
	double LAMBDA, ls;
	LAMBDA = n1 * (lambda-lambdac);
	ls = c + n1*LatIso(phi, e2);
	*x = xs + n2 * LatIso((asin(sin(LAMBDA)/cosh(ls))), 0.);
	*y = ys + n2 * atan(sinh(ls)/cos(LAMBDA));
}
//////////////////////////////////////////////////////////////////////////////
/*alg0035--transfo Gauss-Laborde en géographiques---------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GaussLabGeo(double c, double n1, double n2, double xs, double ys, 
						double e2, double lambdac, double *lambda, double *phi, 
						double x, double y, double epsilon_phi)
{
	double LAMBDA, ls;
	LAMBDA = atan( sinh((x-xs)/n2)/cos((y-ys)/n2) );
	ls = LatIso((asin(sin((y-ys)/n2)/cosh((x-xs)/n2))), 0.);
	*lambda = lambdac + LAMBDA/n1;
	*phi = LatIsoInv(((ls-c)/n1), e2, epsilon_phi);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0036-Transfo géographiques en Mercator Oblique-------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoMercObl(double azc, double c, double n1, double n2, 
					   double xs, double ys, double e2, double lambdac, double phic, 
					   double lambda, double phi, double *x, double *y, double epsilon_phi)
{
	double LAMBDA,LS,PHI,ux,uy,uz,vx,vy,vz;

	LAMBDA=n1*(lambda-lambdac);
	LS=c+n1*LatIso(phi,e2);
	PHI=LatIsoInv(LS,0,epsilon_phi);
	SpherCart(LAMBDA,PHI,&ux,&uy,&uz);
	RotAxe ('y',-phic,ux,uy,uz,&vx,&vy,&vz);
	RotAxe ('x',(XPI2-azc),vx,vy,vz,&ux,&uy,&uz);
	CartSpher(ux,uy,uz,&LAMBDA,&PHI);
	ux=n2*LAMBDA;
	uy=n2*LatIso(PHI,0);
	RotAxe ('z',azc-XPI2,ux,uy,uz,&vx,&vy,&vz);
	*x=vx+xs;
	*y=vy+ys;
}

//////////////////////////////////////////////////////////////////////////////
/*alg0037---Transfo Mercator Oblique en Géographiques----------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::MercOblGeo(double azc, double n1, double n2, double xs, double ys, 
					   double e2, double c, double lambdac, double phic, 
					   double *lambda, double *phi, double x, double y, double epsilon_phi)
{
	double LAMBDA,L,PHI,ux,uy,uz,vx,vy,vz;
	ux=x-xs;
	uy=y-ys;
	uz=0;		/* ??? */
	RotAxe ('z',XPI2-azc,ux,uy,uz,&vx,&vy,&vz);
	LAMBDA=vx/n2;
	PHI=LatIsoInv(vy/n2,0, epsilon_phi);
	SpherCart(LAMBDA,PHI,&ux,&uy,&uz);
	RotAxe ('x',azc-XPI2,ux,uy,uz,&vx,&vy,&vz);
	RotAxe ('y',phic,vx,vy,vz,&ux,&uy,&uz);
	CartSpher(ux,uy,uz,&LAMBDA,&PHI);
	*lambda=lambdac+LAMBDA/n1;
	L=(LatIso(PHI,0)-c)/n1;
	*phi=LatIsoInv(L,e2, epsilon_phi);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0038--Transfo Géographique en Stéréographique Oblique-------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoSterObl(double c, double n1, double n2, double xs, double ys,
					   double e2, double lambdac, double phic, double epsilon_phi, 
					   double lambda, double phi, double *x, double *y)
{
	double LAMBDA,LS,PHI,ux,uy,uz,vx,vy,vz;
	LAMBDA=n1*(lambda-lambdac);
	LS=c+n1*LatIso(phi,e2);
	PHI=LatIsoInv(LS,0,epsilon_phi);
	SpherCart(LAMBDA,PHI,&ux,&uy,&uz);
	RotAxe ('y',XPI2-phic,ux,uy,uz,&vx,&vy,&vz);
	*x=xs+2*n2*vy/(1+vz);
	*y=ys-2*n2*vx/(1+vz);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0039--Transfo Stéréographique Oblique en Géographiques-----------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::SterOblGeo(double n1, double n2, double xs, double ys, 
					   double e2, double c, double lambdac, double phic, double epsilon_phi,
					   double *lambda, double *phi, double x, double y)
{
	double r,LAMBDA,L,PHI,ux,uy,uz,vx,vy,vz;
	ux = x - xs;
	uy = -y + ys;
	r = sqrt( ux*ux + uy*uy );
	if ( r==0 )
		LAMBDA = 0;
	else 
		LAMBDA = 2*atan( ux/(r+uy) );
	PHI = XPI2 - 2*atan( r/(2*n2) );
	SpherCart(LAMBDA,PHI, &ux, &uy, &uz);
	RotAxe ('y', phic - XPI2, ux, uy, uz, &vx, &vy, &vz);
	CartSpher( vx, vy, vz, &LAMBDA, &PHI);
	*lambda = lambdac + LAMBDA/n1;
	L = ( LatIso(PHI,0) - c )/n1;
	*phi = LatIsoInv( L, e2, epsilon_phi);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0040-Transfo Géographiques en Laborde Madagascar---------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoLabMad(double azc, double c, double n1, double n2,
					  double xs, double ys, double e2, double lambdac, double phic,
					  double lambda, double phi, double *x, double *y, double epsilon_phi)
{
	double LAMBDA,LS,PHI,ux,uy,uz,vx,vy,vz;
	LAMBDA = n1 * ( lambda - lambdac );
	LS = c + n1 * LatIso( phi, e2 );
	PHI = LatIsoInv( LS, 0, epsilon_phi );
	SpherCart( LAMBDA, PHI, &ux, &uy, &uz);
	RotAxe ( 'y', -phic, ux, uy, uz, &vx, &vy, &vz );
	RotAxe ( 'x', -XPI2, vx, vy, vz, &ux, &uy, &uz );
	CartSpher( ux, uy, uz, &LAMBDA, &PHI);

#ifdef __cplusplus
	std::complex <double> z( -LAMBDA , LatIso(PHI,0) );
	std::complex <double> C( (1 - cos(2*azc))/12 , sin(2*azc)/12 );
#else
	complex z=complex(-LAMBDA,LatIso(PHI,0));
	complex C=complex((1-cos(2*azc))/12,sin(2*azc)/12);
#endif

	*x = xs + n2 * std::imag( z + C * z * z * z );
	*y = ys + n2 * std::real( z + C * z * z * z );
}
//////////////////////////////////////////////////////////////////////////////
/*alg0041--Transfo de Laborde Madagascar en Géographiques-------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::LabMadGeo(double azc, double n1, double n2, double xs, double ys, 
					  double e2, double c, double lambdac, double phic, 
					  double *lambda, double *phi, double x, double y, double epsilon_phi)
{
	double LAMBDA,L,PHI,ux,uy,uz,vx,vy,vz;
	int i;
#ifdef __cplusplus
	std::complex <double> Z( (y-ys)/n2 , (x-xs)/n2 );
	std::complex <double> C( (1-cos(2*azc))/12 , sin(2*azc)/12 );
	std :: complex <double> z1 ( Z /(Z + C * Z * Z * Z ) );
	std :: complex <double> z0;
#else
	complex C=complex((1-cos(2*azc))/12,sin(2*azc)/12);
	complex Z=complex((y-ys)/n2,(x-xs)/n2);
	complex z1=Z/(Z+C*Z*Z*Z);
	complex z0;
#endif
	i=0;
	do {
		i++;
		z0 = z1;
		z1 = ( Z + 2.0 * C * z0 *z0 *z0 ) / ( 3.0 * C * z0 * z0 + 1.0 );
	} while ( std::norm(Z-z1-C*z1*z1*z1) > epsilon_phi);
	LAMBDA = -std::real(z1);
	PHI = LatIsoInv( std::imag(z1), 0, epsilon_phi);
	SpherCart(LAMBDA, PHI, &ux, &uy, &uz);
	RotAxe ( 'x', XPI2, ux, uy, uz, &vx, &vy, &vz);
	RotAxe ( 'y', phic, vx, vy, vz, &ux, &uy, &uz);
	CartSpher( ux, uy, uz, &LAMBDA, &PHI);
	*lambda = lambdac + LAMBDA/n1;
	L = ( LatIso( PHI, 0 ) - c ) / n1;
	*phi = LatIsoInv( L, e2, epsilon_phi );
//	niter=i;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0042----Calcul des coefficients de la projection Mercator Oblique------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjMercObl(int choix, double azc, double a, double e2, double lambda0, 
							double phi0, double k0, double x0, double y0, 
							double *lambdac, double *phic, double *c, double epsilon_phi,
							double *n1, double *n2, double *xs, double *ys)
{
	double N0, r;
	switch (choix){
	case 1:/*sphere courbure*/
		*lambdac = lambda0;
		*n1 = sqrt( 1 + e2*cos(phi0)*cos(phi0)*cos(phi0)*cos(phi0)/(1-e2));
		*phic = asin( sin(phi0)/ *n1);
		*c = LatIso( *phic, 0) - *n1*LatIso( phi0, e2);
		*n2 = k0 * a * sqrt(1-e2)/(1 - e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0;
		break;
	case 2:/*sphere bitangente*/
		*lambdac = lambda0;
		*n1 = 1;
		*phic = phi0;
		*c = LatIso( *phic, 0) - *n1*LatIso( phi0, e2 );
		*n2 = k0 * a / sqrt(1 - e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0;
		break;
	case 3:/*sphere equatoriale*/
		*lambdac = lambda0;
		*n1 = 1;
		*phic = LatIsoInv( LatIso( phi0, e2 ), 0, epsilon_phi);
		*c = 0;
		*n2 = k0 * a * cos(phi0) / (cos(*phic) * sqrt(1 - e2*sin(phi0)*sin(phi0)));
		*xs = x0;
		*ys = y0;
		break;
	case 4:/*cas Hotine */
		*phic = 0;
		*n1 = sqrt( 1 + e2*cos(phi0)*cos(phi0)*cos(phi0)*cos(phi0)/(1 - e2));
		N0 = a / sqrt(1 - e2*sin(phi0)*sin(phi0));
		r = a * *n1*sqrt(1 - e2)/(1 - e2*sin(phi0)*sin(phi0));
		*n2 = a * k0 * sqrt(1 - e2)/(1 - e2*sin(phi0)*sin(phi0));
		*c = log((r + sqrt(r*r - N0*N0*cos(phi0)*cos(phi0)))/(N0*cos(phi0)))
			- *n1 * LatIso( phi0, e2 );
		*lambdac = lambda0 - asin(tan(azc)*sqrt(r*r*cos(phi0)*cos(phi0))
			/(N0*cos(phi0)))/ *n1;
		*xs = x0;
		*ys = y0;
		break;
	default:
		break;
	}/*end switch*/
}

//////////////////////////////////////////////////////////////////////////////
/*alg0043-Calculs des coefficients de la projection Stéréographique oblique-*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjSterObl(char choix, double a, double e2, double lambda0, 
							double phi0, double k0, double x0, double y0, 
							double *lambdac, double *phic, double *c, 
							double *n1, double *n2, double *xs, double *ys,
							double epsilon_phi)
{
	double e = sqrt(e2);
	double f = exp(e/2*log((1-e)/(1+e)));
	switch (choix){
	case 1:/*sphere courbure*/
		*lambdac=lambda0;
		*n1=sqrt(1+e2*cos(phi0)*cos(phi0)*cos(phi0)*cos(phi0)/(1-e2));
		*phic=asin(sin(phi0)/ *n1);
		*c=LatIso(*phic,0)-*n1*LatIso(phi0,e2);
		*n2=k0*a*sqrt(1-e2)/(1-e2*sin(phi0)*sin(phi0));
		*xs=x0;
		*ys=y0;
		break;
		
	case 2:/*sphere bitangente*/
		*lambdac=lambda0;
		*n1=1;
		*phic=phi0;
		*c=LatIso(*phic,0)-*n1*LatIso(phi0,e2);
		*n2=k0*a/sqrt(1-e2*sin(phi0)*sin(phi0));
		*xs=x0;
		*ys=y0;
		break;
		
	case 3:/*sphere equatoriale*/
		*lambdac=lambda0;
		*n1=1;
		*phic=LatIsoInv(LatIso(phi0,e2),0, epsilon_phi);
		*c=0;
		*n2=k0*a*cos(phi0)/(cos(*phic)*sqrt(1-e2*sin(phi0)*sin(phi0)));
		*xs=x0;
		*ys=y0;
		break;
		
	case 4:/*cas Polaire nord */
		*lambdac=lambda0;
		*n1=1;
		*phic=XPI2;
		*c=0;
		*n2=a*k0/sqrt(1-e2)*f;
		*xs=x0;
		*ys=y0;
		break;
		
	case 5:/*cas Polaire sud */
		*lambdac=lambda0;
		*n1=1;
		*phic=-XPI2;
		*c=0;
		*n2=a*k0/sqrt(1-e2)*f;
		*xs=x0;
		*ys=y0;
		break;
		
	default:
		break;
	}/*end switch*/
}

//////////////////////////////////////////////////////////////////////////////
/*alg0044---Transfo de Géographiques en projection Polyconique--------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoPolycon(double n, double xs, double ys, double e2, 
					   double lambdac, double lambda, double phi, 
					   double *x, double *y)
{
	double beta;
	double u,v;
	u = sin(phi) * (lambda-lambdac);
	v = tan(phi) * sqrt(1 - e2*sin(phi)*sin(phi));
	*x = xs + n*sin(u)/v;
	beta = DevArcMer(phi,e2);
	*y = ys + n*(1-cos(u))/v + n*beta;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0045-Transfo de coord en proj. Polyconique en géographiques------*/
//////////////////////////////////////////////////////////////////////////////

#define CALC_TP(X)	\
    t = sqrt(1 - e2*sin(X)*sin(X)) * tan(X);\
	p = (x-xs)*t;

void XAlgoGeod::PolyconGeo(double n, double xs, double ys, double e2, double lambdac, 
					   double *lambda, double *phi, double x, double y, double epsilon_phi)
{
	double phi0, phi1, beta, t, p;
	phi1 = (y-ys)/n;
	do {
		phi0 = phi1;
		CALC_TP(phi0);
		beta = (n - sqrt(n*n - p*p)) / t;
		beta = (y - ys - beta)/n;
		phi1 = LatArcMer(beta, e2, epsilon_phi);
    } while(fabs(phi1-phi0) > epsilon_phi);
	*phi = phi1;
	CALC_TP(*phi);
	*lambda = lambdac + atan(p /sqrt(n*n - p*p)) / sin(*phi);
}

//////////////////////////////////////////////////////////////////////////////
/*alg0046-Calcul des coefficients de la projection Gauss-Laborde------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjGaussLab(TYP_PROJ choix, double a, double e2, double lambda0, 
							 double phi0, double k0, double x0, double y0, 
							 double *lambdac, double *phic, double *c, double *n1, double *n2, 
							 double *xs, double *ys, double epsilon_phi)
{
	switch (choix)
    {
    case BITAN:
		*lambdac = lambda0;
		*n1 = 1;
		*phic = phi0;
		*c = LatIso(*phic,0) - LatIso(phi0,e2);
		*n2 = k0 * a / sqrt(1-e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0;
		break;
    case EQUA:
		*lambdac = lambda0;
		*n1 = 1;
		*phic = LatIsoInv(LatIso(phi0,e2),0, epsilon_phi);
		*c = 0;
		*n2 = k0 * a * cos(phi0)/(sqrt(1-e2*sin(phi0)*sin(phi0))*cos(*phic));
		*xs = x0;
		*ys = y0;
		break;
    case COURB:
		*lambdac = lambda0;
		*n1 = sqrt(1 + e2*pow(cos(phi0),4)/(1-e2));
		*phic = asin(sin(phi0)/ *n1);
		*c = LatIso(*phic,0) - *n1 * LatIso(phi0,e2);
		*n2 = k0 * a * sqrt(1-e2)/(1-e2*sin(phi0)*sin(phi0));
		*xs = x0;
		*ys = y0 - *n2 * *phic;
		break;
    default:
		/*fprintf(stderr, "Gauss-Laborde type %d: not implemented\n", choix)*/;
		break;
    }
	
}

//////////////////////////////////////////////////////////////////////////////
/*alg0047--Calcul des coefficients de la projection polyconique--------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjPolycon(double a, double e2, double lambda0, double phi0, 
							double k0, double x0, double y0, double *n, 
							double *lambdac, double *xs, double *ys)
{
	double beta, coef[5];
	*lambdac = lambda0;
	*n = k0*a;
	*xs = x0;
	CoeffArcMer(e2, coef);
	beta = DevArcMer(phi0,e2);
	*ys = y0 - *n * beta;
	
}
//////////////////////////////////////////////////////////////////////////////
/*alg0048---Transfo de géographiques en projection de Bonne----------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoBonne(double n, double xs, double ys, double e2, double c,
					 double lambdac, double lambda, double phi,
					 double *x, double *y)
{
	double beta, r, gamma;
	beta = DevArcMer(phi,e2);
	r = c - n*beta;
	gamma = n * cos(phi) * (lambda-lambdac) / (r*sqrt(1-e2*sin(phi)*sin(phi)));
	*x = xs + r*sin(gamma);
	*y = ys - r*cos(gamma);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0049-Transfo de projection de Bonne en G‚ographiques-----------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::BonneGeo(double n, double xs, double ys, double e2, double c,
			  double lambdac, double epsilon_phi,
			  double *lambda, double *phi, double x, double y)
{
	double beta, coef[5], d;
	d = hypot(x-xs, y-ys);
	beta = (c + (c<0 ? d : -d)) / n;
	CoeffArcMer(e2, coef);
	*phi = LatArcMer(beta,e2, epsilon_phi);
	*lambda = lambdac -
		sqrt(1-e2*sin(*phi)*sin(*phi))
		* atan((x-xs)/(y-ys))
		* d
		/ (n*cos(*phi));
}
//////////////////////////////////////////////////////////////////////////////
/*alg0050---Calcul des coefficients-de la projection de Laborde Madagascar */
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjLabMad(double a,double e2,double lambda0,double phi0,double k0,
					double x0,double y0,double az0,double *azc,double *lambdac,double *phic,double *c,
					double *n1,double *n2,double *xs,double *ys)
{
	*lambdac=lambda0;
	*n1=sqrt(1+e2*cos(phi0)*cos(phi0)*cos(phi0)*cos(phi0)/(1-e2));
	*n2=k0*a*sqrt(1-e2)/(1-e2*sin(phi0)*sin(phi0));
	*phic=asin(sin(phi0)/(*n1));
	*c =LatIso(*phic,0)-(*n1*LatIso(phi0,e2));
	*azc=az0;
	*xs=x0;
	*ys=y0;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0051-Calcul des coefficients de la projection de Bonne-----------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjBonne(double a, double e2, double lambda0, double phi0,
				   double k0, double x0, double y0, double *lambdac,double *c,
				   double *n, double *xs,double *ys)
{
	double beta, dy;
	beta = DevArcMer(phi0,e2);
	*n = k0*a;
	dy = *n / (tan(phi0)*sqrt(1-e2*sin(phi0)*sin(phi0)));
	*c = dy + *n * beta;
	*lambdac = lambda0;
	*xs = x0;
	*ys = y0 + dy;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0052-Calcul des coefficients de la projection de Mercator Transverse--*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjMercTr(double a, double e2, double lambda0, double phi0,
					double k0, double x0, double y0, double *lambdac, double *n,
					double *xs, double *ys)
{
	double beta, coef[5];
	CoeffArcMer(e2, coef);
	beta = DevArcMer(phi0, e2);
	*lambdac = lambda0;
	*n = k0*a;
	*xs = x0;
	*ys = y0 - *n * beta;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0053-Calcul des coefficients de la projection ------------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjMerc(double a, double e2, double lambda0, double phi0, double k0,
				  double x0, double y0, double *n, double *xs, double *ys)
{
	*n = k0*cos(phi0)*a/sqrt(1-e2*sin(phi0)*sin(phi0));
	*xs = x0 - *n * lambda0;
	*ys = y0 - *n * LatIso(phi0,e2);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0054   Calcul de constantes de la projection Lambert Sécante---------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjLambSec(double a, double e2, double lambda0, double phi0,
					 double phi1, double phi2, double x0, double y0,
					 double *lambdac, double *n, double *c, double *xs, double *ys)
{
	double n1, n2, l1, l2;
	n1 = GrandeNormale(phi1, a, e2) * cos(phi1);
	n2 = GrandeNormale(phi2, a, e2) * cos(phi2);
	l1 = LatIso(phi1, e2);
	l2 = LatIso(phi2, e2);
	*lambdac = lambda0;
	*n = log(n2/n1) / (l1-l2);
	*c = n1 * exp(*n*l1) / *n;
	*xs = x0;
	if ( fabs(phi0-XPI2)< 1e-009 )
		*ys = y0;
	else
		*ys = y0 + *c * exp(- *n * LatIso(phi0, e2));
}
//////////////////////////////////////////////////////////////////////////////
/*alg0055--Calcul des paramètres de la projection Lambert Sécante---------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CoefProjLambSecParam(double a, double e2, double n, double lambda0,double y0 ,
							double xs, double ys, double c, double epsilon_phi,
							double *lambdac, double *phi0, double *phi1, double *phi2,
							double *x0)
{
	double phit , rt , wt , mt , Dphi, phi , r , y;
	double val;
	*lambdac = lambda0;
	*x0 = xs;
	
	if ( y0 == ys )
		*phi0 = XPI2;
	else
		*phi0 = LatIsoInv( log(c/(ys -y0))/n , e2, epsilon_phi);
	phit = asin(n);
	val =  e2*sin(phit)*sin(phit);
	wt = sqrt( 1.0 - val );
	rt = a * cos(phit) / wt;
	mt =  n * c * exp(-n*LatIso(phit , e2) )/rt;
	Dphi = sqrt( 2.0*(1.0-mt)*wt*wt /( mt* (1.0-e2) ));

	*phi1 = phit - Dphi;
	do	{
		phi = *phi1 ;
		r = a * cos(phi)/ sqrt(1-e2*sin(phi)*sin(phi));
		y = r * exp(n * LatIso(phi , e2));
		*phi1 = phi + ( (n*c - y)*(1 - e2*sin(phi)*sin(phi))*cos(phi) 
			          / ((1-e2)*(n-sin(phi))*n*c) );
	}while(fabs(*phi1 -phi) > epsilon_phi);
	
	*phi2 =  phit + Dphi ; 
	do	{
		phi = *phi2 ;
		r = a * cos(phi)/ sqrt(1-e2*sin(phi)*sin(phi));
		y = r * exp(n * LatIso(phi , e2));
		*phi2 = phi + ( (n*c - y)*(1 - e2*sin(phi)*sin(phi))*cos(phi) 
			          / ((1-e2)*(n-sin(phi))*n*c) );
	}while(fabs(*phi2 -phi) > epsilon_phi);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0056 transfo cart‚s. en g‚ogr. m‚thode non it‚rative de BOWRING */
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartGeo56(double x, double y, double z, double a, double e2,
			   double *lambda, double *phi, double *h)
{
	double p, r, mu;
	p = sqrt(x*x + y*y);
	r = sqrt(x*x + y*y + z*z);
	mu = atan(z/p * (sqrt(1-e2) + a*e2/r));
	*phi = atan((z*sqrt(1-e2) + a*e2*pow(sin(mu),3))/
		(sqrt(1-e2)*(p - a*e2*pow(cos(mu),3))));
	*h = p*cos(*phi) + z*sin(*phi) - a*sqrt(1-e2*sin(*phi)*sin(*phi));
	*lambda = 2*atan(y/(x+p));
}
//////////////////////////////////////////////////////////////////////////////
/*alg0057 passage des paramètres du Lambert sécant au Lambert Tangent-------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::ConstLambTgt(double a, double e2, double phi1, double phi2,
				  double *phit, double *k0, double *n, double *c)
{
	double n1, n2, l1, l2;
	n1 = GrandeNormale(phi1,a,e2) * cos(phi1);
	n2 = GrandeNormale(phi2,a,e2) * cos(phi2);
	l1 = LatIso(phi1,e2);
	l2 = LatIso(phi2,e2);
	*n = log(n2/n1) / (l1-l2);
	*c = n1 * exp(*n*l1) / *n;
	*phit = asin(*n);
	*k0 = *c * tan(*phit) / (GrandeNormale(*phit,a,e2) * exp(*n*LatIso(*phit,e2)));
}
//////////////////////////////////////////////////////////////////////////////
/*alg0058 passage du Lambert tangent au Lambert sécant----------------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::ConstLambSec(double a,double e2,double phit,double k0,
						 double *phi1,double *phi2, double *n, double *c,
						 double epsilon_phi)
{
	double w,m,dm,lambda0,x0,y0,lambdac,xs,ys,phia,phib;
	int i;
	
	x0 = y0 = lambda0=0;
	CoefProjLambTan(a,e2,lambda0,phit,k0,x0,y0,&lambdac,n,c,&xs,&ys);
	
	phia = 100.;
	phib = phit - 0.1;
	for( i = 0 ; fabs(phib-phia)>epsilon_phi ; i++)
	{
		phia = phib;
		w = sqrt(1-e2*sin(phia)*sin(phia));
		m = *n * *c * exp(-*n*LatIso(phia,e2))*w/(a*cos(phia));
		dm = m * (sin(phia) - *n) / (w*w*cos(phia));
		phib = phia + (1-m)/dm;
	}
	*phi1 = phib;
	
	phia = 100.;
	phib = phit + 0.1;
	for( i = 0 ; fabs(phib-phia)>epsilon_phi ;  i++)
	{
		phia = phib;
		w = sqrt(1-e2*sin(phia)*sin(phia));
		m = *n * *c * exp(- *n*LatIso(phia,e2))*w/(a*cos(phia));
		dm = m * (sin(phia) - *n)/(w*w*cos(phia));
		phib = phia + (1-m)/dm;
	}
	*phi2=phib;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0059 Calcul module linéaire et convergence pour stéréographique oblique*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::ModConvSterObl ( double a, double lambda, double phi, double e2, double c, double n1,
					 double n2,double lambdac,double phic,
					 double *conv, double *modlin, double *alter, double epsilon_phi)
					 
{
	double LAMBDA,LS,PHI,ux,uy,uz,vx,vy,vz,dux,duy,duz,dvx,dvy,dvz;
	
	LS = c+n1*LatIso(phi,e2);
	PHI = LatIsoInv (LS,0, epsilon_phi);
	LAMBDA = n1*(lambda-lambdac);
	SpherCart(LAMBDA,PHI,&ux,&uy,&uz);
	RotAxe ('y',XPI2-phic,ux,uy,uz,&vx,&vy,&vz);
	dux = -ux*uz/cos(PHI);
	duy = -uy*uz/cos(PHI);
	duz = cos(PHI);
	RotAxe ('y',XPI2-phic,dux,duy,duz,&dvx,&dvy,&dvz);
	*conv = atan((dvy*(1+vz)-dvz*vy)/(-dvx*(1+vz)+dvz*vx));
	*modlin = (n1*cos(PHI)*sqrt(1-e2*sin(phi)*sin(phi))*2*n2 )/( cos(phi)*a *(1+vz) );
	*alter = (*modlin-1)*1e5;
}
//////////////////////////////////////////////////////////////////////////////
/*alg0060 Calcul module linéaire et convergence pour lambert ---------------*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::ModConvLamb(double lambda, double phi, double a, double e2, 
						double n, double c, double lambdac,  double *conv, 
						double *modlin, double *K, double *alter )
{
	double r = GrandeNormale(phi,a,e2)* cos(phi);
	
	*conv = (lambdac - lambda)*n;
	*modlin = n*c*exp(-n*LatIso(phi,e2)) / r ;
	*alter = (*modlin -1)*1e5;
	*K = (sin(phi)-n)/(*modlin*2*r);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0061 Calcul module linéaire et convergence pour Transverse Mercator ---*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::ModConvMercTr(double lambda, double phi, double e2,
				   double k0, double lambdac, double *conv, double *modlin)
{
	/* méthode utilisée à l'université du New Brunswick (Canada) */
	
	double dlamb = lambdac-lambda;
	double eta2 = e2*cos(phi)*cos(phi)/(1-e2);
	double dl2 = dlamb*cos(phi);
	double t2 = tan(phi)*tan(phi);
	
	dl2 *= dl2;
	
	/* approximation pour fuseau étroit */
	*conv = dlamb * sin(phi) * (
		1 +
		dl2 *     (1 + 3*eta2 + 2*eta2*eta2)/3 +
		dl2*dl2 * (2 - t2)/15
		);
	
	*modlin = k0 * (
		1 +
		dl2     * (1 + eta2)/2 +
		dl2*dl2 * (5 - 4*t2)/24
		);
}
//////////////////////////////////////////////////////////////////////////////
/* Alg0062----Transfo. coordonnées cartésiennes en locales--------------- */
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::CartLoc  (double x0, double y0, double z0,
			   double x1, double y1, double z1,
			   double a, double e2,
			   double *x2, double *y2, double *z2)
{
	double lon, lat, h;
	
	CartGeo56(x0, y0, z0, a, e2, &lon, &lat, &h);
	*x2=         -sin(lon)*(x1-x0)          +cos(lon)*(y1-y0)                  ;
	*y2=-sin(lat)*cos(lon)*(x1-x0) -sin(lat)*sin(lon)*(y1-y0) +cos(lat)*(z1-z0);
	*z2= cos(lat)*cos(lon)*(x1-x0) +cos(lat)*sin(lon)*(y1-y0) +sin(lat)*(z1-z0);
}
//////////////////////////////////////////////////////////////////////////////
/*alg0063--Transfo inverse à 7 paramètres (choisi -> référence)-------------*/
//////////////////////////////////////////////////////////////////////////////

/* On peut décommenter le #define ci-dessous pour se contenter de la formule
   approchée au 1er ordre (en d,rx,ry,rz, au voisinage de 0) */
/* #define ORDRE_1 */
void XAlgoGeod::Tr7ParCR (double tx, double ty, double tz, double d,
	       double rx, double ry, double rz,
	       double  x1, double  y1, double  z1,
	       double *x2, double *y2, double *z2)
  {
  /* emploi de var. interm‚diaires pour autoriser x2 à pointer sur x1, etc... */
  double x,y,z;
#ifndef ORDRE_1
  double e, denom;
#endif

  x1 -= tx;		/* on applique d'abord la translation */
  y1 -= ty;
  z1 -= tz;

  /* il s'agit ensuite d'inverser la matrice A telle que AX = (1+d)X + R^X :
	(1+d -rz  ry)
	(rz	 1+d -rx)
	(-ry rx	 1+d)
  */

#ifdef ORDRE_1
  x = + (1-d)* x1 + rz   * y1 - ry   * z1;
  y = - rz   * x1 + (1-d)* y1 + rx   * z1;
  z = + ry   * x1 - rx   * y1 + (1-d)* z1;
#else
  e = 1+d;
  denom = e * (e*e + rx*rx + ry*ry + rz*rz);	/* proche de 1 donc non nul */
  x = (( e*e  + rx*rx) * x1 + ( e*rz + rx*ry) * y1 + (-e*ry + rx*rz) * z1)/denom;
  y = ((-e*rz + ry*rx) * x1 + ( e*e  + ry*ry) * y1 + ( e*rx + ry*rz) * z1)/denom;
  z = (( e*ry + rz*rx) * x1 + (-e*rx + rz*ry) * y1 + ( e*e  + rz*rz) * z1)/denom;
#endif

  *x2 = x;
  *y2 = y;
  *z2 = z;
  }

//////////////////////////////////////////////////////////////////////////////
/*alg0069--(ex 38bis)Transfo géo->planes Stéréographique Polaire Sud Sécante*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::GeoSterPolSudSec(double e2, double lambdac, double n2, double Xs, 
							 double Ys, double lon, double lat, double epsilon, 
							 double *X,  double *Y)
{
	double LAMBDA,PHI;

	LAMBDA	= lon - lambdac;
	PHI		= LatIsoInv(LatIso(lat,e2),0,epsilon);
	*X		= Xs + 2*n2*( cos(PHI)*sin(LAMBDA)/(1-sin(PHI)) );
	*Y		= Ys - 2*n2*( cos(PHI)*cos(LAMBDA)/(sin(PHI)-1) );
}
//////////////////////////////////////////////////////////////////////////////
/*alg0070--(ex 39bis)Transfo planes Stéréographique Polaire Sud Sécante->géo*/
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::SterPolSudSecGeo(double e2, double lambdac, double n2, double Xs, 
							 double Ys, double X, double Y, double epsilon, 
							 double *lon,  double *lat)
{
	double r, LAMBDA1, PHI1, LAMBDA, PHI, rho;

	r = sqrt( (X-Xs)*(X-Xs) + (Y-Ys)*(Y-Ys) );
	if ( fabs(r) < epsilon )
		LAMBDA1 = 0;
	else 
		LAMBDA1 = 2*atan((X-Xs)/(r-(Y-Ys)));
	PHI1 = XPI2 - 2*atan(r/(2*n2));
	rho = sqrt( cos(PHI1)*cos(LAMBDA1)*cos(PHI1)*cos(LAMBDA1) 
		      + cos(PHI1)*sin(LAMBDA1)*cos(PHI1)*sin(LAMBDA1) );
	if ( fabs(rho) < epsilon ){
		LAMBDA = 0;
		if ( (-1.0*sin(PHI1)) < 0 )
			PHI = -1.0*XPI2;
		else
			PHI = XPI2;
	}
	else{
		LAMBDA = 2*atan( cos(PHI1)*sin(LAMBDA1) 
			             /(rho-cos(PHI1)*cos(LAMBDA1)) );
		PHI    = atan(-1.0*sin(PHI1)/rho);
	}
	*lon = lambdac + LAMBDA;
	*lat = LatIsoInv(LatIso(PHI,0),e2,epsilon);
}

//MODIF SB 31/01/2012 : Lambert azimuthal equal-area
//////////////////////////////////////////////////////////////////////////////
/*alg0072 Transformation de coordonnées géographiques en coordonnées planes */
/*dans la projection "Lambert Azimuthal Equal Area".                        */
//////////////////////////////////////////////////////////////////////////////
double XAlgoGeod::q_(double e2, double phi)
{
  double e = sqrt(e2);
  double sp = sin(phi);
  double esp = e*sp;
  double mesp = 1 - esp;
  double pesp = 1 + esp;
  return (1-e2) * ( sp/(1-pow(esp,2)) - log(mesp/pesp)/2./e );
}
double XAlgoGeod::m_(double e2, double phi)
{
  return cos(phi) / sqrt(1-e2*pow(sin(phi),2));
}
void XAlgoGeod::CoefProjLAEA(double a, double e2, double phi0,
         double *qp, double *beta1, double *Rq, double *D)
{
  double e = sqrt(e2);
  double mesp = 1 - e;
  double pesp = 1 + e;
  double m1 = m_(e2, phi0);
  *qp = (1-e2) * ( 1/(1-e2) - log(mesp/pesp)/2./e );
  *beta1 = asin(q_(e2, phi0) / (*qp));
  *Rq = a*sqrt((*qp)/2);
  *D = a*m1/(*Rq)/cos(*beta1);
}
void XAlgoGeod::GeoLAEA(double a, double e2, double lambda0, double phi0, double qp,
       double Rq, double beta1, double D, double x0, double y0,
       double lambda, double phi, double *x, double *y)
{
  double rho, beta, B;
  if (phi0 == M_PI_2)
  {
    rho = a*sqrt(qp - q_(e2, phi));
    *x = x0 + rho*sin(lambda-lambda0);
    *y = y0 - rho*cos(lambda-lambda0);
  }
  else if (phi0 == -M_PI_2)
  {
    rho = a*sqrt(qp + q_(e2, phi));
    *x = x0 + rho*sin(lambda-lambda0);
    *y = y0 + rho*cos(lambda-lambda0);
  }
  else
  {
    beta = asin(q_(e2, phi) / qp);
    B = Rq * sqrt(2/ (1+ sin(beta1)*sin(beta)
                  + cos(beta1)*cos(beta)*cos(lambda-lambda0)));
    *x = x0 + B*D*cos(beta)*sin(lambda-lambda0);
    *y = y0 + B/D*(cos(beta1)*sin(beta)
             -sin(beta1)*cos(beta)*cos(lambda-lambda0));
  }
}

//MODIF SB 31/01/2012 : Lambert azimuthal equal-area
//////////////////////////////////////////////////////////////////////////////
/*alg0073 Transformation de coordonnées planes dans la projection           */
/*"Lambert Azimutal Equal Area", en coordonnées géographiques.              */
//////////////////////////////////////////////////////////////////////////////
void XAlgoGeod::LAEAgeo(double a, double e2, double lambda0, double phi0, double qp,
       double Rq,	double beta1, double D, double x0, double y0,
       double *lambda, double *phi, double x, double y)
{
  double epsilon_phi = 1e-12 ;
  double rho, ce, q, phiI, phiF;
  x -= x0;
  y -= y0;
  if (fabs(phi0) == M_PI_2)
  {
    rho = sqrt(pow(x,2)+pow(y,2));
    if (phi0 == -M_PI_2)
    {
      *lambda = lambda0 + atan2(x,y);
      q = -qp + (pow(rho/a,2));
    }
    else
    {
      *lambda = lambda0 + atan2(x,-y);
      q = qp - (pow(rho/a,2));
    }
    phiF = asin(q/2.);
    if (fabs(q) == fabs(qp))
    {
      *phi = M_PI_2*q/fabs(q);
    }
    else
    {
      do
      {
        phiI = phiF;
        phiF = phiI + pow((1-e2*pow(sin(phiI),2)), 2)/2./cos(phiI)/(1-e2)*(q-q_(e2, phiI));
      }
      while (abs(phiF-phiI) > epsilon_phi);
      *phi = phiF;
    }
  }
  else
  {
    rho = sqrt(pow(x/D,2)+pow(D*y,2));
    if (rho == 0)
    {
      *lambda = lambda0;
      *phi = phi0;
    }
    else
    {
      ce = 2*asin(rho/2./Rq);
      q = qp*(cos(ce)*sin(beta1) + D*y*sin(ce)*cos(beta1)/rho);
      if (fabs(q) == fabs(qp))
      {
        *phi = M_PI_2*q/fabs(q);
      }
      else
      {
        *lambda = lambda0 + atan2( x*sin(ce),
          (D*rho*cos(beta1)*cos(ce) - pow(D,2)*y*sin(beta1)*sin(ce) ));
        phiF = asin(q/2.);
        do
        {
          phiI = phiF;
          phiF = phiI + pow((1-e2*pow(sin(phiI),2)), 2)/2./cos(phiI)/(1-e2)*(q-q_(e2, phiI));
        }
        while (fabs(phiF-phiI) > epsilon_phi);
        *phi = phiF;
      }
    }
  }
}
