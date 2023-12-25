//-----------------------------------------------------------------------------
//								XNCGeod.h
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 15/10/2013
//
// Geodesie pour la Nouvelle-Caledonie
//-----------------------------------------------------------------------------

#ifndef XNCGEOD_H
#define XNCGEOD_H

#include "XGeodConverter.h"
#include "XGeodGrid.h"

class XNCGeod : public XGeodConverter {
protected:

  typedef struct {
    double ElgA;
    double ElgE2;

    // Parametres de la projection
    double X0;
    double Y0;
    double Lambda0;
    double Phi0;
    double Phi1;
    double Phi2;
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

  DataProjection    m_StartData;
  DataProjection    m_EndData;
  Transformation    m_Transfo;
  XGeodGrid         m_GridIGN72_1;
  XGeodGrid         m_GridIGN72_2;
  XGeodGrid         m_GridNEA74;

  bool SetDataProjection(XProjCode proj, DataProjection* data);

public:
  XNCGeod() {;}
  virtual ~XNCGeod() { ;}

  bool ReadGridIGN72_1(const char* filename, XError* error = NULL);
  bool ReadGridIGN72_2(const char* filename, XError* error = NULL);
  bool ReadGridNEA74(const char* filename, XError* error = NULL);

  virtual bool SetDefaultProjection(XProjCode start_proj, XProjCode end_proj);

  // Conversion normale
  virtual bool Convert(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);
  virtual bool ConvertDeg(double Xi, double Yi, double& Xf, double& Yf, double Z = 0.);
};

#endif // XNCGEOD_H
