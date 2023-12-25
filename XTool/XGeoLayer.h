//-----------------------------------------------------------------------------
//								XGeoLayer.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOLAYER_H
#define _XGEOLAYER_H

#include "XBase.h"
#include <vector>
#include "XGeoObject.h"

class XGeoClass;

class XGeoLayer : public XGeoObject {
protected:
	std::string		m_strName;
	std::string		m_strDescription;
	std::vector<XGeoClass*>		m_Class;

public:
	XGeoLayer();
	virtual ~XGeoLayer();

	virtual inline eType Type() const { return Layer;}
	std::string Description() const { return m_strDescription;}
	void Description(const char* desc) { m_strDescription = desc;}

  virtual XFrame Frame() const;

	XGeoClass* AddClass(const char* name);
	bool RemoveClass(XGeoClass* C);

	inline uint32_t NbClass() const { return (uint32_t)m_Class.size();}
	XGeoClass* Class(uint32_t i) const { if (i < m_Class.size()) return m_Class[i]; return NULL;}

	uint32_t NbVector();

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);
};

#endif //_XGEOLAYER_H
