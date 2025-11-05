//-----------------------------------------------------------------------------
//								XAffinity2D.cpp
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 06/10/00
//-----------------------------------------------------------------------------

#include <cmath>
#include "XAffinity2D.h"
#include "../XTool/XParserXML.h"

// Constructeur
XAffinity2D::XAffinity2D(int nbPt, XPt2D* U, XPt2D* X)
{
	int i;
	XPt2D Xs, Us;
	double sigmaXY, sigmaX2, sigmaY2;
	double sigmaXU, sigmaXV, sigmaYU, sigmaYV;
	
	sigmaXY = sigmaX2 = sigmaY2 = 0.0;
	sigmaXU = sigmaXV = sigmaYU = sigmaYV = 0.0;
	
	// calcul des barycentres
	for (i = 0; i < nbPt; i++) {
		Xs += X[i]; 
		Us += U[i];
	}
	
	Xs /= nbPt;
	Us /= nbPt;
	
	for (i = 0; i < nbPt; i++) {
		sigmaXY += (X[i].X - Xs.X) * (X[i].Y - Xs.Y);
		sigmaX2 += (X[i].X - Xs.X) * (X[i].X - Xs.X);
		sigmaY2 += (X[i].Y - Xs.Y) * (X[i].Y - Xs.Y);
		sigmaXU += (X[i].X - Xs.X) * (U[i].X - Us.X);
		sigmaXV += (X[i].X - Xs.X) * (U[i].Y - Us.Y);
		sigmaYU += (X[i].Y - Xs.Y) * (U[i].X - Us.X);
		sigmaYV += (X[i].Y - Xs.Y) * (U[i].Y - Us.Y);		
	}
	
	double denom = (sigmaX2 * sigmaY2 - sigmaXY * sigmaXY);
	R11 = (sigmaY2 * sigmaXU - sigmaXY * sigmaYU) / denom;
	R12 = (sigmaX2 * sigmaYU - sigmaXY * sigmaXU) / denom;
	R21 = (sigmaY2 * sigmaXV - sigmaXY * sigmaYV) / denom;
	R22 = (sigmaX2 * sigmaYV - sigmaXY * sigmaXV) / denom;

	Tx = Us.X - R11 * Xs.X - R12 * Xs.Y; 
	Ty = Us.Y - R21 * Xs.X - R22 * Xs.Y; 
}

// Calcul
bool XAffinity2D::Compute(std::vector<XPt2D>& U, std::vector<XPt2D>& X)
{
  if (U.size() != X.size())
    return false;
  if (U.size() < 3)
    return false;
  XPt2D Xs, Us;
  double sigmaXY, sigmaX2, sigmaY2;
  double sigmaXU, sigmaXV, sigmaYU, sigmaYV;

  sigmaXY = sigmaX2 = sigmaY2 = 0.0;
  sigmaXU = sigmaXV = sigmaYU = sigmaYV = 0.0;

  // calcul des barycentres
  for (uint32_t i = 0; i < U.size(); i++) {
    Xs += X[i];
    Us += U[i];
  }

  Xs /= (double)U.size();
  Us /= (double)U.size();

  for (uint32_t i = 0; i < U.size(); i++) {
    sigmaXY += (X[i].X - Xs.X) * (X[i].Y - Xs.Y);
    sigmaX2 += (X[i].X - Xs.X) * (X[i].X - Xs.X);
    sigmaY2 += (X[i].Y - Xs.Y) * (X[i].Y - Xs.Y);
    sigmaXU += (X[i].X - Xs.X) * (U[i].X - Us.X);
    sigmaXV += (X[i].X - Xs.X) * (U[i].Y - Us.Y);
    sigmaYU += (X[i].Y - Xs.Y) * (U[i].X - Us.X);
    sigmaYV += (X[i].Y - Xs.Y) * (U[i].Y - Us.Y);
  }

  double denom = (sigmaX2 * sigmaY2 - sigmaXY * sigmaXY);
  R11 = (sigmaY2 * sigmaXU - sigmaXY * sigmaYU) / denom;
  R12 = (sigmaX2 * sigmaYU - sigmaXY * sigmaXU) / denom;
  R21 = (sigmaY2 * sigmaXV - sigmaXY * sigmaYV) / denom;
  R22 = (sigmaX2 * sigmaYV - sigmaXY * sigmaXV) / denom;

  Tx = Us.X - R11 * Xs.X - R12 * Xs.Y;
  Ty = Us.Y - R21 * Xs.X - R22 * Xs.Y;
  return true;
}

