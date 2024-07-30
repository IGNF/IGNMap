//-----------------------------------------------------------------------------
//								XGeoVector.cpp
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 21/03/2003
//-----------------------------------------------------------------------------

#include <algorithm>
#include <cctype>
#include <cstring>

#include "XGeoVector.h"
#include "XGeoMap.h"
#include "XPolygone2D.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoVector::XGeoVector()
{
	m_Repres = NULL;
	m_Class = NULL;
	m_Frame = XFrame(XGEO_NO_DATA,XGEO_NO_DATA,XGEO_NO_DATA,XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Renvoi la representation du vecteur
//-----------------------------------------------------------------------------
XGeoRepres* XGeoVector::Repres()
{
	if (m_Repres != NULL)
		return m_Repres;
	if (m_Class != NULL)
		return m_Class->Repres();
	return NULL;
}

//-----------------------------------------------------------------------------
// Change la representation du vecteur
//-----------------------------------------------------------------------------
void XGeoVector::Repres(XGeoRepres* repres)
{
  if (m_Repres != NULL)
    if (m_Repres->Deletable())
      delete m_Repres;
	m_Repres = repres;
}

//-----------------------------------------------------------------------------
// Renvoi si le vecteur est selectionnable
//-----------------------------------------------------------------------------
bool XGeoVector::IsSelectable() const
{
	if (!Selectable())
		return false;
	if (!m_Class->Selectable())
		return false;
	if (!m_Class->Layer()->Selectable())
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Indique si un vecteur est proche d'un point terrain
//-----------------------------------------------------------------------------
bool XGeoVector::IsNear2D(const XPt2D& P, double dist)
{
	if (P.X < m_Frame.Xmin - dist)
		return false;
	if (P.X > m_Frame.Xmax + dist)
		return false;
	if (P.Y < m_Frame.Ymin - dist)
		return false;
	if (P.Y > m_Frame.Ymax + dist)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Indique si le vecteur est connecte au point
//-----------------------------------------------------------------------------
bool XGeoVector::IsConnected(const XPt2D& P)
{
	if (!m_Frame.IsIn(P))
		return false;
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom2D())
			return false;
	}

	XPt2D A, B;
	A = Pt(0);
	B = Pt(NbPt() - 1);

	if (!loaded)
		Unload();

	if ((A == P)||(B == P))
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Indique si le point est un des sommets du vecteur
//-----------------------------------------------------------------------------
bool XGeoVector::IsSom2D(const XPt2D& P, int &index, double epsilon)
{
  if (!m_Frame.IsIn(P))
    return false;
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom2D())
      return false;
  }

  bool flag = false;
  for (uint32_t i = 0; i < NbPt(); i++) {
    if (fabs(P.X - Pt(i).X)>epsilon)
      continue;
    if (fabs(P.Y - Pt(i).Y)>epsilon)
      continue;
    index = i;
    flag = true;
    break;
  }

  if (!loaded)
    Unload();
  return flag;
}

//-----------------------------------------------------------------------------
// Recherche de la valeur d'un attribut
//-----------------------------------------------------------------------------
std::string XGeoVector::FindAttribute(std::string att_name, bool no_case)
{
	std::vector<std::string> V;
	if (!ReadAttributes(V))
		return "";
	for (uint32_t i = 0; i < V.size(); i+=2) {
		if (no_case)
			std::transform(V[i].begin(), V[i].end(), V[i].begin(), [](char c) {return static_cast<char>(std::tolower(c));});
		if (V[i] == att_name)
			return V[i+1];
	}
	return "";
}

//-----------------------------------------------------------------------------
// Mise a jour de la valeur d'un attribut
//-----------------------------------------------------------------------------
bool XGeoVector::UpdateAttribute(std::string att_name, std::string value, bool no_case)
{
	std::vector<std::string> V;
	if (!ReadAttributes(V))
		return false;
	for (uint32_t i = 0; i < V.size(); i+=2) {
		if (no_case)
			std::transform(V[i].begin(), V[i].end(), V[i].begin(), [](char c) {return static_cast<char>(std::tolower(c));});
		if (V[i] == att_name) {
			V[i+1] = value;
			WriteAttributes(V);
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Type vecteur
//-----------------------------------------------------------------------------
std::string XGeoVector::TypeVectorString()
{
	switch(TypeVector()) {
		case Null : return "Null";
		case Point : return "Point 2D";
		case PointZ : return "Point 3D";
		case MPoint : return "Multi-Point 2D";
		case MPointZ : return "Multi-Point 3D</B><BR>";
		case Line : return "Ligne 2D";
		case LineZ : return "Ligne 3D";
		case MLine : return "Multi-Ligne 2D";
		case MLineZ : return "Multi-Ligne 3D";
		case Poly : return "Polygone 2D";
		case PolyZ : return "Polygone 3D";
		case MPoly : return "Multi-Polygone 2D";
		case MPolyZ : return "Multi-Polygone 3D";
		case Raster : return "Raster";
		case DTM : return "MNT";
		case LAS: return "LAS";
	}
	return "Type inconnu";
}

//-----------------------------------------------------------------------------
// Ecriture des informations sur l'objet
//-----------------------------------------------------------------------------
bool XGeoVector::WriteHtml(std::ostream* out)
{
  *out << "<HTML>";
  *out << "<HEAD>";
  XGeoMap* map = Map();
  if (map != NULL)
    if (map->UTF8())
      *out << "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />";
  *out << "</HEAD>";
  *out << "<BODY>";
	out->setf(std::ios::fixed);
	if ((m_Frame.Xmax < 180)&&(m_Frame.Ymax < 180))
		out->precision(6);
	else
		out->precision(2);
	if (Class() != NULL)
		if (Class()->Layer() != NULL) {
			*out << "<I>Theme</I> : " << Class()->Layer()->Name() << "<BR>";
			*out << "<I>Classe</I> : <B>" << Class()->Name() << "</B><BR><BR>";
		}

	//*out << "<hr style=\"width: 100%; height: 2px;\">";
	std::vector<std::string> V;
	if (ReadAttributes(V)) {
		if (V.size() > 0) {
			*out << "<hr style=\"width: 100%; height: 2px;\">";
			*out << "<TABLE BORDER=\"0\" CELLPADDING=\"1\" CELLSPACING=\"0\"> <TBODY> ";
			for(uint32_t i = 0; i < V.size() / 2; i++) {
				std::string att_name = V[2 * i];
				if (att_name != " ")
					*out << "<TR><TD><I>" << V[2 * i] << " : </I><BR></TD> <TD>" << V[2*i + 1] << "<BR></TD></TR>";
				else
					*out << "<TR><TD><I>" << Class()->AttributName(i) << " : </I><BR></TD> <TD>" << V[2*i + 1] << "<BR></TD></TR>";
			}
			*out << "</TBODY></TABLE>";
		}
	}

	// Chargement de l'objet
	if (!LoadGeom()) {
		*out << "Impossible de charger l'objet !";
		*out << "</BODY> </HTML>";
		return true;
	}

	// Calcul de la longueur, de la surface, des pentes
	double d = Length(), slope = 0., min = 0., max = 0.;
	if (d > 0.) {
		*out << "<hr style=\"width: 100%; height: 2px;\">";
		*out << "<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"0\"> <TBODY> ";
		*out << "<TR><TD><I>Longueur : </I><BR></TD> <TD><B>" << d << " m</B><BR></TD></TR>";
		d = Length3D();
    if (Is3D())
			*out << "<TR><TD><I>Longueur 3D : </I><BR></TD> <TD><B>" << d << " m</B><BR></TD></TR>";

		d = Area();
    //if (d > 0.) {
			*out << "<TR><TD><I>Surface : </I><BR></TD> <TD><B>" << d << " m2</B><BR></TD></TR>";
			*out << "<TR><TD><I>Surface : </I><BR></TD> <TD><B>" << d * 0.0001 << " ha</B><BR></TD></TR>";
    //}
		slope = SlopeFilter(min, max);
		if ((slope != 0.)||(min < 0.)||(max > 0.)) {
			*out << "<TR><TD><I>Pente moyenne : </I><BR></TD> <TD><B>" << slope * 100. << " %</B><BR></TD></TR>";
			*out << "<TR><TD><I>Pente min : </I><BR></TD> <TD><B>" << min * 100. << " %</B><BR></TD></TR>";
			*out << "<TR><TD><I>Pente max : </I><BR></TD> <TD><B>" << max * 100. << " %</B><BR></TD></TR>";
		}
		slope = DeltaZ(max, min);
		if ((slope != 0.)||(min < 0.)||(max > 0.)) {
			*out << "<TR><TD><I>Denivele : </I><BR></TD> <TD><B>" << slope << " m</B><BR></TD></TR>";
			*out << "<TR><TD><I>Denivele positive : </I><BR></TD> <TD><B>" << max << " m</B><BR></TD></TR>";
			*out << "<TR><TD><I>Denivele negative : </I><BR></TD> <TD><B>" << min << " m</B><BR></TD></TR>";
		}
		*out << "</TBODY></TABLE>";
	}

	// Affichage de la geometrie
	*out << "<hr style=\"width: 100%; height: 2px;\">";
	*out << "<I>Type</I> : <B>" << TypeVectorString() << "</B><BR>";
	*out << "<I>Nombre de points</I> : " << NbPt() << "<BR>";
	*out << "<I>Nombre de parties</I> : " << NbPart() << "<BR><BR>";

	*out << "<I>Emprise</I> : (" << m_Frame.Xmin << " ; " << m_Frame.Ymin << ") - (" <<
						m_Frame.Xmax << " ; " << m_Frame.Ymax << ")<BR>";

	if (Is3D()) {
		*out << "<I>Emprise alti</I> : (";
		if (Zmin() > XGEO_NO_DATA)
			*out << Zmin() << " ; ";
		else
			*out << "NOZ ; ";

		if (Zmax() > XGEO_NO_DATA)
			*out << Zmax() << ")<BR>";
		else
			*out << "NOZ)<BR>";
	}
	*out << "<hr style=\"width: 100%; height: 2px;\">";
	XPt2D P;
	double z;
	uint32_t part = 1;
	for (uint32_t i = 0; i < NbPt(); i++) {
		P = Pt(i);
		z = Z(i);
		for (uint32_t j = part; j < NbPart(); j++)
			if (i == Part(j)) {
				part ++;
				*out << "<hr style=\"width: 100%; height: 2px;\">";
			}
    *out << "[" << i+1 << "] : " << P.X << " ; " << P.Y;
    if (z > XGEO_NO_DATA)
      *out << " ; " << z;
    if (HasCustomData())
      *out << " ; " << Data(i);
    *out << "<BR>";
  }
  *out << "<hr style=\"width: 100%; height: 2px;\">";
  WriteGeoJson(out, &V);

	Unload();

	*out << "</BODY> </HTML>";

	return true;
}

//-----------------------------------------------------------------------------
// Export XML
//-----------------------------------------------------------------------------
bool XGeoVector::XmlWrite(std::ostream* out)
{
	*out << "<xgeovector>" << std::endl;
	*out << "<layer> " << Class()->Layer()->Name() << " </layer>" << std::endl;
	*out << "<class> " << Class()->Name() << " </class>" << std::endl;
	m_Frame.XmlWrite(out);

	// Ecriture de la geometrie
	*out << "<type> " << (int)TypeVector() << " </type>" << std::endl;
	*out << "<nb_part> " << NbPart() << " </nb_part>" << std::endl;
	*out << "<nb_pt> " << NbPt() << " </nb_pt>" << std::endl;

	*out << "<part> ";
  uint32_t i;
  for (i = 0; i < NbPart(); i++)
		*out << Part(i) << " ";
	*out << "</part>" << std::endl;

	*out << "<pt> ";
	for (i = 0; i < NbPt(); i++)
		*out << Pt(i).X << " " << Pt(i).Y << " ";
	*out << "</pt>" << std::endl;

	*out << "<z> ";
	for (i = 0; i < NbPt(); i++)
		*out << Z(i) << " ";
	*out << "</z>" << std::endl;

	// Ecriture des attributs
	std::vector<std::string> V;
	ReadAttributes(V);
	if (V.size() > 0) {
		*out << "<attribut_list>" << std::endl;
		for (i = 0; i < V.size() / 2; i++)
			*out << "<attribut> <name> " << V[2*i] << " </name> <value>" << V[2*i+1]
						<< " </value> </attribut>" << std::endl;
		*out << "</attribut_list>" << std::endl;
	}

	// Ecriture de la representation
	if (m_Repres != NULL)
		m_Repres->XmlWrite(out);

	*out << "</xgeovector>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Test pour voir les Z aberrant (par ex Z sol pour un objet de sursol)
//-----------------------------------------------------------------------------
bool XGeoVector::TestZSol(XPt3D& P, double noz)
{
	P = XPt3D(XGEO_NO_DATA, XGEO_NO_DATA, XGEO_NO_DATA);
	if (!Is3D())
		return true;
	if (NbPt() < 2)
		return true;

	if(!LoadGeom())
		return false;
  uint32_t i, nb = 0;
	double zmean = 0., std_dev = 0., z;
  for (i = 0; i < NbPt(); i++) {
		z = Z(i);
    if (z <= noz) {
			Unload();
			return false;
		}
		zmean += z;
		std_dev += (z * z);
		nb++;
	}

	zmean /= nb;
	std_dev /= nb;
	std_dev -= (zmean * zmean);
	std_dev = sqrt(std_dev);
	if (std_dev < 1.) {
		Unload();
		return true;
	}

	bool flag = true;
	for (i = 0; i < NbPt() - 1; i++) {
		z = fabs(Z(i+1) - Z(i));
		if (z > 3. * std_dev) {
			flag = false;
			P.Z = z;
			P.X = Pt(i+1).X;
			P.Y = Pt(i+1).Y;
			break;
		}
	}

	Unload();
	return flag;
}

//-----------------------------------------------------------------------------
// Test les objets qui ont un Z a 0
//-----------------------------------------------------------------------------
bool XGeoVector::TestZ0(std::string& source)
{
	if (!Is3D())
		return true;
	if(!LoadGeom())
		return false;

	bool flag = false;
	for (uint32_t i = 0; i < NbPt(); i++) {
		if (Z(i) < 0.001) {
			flag = true;
			break;
		}
	}
	Unload();
	if (!flag)
		return true;

	std::vector<std::string> V;
	if (ReadAttributes(V)) {
		for(uint32_t i = 0; i < V.size() / 2; i++)
			if (V[2 * i].compare("SOURCE")==0) {
				source =  V[2*i + 1];
				return false;
			}
	}
	source = "inconnue";
	return false;
}

//-----------------------------------------------------------------------------
// Longueur de la geometrie
//-----------------------------------------------------------------------------
double XGeoVector::Length()
{
	if (NbPt() < 2)
		return 0.;
	switch(TypeVector()) {
		case Null : return 0.;
		case Point : return 0.;
		case PointZ : return 0.;
		case MPoint : return 0.;
		case MPointZ : return 0.;
		default:;
	}

	double d = 0.;
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	XPt2D A, B;
	uint32_t part = 1;
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		A = Pt(i);
		B = Pt(i+1);
		for (uint32_t j = part; j < NbPart(); j++)
			if ((i + 1) == Part(j)) {
				B = Pt(i);
			}
		d += dist(A, B);
	}
	if (!loaded)
		Unload();

	return d;
}

//-----------------------------------------------------------------------------
// Longueur de la geometrie en 3D
//-----------------------------------------------------------------------------
double XGeoVector::Length3D()
{
	if (!Is3D())
		return Length();
	if (NbPt() < 2)
		return 0.;
	switch(TypeVector()) {
		case Null : return 0.;
		case Point : return 0.;
		case PointZ : return 0.;
		case MPoint : return 0.;
		case MPointZ : return 0.;
		default:;
	}

	double d = 0.;
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	XPt3D A, B;
	uint32_t part = 1;
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		A = Pt(i);
		A.Z = Z(i);
		B = Pt(i+1);
		B.Z = Z(i+1);
		for (uint32_t j = part; j < NbPart(); j++)
			if ((i + 1) == Part(j)) {
				B = Pt(i);
				B.Z = Z(i);
			}
		d += dist(A, B);
	}
	if (!loaded)
		Unload();

	return d;
}

//-----------------------------------------------------------------------------
// Longueur de la geometrie et min / max des segments
//-----------------------------------------------------------------------------
double XGeoVector::Length(double& min, double& max)
{
	min = max = 0.;
	if (NbPt() < 2)
		return 0.;
	switch(TypeVector()) {
		case Null : return 0.;
		case Point : return 0.;
		case PointZ : return 0.;
		case MPoint : return 0.;
		case MPointZ : return 0.;
		default:;
	}

	double d = 0., dist_ab;
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	XPt2D A, B;
  min = XMax(FrameW(), FrameH());
	uint32_t part = 1;
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		A = Pt(i);
		B = Pt(i+1);
		for (uint32_t j = part; j < NbPart(); j++)
			if ((i + 1) == Part(j)) {
				B = Pt(i);
			}
		dist_ab = dist(A, B);
		if (dist_ab > 0.) {
			d += dist_ab;
			min = XMin(min, dist_ab);
			max = XMax(max, dist_ab);
		}
	}
	if (!loaded)
		Unload();

	return d;
}

//-----------------------------------------------------------------------------
// Surface de la geometrie
//-----------------------------------------------------------------------------
double XGeoVector::Area()
{
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return false;
  }
  double area = 0.;
  XPolygone2D poly;
  uint32_t nbpt;
  for (uint32_t i = 0; i < NbPart(); i++) {
    if (i < NbPart() - 1)
      nbpt = Part(i+1) - Part(i);
    else
      nbpt = NbPt() - Part(i);
    for (uint32_t j = Part(i); j < Part(i) + nbpt; j++)
      poly.AddPt(Pt(j));

    area += poly.Area();
    poly.Empty();
  }
  if (!loaded)
    Unload();

  return area;
}

//-----------------------------------------------------------------------------
// Calcul de la pente
//-----------------------------------------------------------------------------
double XGeoVector::Slope(double& min, double& max)
{
	if (!Is3D())
		return 0.;
	if (NbPt() < 2)
		return 0.;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	XPt2D A, B;
	double dist_ab, zA, zB, slope, d = 0.;
	min = max = 0.;
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		A = Pt(i);
		B = Pt(i+1);
		zA = Z(i);
		zB = Z(i+1);
		dist_ab = dist(A, B);
		if (dist_ab > 0.) {
			d += dist_ab;
			slope = (zB - zA) / dist_ab;
			min = XMin(min, slope);
			max = XMax(max, slope);
		}
	}
	zA = Z(0);
	zB = Z(NbPt() - 1);
	if (!loaded)
		Unload();

	if (d > 0.)
		return (zB - zA) / d;
	return 0.;
}

