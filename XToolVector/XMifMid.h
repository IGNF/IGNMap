//-----------------------------------------------------------------------------
//								XMifmid.h
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/09/2003
//-----------------------------------------------------------------------------

#ifndef _XMIFMID_H
#define _XMIFMID_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoly.h"

class XGeodConverter;
class XGeoBase;

class XMifMid : public XGeoMap {
protected:
	uint16_t										m_nVersion;
	std::vector<std::string>	m_Column;
	std::vector<std::string>	m_Type;
	char											m_Sep;				// Separateur dans le fichier MID

	std::string								m_strFilename;
	std::ifstream							m_In;
	std::ifstream							m_Mid;				// Fichier de donnees
	uint32_t										m_nRecord;		// Position en cours dans le fichier de donnees

	std::vector<std::string>		m_Att;				// Derniers attributs lus
	uint32_t										m_nAttPos;		// Position des attributs

	uint32_t										m_nNbPoint;		// Nombre d'objets ponctuels
	uint32_t										m_nNbLine;		// Nombre d'objets lineaires
	uint32_t										m_nNbPoly;		// Nombre d'objets surfaciques

	std::vector<XGeoRepres*>	m_Repres;			// Representations stockee dans le fichier

	bool ReadHeader(XError* error = NULL);
	bool ReadColumns(uint16_t nb);

	XGeoRepres* AddRepres(XGeoRepres* repres);

public:
	XMifMid();
	virtual ~XMifMid();

	inline std::ifstream* IStream() { return &m_In;}

	void Class(XGeoClass* C);
	XGeoClass* Class();

	bool Read(const char* filename,  bool extract_repres = false, XError* error = NULL);
	bool ReadAttributes(uint32_t num, std::vector<std::string>& V);
	std::string FindAttribute(uint32_t num, std::string att_name);
	bool ReadRepres(XGeoRepres* repres);

	bool Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error = NULL);

	uint32_t MifColor(uint32_t c);

  static XGeoClass* ImportMifMid(XGeoBase* base, const char* path, XGeoMap* map = NULL);
};

//-----------------------------------------------------------------------------
// Point
//-----------------------------------------------------------------------------
class XMifMidPoint2D : public XGeoPoint2D {
protected :
	uint32_t				m_nId;						// Numero d'ordre dans le fichier
	XMifMid*				m_File;

public:
	XMifMidPoint2D(XMifMid* file = NULL, uint32_t id = 0) { m_File = file; m_nId = id;}

	inline XMifMid* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* /*out*/, XError* /*error*/ = NULL) { return false; }

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return m_File->ReadAttributes(m_nId, V);}
	virtual std::string FindAttribute(std::string att_name) { return m_File->FindAttribute(m_nId, att_name);}
};

//-----------------------------------------------------------------------------
// Ligne
//-----------------------------------------------------------------------------
class XMifMidLine2D : public XGeoLine2D {
protected :
	uint32_t				m_nId;						// Numero d'ordre dans le fichier
	std::streampos	m_Pos;						// Position de la geometrie dans le fichier
	XMifMid*				m_File;

public:
	XMifMidLine2D(XMifMid* file = NULL, uint32_t id = 0) { m_File = file; m_nId = id; m_Pos = 0; }

	inline XMifMid* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* /*out*/, XError* /*error*/ = NULL) { return false; }

	virtual bool LoadGeom();

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return m_File->ReadAttributes(m_nId, V);}
	virtual std::string FindAttribute(std::string att_name) { return m_File->FindAttribute(m_nId, att_name);}
};

//-----------------------------------------------------------------------------
// PolyLigne
//-----------------------------------------------------------------------------
class XMifMidMLine2D : public XGeoMLine2D {
protected :
	uint32_t				m_nId;						// Numero d'ordre dans le fichier
	std::streampos	m_Pos;						// Position de la geometrie dans le fichier
	XMifMid*				m_File;

public:
	XMifMidMLine2D(XMifMid* file = NULL, uint32_t id = 0) { m_File = file; m_nId = id; m_Pos = 0; }

	inline XMifMid* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, uint32_t num = 1, XError* error = NULL);
	bool Write(std::ofstream* /*out*/, XError* /*error*/ = NULL) { return false; }

	virtual bool LoadGeom();

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return m_File->ReadAttributes(m_nId, V);}
	virtual std::string FindAttribute(std::string att_name) { return m_File->FindAttribute(m_nId, att_name);}
};

//-----------------------------------------------------------------------------
// Polygone
//-----------------------------------------------------------------------------
class XMifMidMPoly2D : public XGeoMPoly2D {
protected :
	uint32_t				m_nId;						// Numero d'ordre dans le fichier
	std::streampos	m_Pos;						// Position de la geometrie dans le fichier
	XMifMid*				m_File;
//	XPt2D					m_Centroide;			// Centroide optionnel

public:
	XMifMidMPoly2D(XMifMid* file = NULL, uint32_t id = 0) { m_File = file; m_nId = id; m_Pos = 0; }

	inline XMifMid* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

//	virtual inline XPt2D Centroide() const { return m_Centroide;}
//	void Centroide(double x, double y) { m_Centroide = XPt2D(x, y);}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* /*out*/, XError* /*error*/ = NULL) { return false; }

	virtual bool LoadGeom();

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return m_File->ReadAttributes(m_nId, V);}
	virtual std::string FindAttribute(std::string att_name) { return m_File->FindAttribute(m_nId, att_name);}
};

#endif //_XMIFMID_H
