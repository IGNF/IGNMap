//-----------------------------------------------------------------------------
//								XTAChantier.h
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 12/01/2004
//-----------------------------------------------------------------------------

#ifndef _XTACHANTIER_H
#define _XTACHANTIER_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoVector.h"
#include "../XTool/XGeoPoly.h"

class XGeoBase;

//-----------------------------------------------------------------------------
// Chantier TA
//-----------------------------------------------------------------------------
class XTAChantier : public XGeoMap {
protected:
	std::string				m_strProjection;
	std::string				m_strImagePath;

public:
	void Class(XGeoClass* C);

	void ImagePath(const char* path) { m_strImagePath = path;}
	std::string ImagePath() { return m_strImagePath;}

	bool Read(const char* filename, XError* error = NULL);

	static XGeoClass* ImportTA(XGeoBase* base, const char* path, XGeoMap* map = nullptr);
};

//-----------------------------------------------------------------------------
// Cliche
//-----------------------------------------------------------------------------
class XTACliche : public XGeoPoly2D {
public:
	enum eStyle { Standard, MultiSpectral, DoubleFauchee, Argentique, AvantArriere};
	enum eVisee { Verticale, Gauche, Droite, Avant, Arriere};
	enum eQualite { Bonne = 0, Nuage = 2, Reacteur = 4, Surexpose = 8, Sousexpose = 16,
									Ombre = 32,Reflet = 64, AutreDefaut = 128};
	enum eOrientation { N2S, S2N, E2W, W2E};

protected:
	// Chantier
	XTAChantier*	m_TA;
	
	// Donnees du XTAElement
	uint32_t			m_nNum;						// Numero
	eQualite		m_Qualite;				// Qualite
	std::string	m_strNote;				// Commentaires
	
	// Donnees du XCliche
	std::string		m_strImage;		// Nom de l'image
	std::string		m_strOrigine;	// Nom du capteur
	XPt3D					m_CoorTraj;		// Coordonnees brutes de trajectographie
	uint32_t				m_nSection;		// Section GPS trajecto ou 0 si navigation
	bool					m_bNavInterp;	// Indique si les coordonnees de navigation ont ete interpolees
	bool					m_bZI;				// Indique que le cliche touche une ZI
	std::string		m_strDispo;		// Indique que le cliche est disponible a la phototheque
	eStyle				m_Style;			// Style du cliche : standard, multispectral, ...
	eVisee				m_Visee;			// Type de visee : verticale, avant, arriere, ...
	eOrientation	m_Orientation;// Orientation du capteur
	uint32_t				m_nDate;			// Date
	double				m_dTime;			// Heure
	double				m_dResolMin;	// Resolution minimum
	double				m_dResolMoy;	// Resolution moyenne
	double				m_dResolMax;	// Resolution maximum
	double				m_dOverlap;		// Recouvrement moyen
	double				m_dOverMin;		// Recouvrement minimum
	double				m_dOverMax;		// Recouvrement maximum
	double				m_dSunHeight;	// Hauteur du soleil (en degres)

	// Donnees du XModel
	XPt3D					m_S;					// Sommet de prise de vue
	double				m_dKappa;
	double				m_dOmega;
	double				m_dPhi;

	// Donnees de la chambre pour les prises de vues argentiques
	uint32_t			m_nCameraW;	// Largeur utile de la chambre en mm
	uint32_t			m_nCameraH;	// Hauteur utile de la chambre en mm

public:
	XTACliche(XTAChantier* ta = NULL, uint32_t date = 0, uint32_t ori = 0) 
	{
		m_TA = ta; m_nDate = date; m_Orientation = (eOrientation)ori; 
		m_nCameraW = m_nCameraH = 0;
		m_dKappa = m_dOmega = m_dPhi = m_dTime = 0.;
		m_dResolMin = m_dResolMoy = m_dResolMax = m_dOverlap = m_dOverMin = m_dOverMax = m_dSunHeight = 0.;
		m_bNavInterp = m_bZI = false;
		m_nNum = m_nSection = 0;
		m_Style = Standard;
		m_Qualite = Bonne;
		m_Visee = Verticale;
	}

	virtual void Unload() { ;}
	virtual bool LoadGeom() { return true;}

	virtual std::string Name() { char buf[20]; snprintf(buf, 20, "%u", m_nNum); return buf;}
	virtual std::string ImageName() { return m_strImage;}

	virtual inline XGeoMap* Map() const { return m_TA;}

	double Resol() const { return m_dResolMoy;}
	double Cap() const { 	double cap = 90. - (m_dKappa * 180.)/XPI; if(cap < 0) cap += 360.; return cap;}
	bool IsDigital() const { if (m_Style != Argentique) return true; return false;}
	eOrientation Orientation() const { return m_Orientation;}
	void SetCameraDimension(uint32_t w, uint32_t h) { m_nCameraW = w; m_nCameraH = h; }
	uint32_t CameraW() const { return m_nCameraW; }
	uint32_t CameraH() const { return m_nCameraH; }

	virtual XPt2D Centroide() { return m_S;}
	virtual bool HasCentroide() { return true;}

	virtual	bool ReadAttributes(std::vector<std::string>& V);

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0);
};

#endif //_XTACHANTIER_H