//-----------------------------------------------------------------------------
// Calcul de la pente avec filtrage des aberrations
//-----------------------------------------------------------------------------
double XGeoVector::SlopeFilter(double& min, double& max)
{
	if (!Is3D())
		return 0.;
	if (NbPt() < 2)
		return 0.;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	double length_max, length_min;
	double length = Length(length_min, length_max);
	XPt2D A, B;
	double dist_ab, zA, zB, d = 0., slope;
	min = max = 0.;
	A = Pt(0);
	zA = Z(0);
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		B = Pt(i+1);
		zB = Z(i+1);
		dist_ab = dist(A, B);
		d += dist_ab;
		if (dist_ab > length_max) {
			slope = (zB - zA) / d;
			min = XMin(min, slope);
			max = XMax(max, slope);
			d = 0;
			A = B;
			zA = zB;
		}
	}
	zA = Z(0);
	zB = Z(NbPt() - 1);
	if (!loaded)
		Unload();

	return (zB - zA) / length;
}

//-----------------------------------------------------------------------------
// Calcul des deniveles
//-----------------------------------------------------------------------------
double XGeoVector::DeltaZ(double& pos, double& neg)
{
	pos = neg = 0.;
	if (!Is3D())
		return 0.;
	if (NbPt() < 2)
		return 0.;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return 0.;
	}

	double zA, zB, dz;
	zB = Z(0);
	for (uint32_t i = 1; i < NbPt(); i++) {
		zA = zB;
		zB = Z(i);
		dz = zB - zA;
		if (dz > 0.)
			pos += dz;
		else
			neg -= dz;
	}
	zA = Z(0);
	if (!loaded)
		Unload();

	return (zB - zA);
}

