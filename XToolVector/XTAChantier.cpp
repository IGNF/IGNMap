//-----------------------------------------------------------------------------
//								XTAChantier.cpp
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 12/01/2004
//-----------------------------------------------------------------------------

#include "XTAChantier.h"
#include "../XTool/XParserXML.h"
#include "../XTool/XXml.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XPolygone2D.h"
#include "../XToolAlgo/XTime.h"
#include "../XTool/XPath.h"
#include <sstream>

//-----------------------------------------------------------------------------
// Import d'un TA dans une XGeoBase
//-----------------------------------------------------------------------------
XGeoClass* XTAChantier::ImportTA(XGeoBase* base, const char* path, XGeoMap* /*map*/)
{
	XPath P;
	XTAChantier* file = new XTAChantier;
	XGeoClass* C = nullptr;

	if (file->Read(path)) {
		C = base->AddClass("TA", P.Name(path, false).c_str());
		file->Class(C);
		base->AddMap(file);
		base->SortClass();
		XGeoRepres* repres = C->Repres();
		if (repres != nullptr) {
			repres->Name("TA");
			repres->FillColor(0xFFFFFF00);
			repres->Color(0xFFAA1100);
		}
	}
	else
		delete file;
		
	return C;
}

//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XTAChantier::Class(XGeoClass* C)
{
	for (uint32_t i = 0; i < m_Data.size(); i++){
		XGeoVector* record = (XGeoVector*)m_Data[i];
		record->Class(C);
		C->Vector(record);
	}
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier
//-----------------------------------------------------------------------------
bool XTAChantier::Read(const char* filename, XError* error)
{
	XParserXML parser;
	if (!parser.Parse(filename))
		return XErrorError(error, "XTAChantier::XmlRead", XError::eBadFormat);

	XParserXML chantier = parser.FindSubParser("/TA/chantier");
	if (chantier.IsEmpty())
		return XErrorError(error, "XTAChantier::XmlRead", XError::eBadFormat);

	// Donnees sur la PVA
	m_strName = XXml::XmlToOem(chantier.ReadNode("/chantier/nom"));
	m_strProjection = XXml::XmlToOem(chantier.ReadNode("/chantier/projection"));

	// Donnees image
	XPath P;
	m_strImagePath = P.Path(filename);

	// Lecture des vols
	uint32_t nb_vol = 0;
	uint32_t date, orientation = 0, camW = 0, camH = 0;
	XParserXML vol;
	while(true){
		vol = chantier.FindSubParser("/chantier/vol", nb_vol);
		if (vol.IsEmpty())
			break;
		date = vol.ReadNodeAsUInt32("/vol/date");
		// Lecture du capteur
		XParserXML system;
		system = vol.FindSubParser("/vol/system_sensor", 0);
		if (!system.IsEmpty()) {
			XParserXML sensor = system.FindSubParser("/system_sensor/sensor", 0);
			orientation = sensor.ReadNodeAsUInt32("/sensor/orientation");
			// Lecture de la dimension utile du sensor
			XParserXML useful_frame = sensor.FindSubParser("/sensor/usefull-frame", 0);
			if (!useful_frame.IsEmpty()) {
				XParserXML rect = useful_frame.FindSubParser("/usefull-frame/rect", 0);
				if (!rect.IsEmpty()) {
					camW = rect.ReadNodeAsUInt32("/rect/w");
					camH = rect.ReadNodeAsUInt32("/rect/h");
				}
			}
		}
		// Lecture des bandes
		uint32_t nb_bande = 0;
		XParserXML bande;
		while(true) {
			bande = vol.FindSubParser("/vol/bande", nb_bande);
			if (bande.IsEmpty())
				break;

			// Lecture des cliches
			uint32_t nb_cli = 0;
			XParserXML cliche;
			while (true) {
				cliche = bande.FindSubParser("/bande/cliche", nb_cli);
				XTACliche* cli = new XTACliche(this, date, orientation);
				if (!cli->XmlRead(&cliche)) {
					delete cli;
					break;
				}
				if (!AddObject(cli)) {
					delete cli;
					break;
				}
				cli->SetCameraDimension(camW, camH);
				nb_cli++;
			}
		nb_bande++;

		}
		nb_vol++;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un cliche
//-----------------------------------------------------------------------------
bool XTACliche::XmlRead(XParserXML* parser, uint32_t num)
{
	XParserXML cliche = parser->FindSubParser("/cliche", num);
	if (cliche.IsEmpty())
		return false;

	m_strImage = XXml::XmlToOem(cliche.ReadNode("/cliche/image"));
	m_strOrigine = XXml::XmlToOem(cliche.ReadNode("/cliche/origine"));
	m_nNum = cliche.ReadNodeAsUInt32("/cliche/number");
	Visible(cliche.ReadNodeAsBool("/cliche/actif"));
	m_bZI = cliche.ReadNodeAsBool("/cliche/zi");
	m_Qualite = (eQualite)cliche.ReadNodeAsInt("/cliche/qualite");
	m_strNote = XXml::XmlToOem(cliche.ReadNode("/cliche/note"));
	m_dTime = cliche.ReadNodeAsDouble("/cliche/time");
	if (m_dTime == 0.)
		m_dTime = -1.;
	m_dSunHeight = cliche.ReadNodeAsDouble("/cliche/sun_height");
	m_nSection = cliche.ReadNodeAsUInt32("/cliche/section");
	m_bNavInterp = cliche.ReadNodeAsBool("/cliche/nav_interpol");
	m_Style = (eStyle)cliche.ReadNodeAsInt("/cliche/style");
	m_Visee = (eVisee)cliche.ReadNodeAsInt("/cliche/visee");
	m_dResolMoy = cliche.ReadNodeAsDouble("/cliche/resolution_moy");
	m_dResolMin = cliche.ReadNodeAsDouble("/cliche/resolution_min");
	if (m_dResolMin == 0.) m_dResolMin = m_dResolMoy;
	m_dResolMax = cliche.ReadNodeAsDouble("/cliche/resolution_max");
	if (m_dResolMax == 0.) m_dResolMax = m_dResolMoy;
	m_dOverlap = cliche.ReadNodeAsDouble("/cliche/overlap");
	m_dOverMax = cliche.ReadNodeAsDouble("/cliche/overlap_max");
	if (m_dOverMax == 0.) m_dOverMax = m_dOverlap;
	m_dOverMin = cliche.ReadNodeAsDouble("/cliche/overlap_min");
	if (m_dOverMin == 0.) m_dOverMin = m_dOverlap;

	// Lecture du polygone d'emprise
	XParserXML polygon = parser->FindSubParser("/cliche/polygon2d");
	if (polygon.IsEmpty())
		return false;
	XPolygone2D poly;
	if (!poly.XmlRead(&polygon))
		return false;
	m_nNumPoints = poly.NbPt() + 1;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	for (uint32_t i = 0; i < poly.NbPt(); i++) {
		m_Pt[i].X = poly.Pt(i).X;
		m_Pt[i].Y = poly.Pt(i).Y;
	}
	m_Pt[poly.NbPt()] = m_Pt[0];
	m_Frame = poly.Frame();

	// Lecture du modele
	XParserXML model = parser->FindSubParser("/cliche/model");
	if (!model.IsEmpty()) {
		XParserXML sommet = model.FindSubParser("/model/pt3d");
		m_S.XmlRead(&sommet);
		m_dKappa = model.ReadNodeAsDouble("/model/kappa");
		m_dPhi = model.ReadNodeAsDouble("/model/phi");
		m_dOmega = model.ReadNodeAsDouble("/model/omega");

		// Lecture du quaternion
		XParserXML quater = model.FindSubParser("/model/quaternion");
		if (!quater.IsEmpty()) {
			double qx, qy, qz, qw;
			qx = quater.ReadNodeAsDouble("/quaternion/x");
			qy = quater.ReadNodeAsDouble("/quaternion/y");
			qz = quater.ReadNodeAsDouble("/quaternion/z");
			qw = quater.ReadNodeAsDouble("/quaternion/w");
			m_dOmega = atan2(2.*(qy*qz + qx*qw), qw*qw -qx*qx -qy*qy + qz*qz);
			m_dPhi = asin(-2.*(qx*qz - qw*qy));
			m_dKappa = atan2(2.*(qx*qy + qw*qz), qw*qw + qx*qx - qy*qy - qz*qz);
		}
	}

	XParserXML trajecto = parser->FindSubParser("/cliche/trajecto/pt3d");
	if (!trajecto.IsEmpty())
		m_CoorTraj.XmlRead(&trajecto);

	return true;

}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XTACliche::ReadAttributes(std::vector<std::string>& V)
{
	std::stringstream data;
	data.setf(std::ios::fixed);
	data.precision(2);
	V.clear();

	V.push_back("Numero");
	data << m_nNum;
	V.push_back(data.str());
	data.str("");

	V.push_back("Image");
	V.push_back(m_strImage);

	V.push_back("Note");
	V.push_back(m_strNote);

	// Style
	std::string style;
	switch(m_Style) {
		case Standard : style = "Standard"; break;
		case MultiSpectral : style = "Numerique multispectral"; break;
		case DoubleFauchee : style = "Numerique double fauchee"; break;
		case Argentique : style = "Argentique"; break;
		case AvantArriere : style = "Numerique avant arriere"; break;
	}
	V.push_back("Style");
	V.push_back(style);

	// Orientation
	std::string orientation;
	V.push_back("Orientation");
	switch (m_Orientation) {
		case N2S : orientation = "N->S";
		case S2N : orientation = "S->N";
		case E2W : orientation = "E->W";
		case W2E : orientation = "W->E";
	}
	V.push_back(orientation);

	// Qualite
	std::string Qualite;
	if(m_Qualite == 0)
		Qualite = "Bonne";
	if (m_Qualite &Nuage)
		Qualite += " Nuage";
	if (m_Qualite &Ombre)
		Qualite += " Ombre";
	if (m_Qualite &Reacteur)
		Qualite += " Reacteur";
	if (m_Qualite &Surexpose)
		Qualite += " Surexpose";
	if (m_Qualite &Sousexpose)
		Qualite += " Sous-expose";
	if (m_Qualite &Reflet)
		Qualite += " Reflet";
	if (m_Qualite &AutreDefaut)
		Qualite += " Defaut";
	V.push_back("Qualite");
	V.push_back(Qualite);

	data << m_S.Z;
	V.push_back("Altitude");
	V.push_back(data.str());
	data.str("");

	if (m_Style != Argentique) {
		V.push_back("Resolution moyenne");
		data << m_dResolMoy;
	} else {
		V.push_back("Echelle moyenne");
		data << m_dResolMoy * 1000;
	}

	V.push_back(data.str());
	data.str("");

	// Date et heure
	V.push_back("Date");
	V.push_back(XTime::DMYYToString(m_nDate));
	V.push_back("Heure");
	V.push_back(XTime::HMSDecToString(m_dTime));

	// Cap
	data << Cap();//cap en degres
	V.push_back("Cap (en degres)");
	V.push_back(data.str());
	data.str("");

	V.push_back("Hauteur du soleil (en degres)");
	data << (m_dSunHeight* 180.) / XPI;
	V.push_back(data.str());
	data.str("");

	V.push_back("Cliche en ZI");
	if (m_bZI)
		V.push_back("Oui");
	else
		V.push_back("Non");
	/*
	V.push_back("Lien");
	data << m_strImage << "?" << "xmin=" << m_Frame.Xmin
			 << "&ymax=" << m_Frame.Ymax << "&xmax=" << m_Frame.Xmax
			 << "&ymin=" << m_Frame.Ymin << "&cap=" << Cap();
	*/
	V.push_back(data.str());
	data.str("");

	return true;
}

//-----------------------------------------------------------------------------
// Nom du chemin image (sans extension
//-----------------------------------------------------------------------------
std::string XTACliche::ImagePath()
{
	std::string strTAFolder;
	if (m_TA != nullptr)
		strTAFolder = m_TA->ImagePath();
	XPath path;
	return path.FullName(strTAFolder.c_str(), m_strImage.c_str());
}