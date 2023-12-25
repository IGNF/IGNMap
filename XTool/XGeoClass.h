//-----------------------------------------------------------------------------
//								XGeoClass.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOCLASS_H
#define _XGEOCLASS_H

#include "XGeoObject.h"
#include "XGeoLayer.h"
#include "XGeoRepres.h"

class XGeoVector;

//-----------------------------------------------------------------------------
// Classe XGeoAttribut
//-----------------------------------------------------------------------------
class XGeoAttribut {
public:
	enum eType { Bool, Int16, Int32, Double, String, List, NumericN, NumericF};
protected:
	std::string		m_strName;
	std::string		m_strShortName;		// Nom court (par exemple pour les fichiers DBF)
	std::string		m_strDescription;
	eType					m_Type;
	uint32_t				m_nLength;
	uint8_t					m_nDecCount;
public:
	XGeoAttribut() { m_strName = "Indéfini"; m_strShortName = "Indef"; m_Type = String; m_nLength = 64; m_nDecCount = 0;}
	XGeoAttribut(const char* name, const char* shortname, eType type, uint16_t length, uint8_t dec = 0) :
						m_strName(name), m_strShortName(shortname), m_Type(type), m_nLength(length), m_nDecCount(dec) {;}
	XGeoAttribut(std::string name, std::string shortname, std::string desc, eType type, uint16_t length, uint8_t dec = 0) :
							m_strName(name), m_strShortName(shortname), m_strDescription(desc), m_Type(type),
							m_nLength(length) ,m_nDecCount(dec){;}
	virtual ~XGeoAttribut() {;}

	std::string Name() const { return m_strName;}
	std::string ShortName(uint32_t num = 0) const;
	std::string Description() const { return m_strDescription;}
	std::string Type2String(eType type) const;
	eType String2Type(const char* type) const;
	inline eType Type() const { return m_Type;}
	inline uint32_t Length() const { return m_nLength;}
	inline uint8_t DecCount() const { return m_nDecCount;}
	std::string TypeString() const { return Type2String(m_Type);}

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Classe XGeoSchema
//-----------------------------------------------------------------------------
class XGeoSchema {
protected:
	std::string			m_strName;
	std::vector<XGeoAttribut> m_Attrib;

public:
	XGeoSchema() {;}
	XGeoSchema(const char* name) { m_strName = name;}
	virtual ~XGeoSchema() {;}

	std::string Name() const { return m_strName;}
	uint32_t NbAttribut() { return (uint32_t)m_Attrib.size();}

	std::string AttributName (uint32_t i) const;
	std::string AttributShortName (uint32_t i) const;
	XGeoAttribut::eType AttributType (uint32_t i) const;
	uint32_t AttributLength(uint32_t i) const;
	uint32_t AttributDecCount(uint32_t i) const;
	std::string AttributTypeString(uint32_t i) const;

	bool AddAttribut(std::string name, std::string shortname, std::string desc, XGeoAttribut::eType type,
									 uint32_t length, uint8_t dec = 0);
	bool SortAttribut(std::vector<std::string>* V);

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Classe XGeoClass
//-----------------------------------------------------------------------------
class XGeoClass : public XGeoObject {
protected:
	std::string		m_strName;
	std::string		m_strDescription;
	XGeoLayer*		m_Layer;
	XGeoRepres		m_Repres;		// Representation associee a la classe
	XGeoSchema		m_Schema;
	XGeoVector*		m_Mask;
	std::vector<XGeoVector*>	m_Vector;

public:
	XGeoClass() {m_Layer = NULL; m_Mask = NULL;}
	XGeoClass(const char* name, XGeoLayer* layer = NULL) { m_strName = name; m_Layer = layer; m_Mask = NULL;}
	virtual ~XGeoClass() {;}

	virtual inline eType Type() const { return Class;}
	std::string Description() const { return m_strDescription;}
	void Description(const char* desc) { m_strDescription = desc;}
  bool IsRaster();
  bool IsDTM();
  bool IsVector();
	bool IsLAS();

	bool SetSchema(XGeoSchema& schema) { m_Schema = schema; return true;}

	virtual inline XGeoRepres* Repres() { return &m_Repres;}
	inline XGeoLayer* Layer() { return m_Layer;}

	inline uint32_t NbVector() const { return (uint32_t)m_Vector.size();}
	XGeoVector* Vector(uint32_t i) { if (i < m_Vector.size()) return m_Vector[i]; return NULL;}
	bool Vector(XGeoVector* V);
	bool RemoveVector(XGeoVector* V);
	bool RemoveAllVectors();
	bool Sort();
  bool QuickSort();
	bool UpdateFrame();
	uint32_t Find(std::vector<XGeoVector*>* T, XPt2D& P, double dist);
	uint32_t FindConnection(std::vector<XGeoVector*>* T, XPt2D& P);

	uint32_t ZOrder() { return m_Repres.ZOrder();}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}
  virtual double Zmin() const;
  virtual double Zmax() const;

	std::string AttributName(uint32_t i) const { return m_Schema.AttributName(i);}
	XGeoSchema* Schema() { return &m_Schema;}

	inline XGeoVector* Mask() { return m_Mask;}
	void Mask(XGeoVector* mask) { m_Mask = mask;}

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

#endif //_XGEOCLASS_H
