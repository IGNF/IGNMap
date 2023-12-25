//-----------------------------------------------------------------------------
//								XGeoLayer.cpp
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#include "XGeoLayer.h"
#include "XGeoClass.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoLayer::XGeoLayer()
{
	m_strName = "Sans nom";
	m_strDescription = "Pas de description";
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoLayer::~XGeoLayer()
{
	for (uint32_t i = 0; i < m_Class.size(); i++) 
		delete m_Class[i];
	m_Class.clear();
}

//-----------------------------------------------------------------------------
// Calcul du cadre
//-----------------------------------------------------------------------------
XFrame XGeoLayer::Frame() const
{
  XFrame F;
  for (uint32_t i = 0; i < m_Class.size(); i++)
    F += m_Class[i]->Frame();
  return F;
}

//-----------------------------------------------------------------------------
// Ajout d'une classe
//-----------------------------------------------------------------------------
XGeoClass* XGeoLayer::AddClass(const char* name)
{
	for (uint32_t i = 0; i < m_Class.size(); i++) 
		if (m_Class[i]->Name().compare(name) == 0)
			return m_Class[i];

	XGeoClass* C = new XGeoClass(name, this);
	m_Class.push_back(C);
	return C;
}

//-----------------------------------------------------------------------------
// Retire une classe du layer
//-----------------------------------------------------------------------------
bool XGeoLayer::RemoveClass(XGeoClass* C)
{
	std::vector<XGeoClass*>::iterator iter;
	for (iter = m_Class.begin(); iter != m_Class.end(); iter++)
		if (*iter == C) {
			delete C;
			m_Class.erase(iter);
			return true;
		}
	return false;
}

//-----------------------------------------------------------------------------
// Nombre de vecteur du layer
//-----------------------------------------------------------------------------
uint32_t XGeoLayer::NbVector()
{
	uint32_t nb = 0;
	for (uint32_t i = 0; i < m_Class.size(); i++) 
		nb += m_Class[i]->NbVector();
	return nb;
}

//-----------------------------------------------------------------------------
// Import XML 
//-----------------------------------------------------------------------------
bool XGeoLayer::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML layer = parser->FindSubParser("/xgeolayer", num);
	if (layer.IsEmpty())
		return XErrorError(error, "XGeoLayer::XmlRead", XError::eBadFormat);
	m_strName = layer.ReadNode("/xgeolayer/name");
	m_strDescription = layer.ReadNode("/xgeolayer/description");

	std::vector<XParserXML> vec;
	parser->FindAllSubParsers("/xgeolayer/xgeoclass", &vec);
	for (uint32_t i = 0; i < vec.size(); i++) {
		XGeoClass* C = new XGeoClass;
		if (!C->XmlRead(&vec[i])) {
			delete C;
			continue;
		}
		m_Class.push_back(C);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Export XML
//-----------------------------------------------------------------------------
bool XGeoLayer::XmlWrite(std::ostream* out)
{
	*out << "<xgeolayer>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	*out << "<description> " << m_strDescription << " </description>" << std::endl;
	for (uint32_t i = 0; i < m_Class.size(); i++) 
		m_Class[i]->XmlWrite(out);
	*out << "</xgeolayer>" << std::endl;

	return out->good();
}