//-----------------------------------------------------------------------------
// Interpolation des Z a 0
//-----------------------------------------------------------------------------
bool XGeoVector::InterpolZ0()
{
	if (!Is3D())
		return false;
	if (NbPt() < 2)
		return false;

	if (!LoadGeom())
		return false;

	// Verification des Z
	double* T = Z();
	uint32_t nbZ0 = 0;
	for (uint32_t k = 0; k < NbPt(); k++)
		if (T[k] < 0.1)
			nbZ0++;

	if (nbZ0 < 1)				// Tous les Z sont bons
		return true;
	if (nbZ0 == NbPt())	// Tous les Z sont a 0
		return false;


	XPt2D A, B;
	double dist_ab, dA = 0., d = 0., z = 0.;

	// Correction des Z initiaux
  uint32_t i, pos = 0;
  for (i = 0; i < NbPt(); i++) {
		if (T[i] > 0.1) {
			pos = i;
			z = T[i];
			break;
		}
	}
	if (pos > 0)
		for (i = 0; i < pos; i++)
			T[i] = z;

	// Correction des Z finaux
	for (i = NbPt() - 1; i >= 0; i--) {
		if (T[i] > 0.1) {
			pos = i;
			z = T[i];
			break;
		}
	}
	if (pos < NbPt() - 1)
		for (i = NbPt() - 1; i > pos; i--)
			T[i] = z;

	// Z nul au sein de la geometrie
	for (i = 1; i < NbPt() - 1; i++) {
		if (T[i] > 0.1)
			continue;
		A = Pt(i);
		dA = dist(A, Pt(i-1));
		d = dA;
		for (uint32_t j = i + 1; j < NbPt(); j++) {
			B = Pt(j);
			dist_ab = dist(A, B);
			d += dist_ab;
			if (T[j] > 0.1) {
				T[i] = (T[j] - T[i-1]) * dA / d + T[i -1];
				break;
			}
			A = B;
		}
	}

	// Recalcul de l'emprise
	double* range = ZRange();
	if (range != NULL) {
		range[0] = range[1] = T[0];
		for (i = 1; i < NbPt(); i++) {
			range[0] = XMin(range[0], T[i]);
			range[1] = XMax(range[1], T[i]);
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Echantillonnage de l'altitude
//-----------------------------------------------------------------------------
bool XGeoVector::Altitude(std::vector<double>& Alti, double step, bool interpol)
{
	if (!Is3D())
		return false;
	if (NbPt() < 2)
		return false;
	if (step <= 0.1)
		return false;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	if (!InterpolZ0())
		return false;

	double length = Length();
	if (length < 2 * step)
		return false;

	XPt2D A, B;
	double dist_ab, zA, zB, dA = 0., dB = 0., sample = 0., z;

	// Remplissage du tableau
	A = Pt(0);
	zA = Z(0);

	Alti.push_back(zA);
	/*
	for (uint32_t i = 1; i < length / step; i++) {
		sample = i * step;

		if (d < sample)
			for (; next < NbPt() - 1; next++) {
				B = Pt(next+1);
				zB = Z(next+1);
				dist_ab = dist(A, B);
				d += dist_ab;
				if (d >= sample) {
					z = (zB - zA) * sample / dist_ab + zB - d * (zB - zA) / dist_ab;
					Alti.push_back(z);
					A = B;
					zA = zB;
					break;
				}
				A = B;
				zA = zB;
			}
	}
	*/

	uint32_t p, q;
	for (uint32_t i = 1; i < NbPt(); i++) {
		B = Pt(i);
		zB = Z(i);
		dist_ab	= dist(A, B);
		dB = dA + dist_ab;

		p = (uint32_t)ceil(dA / step);
		q = (uint32_t)floor(dB / step);
		if (q >= p) {
			for (uint32_t k = p; k <= q; k++) {
				sample = k * step;
				z = (zA * (dB - sample) + zB * (sample - dA)) / dist_ab;
				Alti.push_back(z);
			}
		}

		A = B;
		zA = zB;
		dA = dB;
	}

	if (!loaded)
		Unload();

	return true;
}

//-----------------------------------------------------------------------------
// Echantillonnage de la planimetrie
//-----------------------------------------------------------------------------
bool XGeoVector::Position(std::vector<XPt3D>& Pos, double step)
{
	Pos.clear();
	if (NbPt() < 2)
		return false;
	if (step <= 0.1)
		return false;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	XPt3D A, B, M;
	double d;
	uint32_t nb_inter;
	B = Pt(0);
	for (uint32_t i = 1; i < NbPt(); i++) {
		A = B;
		B = Pt(i);
		Pos.push_back(A);
		d = dist(A, B);
		if (d < step)
			continue;
		nb_inter = (uint32_t)floor(d / step);
		for (uint32_t j = 1; j <= nb_inter; j++) {
			M = A + ((j * step)/d)*(B - A);
			Pos.push_back(M);
		}
	}
	Pos.push_back(B);

	if (!loaded)
		Unload();

	return true;
}

//-----------------------------------------------------------------------------
// Point d'accrochage pour les labels
//-----------------------------------------------------------------------------
XPt2D XGeoVector::LabelPoint(uint16_t* rot)
{
	if (NbPt() <= 1) {
		if (rot != NULL) *rot = 0;
		return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
	}
	if (NbPt() == 2) {
		if (rot != NULL) {
			if (LoadGeom()) {
				XPt2D A = Pt(0);
				XPt2D B = Pt(1);
				*rot = (uint16_t)XRint((asin( fabs(B.Y - A.Y) / dist(A, B)) * 180. / XPI));
				if ((A.X < B.X)&&(A.Y > B.Y))
					*rot = 360 - *rot;
				if ((A.X > B.X)&&(A.Y < B.Y))
					*rot = 360 - *rot;

				*rot = (uint16_t)XRint(*rot * 0.1) * 10;
				Unload();
			}
		}
		return m_Frame.Center();
	}
	if (LoadGeom()) {
		XPt2D A = Pt(0);
		XPt2D M = Pt(NbPt() / 2);
		XPt2D B = Pt(NbPt() - 1);
		double d = 0., l = Length() * 0.5;
		for (uint32_t i = 0; i < NbPt() - 1; i++) {
			A = Pt(i);
			B = Pt(i + 1);
			d += dist(A, B);
			if (d > l)
				break;
		}
		M = (A + B) * 0.5;

		if (rot != NULL) {
			*rot = (uint16_t)XRint((asin( fabs(B.Y - A.Y) / dist(A, B)) * 180. / XPI));
			if ((A.X < B.X)&&(A.Y > B.Y))
				*rot = 360 - *rot;
			if ((A.X > B.X)&&(A.Y < B.Y))
				*rot = 360 - *rot;
		}

		Unload();
		return M;
	}
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Barycentre 2D de la geometrie
//-----------------------------------------------------------------------------
XPt2D XGeoVector::Barycentre2D()
{
	if (NbPt() <= 1)
		return XPt2D(m_Frame.Xmin, m_Frame.Ymax);

	if (!LoadGeom())
		return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);

	XPt2D C;
	for (uint32_t i = 0; i < NbPt(); i++)
		C += Pt(i);
	C /= NbPt();
	return C;
}

//-----------------------------------------------------------------------------
// Centre compensé de la géométrie
//-----------------------------------------------------------------------------
XPt2D XGeoVector::Center2D(double d)
{
	if (NbPt() <= 1)
		return XPt2D(m_Frame.Xmin, m_Frame.Ymax);

	if (!LoadGeom())
		return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);

	XPt2D C, M;
	double d2 = d * d;
	// 1er point
	uint32_t nbpt = 1;
	C = Pt(0);
	M = Pt(0);

	for (uint32_t i = 1; i < NbPt() - 1; i++) {
		if (dist2(M, Pt(i)) < d2)
			continue;
		M = Pt(i);
		C += M;
		nbpt++;
	}

	// Dernier point
	C += Pt(NbPt() - 1);
	nbpt++;

	C /= nbpt;
	return C;
}

//-----------------------------------------------------------------------------
// Centroide de la geometrie
//-----------------------------------------------------------------------------
XPt2D XGeoVector::Centroide2D()
{
	XPt2D C = m_Frame.Center();
	if (!IsClosed())
		return C;
	if (!LoadGeom())
		return C;
//		return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);

	if (IsIn2D(C)) {Unload(); return C;}

	C = Barycentre2D();
	if (IsIn2D(C)) {Unload(); return C;}

	C += m_Frame.Center();
	C *= 0.5;
	if (IsIn2D(C)) {Unload(); return C;}

	// Recherche a partir du centre
	C = m_Frame.Center();
	XPt2D P;
	double dx , dy;
  uint16_t i;
  for (i = 0; i < 10; i++) {
    dx = FrameW() * 0.05 * i;
    dy = FrameH() * 0.05 * i;
		P = XPt2D(C.X - dx, C.Y);						// O
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X - dx, C.Y + dy);			// NO
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X, C.Y + dy);						// N
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X + dx, C.Y + dy);			// NE
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X + dx, C.Y);						// E
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X + dx, C.Y - dy);			// SE
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X, C.Y - dy);						// S
		if (IsIn2D(P)) {Unload(); return P;}
		P = XPt2D(C.X - dx, C.Y - dy);			// S0
		if (IsIn2D(P)) {Unload(); return P;}
	}

	// Recherche sur les diagonales
	XPt2D A, B;
	for (i = 1; i < NbPt() - 1; i++) {
		A = Pt(i - 1);
		B = Pt(i + 1);
		P = (A + B) * 0.5;
		if (IsIn2D(P)) {Unload(); return P;}
	}

	Unload();
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Centroide de la geometrie avec distance minimum
//-----------------------------------------------------------------------------
/*
XPt2D XGeoVector::Centroide2D(double d)
{
	XPt2D C = m_Frame.Center();
	if (!IsClosed())
		return C;

	if (!LoadGeom())
		return C;
	//	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);

	if (IsIn2D(C, d)) {Unload(); return C;}

	C = Barycentre2D();
	if (IsIn2D(C, d)) {Unload(); return C;}

	C += m_Frame.Center();
	C *= 0.5;
	if (IsIn2D(C, d)) {Unload(); return C;}

	XPt2D P;
	double dx , dy;
  int i, j;
	C = m_Frame.Center();
	dx = Width() * 0.025;
	dy = Height() * 0.025;
  for (i = 1; i < 20; i++) {
		P = XPt2D(C.X - i * dx, C.Y + i * dy);
    for (j = -i; j <= i; j++) {
			if (IsIn2D(P, d)) {Unload(); return P;}
			P.X += dx;
		}

		P = XPt2D(C.X - i * dx, C.Y - i * dy);
		for (j = -i; j <= i; j++) {
			if (IsIn2D(P, d)) {Unload(); return P;}
			P.X += dx;
		}

		P = XPt2D(C.X - i * dx, C.Y + (i - 1) * dy);
		for (j = (1 - i); j < i; j++) {
			if (IsIn2D(P, d)) {Unload(); return P;}
			P.Y -= dy;
		}

		P = XPt2D(C.X + i * dx, C.Y + (i - 1) * dy);
		for (j = (1 - i); j < i; j++) {
			if (IsIn2D(P, d)) {Unload(); return P;}
			P.Y -= dy;
		}
	}
	Unload();
	if ((d >= 0.01 * Width())&&(d >= 0.01 * Height()))
		return Centroide2D(d*0.75);
	return Centroide2D();
}
*/
XPt2D XGeoVector::Centroide2D(double dist_min)
{
  XPt2D C = m_Frame.Center();
  if (!IsClosed())
    return C;

  if (!LoadGeom())
    return C;
  //	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
  double d = dist_min;
  while((d >= 0.01 * FrameW())&&(d >= 0.01 * FrameH())) {
    if (IsIn2D(C, d)) {Unload(); return C;}

    C = Barycentre2D();
    if (IsIn2D(C, d)) {Unload(); return C;}

    C += m_Frame.Center();
    C *= 0.5;
    if (IsIn2D(C, d)) {Unload(); return C;}

    XPt2D P;
    double dx , dy;
    int i, j;
    C = m_Frame.Center();
    dx = FrameW() * 0.025;
    dy = FrameH() * 0.025;
    for (i = 1; i < 20; i++) {
      P = XPt2D(C.X - i * dx, C.Y + i * dy);
      for (j = -i; j <= i; j++) {
        if (IsIn2D(P, d)) {Unload(); return P;}
        P.X += dx;
      }

      P = XPt2D(C.X - i * dx, C.Y - i * dy);
      for (j = -i; j <= i; j++) {
        if (IsIn2D(P, d)) {Unload(); return P;}
        P.X += dx;
      }

      P = XPt2D(C.X - i * dx, C.Y + (i - 1) * dy);
      for (j = (1 - i); j < i; j++) {
        if (IsIn2D(P, d)) {Unload(); return P;}
        P.Y -= dy;
      }

      P = XPt2D(C.X + i * dx, C.Y + (i - 1) * dy);
      for (j = (1 - i); j < i; j++) {
        if (IsIn2D(P, d)) {Unload(); return P;}
        P.Y -= dy;
      }
    }
    Unload();
    d = d*0.75;
  }
  return Centroide2D();
}

