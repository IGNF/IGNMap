//-----------------------------------------------------------------------------
//								XLambert.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/03/2003
//-----------------------------------------------------------------------------

#include "XLambert.h"
#include "XAlgoGeod.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XLambert::XLambert()
{
	// GRS 80
	GRS80_a   = 6378137.0000;
	GRS80_e2  = 0.006694380036;

	// Clarke 1880
  Clark_a   = 6378249.2000;
  Clark_e2  = 0.006803487646;

	// Parametres de transformation standard
  Tx  = -168;
  Ty  = -60;
  Tz  = 320;
  D   = 0.;
  Rx  = 0.;
  Ry  = 0.;
  Rz  = 0.;

	//Lambert 2 Etendu
	Lamb2E_x0     =  600000;	// Coordonnees de l'origine
	Lamb2E_y0     = 2200000;
	Lamb2E_lambda0	= XAlgoGeod::Pi() *(2.33722916667)/180.; //méridien de Paris
	Lamb2E_phi0			= XAlgoGeod::Pi()*(52./200.);
	Lamb2E_k0				= 0.99987742;		// Facteur d'echelle

	XAlgoGeod::CoefProjLambTan(Clark_a, Clark_e2, Lamb2E_lambda0, Lamb2E_phi0,
														 Lamb2E_k0, Lamb2E_x0, Lamb2E_y0,
														 &Lamb2E_lambdac, &Lamb2E_n, &Lamb2E_c, &Lamb2E_xs, &Lamb2E_ys);


	m_Proj93 = RGF93;
	SetParametersGRS(Lambert93);

  // LAEA
  Laea_x0 = 4321000.;
  Laea_y0 = 3210000.;
  Laea_lambda0 = (XAlgoGeod::Pi() * 10.) / 180.;  // Longitude origine
  Laea_phi0 = (XAlgoGeod::Pi() * 52.) / 180.;     // Latitude origine
  XAlgoGeod::CoefProjLAEA(GRS80_a, GRS80_e2, Laea_phi0, &Laea_qp, &Laea_beta1, &Laea_Rq, &Laea_D);

  // UTM
  m_ProjUTM = RGF93;
  SetParametersUTM(ETRS89TM31);

	m_Grid = NULL;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XLambert::~XLambert()
{
	if (m_Grid != NULL)
		delete[] m_Grid;
	m_Grid = NULL;
}

//-----------------------------------------------------------------------------
// Conversion Lambert2E -> Lambert 93
//-----------------------------------------------------------------------------
void XLambert::Convert2ETo93(double x2e, double y2e, double* x93, double* y93, double Z, bool rgf93)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;

	// Transformation Lambert -> Geographique
	XAlgoGeod::LambertGeo(Clark_e2, Lamb2E_n, Lamb2E_c, Lamb2E_lambdac, epsilon_phi, 
												Lamb2E_xs, Lamb2E_ys, &lambda, &phi, 
												x2e, y2e);

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(Clark_a, Clark_e2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParRc(Tx, Ty, Tz, D, 
											Rx, Ry, Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 GRS80_a, GRS80_e2, epsilon_phi, 
											 &lambda, &phi, &h);
	
	if (m_Grid != NULL) {
		double tx = Tx, ty = Ty, tz = Tz;
		if (Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz)) {
			XAlgoGeod::Tr7ParRc(tx, ty, tz, D, 
													Rx, Ry, Rz, 
													x1, y1, z1, 
													&x2, &y2, &z2);
			XAlgoGeod::CartGeo12(x2, y2, z2,
														GRS80_a, GRS80_e2, epsilon_phi, 
														&lambda, &phi, &h);
		}
	}

	// Transformation Geographique -> Lambert
	if (!rgf93)
		XAlgoGeod::GeoLambert(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, Lamb93_xs, 
													Lamb93_ys, lambda, phi,
													x93, y93);
	else {
		*x93 = lambda;
		*y93 = phi;
	}
}

