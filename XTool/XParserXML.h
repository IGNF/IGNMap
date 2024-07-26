//*----------------------------------------------------------------------------
//								XParserXML.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 08/08/00
//*----------------------------------------------------------------------------

#ifndef _XPARSERXML_H
#define _XPARSERXML_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include "XBase.h"

//-----------------------------------------------------------------------------
// Classe XNodeXML
//				-> La classe XNodeXML represente un noeud dans un arbre XML
//-----------------------------------------------------------------------------
class XNodeXML {
protected:
	std::string		m_strName;	// Nom du noeud
	std::string		m_strValue;	// Valeur eventuellement stockee par le noeud
	std::list<XNodeXML*>	m_Tree;	// Noeuds fils

	std::string GetToken(std::istream* in);
	bool FindAttributes();
public:
	XNodeXML(std::string name="", std::string value="") : m_strName(name), m_strValue(value) {;}
	virtual ~XNodeXML();
	void DeleteNodes();

	XNodeXML& operator=(const XNodeXML& N);
	XNodeXML* GetNode(std::string nodename, uint32_t num = 0);

	bool IsEmpty() { if ((m_strName=="")&&(m_strValue=="")&&(m_Tree.empty())) return true;
					 return false;}

	std::string Name() const { return m_strName;}
	std::string Value() const { return m_strValue;}
	std::list<XNodeXML*>* Tree() { return &m_Tree;}

	bool Read(std::istream* in, bool root = false);
	bool FindNode(std::string nodename, uint32_t num = 0);
	std::string ReadNode(std::string nodename, uint32_t num = 0);
	uint32_t ReadArrayNode(std::string nodename, std::vector<std::string>* V);
	uint32_t ReadAllNodes(std::string nodename, std::vector<XNodeXML*>* T);
	bool Write(std::ostream* out, bool root = true);
};

//-----------------------------------------------------------------------------
// Classe XParserXML
//				-> La classe XParserXML permet de charger une arborescence XML
//-----------------------------------------------------------------------------
class XParserXML {
protected:
	XNodeXML		m_Root;			// Racine de l'arborescence XML
	XNodeXML*		m_Sub;			// Utilise pour definir un sub-parser
	std::string	m_strFile;	// Nom du fichier
public:
	XParserXML() {m_Sub = NULL;}
	XParserXML(const XParserXML& N) { m_Root = N.m_Root; m_strFile = N.m_strFile; m_Sub = N.m_Sub;}
	virtual ~XParserXML() {;}

	XParserXML& operator=(const XParserXML& N)
		{if (this != &N) {m_Root=N.m_Root; m_strFile = N.m_strFile; m_Sub = N.m_Sub;} return *this;}

	bool IsEmpty() { if (m_Sub != NULL) return m_Sub->IsEmpty(); return m_Root.IsEmpty();}

	XNodeXML* Root() { if (m_Sub != NULL) return m_Sub; return &m_Root;}

	bool Parse(std::string filename);
  bool Parse(std::istream* stream);
	std::string Filename() const { return m_strFile;}
	bool FindNode(std::string nodename, uint32_t num = 0);
	std::string ReadNode(std::string nodename, uint32_t num = 0);
	int ReadNodeAsInt(std::string nodename, uint32_t num = 0);
	uint32_t ReadNodeAsUInt32(std::string nodename, uint32_t num = 0);
	uint32_t ReadNodeAsHexUInt32(std::string nodename, uint32_t num = 0);
	double ReadNodeAsDouble(std::string nodename, uint32_t num = 0);
	bool ReadNodeAsBool(std::string nodename, uint32_t num = 0);

	uint32_t ReadArrayNode(std::string nodename, std::vector<std::string>* V);
	uint32_t ReadArrayNodeAsInt(std::string nodename, std::vector<int>* V);
	uint32_t ReadArrayNodeAsUInt32(std::string nodename, std::vector<uint32_t>* V);
	uint32_t ReadArrayNodeAsDouble(std::string nodename, std::vector<double>* V);

	XParserXML FindSubParser(std::string nodename, uint32_t num = 0);
	uint32_t FindAllSubParsers(std::string nodename, std::vector<XParserXML>* T);

	bool Write(std::ostream* out, std::string xsl_file);
	bool Write(std::string filename, std::string xsl_file);
};

#endif //_XPARSERXML_H