//-----------------------------------------------------------------------------
// Indique si un point est contenu dans la geometrie avec une distance limite
//-----------------------------------------------------------------------------
bool XGeoVector::IsIn2D(const XPt2D& P, double d)
{
	if (!IsIn2D(P))
		return false;
	double rd = sqrt(d);
	XPt2D M = XPt2D(P.X - rd, P.Y - rd);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X + rd, P.Y - rd);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X + rd, P.Y + rd);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X - rd, P.Y + rd);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X - d, P.Y);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X + d, P.Y);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X, P.Y - d);
	if (!IsIn2D(M))
		return false;
	M = XPt2D(P.X, P.Y + d);
	if (!IsIn2D(M))
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Indique si un point
//-----------------------------------------------------------------------------
bool XGeoVector::IsNear3D(XPt2D M, double dist, XFrame* F, double zmin, double zmax,
													XPt3D& A, XPt3D& B, double Zfactor)
{
	if (!Is3D())
		return false;
	if (!LoadGeom())
		return false;
	double X0 = F->Xmin, Y0 = F->Ymax;
	XPt2D P, T;
	double z, d2 = dist * dist;
	for (uint32_t i = 0; i < NbPt(); i++) {
		P = Pt(i);
		z = Z(i) * Zfactor;
		T.X = (P.X - X0)*A.X + (Y0 - P.Y) * A.Y + (zmax - z) * A.Z;
		T.Y = (P.X - X0)*B.X + (Y0 - P.Y) * B.Y + (zmax - z) * B.Z;
		if (dist2(M, T) < d2) {
			Unload();
			return true;
		}
	}
	Unload();
	return false;
}