//-----------------------------------------------------------------------------
// Conversion Lambert93 -> Lambert 2E
//-----------------------------------------------------------------------------
void XLambert::Convert93To2E(double x93, double y93, double* x2e, double* y2e, double Z, bool rgf93)
{
	double lambda, phi, h = Z;
	double epsilon_phi = 0.000000001;

	// Transformation Lambert -> Geographique
	if (!rgf93)
		XAlgoGeod::LambertGeo(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, epsilon_phi, 
													Lamb93_xs, Lamb93_ys, &lambda, &phi, 
													x93, y93);
	else {
		lambda = x93;
		phi = y93;
	}

	// Transformation Geographique -> Cartesienne
	double x1, y1, z1;
	XAlgoGeod::GeoCart(GRS80_a, GRS80_e2,
										 lambda, phi, h,
										 &x1, &y1, &z1);

	double tx = Tx, ty = Ty, tz = Tz;
	if (m_Grid != NULL) 
		Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz);

	// Changement de systeme geodesique
	double x2, y2, z2;
	XAlgoGeod::Tr7ParRc(-tx, -ty, -tz, D, 
											-Rx, -Ry, -Rz, 
											x1, y1, z1, 
											&x2, &y2, &z2);

	// Transformation Cartesienne -> Geographique
	XAlgoGeod::CartGeo12(x2, y2, z2,
											 Clark_a, Clark_e2, epsilon_phi, 
											 &lambda, &phi, &h);

	// Transformation Geographique -> Lambert
	XAlgoGeod::GeoLambert(Clark_e2, Lamb2E_n, Lamb2E_c, Lamb2E_lambdac, Lamb2E_xs, 
												Lamb2E_ys, lambda, phi,
												x2e, y2e);
}

