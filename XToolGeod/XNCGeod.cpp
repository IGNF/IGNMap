//-----------------------------------------------------------------------------
//								XNCGeod.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 15/10/2013
//
// geodesie pour la Nouvelle-Caledonie
//-----------------------------------------------------------------------------

#include "XNCGeod.h"
#include "XAlgoGeod.h"

//-----------------------------------------------------------------------------
// Lecture d'une grille binaire
//-----------------------------------------------------------------------------
bool XNCGeod::ReadGridIGN72_1(const char* filename, XError* error)
{
  m_GridIGN72_1.Unload();
  m_GridIGN72_2.Unload();
  return m_GridIGN72_1.ReadGrid(filename, error);
}

//-----------------------------------------------------------------------------
// Lecture d'une grille binaire
//-----------------------------------------------------------------------------
bool XNCGeod::ReadGridIGN72_2(const char* filename, XError* error)
{
  return m_GridIGN72_2.ReadGrid(filename, error);
}

//-----------------------------------------------------------------------------
// Lecture d'une grille binaire
//-----------------------------------------------------------------------------
bool XNCGeod::ReadGridNEA74(const char* filename, XError* error)
{
  return m_GridNEA74.ReadGrid(filename, error);
}

//-----------------------------------------------------------------------------
// Choix des projections par defaut
//-----------------------------------------------------------------------------
bool XNCGeod::SetDefaultProjection(XProjCode start_proj, XProjCode end_proj)
{
  if ((start_proj == NC_IGN72)&&(end_proj == NC_NEA74))
    return false;
  if ((end_proj == NC_IGN72)&&(start_proj == NC_NEA74))
    return false;

  m_StartProjection = start_proj;
  m_EndProjection = end_proj;

  if (!SetDataProjection(start_proj, &m_StartData))
    return false;
  if (!SetDataProjection(end_proj, &m_EndData))
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Fixe les parametres de projection
//-----------------------------------------------------------------------------
bool XNCGeod::SetDataProjection(XProjCode proj, DataProjection* data)
{
  if (proj == NC_IGN72) {
    data->ElgA = 6378388.0;
    data->ElgE2 = 0.006722670022;

    data->X0 = 500000.;
    data->Y0 = 10000000.;
    data->Lambda0 = (165.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = 0.;
    data->K0 = 0.9996;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjMercTr(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
      data->K0, data->X0, data->Y0, &(data->LambdaC), &(data->N), &(data->Xs), &(data->Ys));

    // Transformation a 7 parametres
    m_Transfo.Tx = 97.295;
    m_Transfo.Ty = -263.247;
    m_Transfo.Tz = 310.882;
    m_Transfo.D = 13.325904e-6;
    m_Transfo.Rx = (-1.599871 / 3600.) * XAlgoGeod::Pi() / 180.;
    m_Transfo.Ry = (0.838616 / 3600.) * XAlgoGeod::Pi() / 180.;
    m_Transfo.Rz = (3.140889 / 3600.) * XAlgoGeod::Pi() / 180.;

    if (m_GridIGN72_1.IsLoaded()) {
      m_Transfo.Tx = -11.64;
      m_Transfo.Ty = -348.60;
      m_Transfo.Tz = 291.98;
      m_Transfo.D = 0.;
      m_Transfo.Rx = 0.;
      m_Transfo.Ry = 0.;
      m_Transfo.Rz = 0.;
    }

    return true;
  }

  if (proj == NC_NEA74) {
    data->ElgA = 6378388.0;
    data->ElgE2 = 0.006722670022;

    data->X0 = 8.313;
    data->Y0 = -2.354;
    data->Lambda0 = (166. + 26./60. + 33./3600.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = -(22. + 16./60. + 11./3600.)*XAlgoGeod::Pi() / 180.;
    data->K0 = 1.;
    data->Phi1 = -(22. + 14./60. + 41./3600.)*XAlgoGeod::Pi() / 180.;
    data->Phi2 = -(22. + 17./60. + 41./3600.)*XAlgoGeod::Pi() / 180.;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjLambSec(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
                               data->Phi1, data->Phi2, data->X0, data->Y0,
                               &(data->LambdaC), &(data->N), &(data->C), &(data->Xs), &(data->Ys));

    // Transformation a 7 parametres
    m_Transfo.Tx = -166.207;
    m_Transfo.Ty = -154.777;
    m_Transfo.Tz = 254.831;
    m_Transfo.D = -30.859839e-6;
    m_Transfo.Rx = (-37.544387 / 3600.) * XAlgoGeod::Pi() / 180.;
    m_Transfo.Ry = (7.701146 / 3600.) * XAlgoGeod::Pi() / 180.;
    m_Transfo.Rz = (-10.202481 / 3600.) * XAlgoGeod::Pi() / 180.;

    if (m_GridNEA74.IsLoaded()) {
      m_Transfo.Tx = -10.18;
      m_Transfo.Ty = -350.43;
      m_Transfo.Tz = 291.37;
      m_Transfo.D = 0.;
      m_Transfo.Rx = 0.;
      m_Transfo.Ry = 0.;
      m_Transfo.Rz = 0.;
    }

    return true;
  }

  if ((proj == NC_RGNC91_Lambert)||(proj == RGF93)) {
    data->ElgA = 6378137.0;
    data->ElgE2 = 0.0066943800229;

    data->X0 = 400000.;
    data->Y0 = 300000.;
    data->Lambda0 = (166.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = -(21 + 30. / 60.)*XAlgoGeod::Pi() / 180.;
    data->K0 = 1.;
    data->Phi1 = -(20 + 40. / 60.)*XAlgoGeod::Pi() / 180.;
    data->Phi2 = -(22 + 20. / 60.)*XAlgoGeod::Pi() / 180.;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjLambSec(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
                               data->Phi1, data->Phi2, data->X0, data->Y0,
                               &(data->LambdaC), &(data->N), &(data->C), &(data->Xs), &(data->Ys));
    return true;
  }

  if (proj == NC_RGNC91_UTM57) {
    data->ElgA = 6378137.0;
    data->ElgE2 = 0.0066943800229;

    data->X0 = 500000.;
    data->Y0 = 10000000.;
    data->Lambda0 = (159.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = 0.;
    data->K0 = 0.9996;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjMercTr(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
      data->K0, data->X0, data->Y0, &(data->LambdaC), &(data->N), &(data->Xs), &(data->Ys));

    return true;
  }

  if (proj == NC_RGNC91_UTM58) {
    data->ElgA = 6378137.0;
    data->ElgE2 = 0.0066943800229;

    data->X0 = 500000.;
    data->Y0 = 10000000.;
    data->Lambda0 = (165.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = 0.;
    data->K0 = 0.9996;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjMercTr(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
      data->K0, data->X0, data->Y0, &(data->LambdaC), &(data->N), &(data->Xs), &(data->Ys));

    return true;
  }

  if (proj == NC_RGNC91_UTM59) {
    data->ElgA = 6378137.0;
    data->ElgE2 = 0.0066943800229;

    data->X0 = 500000.;
    data->Y0 = 10000000.;
    data->Lambda0 = (171.)*XAlgoGeod::Pi() / 180.;
    data->Phi0 = 0.;
    data->K0 = 0.9996;

    // Calcul des paramètres de la projection indépendants de la position
    XAlgoGeod::CoefProjMercTr(data->ElgA, data->ElgE2, data->Lambda0, data->Phi0,
      data->K0, data->X0, data->Y0, &(data->LambdaC), &(data->N), &(data->Xs), &(data->Ys));

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Conversion normale
//-----------------------------------------------------------------------------
bool XNCGeod::Convert(double Xi, double Yi, double& Xf, double& Yf, double Z)
{
  double lambda = 0., phi = 0., h = Z;
  double epsilon_phi = 0.000000001;
  XGeodGrid *grid1 = NULL, *grid2 = NULL;

  // Choix des grilles
  if ((m_StartProjection == NC_IGN72)||(m_EndProjection == NC_IGN72)) {
    if (m_GridIGN72_1.IsLoaded())
      grid1 = &m_GridIGN72_1;
    if (m_GridIGN72_2.IsLoaded())
      grid2 = &m_GridIGN72_2;
  }
  if ((m_StartProjection == NC_NEA74)||(m_EndProjection == NC_NEA74)) {
    if (m_GridNEA74.IsLoaded())
      grid1 = &m_GridNEA74;
  }

  if (m_StartProjection == RGF93) {
    lambda = Xi;
    phi = Yi;
  }

  if (m_StartProjection == NC_RGNC91_Lambert)
    XAlgoGeod::LambertGeo(m_StartData.ElgE2, m_StartData.N, m_StartData.C, m_StartData.LambdaC,
                          epsilon_phi, m_StartData.Xs, m_StartData.Ys, &lambda, &phi, Xi, Yi);

  if ((m_StartProjection >= NC_RGNC91_UTM57)&&(m_StartProjection <= NC_RGNC91_UTM59))
    XAlgoGeod::MercTrGeo(m_StartData.Lambda0, m_StartData.N, m_StartData.Xs, m_StartData.Ys,
                         m_StartData.ElgE2, &lambda, &phi, Xi, Yi, epsilon_phi);

  if ((m_StartProjection == NC_IGN72)||(m_StartProjection == NC_NEA74)) {
    // Transformation UTM -> Geographique
    if (m_StartProjection == NC_IGN72)
      XAlgoGeod::MercTrGeo(m_StartData.Lambda0, m_StartData.N, m_StartData.Xs, m_StartData.Ys,
                           m_StartData.ElgE2, &lambda, &phi, Xi, Yi, epsilon_phi);

    // Transformation Lambert -> Geographique
    if (m_StartProjection == NC_NEA74)
      XAlgoGeod::LambertGeo(m_StartData.ElgE2, m_StartData.N, m_StartData.C, m_StartData.LambdaC,
                            epsilon_phi, m_StartData.Xs, m_StartData.Ys, &lambda, &phi, Xi, Yi);

    // Transformation Geographique -> Cartesienne
    double x1, y1, z1;
    XAlgoGeod::GeoCart(m_StartData.ElgA, m_StartData.ElgE2,
                       lambda, phi, h,
                       &x1, &y1, &z1);
    // Changement de systeme geodesique
    double x2, y2, z2;
    XAlgoGeod::Tr7ParRc(m_Transfo.Tx, m_Transfo.Ty, m_Transfo.Tz, m_Transfo.D,
                        m_Transfo.Rx, m_Transfo.Ry, m_Transfo.Rz,
                        x1, y1, z1,
                        &x2, &y2, &z2);
    // Transformation Cartesienne -> Geographique
    XAlgoGeod::CartGeo12(x2, y2, z2,
                         m_EndData.ElgA, m_EndData.ElgE2, epsilon_phi,
                         &lambda, &phi, &h);
    lambda += XAlgoGeod::Pi();

    if (grid1 != NULL) {
      double tx = m_Transfo.Tx, ty = m_Transfo.Ty, tz = m_Transfo.Tz;
      if (grid1->Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz)) {
        if (grid2 != NULL)
          grid2->Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz);
        XAlgoGeod::Tr7ParRc(tx, ty, tz, m_Transfo.D,
                            m_Transfo.Rx, m_Transfo.Ry, m_Transfo.Rz,
                            x1, y1, z1,
                            &x2, &y2, &z2);
        XAlgoGeod::CartGeo12(x2, y2, z2,
                             m_EndData.ElgA, m_EndData.ElgE2, epsilon_phi,
                             &lambda, &phi, &h);
        lambda += XAlgoGeod::Pi();
      }
    }
  }

  if (m_EndProjection == RGF93) {
    Xf = lambda;
    Yf = phi;
    return true;
  }

  if (m_EndProjection == NC_RGNC91_Lambert) {
    XAlgoGeod::GeoLambert(m_EndData.ElgE2, m_EndData.N, m_EndData.C, m_EndData.LambdaC,
                          m_EndData.Xs, m_EndData.Ys, lambda, phi, &Xf, &Yf);
    return true;
  }

  if ((m_EndProjection >= NC_RGNC91_UTM57)&&(m_EndProjection <= NC_RGNC91_UTM59)) {
    XAlgoGeod::GeoMercTr(m_EndData.Lambda0, m_EndData.N, m_EndData.Xs, m_EndData.Ys,
                         m_EndData.ElgE2, lambda, phi, &Xf, &Yf);

    return true;
  }

  if ((m_EndProjection == NC_IGN72)||(m_EndProjection == NC_NEA74)) {
    // Transformation Geographique -> Cartesienne
    h += 60.447; // Hauteur ellipsoidale de l'altitude 0
    double x1, y1, z1;
    XAlgoGeod::GeoCart(m_StartData.ElgA, m_StartData.ElgE2,
                       lambda, phi, h,
                       &x1, &y1, &z1);

    // Changement de systeme geodesique
    double x2, y2, z2;
    if (grid1 != NULL) {
      double tx = m_Transfo.Tx, ty = m_Transfo.Ty, tz = m_Transfo.Tz;
      grid1->Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz);
      if (grid2 != NULL)
        grid2->Interpol(lambda * 180. / XAlgoGeod::Pi(), phi * 180. / XAlgoGeod::Pi(), &tx, &ty, &tz);

      XAlgoGeod::Tr7ParCR(tx, ty, tz, m_Transfo.D,
                          m_Transfo.Rx, m_Transfo.Ry, m_Transfo.Rz,
                          x1, y1, z1,
                          &x2, &y2, &z2);
    } else
      XAlgoGeod::Tr7ParCR(m_Transfo.Tx, m_Transfo.Ty, m_Transfo.Tz, m_Transfo.D,
                          m_Transfo.Rx, m_Transfo.Ry, m_Transfo.Rz,
                          x1, y1, z1,
                          &x2, &y2, &z2);

    // Transformation Cartesienne -> Geographique
    XAlgoGeod::CartGeo12(x2, y2, z2,
                         m_EndData.ElgA, m_EndData.ElgE2, epsilon_phi,
                         &lambda, &phi, &h);
    lambda += XAlgoGeod::Pi();

    // Transformation Geographique -> UTM
    if (m_EndProjection == NC_IGN72)
      XAlgoGeod::GeoMercTr(m_EndData.Lambda0, m_EndData.N, m_EndData.Xs, m_EndData.Ys,
                           m_EndData.ElgE2, lambda, phi, &Xf, &Yf);

    // Transformation Geographique -> Lambert
    if (m_EndProjection == NC_NEA74)
      XAlgoGeod::GeoLambert(m_EndData.ElgE2, m_EndData.N, m_EndData.C, m_EndData.LambdaC,
                            m_EndData.Xs, m_EndData.Ys, lambda, phi, &Xf, &Yf);

    return true;
  }

  return false;

}

//-----------------------------------------------------------------------------
// Conversion avec passage en degree decimaux
//-----------------------------------------------------------------------------
bool XNCGeod::ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z)
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