//-----------------------------------------------------------------------------
// Regarde si un point est contenu dans le polygone
//-----------------------------------------------------------------------------
bool XGeoVector::IsIn2D(const XPt2D& P)
{
	if (!IsClosed())
		return false;
  if (!m_Frame.IsIn(P))
    return false;
	double alpha, delta = 0.0;
	int nb_inter = 0;
	XPt2D A, B, C;
	bool loaded = true, inside = false;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	int i, j, k;
	for (i = 0; i < NbPart(); i++) {
		if (i == NbPart() - 1)
			k = NbPt();
		else
			k = Part(i + 1);
		for (j = Part(i); j < k - 1; j++) {
			A = Pt(j);
			B = Pt(j + 1);
			if (A == P){
				nb_inter = 1;
				if (!loaded)
					Unload();
				return true;
			}
			alpha = ((A.X - A.Y) -  (P.X + delta - P.Y)) / ((A.X - A.Y) -  (B.X - B.Y));
			if (alpha == 0.0) {
				delta += 0.05;
				nb_inter = 0;
				i = -1;
				j = k;
				continue;
			}

			if ((alpha <= 0.0)||(alpha > 1.0))
				continue;
			C.X = A.X + alpha * (B.X - A.X) - P.X;
			C.Y = A.Y + alpha * (B.Y - A.Y) - P.Y;
			if ((C.X > 0.0)&&(C.Y > 0.0))
				nb_inter++;
		}
	}

	if ((nb_inter % 2) != 0) 	// Nombre impair d'intersection
		inside = true;
	if (!loaded)
		Unload();
	return inside;
}