//-----------------------------------------------------------------------------
// Interpolation sur la grille
//-----------------------------------------------------------------------------
bool XLambert::Interpol(double lambda, double phi, double* tx, double* ty, double* tz)
{
	if (m_Grid == NULL)
		return false;

	if (lambda < m_dLongMin)
		return false;
	if (lambda > m_dLongMax)
		return false;
	if (phi < m_dLatMin)
		return false;
	if (phi > m_dLatMax)
		return false;

	uint32_t nb_long = (lambda - m_dLongMin) / m_dLongStep;
	uint32_t nb_lat = (phi - m_dLatMin) /  m_dLatStep;
	double x = (lambda - (nb_long * m_dLongStep + m_dLongMin)) / m_dLongStep;
	double y = (phi - (nb_lat * m_dLatStep + m_dLatMin)) / m_dLatStep;

  XPt3D T1(m_Grid[nb_long * m_nNbLat + nb_lat]);
  XPt3D T2(m_Grid[nb_long * m_nNbLat + nb_lat + 1]);
  XPt3D T3(m_Grid[(nb_long + 1) * m_nNbLat + nb_lat]);
  XPt3D T4(m_Grid[(nb_long + 1) * m_nNbLat + nb_lat + 1]);

	XPt3D TM = (1 - x)*(1 - y)*T1 + (1 - x) * y * T2 + x * (1 - y)*T3 + x * y * T4;
	*tx = TM.X;
	*ty = TM.Y;
	*tz = TM.Z;

	return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une grille
//-----------------------------------------------------------------------------
bool XLambert::LoadGrid(const char* filename, XError* error)
{
	std::ifstream in;
	in.open(filename);
	if (!in.good())
		return XErrorError(error, "XLambert::LoadGrid", XError::eIOOpen);

	std::string token;
	char buf[1024];
	// Premiere ligne d'entete
	in >> token;
	if (token.compare("GR3D") != 0)
		return XErrorError(error, "XLambert::LoadGrid", XError::eBadFormat);
	in.getline(buf, 1023);
	// Deuxieme ligne d'entete
	in >> token;
	if (token.compare("GR3D1") != 0)
		return XErrorError(error, "XLambert::LoadGrid", XError::eBadFormat);
	in >> m_dLongMin >> m_dLongMax >> m_dLatMin >> m_dLatMax >> m_dLongStep >> m_dLatStep;
	in >> token;
	// Trosieme ligne d'entete
	if (token.compare("GR3D2") != 0)
		return XErrorError(error, "XLambert::LoadGrid", XError::eBadFormat);
	in.getline(buf, 1023);
	// Quatrieme ligne d'entete
	in >> token;
	if (token.compare("GR3D3") != 0)
		return XErrorError(error, "XLambert::LoadGrid", XError::eBadFormat);
	in.getline(buf, 1023);

	m_nNbLong = XRint((m_dLongMax - m_dLongMin) / m_dLongStep) + 1;
	m_nNbLat = XRint((m_dLatMax - m_dLatMin) / m_dLatStep) + 1;
	uint32_t nb_lig = m_nNbLong * m_nNbLat;
	if (m_Grid != NULL)
		delete[] m_Grid;
  m_Grid = new XPt3[nb_lig];
	if (m_Grid == NULL)
		return XErrorError(error, "XLambert::LoadGrid", XError::eAllocation);

	// Chargement de la grille
	double longitude, latitude, tx, ty, tz;
	int n, precision;
	std::string feuille;
	uint32_t pos;
	for(uint32_t i = 0; i < nb_lig; i++) {
		in >> n >> longitude >> latitude >> tx >> ty >> tz >> precision >> feuille; 
		pos = XRint((longitude - m_dLongMin) / m_dLongStep) * m_nNbLat + XRint((latitude - m_dLatMin) / m_dLatStep);
    m_Grid[pos].X = tx;
    m_Grid[pos].Y = ty;
    m_Grid[pos].Z = tz;
  }
	if (!in.good()) {
		delete[] m_Grid;
		m_Grid = NULL;
		return XErrorError(error, "XLambert::LoadGrid", XError::eBadData);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'une grille binaire
//-----------------------------------------------------------------------------
bool XLambert::WriteGrid(const char* filename, XError* error)
{
	std::ofstream out;
	out.open(filename, std::ios::out | std::ios::binary);
	if (!out.good())
		return XErrorError(error, "XLambert::WriteGrid", XError::eIOOpen);
	out.write((char*)&m_dLatMin, sizeof(double));
	out.write((char*)&m_dLatMax, sizeof(double));
	out.write((char*)&m_dLongMin, sizeof(double));
	out.write((char*)&m_dLongMax, sizeof(double));
	out.write((char*)&m_dLatStep, sizeof(double));
	out.write((char*)&m_dLongStep, sizeof(double));
	out.write((char*)&m_nNbLat, sizeof(uint32_t));
	out.write((char*)&m_nNbLong, sizeof(uint32_t));

  out.write((char*)m_Grid, sizeof(XPt3) * m_nNbLat * m_nNbLong);
	return out.good();
}

//-----------------------------------------------------------------------------
// Lecture d'une grille binaire
//-----------------------------------------------------------------------------
bool XLambert::ReadGrid(const char* filename, XError* error)
{
	std::ifstream in;
	in.open(filename, std::ios::in | std::ios::binary);
	if (!in.good())
		return XErrorError(error, "XLambert::ReadGrid", XError::eIOOpen);
	in.read((char*)&m_dLatMin, sizeof(double));
	in.read((char*)&m_dLatMax, sizeof(double));
	in.read((char*)&m_dLongMin, sizeof(double));
	in.read((char*)&m_dLongMax, sizeof(double));
	in.read((char*)&m_dLatStep, sizeof(double));
	in.read((char*)&m_dLongStep, sizeof(double));
	in.read((char*)&m_nNbLat, sizeof(uint32_t));
	in.read((char*)&m_nNbLong, sizeof(uint32_t));

	// Allocation de la grille
	if (m_Grid != NULL)
		delete[] m_Grid;
  m_Grid = new XPt3[m_nNbLat * m_nNbLong];
	if (m_Grid == NULL)
		return XErrorError(error, "XLambert::ReadGrid", XError::eAllocation);

  in.read((char*)m_Grid, sizeof(XPt3) * m_nNbLat * m_nNbLong);
	return in.good();
}

//-----------------------------------------------------------------------------
// Conversion de coordonnees d'un systeme dans un autre
//-----------------------------------------------------------------------------
bool XLambert::Convert(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
											 double& Xf, double& Yf, double Z)
{
  if (start_proj == end_proj) {
    Xf = Xi;
    Yf = Yi;
    return true;
  }

	if ((start_proj == RGF93)||(end_proj == RGF93))
    return ConvertRGF93(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);

  // Gestion du Lambert LAEA
  if ((start_proj == ETRS89LAEA)||(end_proj == ETRS89LAEA))
    return ConvertLAEA(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);

  // Gestion de l'UTM
  if ((start_proj >= ETRS89TM30)||(end_proj >= ETRS89TM30))
    return ConvertUTM(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);

  // Conversion entre Lambert anciens
	if ((start_proj <= Lambert2E)&&(end_proj <= Lambert2E))
		return ConvertClarke(start_proj, end_proj, Xi, Yi, Xf, Yf);

  // Conversion Lambert 2 etendu vers Lambert RGF93
	if ((start_proj == Lambert2E)&&(end_proj > Lambert2E)) {
		SetParametersGRS(end_proj);
		Convert2ETo93(Xi, Yi, &Xf, &Yf, Z);
		return true;
	}

  // Conversion Lambert RGF93 vers Lambert 2 etendu
  if ((end_proj == Lambert2E)&&(start_proj > Lambert2E)) {
		SetParametersGRS(start_proj);
		Convert93To2E(Xi, Yi, &Xf, &Yf, Z);
		return true;
	}

	// Conversion entre Lambert93 et Lambert CC
	if ((end_proj >= Lambert93)&&(start_proj >= Lambert93))
		return ConvertGRS(start_proj, end_proj, Xi, Yi, Xf, Yf);

  // Conversion entre Lambert Zone et Lambert 93
	if ((start_proj < Lambert2E)&&(end_proj > Lambert2E)) {
		double x2e, y2e;
		SetParametersGRS(end_proj);
		ConvertClarke(start_proj, Lambert2E, Xi, Yi, x2e, y2e);
		Convert2ETo93(x2e, y2e, &Xf, &Yf, Z);
		return true;
	}

  // Conversion entre Lambert 93 et Lambert Zone
  if ((end_proj < Lambert2E)&&(start_proj > Lambert2E)) {
		double x2e, y2e;
		SetParametersGRS(start_proj);
		Convert93To2E(Xi, Yi, &x2e, &y2e, Z);
		ConvertClarke(Lambert2E, end_proj, x2e, y2e, Xf, Yf);
		return true;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Conversion Lambert Zone vers Lambert 2 etendu (ellipsoide de Clarke)
//-----------------------------------------------------------------------------
bool XLambert::ConvertClarke(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
														 double& Xf, double& Yf)
{
	if (start_proj == end_proj) {
		Xf = Xi;
		Yf = Yi;
		return true;
	}
	// Gestion du Lambert Carto en entree
	if (start_proj < Lambert2E) {
		if (Yi > 1e6)
			Yi = Yi - (double)start_proj * 1e6;
	}
	double gammaF, pound, RF;
	double nI, cI, xI, yI, nF, cF, xF, yF;
	SetParameterClarke(start_proj, nI, cI, xI, yI);
	SetParameterClarke(end_proj, nF, cF, xF, yF);

	gammaF = (nF / nI) * atan((xI - Xi)/(Yi - yI));
	pound = log(cI / sqrt((Xi - xI)*(Xi - xI) + 
							(Yi - yI)*(Yi - yI))) / nI;
	RF = cF * exp(-nF * pound);
	
	Xf = xF + RF * sin(gammaF);
	Yf = yF - RF * cos(gammaF);
	return true;
}

//-----------------------------------------------------------------------------
// Conversion entre Lambert 93 et Lambert CC
//-----------------------------------------------------------------------------
bool XLambert::ConvertGRS(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
														 double& Xf, double& Yf)
{
	double lambda, phi;
	double epsilon_phi = 0.000000001;

	// Transformation Lambert -> Geographique
	SetParametersGRS(start_proj);
	XAlgoGeod::LambertGeo(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, epsilon_phi, 
												Lamb93_xs, Lamb93_ys, &lambda, &phi, 
												Xi, Yi);

	// Transformation Geographique -> Lambert
	SetParametersGRS(end_proj);
	XAlgoGeod::GeoLambert(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, Lamb93_xs, 
												Lamb93_ys, lambda, phi,
												&Xf, &Yf);
	return true;
}

//-----------------------------------------------------------------------------
// Fixe les parametres pour les conversions entre Lambert sur l'ellipsoide de Clarke
//-----------------------------------------------------------------------------
bool XLambert::SetParameterClarke(XProjCode proj, double& N, double& C, double& X, double& Y)
{
	switch(proj) {
		case Lambert1:
			N = 0.7604059656;
			C = 11603796.98;
			X = 600000.000;
			Y = 5657616.674;
			return true;
		case Lambert2:
			N = 0.7289686274;
			C = 11745793.39;
			X = 600000.000;
			Y = 6199695.768;
			return true;
		case Lambert3:
			N = 0.6959127966;
			C = 11947992.52;
			X = 600000.000;
			Y = 6791905.085;
			return true;
		case Lambert4:
			N = 0.6712679322;
			C = 12136281.99;
			X = 234.358;
			Y = 7239161.542;
			return true;
		case Lambert2E:
			N = 0.7289686274;
			C = 11745793.39;
			X = 600000.000;
			Y = 8199695.768;
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Fixe les parametres de projection pour les projections sur le GRS 1980
//-----------------------------------------------------------------------------
bool XLambert::SetParametersGRS(XProjCode projection)
{
	if (projection == m_Proj93)
		return true;
	if (projection <= Lambert2E)
		return false;	
  if (projection > ETRS89LCC)
		return false;

	// Lambert93 et Lambert CC
	int index = (projection - LambertCC42);
	switch(projection) {
		case Lambert93:
			Lamb93_x0     =  700000;	// Coordonnees de l'origine
			Lamb93_y0     = 6600000;
			Lamb93_lambda0		= XAlgoGeod::Pi() *(3.)/180.; 
			Lamb93_phi0			= XAlgoGeod::Pi()*(46.5)/180.;    // Latitude origine         
			Lamb93_phi1			= XAlgoGeod::Pi()*(44.)/180.;			// Paralleles d'echelle conservee             
			Lamb93_phi2			= XAlgoGeod::Pi()*(49.)/180.;             
			break;
    case ETRS89LCC:
      Lamb93_x0     =  4000000;	// Coordonnees de l'origine
      Lamb93_y0     = 2800000;
      Lamb93_lambda0		= XAlgoGeod::Pi() *(10.)/180.;
      Lamb93_phi0			= XAlgoGeod::Pi()*(52)/180.;    // Latitude origine
      Lamb93_phi1			= XAlgoGeod::Pi()*(35.)/180.;			// Paralleles d'echelle conservee
      Lamb93_phi2			= XAlgoGeod::Pi()*(65.)/180.;
      break;
		default :
			Lamb93_x0     =  1700000.;	// Coordonnees de l'origine
			Lamb93_y0     = 1200000. + index * 1000000;
			Lamb93_lambda0		= XAlgoGeod::Pi() *(3.)/180.; 
			Lamb93_phi0			= XAlgoGeod::Pi()*(42 + index)/180.;    // Latitude origine         
			Lamb93_phi1			= XAlgoGeod::Pi()*(41.25 + index)/180.;			// Paralleles d'echelle conservee             
			Lamb93_phi2			= XAlgoGeod::Pi()*(42.75 + index)/180.;             
	}
	XAlgoGeod::CoefProjLambSec(GRS80_a, GRS80_e2, Lamb93_lambda0, Lamb93_phi0,
														 Lamb93_phi1, Lamb93_phi2, Lamb93_x0, Lamb93_y0,
														 &Lamb93_lambdac, &Lamb93_n, &Lamb93_c, &Lamb93_xs, &Lamb93_ys);
	m_Proj93 = projection;
	return true;
}


//-----------------------------------------------------------------------------
// Conversion en coordonnees geographiques RGF93 (lambda : X ; phi : Y)
//-----------------------------------------------------------------------------
bool XLambert::ConvertRGF93(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
											 double& Xf, double& Yf, double Z)
{
	if (start_proj == end_proj) {
		Xf = Xi;
		Yf = Yi;
		return true;
	}
  double epsilon_phi = 0.000000001;

  // RGF93 vers LAEA
  if ((start_proj == RGF93)&&(end_proj == ETRS89LAEA)) {
    XAlgoGeod::GeoLAEA(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                       Laea_qp, Laea_Rq, Laea_beta1, Laea_D,
                       Laea_x0, Laea_y0, Xi, Yi, &Xf, &Yf);
    return true;
  }
  // LAEA vers RGF93
  if ((start_proj == ETRS89LAEA)&&(end_proj == RGF93)) {
    XAlgoGeod::LAEAgeo(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                       Laea_qp, Laea_Rq,	Laea_beta1, Laea_D,
                       Laea_x0, Laea_y0, &Xf, &Yf, Xi, Yi);
    return true;
  }

  // RGF93 vers UTM
  if ((start_proj == RGF93)&&(end_proj >= ETRS89TM30)&&(end_proj <= ETRS89TM32)) {
    SetParametersUTM(end_proj);
    XAlgoGeod::GeoMercTr(Utm_lambda0, Utm_n, Utm_xs, Utm_ys,
                         GRS80_e2, Xi, Yi, &Xf, &Yf);
    return true;
  }
  // UTM vers RGF93
  if ((start_proj >= ETRS89TM30)&&(start_proj <= ETRS89TM32)&&(end_proj == RGF93)) {
    SetParametersUTM(start_proj);
    XAlgoGeod::MercTrGeo(Utm_lambda0, Utm_n, Utm_xs, Utm_ys, GRS80_e2,
                        &Xf, &Yf, Xi, Yi, epsilon_phi);
    return true;
  }

  // Lambert RGF93 vers RGF93
	if ((start_proj >= Lambert93)&&(end_proj == RGF93)) {
    SetParametersGRS(start_proj);
    XAlgoGeod::LambertGeo(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, epsilon_phi,
												Lamb93_xs, Lamb93_ys, &Xf, &Yf, 
												Xi, Yi);
		return true;
	}

  // RGF93 vers lambert RGF93
	if ((end_proj >= Lambert93)&&(start_proj == RGF93)) {
		// Transformation Geographique -> Lambert
		SetParametersGRS(end_proj);
		XAlgoGeod::GeoLambert(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, Lamb93_xs, 
													Lamb93_ys, Xi, Yi,
													&Xf, &Yf);
		return true;
	}

  // Lambert NTF vers RGF93
	if ((start_proj <= Lambert2E)&&(end_proj == RGF93)) {
		double xle, yle;
		ConvertClarke(start_proj, Lambert2E, Xi, Yi, xle, yle);
		Convert2ETo93(xle, yle, &Xf, &Yf, Z, true);
		return true;
	}

  // RGF93 vers Lambert NTF
	if ((end_proj <= Lambert2E)&&(start_proj == RGF93)) {
		double xle, yle;
		Convert93To2E(Xi, Yi, &xle, &yle, Z, true);
		ConvertClarke(Lambert2E, end_proj, xle, yle, Xf, Yf);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Conversion avec passage en degree decimaux
//-----------------------------------------------------------------------------
bool XLambert::ConvertDeg(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi, 
													double& Xf, double& Yf, double Z)
{
	if ((start_proj != RGF93)&&(end_proj != RGF93))
		return Convert(start_proj, end_proj, Xi, Yi, Xf, Yf, Z);

	double xi = Xi, yi = Yi;
	if (start_proj == RGF93) {
		xi *= (XPI / 180.);
		yi *= (XPI / 180.);
	}
	bool flag = Convert(start_proj, end_proj, xi, yi, Xf, Yf, Z);
	if (end_proj == RGF93) {
		Xf *= (180. / XPI);
		Yf *= (180. / XPI);
	}
	return flag;
}

//-----------------------------------------------------------------------------
// Alteration lineaire
//-----------------------------------------------------------------------------
double XLambert::AltLin(XProjCode start_proj, double Xi, double Yi)
{
  if (start_proj == RGF93)
    return 0;
  double lambda, phi;
  double epsilon_phi = 0.000000001;
  double conv, modlin, K, alter, Xf, Yf;

  // Lambert NTF
  if (start_proj <= Lambert2E) {
    // Passage en lambert 2 etendu
    ConvertClarke(start_proj, Lambert2E, Xi, Yi, Xf, Yf);
    double nI, cI, xI, yI;
    SetParameterClarke(Lambert2E, nI, cI, xI, yI);

    // Transformation Lambert -> Geographique
    XAlgoGeod::LambertGeo(Clark_e2, nI, cI, Lamb2E_lambdac, epsilon_phi,
                          xI, yI, &lambda, &phi,
                          Xi, Yi);
    XAlgoGeod::ModConvLamb(lambda, phi,  Clark_a, Clark_e2,
                nI, cI, Lamb2E_lambdac,  &conv,
                &modlin, &K, &alter );
    return alter;
  }

  // Lambert 93 et Lambert CC
  SetParametersGRS(start_proj);
  XAlgoGeod::LambertGeo(GRS80_e2, Lamb93_n, Lamb93_c, Lamb93_lambdac, epsilon_phi,
                        Lamb93_xs, Lamb93_ys, &lambda, &phi,
                        Xi, Yi);
  XAlgoGeod::ModConvLamb(lambda, phi,  GRS80_a, GRS80_e2,
              Lamb93_n, Lamb93_c, Lamb93_c,  &conv,
              &modlin, &K, &alter );
  return alter;
}

//-----------------------------------------------------------------------------
// Gestion du LAEA
//-----------------------------------------------------------------------------
bool XLambert::ConvertLAEA(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                           double& Xf, double& Yf, double Z)
{
  double lambda = 0., phi = 0.;

  XAlgoGeod::LAEAgeo(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                     Laea_qp, Laea_Rq,	Laea_beta1, Laea_D,
                     Laea_x0, Laea_y0, &lambda, &phi, Xi, Yi);

  XAlgoGeod::GeoLAEA(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                     Laea_qp, Laea_Rq, Laea_beta1, Laea_D,
                     Laea_x0, Laea_y0, lambda, phi, &Xf, &Yf);

  if (end_proj == ETRS89LAEA) {
    if (!ConvertRGF93(start_proj, RGF93, Xi, Yi, lambda, phi, Z))
      return false;

    XAlgoGeod::GeoLAEA(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                       Laea_qp, Laea_Rq, Laea_beta1, Laea_D,
                       Laea_x0, Laea_y0, lambda, phi, &Xf, &Yf);
    return true;
  }
  if (start_proj == ETRS89LAEA) {
    XAlgoGeod::LAEAgeo(GRS80_a, GRS80_e2, Laea_lambda0, Laea_phi0,
                       Laea_qp, Laea_Rq,	Laea_beta1, Laea_D,
                       Laea_x0, Laea_y0, &lambda, &phi, Xi, Yi);
    return ConvertRGF93(RGF93, end_proj, lambda, phi, Xf, Yf, Z);
  }
  return false;
}

//-----------------------------------------------------------------------------
// Gestion de l'UTM
//-----------------------------------------------------------------------------
bool XLambert::SetParametersUTM(XProjCode projection)
{
  if (projection == m_ProjUTM)
    return true;
  if (projection < ETRS89TM30)
    return false;
  if (projection > ETRS89TM32)
    return false;

  double lambda0 = -3. + (projection - ETRS89TM30)*6.;  // Longitude origine en degres
  Utm_x0 = 500000.;
  Utm_y0 = 0;
  Utm_lambda0 = lambda0*XAlgoGeod::Pi() / 180.;
  Utm_phi0 = 0.;
  Utm_K0 = 0.9996;
  m_ProjUTM = projection;

  // Calcul des paramètres de la projection indépendants de la position
  XAlgoGeod::CoefProjMercTr(GRS80_a, GRS80_e2, Utm_lambda0, Utm_phi0,
    Utm_K0, Utm_x0, Utm_y0, &Utm_lambdac, &Utm_n, &Utm_xs, &Utm_ys);

  return true;
}

bool XLambert::ConvertUTM(XProjCode start_proj, XProjCode end_proj, double Xi, double Yi,
                          double& Xf, double& Yf, double Z)
{
  double lambda, phi;
  if (!ConvertRGF93(start_proj, RGF93, Xi, Yi, lambda, phi, Z))
    return false;
  return ConvertRGF93(RGF93, end_proj, lambda, phi, Xf, Yf, Z);
}