// Calcul avec elimination des erreurs
uint32_t XAffinity2D::Compensation(std::vector<XPt2D>& U, std::vector<XPt2D>& X, double nb_rmq)
{
  if (!Compute(U, X))
    return 0;
  std::vector<XPt2D> V = U;
  std::vector<XPt2D> Y = X;

  double rmq, res, res_max;
  uint32_t index = 0;
  while(true) {
    res_max = rmq = 0.;
    for (uint32_t i = 0; i < V.size(); i++) {
      res = Residu(V[i], Y[i]).norm();
      rmq += res;
      if (res > res_max) {
        res_max = res;
        index = i;
      }
    }
    rmq /= V.size();
    if (res_max > nb_rmq * rmq) {
      V.erase(V.begin()+index);
      Y.erase(Y.begin()+index);
      if (!Compute(V, Y))
        return 0;
    } else
      return (uint32_t)V.size();
  }
  return 0;
}

// Calcul dans le sens direct
XPt2D XAffinity2D::Direct(const XPt2D& M) const
{
	return XPt2D(Tx + R11*M.X + R12*M.Y,
				 Ty + R21*M.X + R22*M.Y);
}

void XAffinity2D::Direct(const XPt2D& M, double& U, double& V)
{
	U = Tx + R11 * M.X + R12 * M.Y;
	V = Ty + R21 * M.X + R22 * M.Y;
}
	
// Calcul dans le sens indirect
XPt2D XAffinity2D::Indirect(const XPt2D& M) const
{
	double denom = R22 * R11 - R12 * R21;
	return XPt2D((R22 * (M.X - Tx) - R12 * (M.Y - Ty)) / denom,
				 (R11 * (M.Y - Ty) - R21 * (M.X - Tx)) / denom);
}

void XAffinity2D::Indirect(const XPt2D& M, double& X, double& Y)
{
	double denom = R22 * R11 - R12 * R21;
	X = (R22 * (M.X - Tx) - R12 * (M.Y - Ty)) / denom;
	Y = (R11 * (M.Y - Ty) - R21 * (M.X - Tx)) / denom;
}

// Calcul du residu : valeur calculee - valeur theorique
XPt2D XAffinity2D::Residu(const XPt2D& U, const XPt2D& X) const
{
	return Direct(X) - U;
}

// Calcul du rmq pour un jeu de mesures
XPt2D XAffinity2D::Rmq(int nbPt, XPt2D* U, XPt2D* X) const
{
	int i;
	XPt2D rmq, res;
	for (i = 0; i < nbPt; i++) {
		res = Residu(U[i], X[i]);
		rmq += XPt2D(res.X * res.X, res.Y * res.Y);
	}
	return XPt2D( sqrt(rmq.X / nbPt), sqrt(rmq.Y / nbPt));
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XAffinity2D::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML affi = parser->FindSubParser("/affinity2d", num);
	if (affi.IsEmpty())
		return XErrorError(error, "XAffinity2D::XmlRead", XError::eBadFormat);

	Tx = affi.ReadNodeAsDouble("/affinity2d/Tx");
	Ty = affi.ReadNodeAsDouble("/affinity2d/Ty");
	R11 = affi.ReadNodeAsDouble("/affinity2d/R11");
	R12 = affi.ReadNodeAsDouble("/affinity2d/R12");
	R21 = affi.ReadNodeAsDouble("/affinity2d/R21");
	R22 = affi.ReadNodeAsDouble("/affinity2d/R22");

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XAffinity2D::XmlWrite(std::ostream* out)
{
	std::streamsize prec = out->precision(2);						// Sauvegarde des parametres du flux
	std::ios::fmtflags flags = out->setf(std::ios::fixed);

	*out << "<affinity2d>" << std::endl;
	*out << "<Tx> " << Tx << " </Tx>" << std::endl;
	*out << "<Ty> " << Ty << " </Ty>" << std::endl;
	*out << "<R11> " << R11 << " </R11>" << std::endl;
	*out << "<R12> " << R12 << " </R12>" << std::endl;
	*out << "<R21> " << R21 << " </R21>" << std::endl;
	*out << "<R22> " << R22 << " </R22>" << std::endl;
	*out << "</affinity2d>" << std::endl;

	out->precision(prec);		// Restauration des parametres du flux
	out->unsetf(std::ios::fixed);
	out->setf(flags);
	return out->good();
}