//-----------------------------------------------------------------------------
// Test si un segment passe sur la geometrie
//-----------------------------------------------------------------------------
bool XGeoVector::IsIn2D(const XPt2D& M, const XPt2D& P)
{
	XPt2D A, B;
	double prod1, prod2;
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}
	uint32_t i, j, k;
	for (i = 0; i < NbPart(); i++) {
		if (i == NbPart() - 1)
			k = NbPt();
		else
			k = Part(i + 1);
		for (j = Part(i); j < k - 1; j++) {
			A = Pt(j);
			B = Pt(j+1);

			prod1 = prodCross(M, A, B);
			prod2 = prodCross(P, A, B);
			if (prod1 * prod2 > 0.)
				continue;
			prod1 = prodCross(A, M, P);
			prod2 = prodCross(B, M, P);
			if (prod1 * prod2 < 0.) {
				if (!loaded)
					Unload();
				return true;
			}
		}
	}
	if (!loaded)
		Unload();
	return false;
}

//-----------------------------------------------------------------------------
// Test si la geometrie contient entierement un cadre
//-----------------------------------------------------------------------------
bool XGeoVector::IsIn2D(const XFrame& F)
{
	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	XPt2D A(F.Xmin, F.Ymin), B(F.Xmax, F.Ymin), C(F.Xmin, F.Ymax), D(F.Xmax, F.Ymax);
	bool out = true;
	if (!IsIn2D(A))
		out = false;
	if (out)
		if (!IsIn2D(B))
			out = false;
	if (out)
		if (!IsIn2D(C))
			out =  false;
	if (out)
		if (!IsIn2D(D))
			out = false;
	/*
	if (!IsIn2D(A, B))
		return false;
	if (!IsIn2D(A, C))
		return false;
	if (!IsIn2D(C, D))
		return false;
	if (!IsIn2D(B, D))
		return false;
	*/
	if (!loaded)
		Unload();
	return out;
}

//-----------------------------------------------------------------------------
// Test si la geometrie intersecte un cadre
//-----------------------------------------------------------------------------
bool XGeoVector::Intersect(const XFrame& F)
{
  // Le cadre peut contenir toute la geometrie
  if (m_Frame.IsIn(F))
    return true;
  // Le cadre peut contenir 2 points du cadre de la geometrie => intersection
  uint32_t nb_inter = 0;
  if (F.IsIn(XPt2D(m_Frame.Xmin, m_Frame.Ymin))) nb_inter++;
  if (F.IsIn(XPt2D(m_Frame.Xmin, m_Frame.Ymax))) nb_inter++;
  if (F.IsIn(XPt2D(m_Frame.Xmax, m_Frame.Ymax))) nb_inter++;
  if (F.IsIn(XPt2D(m_Frame.Xmax, m_Frame.Ymin))) nb_inter++;
  if (nb_inter > 1)
    return true;

	bool loaded = true;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	XPt2D A(F.Xmin, F.Ymin), B(F.Xmax, F.Ymin), C(F.Xmin, F.Ymax), D(F.Xmax, F.Ymax);
	bool inter = false;
	// Les sommets du cadre sont-ils dans la geometrie
	if (IsIn2D(A))
		inter = true;
	if (!inter)
		if (IsIn2D(B))
			inter = true;
	if (!inter)
		if (IsIn2D(C))
			inter = true;
	if (!inter)
		if (IsIn2D(D))
			inter = true;

	// Les bord du cadre passent-ils sur la geometrie
	if (!inter)
		if (IsIn2D(A, B))
			inter = true;
	if (!inter)
		if (IsIn2D(A, C))
			inter = true;
	if (!inter)
		if (IsIn2D(C, D))
			inter = true;
	if (!inter)
		if (IsIn2D(B, D))
			inter = true;

	// Le cadre peut contenir integralement la geometrie
	if (!inter) {
		if (NbPt() > 0) {
			XPt2D P = Pt(0);
			inter = F.IsIn(P);
		}
	}

	if (!loaded)
		Unload();
	return inter;
}

//-----------------------------------------------------------------------------
// Test si la geometrie intersecte un segment :
//    3 si confondu avec un arc de la geometrie
//    2 si intersection a un sommet
//    1 si intersection
//    0 si aucune intersection
//    -1 si confondu partiellement
//-----------------------------------------------------------------------------
int XGeoVector::Intersect(const XPt2D& M, const XPt2D& P, double epsilon)
{
  XPt2D A, B;
  double prod1, prod2;
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return false;
  }
  uint32_t i, j, k;
  bool sommet = false;
  for (i = 0; i < NbPart(); i++) {
    if (i == NbPart() - 1)
      k = NbPt();
    else
      k = Part(i + 1);
    for (j = Part(i); j < k - 1; j++) {
      A = Pt(j);
      B = Pt(j+1);

      if (A.egal(M, epsilon)) {
        if (B.egal(P, epsilon))
          return 3; // AB = MP
        prod1 = prodCross(P, A, B);
        if (fabs(prod1) < epsilon)
          return -1;
        sommet = true;
      }
      if (A.egal(P, epsilon)) {
        if (B.egal(M, epsilon))
          return 3; // AB = PM
        prod1 = prodCross(M, A, B);
        if (fabs(prod1) < epsilon)
          return -1;
        sommet = true;
      }

      if (B.egal(M, epsilon)) {
        if (A.egal(P, epsilon))
          return 3; // BA = MP
        prod1 = prodCross(P, A, B);
        if (fabs(prod1) < epsilon)
          return -1;
        sommet = true;
      }
      if (B.egal(P, epsilon)) {
        if (A.egal(M, epsilon))
          return 3; // BA = PM
        prod1 = prodCross(M, A, B);
        if (fabs(prod1) < epsilon)
          return -1;
        sommet = true;
      }

      prod1 = prodCross(M, A, B);
      prod2 = prodCross(P, A, B);
      if (prod1 * prod2 > 0.)
        continue;
      prod1 = prodCross(A, M, P);
      prod2 = prodCross(B, M, P);
      if (prod1 * prod2 < 0.) {
        if (!loaded)
          Unload();
        return 1; // Intersection en un point
      }
    }
  }
  if (!loaded)
    Unload();
  if (sommet)
    return 2;
  return 0; // Aucune intersection
}

//-----------------------------------------------------------------------------
// Test si la geometrie s'auto-interesecte
//-----------------------------------------------------------------------------
bool XGeoVector::AutoIntersect(XPt2D& M, bool doublons)
{
	if (NbPt() < 3)
		return false;

	uint32_t step = 1;
	if (!doublons)
		step = 2;		// On ne veut pas les doublons

	bool loaded = true, flag = false;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	XPt2D A, B;
	for (uint32_t i = 0; i < NbPt() - 1; i++) {
		A = Pt(i);
		for (uint32_t j = i + step; j < NbPt() - 1; j++) {
			B = Pt(j);
			if (A == B) {
				flag = true;
				M = A;
				if (NbPart() <= 1)
					break;
				for (uint32_t k = 0; k < NbPart() - 1; k++) {
					if ((i == Part(k)) &&(j == Part(k+1) - 1))
						flag = false;
				}
				if (flag)
					break;
			}
		}
		if (flag)
			break;
	}

	if (!loaded)
		Unload();

	return flag;
}

//-----------------------------------------------------------------------------
// Test si la geometrie a des doublons
//-----------------------------------------------------------------------------
bool XGeoVector::Doublons(XPt2D& M)
{
	if (NbPt() < 2)
		return false;

	bool loaded = true, flag = false;
	if (!IsLoaded()) {
		loaded = false;
		if (!LoadGeom())
			return false;
	}

	XPt2D A, B;
	A = Pt(0);

	uint32_t nbpt;
	for (uint32_t i = 0; i < NbPart(); i++) {
		if (i < NbPart() - 1)
			nbpt = Part(i+1) - Part(i);
		else
			nbpt = NbPt() - Part(i);
		A = Pt(Part(i));
		for (uint32_t j = Part(i)+1; j < Part(i) + nbpt; j++) {
			B =  Pt(j);
			if (A == B) {
				flag = true;
				M = A;
				break;
			}
			A = B;
		}
		if (flag)
			break;
	}

	if (!loaded)
		Unload();

	return flag;
}

//-----------------------------------------------------------------------------
// Donne le ZMax, ZMin, ZMean sur un cadre et renvoie le nombre de noeuds
//-----------------------------------------------------------------------------
uint32_t  XGeoVector::ZFrame(const XFrame& F, double* zmax, double* zmin, double* zmean)
{
  *zmax = *zmin = *zmean = XGEO_NO_DATA;
  if (!Is3D())
    return 0;
  if (!Intersect(F))
    return 0;
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return 0;
  }

  XPt2D P;
  double newZ;
  uint32_t nbz = 0;
  *zmean = 0.;
  *zmin = 1e31;
  *zmax = -1e31;
  for(uint32_t i = 0; i < NbPt(); i++) {
    P = Pt(i);
    if (!F.IsIn(P))
      continue;
    newZ = Z(i);
    if (newZ > XGEO_NO_DATA) {
      nbz++;
      *zmean += newZ;
      *zmin = XMin(*zmin, newZ);
      *zmax = XMax(*zmax, newZ);
    }
  }
  if (nbz > 0)
    *zmean = *zmean / nbz;
  else
    *zmax = *zmin = *zmean = XGEO_NO_DATA;

  if (!loaded)
    Unload();

  return nbz;
}

//-----------------------------------------------------------------------------
// Donne le ZMean
//-----------------------------------------------------------------------------
double XGeoVector::ZMean()
{
  if (!Is3D())
    return XGEO_NO_DATA;
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return XGEO_NO_DATA;
  }

  double zmean = 0;
  double newZ;
  uint32_t nbz = 0;
  for(uint32_t i = 0; i < NbPt(); i++) {
    newZ = Z(i);
    if (newZ > XGEO_NO_DATA) {
      nbz++;
      zmean += newZ;
    }
  }
  if (nbz > 0)
    zmean = zmean / nbz;
  else
    zmean = XGEO_NO_DATA;

  if (!loaded)
    Unload();

  return zmean;
}

//-----------------------------------------------------------------------------
// Generalisation du trace (algo Visvalingam–Whyatt)
//-----------------------------------------------------------------------------
bool XGeoVector::Generalize(std::vector<XPt2D>& T, uint32_t nb_max_pt)
{
  T.clear();
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return false;
  }

  if (NbPt() < 3) {
    for (uint32_t i = 0; i < NbPt(); i++)
      T.push_back(Pt(i));
    return true;
  }

  std::vector<XPt2D> V;
  uint32_t nbpt = NbPt();
  if (NbPart() > 1)
    nbpt = Part(1); // On ne gere pas les trous
  if (IsClosed())
    nbpt--;
  for (uint32_t i = 0; i < nbpt; i++)
    V.push_back(Pt(i));

  double min_area = 0., tri_area = 0.;
  uint32_t index = 0;
  while(V.size() > nb_max_pt) {
    for (uint32_t i = 0; i < V.size(); i++) {
      if (i == 0) {
        min_area = area(V[V.size()-1], V[0], V[1]);
        index = 0;
        continue;
      }
      if (i == (V.size()-1)) {
        tri_area = area(V[V.size()-2], V[V.size()-1], V[0]);
        if (tri_area < min_area) {
          min_area = tri_area;
          index = i;
        }
        continue;
      }
      tri_area = area(V[i-1], V[i], V[i+1]);
      if (tri_area < min_area) {
        min_area = tri_area;
        index = i;
      }
    }
    V.erase(V.begin() + index);
  }
  if (IsClosed()) {
    // Recherche du point Bottom Left
    uint32_t botleft = 0;
    XPt2D BL = V[0];
    for (uint32_t i = 1; i < V.size(); i++){
      if (V[i].Y < BL.Y) {
        BL = V[i];
        botleft = i;
        continue;
      }
      if (fabs(V[i].Y - BL.Y) < 0.01)
        if (V[i].X < BL.X) {
          BL = V[i];
          botleft = i;
        }
    }
    std::rotate(V.begin(), V.begin()+botleft, V.end());
    // Verification du sens
    if (V[V.size() - 1].X > V[0].X)
      std::reverse(V.begin(), V.end());

    V.push_back(V[0]);
  }
  // Filtrage
  T.push_back(V[0]);
  for (uint32_t i = 1; i < V.size()-1; i++) {
    if (dist_droite(V[i-1], V[i+1], V[i]) > 0.1)
      T.push_back((V[i]));
  }
  T.push_back(V[0]);
  if (T.size() < 4)
    T = V;
  if (!loaded)
    Unload();
  return true;
}

//-----------------------------------------------------------------------------
// Generalisation du trace en gardant les angles importants
//-----------------------------------------------------------------------------
bool XGeoVector::Generalize(std::vector<XPt2D>& T, double min_angle)
{
  T.clear();
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom())
      return false;
  }

  if (NbPt() < 3) {
    for (uint32_t i = 0; i < NbPt(); i++)
      T.push_back(Pt(i));
    return true;
  }

  XPt2D J, K, L;
  double alpha = 0.;
  uint32_t last_in;
  std::vector<XPt2D> Pt_in;

  for (uint32_t part = 0; part < NbPart(); part++) {
    Pt_in.clear();
    if (part == NbPart() - 1)
      last_in = NbPt();
    else
      last_in = Part(part + 1);
    for (uint32_t j = Part(part); j < last_in; j++)
      Pt_in.push_back(Pt(j));

    for (uint32_t j = 0; j < Pt_in.size() - 1; j++) {
      K = Pt_in[j];
      if (j > 0)
        J = Pt_in[j-1];
      else
        J = Pt_in[Pt_in.size()-2];
      if (T.size() > 0) J = T[T.size() - 1];
      L = Pt_in[j+1];
      //alpha = prodScal((K-J), (K-L)) / (dist(K,J)*dist(K,L));
      alpha = acos(prodScal((J-K), (L-K)) / (dist(K,J)*dist(K,L)));
      if (prodCross(J, K, L) < 0)
        alpha = 2*XPI - alpha;
      if (fabs(alpha - XPI) < min_angle) continue;  // Point aligne
      T.push_back(K);
    }
  }

  if (!loaded)
    Unload();
  return true;
}

//-----------------------------------------------------------------------------
// Donne les intersections en X pour la droite y = Y
//-----------------------------------------------------------------------------
bool XGeoVector::XIntersector(std::vector<double>& T, double Y, double epsilon)
{
  T.clear();
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom2D())
      return false;
  }

  XPt2D A, B;
  double alpha, Xinter;
  int i, j, k;
  for (i = 0; i < NbPart(); i++) {
    if (i == NbPart() - 1)
      k = NbPt();
    else
      k = Part(i + 1);
    for (j = Part(i); j < k - 1; j++) {
      A = Pt(j);
      B = Pt(j + 1);
      if ((A.Y > Y)&&(B.Y > Y)) // AB : segment au dessus de la droite
        continue;
      if ((A.Y < Y)&&(B.Y < Y)) // AB : segment en dessous de la droite
        continue;
      if (fabs(B.X - A.X) < epsilon) {  // AB : segment vertical
        T.push_back((B.X + A.X) * 0.5);
        continue;
      }
      if (fabs(B.Y - A.Y) < epsilon) {  // AB : segment horizontal
        //T.push_back(A.X);
        //T.push_back(B.X);
        continue;
      }
      // Intersection
      alpha = (B.Y - A.Y)/(B.X - A.X);  // Coefficient directeur de AB
      Xinter = (Y - A.Y) / alpha + A.X;
      T.push_back(Xinter);
    }
  }

  if (!loaded)
    Unload();
  return true;
}

//-----------------------------------------------------------------------------
// Rasterisation
//-----------------------------------------------------------------------------
bool XGeoVector::Rasterize(double gsd, double& Xmin, double& Ymax, uint32_t& W, uint32_t& H, uint8_t** area)
{
  if (!IsClosed())
    return false;
  bool loaded = true;
  if (!IsLoaded()) {
    loaded = false;
    if (!LoadGeom2D())
      return false;
  }

  XFrame F = Frame();
  double X0 = floor(F.Xmin /gsd) * gsd;
  double Y0 = ceil(F.Ymax / gsd) * gsd;
  W = (uint32_t)ceil(F.Width() / gsd) + 1;
  H = (uint32_t)ceil(F.Height() / gsd) + 1;
  uint8_t* buf = new uint8_t[W * H];
  ::memset(buf, 0, W*H);
  for (uint32_t i = 0; i < H; i++) {
    uint8_t* line = &buf[W*i];
    for (int step = 0; step < 4; step++) {
      double Y = Y0 - i * gsd + step * gsd * 0.25;
      std::vector<double> T;
      XIntersector(T, Y);
      if (T.size() < 2)
        continue;
      std::sort(T.begin(), T.end());
      std::vector<uint32_t> U;
      for (uint32_t j = 0; j < T.size(); j++) {
        U.push_back(XRint(((T[j] - X0) / gsd)));
      }
      std::vector<uint32_t>::iterator iter = std::unique(U.begin(), U.end());
			U.erase(iter, U.end());
      for (uint32_t j = 0; j < U.size() / 2; j++) {
        for (uint32_t k = U[j*2]; k < U[j*2 + 1]; k++)
          line[k] += 1;
      }
    }
    for (uint32_t j = 0; j < W; j++)
      if (line[j] < 2)
        line[j] = 0;
    else
        line[j] = 255;
  }
  Xmin = X0;
  Ymax = Y0;
  *area = buf;

  if (!loaded)
    Unload();
  return true;
}


//-----------------------------------------------------------------------------
// Ecriture en GeoJSON
//-----------------------------------------------------------------------------
bool XGeoVector::WriteGeoJson(std::ostream* json, std::vector<std::string>* Att)
{
  if (!LoadGeom())
    return false;

  json->setf(std::ios::fixed);
  if ((m_Frame.Xmax < 180)&&(m_Frame.Ymax < 180))
    json->precision(6);
  else
    json->precision(2);

  *json << "{\n \"type\": \"Feature\",\n";
  *json << " \"geometry\": {\n";
  *json << "  \"type\": ";

  switch(TypeVector()) {
    case Point : *json << "\"Point\",\n"; break;
    case PointZ : *json << "\"Point\",\n"; break;
    case MPoint : *json << "\"MultiPoint\",\n"; break;
    case MPointZ : *json << "\"MultiPoint\",\n"; break;
    case Line : *json << "\"LineString\",\n"; break;
    case LineZ : *json << "\"LineString\",\n"; break;
    case MLine : *json << "\"MultiLineString\",\n"; break;
    case MLineZ : *json << "\"MultiLineString\",\n"; break;
    case Poly : *json << "\"Polygon\",\n"; break;
    case PolyZ : *json << "\"Polygon\",\n"; break;
    case MPoly : *json << "\"MultiPolygon\",\n"; break;
    case MPolyZ : *json << "\"MultiPolygon\",\n"; break;
		case Raster: *json << "\"Polygon\",\n"; break;	// On ecrit l'emprise du raster
		case DTM: *json << "\"Polygon\",\n"; break; // On ecrit l'emprise du MNT
		case LAS: *json << "\"Polygon\",\n"; break; // On ecrit l'emprise du LAS
		default: return false;
  }

  *json << "  \"coordinates\": ";
  if (NbPt() == 1) {
    *json << "[" << m_Frame.Xmin << " , " << m_Frame.Ymax;
    if (Z(0) > XGEO_NO_DATA)
      *json << " , " << Z(0) << "]";
    else
      *json << "]";
  } else {
    XPt2D P;
    double z;
    uint32_t part = 1;
    if (NbPart() > 1)
      *json << "[ [ ";
    else
      *json << "[ ";
    for (uint32_t i = 0; i < NbPt(); i++) {
      P = Pt(i);
      z = Z(i);
      for (uint32_t j = part; j < NbPart(); j++)
        if (i == Part(j)) {
          part ++;
          *json << "] [";
        }
      *json << "[" << P.X << " , " << P.Y;
      if (z > XGEO_NO_DATA)
        *json << " , " << z << "] ";
      else
        *json << "] ";
      if (i < (NbPt() - 1)) {
        if (part == NbPart())
          *json << " , ";
        else
          if (i != Part(part))
            *json << " , ";
      }
    }
    if (NbPart() > 1)
      *json << "] ] ";
    else
      *json << "] ";
  }

  Unload();

  *json << "\n }";  // Fin de la geometrie

  std::vector<std::string> V;
  if (Att == NULL) {
    if (!ReadAttributes(V)) {
      *json << "}";   // Fin de la feature
      return true;
    }
  } else
    V = *Att;
  if (V.size() > 0) {
    *json << ",\n \"properties\": {\n";
    for (uint32_t i = 0; i < V.size() / 2; i++) {
      if (V[2*i+1].size() < 1)
        continue;
      if (V[2*i+1] == " ")
        continue;
      *json << "   \"" << V[2*i] << "\":\"" << V[2*i+1] << "\"";
      if (i != (V.size() / 2 - 1))
        *json << ",\n";
    }
    *json << " }\n";
  }

  *json << "}";   // Fin de la feature

/*
    { "type": "Feature",
            "geometry": {
              "type": "LineString",
              "coordinates": [
                [102.0, 0.0], [103.0, 1.0], [104.0, 0.0], [105.0, 1.0]
                ]
              },
            "properties": {
              "prop0": "value0",
              "prop1": 0.0
              }
            }
*/

  return true;
}

//-----------------------------------------------------------------------------
// Predicat pour les tries en X
//-----------------------------------------------------------------------------
bool predXGeoVectorX(XGeoVector* A, XGeoVector* B)
{
	if (A->Frame().Xmin < B->Frame().Xmin)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Predicat pour les tries en fonction de la longueur
//-----------------------------------------------------------------------------
bool predXGeoVectorLength(XGeoVector* A, XGeoVector* B)
{
  if (A->Length() > B->Length())
    return true;
  return false;
}
