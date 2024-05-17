//-----------------------------------------------------------------------------
//								XGeoClass.cpp
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#include <algorithm>
#include "XGeoClass.h"
#include "XGeoVector.h"


//-----------------------------------------------------------------------------
// Convertion d'un type d'attribut en chaine de caracteres
//-----------------------------------------------------------------------------
std::string XGeoAttribut::Type2String(eType type) const
{
	switch(type) {
		case Bool: return "bool";
		case Int16: return "int16";
		case Int32: return "int32";
		case Double: return "double";
		case String: return "string";
		case List: return "list";
		case NumericN: return "numericN";
		case NumericF: return "numericF";
	}
	return "";
}

//-----------------------------------------------------------------------------
// Convertion d'une chaine de caracteres entype d'attribut
//-----------------------------------------------------------------------------
XGeoAttribut::eType XGeoAttribut::String2Type(const char* type) const
{
	std::string s = type;
	std::transform(s.begin(), s.end(), s.begin(), [](char c) {return static_cast<char>(std::tolower(c)); });
	if (s == "bool") return Bool;
	if (s == "int16") return Int16;
	if (s == "int32") return Int32;
	if (s == "double") return Double;
	if (s == "string") return String;
	if (s == "list") return List;
	if (s == "numericN") return NumericN;
	if (s == "numericF") return NumericF;
	return String;
}

//-----------------------------------------------------------------------------
// Creation d'un nom court d'attribut
//-----------------------------------------------------------------------------
std::string XGeoAttribut::ShortName(uint32_t num) const
{
	if (m_strShortName.size() > 0)
		return m_strShortName;
	std::string name = m_strName, shortname;
	for (uint32_t i = 0; i < name.size(); i++) {
		char c = name[i];
		if (shortname.size() > 7) break;
		if (((c >= 65)&&(c <= 90))||((c >= 97)&&(c <= 122)))
			shortname += c;
	}
	if (num > 0) {
		char buf[10];
		::sprintf(buf, "%u", num);
		shortname += buf;
	}
	return shortname;
}

//-----------------------------------------------------------------------------
// Lecture d'une definition d'attribut dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoAttribut::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML att = parser->FindSubParser("/xgeoattribut", num);
	if (att.IsEmpty())
		return XErrorError(error, "XGeoAttribut::XmlRead", XError::eBadFormat);
	m_strName = att.ReadNode("/xgeoattribut/name");
	m_strShortName = att.ReadNode("/xgeoattribut/short_name");
	m_strDescription = att.ReadNode("/xgeoattribut/description");
	m_nLength = att.ReadNodeAsUInt32("/xgeoattribut/length");
	m_nDecCount = (uint8_t)att.ReadNodeAsUInt32("/xgeoattribut/dec_count");
	std::string type = att.ReadNode("/xgeoattribut/type");
	m_Type = String2Type(type.c_str());
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'une definition d'attribut dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoAttribut::XmlWrite(std::ostream* out)
{
	*out << "<xgeoattribut>" << std::endl;
	*out << "<name> " << m_strName << "</name>" << std::endl;
	*out << "<short_name> " << m_strShortName << "</short_name>" << std::endl;
	*out << "<description> " << m_strDescription << "</description>" << std::endl;
	*out << "<type> " << Type2String(m_Type) << "</type>" << std::endl;
	*out << "<length> " << m_nLength << "</length>" << std::endl;
	*out << "<dec_count> " << m_nDecCount << "</dec_count>" << std::endl;
	*out << "</xgeoattribut>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Renvoi le nom de l'attribut
//-----------------------------------------------------------------------------
std::string XGeoSchema::AttributName(uint32_t i) const
{
	if (i >= m_Attrib.size())
		return " ";
	return m_Attrib[i].Name();
}

//-----------------------------------------------------------------------------
// Renvoi le nom court de l'attribut
//-----------------------------------------------------------------------------
std::string XGeoSchema::AttributShortName (uint32_t i) const
{
	if (i >= m_Attrib.size())
		return " ";
	return m_Attrib[i].ShortName(i);
}

//-----------------------------------------------------------------------------
// Renvoi le type de l'attribut
//-----------------------------------------------------------------------------
XGeoAttribut::eType XGeoSchema::AttributType (uint32_t i) const
{
	if (i >= m_Attrib.size())
		return XGeoAttribut::String;
	return m_Attrib[i].Type();
}

//-----------------------------------------------------------------------------
// Renvoi la longueur de l'attribut
//-----------------------------------------------------------------------------
uint32_t XGeoSchema::AttributLength(uint32_t i) const
{
	if (i >= m_Attrib.size())
		return 0;
	return m_Attrib[i].Length();
}

//-----------------------------------------------------------------------------
// Renvoi le nombre de chiffre apres la virgule de l'attribut
//-----------------------------------------------------------------------------
uint32_t XGeoSchema::AttributDecCount(uint32_t i) const
{
	if (i >= m_Attrib.size())
		return 0;
	return m_Attrib[i].DecCount();
}

//-----------------------------------------------------------------------------
// Renvoi le type de l'attribut sous forme d'une chaine
//-----------------------------------------------------------------------------
std::string XGeoSchema::AttributTypeString(uint32_t i) const
{
	if (i >= m_Attrib.size())
		return "";
	return m_Attrib[i].TypeString();
}

//-----------------------------------------------------------------------------
// Ajout d'un attribut
//-----------------------------------------------------------------------------
bool XGeoSchema::AddAttribut(std::string name, std::string shortname, std::string desc,
														 XGeoAttribut::eType type, uint32_t length, uint8_t dec)
{
	XGeoAttribut attrib(name, shortname, desc, type, length, dec);
	for (uint32_t i = 0; i < m_Attrib.size(); i++) {
		if (m_Attrib[i].Name() == attrib.Name()) {
			m_Attrib[i] = attrib;
			return true;
		}
	}
	m_Attrib.push_back(attrib);
	return true;
}

//-----------------------------------------------------------------------------
// Trie des attributs dans l'ordre du schema
//-----------------------------------------------------------------------------
bool XGeoSchema::SortAttribut(std::vector<std::string>* V)
{
	std::vector<std::string> Att;
	int cmpt, nb = V->size()/2;
	for (uint32_t i = 0; i < m_Attrib.size(); i++) {
		cmpt = -1;
		for(int j = 0; j < nb; j++) {
			if (((*V)[2*j] == m_Attrib[i].Name())||(((*V)[2*j] == m_Attrib[i].ShortName()))) {
				cmpt = j;
				break;
			}
		}
		Att.push_back(m_Attrib[i].ShortName());
		if (cmpt != -1)
			Att.push_back((*V)[2*cmpt + 1]);
		else
			Att.push_back(" ");
	}
	V->clear();
	*V = Att;
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un schema dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoSchema::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML schema = parser->FindSubParser("/xgeoschema", num);
	if (schema.IsEmpty())
		return XErrorError(error, "XGeoSchema::XmlRead", XError::eBadFormat);

	m_strName = schema.ReadNode("/xgeoschema/name", num);
	// Lecture des attributs
	uint32_t nb = 0;
	XParserXML att;
	while(true){
		att = schema.FindSubParser("/xgeoschema/xgeoattribut", nb);
		if (att.IsEmpty())
			break;
		XGeoAttribut A;
		if (A.XmlRead(&att, 0))
			m_Attrib.push_back(A);

		nb++;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'un schema dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoSchema::XmlWrite(std::ostream* out)
{
	*out << "<xgeoschema>" << std::endl;
	*out << "<name> " << m_strName << "</name>" << std::endl;
	for (uint32_t i = 0; i < m_Attrib.size(); i++)
		m_Attrib[i].XmlWrite(out);
	*out << "</xgeoschema>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Ajout d'un vecteur
//-----------------------------------------------------------------------------
bool XGeoClass::Vector(XGeoVector* V)
{
	/*
	for (uint32_t i = 0; i < m_Vector.size(); i++)
		if (m_Vector[i] == V)
			return true;
	*/
	m_Vector.push_back(V);
	m_Frame += V->Frame();
	return true;
}

//-----------------------------------------------------------------------------
// Retrait d'un vecteur
//-----------------------------------------------------------------------------
bool XGeoClass::RemoveVector(XGeoVector* V)
{
	std::vector<XGeoVector*>::iterator iter;
	for (iter = m_Vector.begin(); iter != m_Vector.end(); iter++)
		if (*iter == V) {
			m_Vector.erase(iter);
			return true;
		}
	return false;
}

//-----------------------------------------------------------------------------
// Retire tous les vecteurs
//-----------------------------------------------------------------------------
bool XGeoClass::RemoveAllVectors()
{
	m_Vector.clear();
	return true;
}

//-----------------------------------------------------------------------------
// Mise a jour du cadre de la classe
//-----------------------------------------------------------------------------
bool XGeoClass::UpdateFrame()
{
	m_Frame = XFrame();
	for (uint32_t i = 0; i < m_Vector.size(); i++)
			m_Frame += m_Vector[i]->Frame();
	return true;
}

//-----------------------------------------------------------------------------
// Trie des vecteur en fonction de la longueur
//-----------------------------------------------------------------------------
bool VectorLength(XGeoVector* A, XGeoVector* B)
{
	return (A->Length() > B->Length());
}

//-----------------------------------------------------------------------------
// Trie des vecteurs en fonction de la longueur
//-----------------------------------------------------------------------------
bool XGeoClass::Sort()
{
	std::stable_sort(m_Vector.begin(), m_Vector.end(), VectorLength);
	return true;
}

//-----------------------------------------------------------------------------
// Trie des vecteur en fonction de la dimension du rectangle englobant
//-----------------------------------------------------------------------------
bool VectorFrameLength(XGeoVector* A, XGeoVector* B)
{
  return (A->Frame().Size() > B->Frame().Size());
}

//-----------------------------------------------------------------------------
// Trie des vecteurs en fonction de la dimension du rectangle englobant
//-----------------------------------------------------------------------------
bool XGeoClass::QuickSort()
{
  std::stable_sort(m_Vector.begin(), m_Vector.end(), VectorFrameLength);
  return true;
}

//-----------------------------------------------------------------------------
// Recherche des objets proches d'un point
//-----------------------------------------------------------------------------
uint32_t XGeoClass::Find(std::vector<XGeoVector*>* T, XPt2D& P, double dist)
{
	XGeoVector* vector;
	T->clear();
	for (uint32_t i = 0; i < m_Vector.size(); i++) {
		vector = m_Vector[i];
		if (vector->IsNear2D(P, dist))
			T->push_back(vector);
	}
	return T->size();
}

//-----------------------------------------------------------------------------
// Recherche des objets connectes
//-----------------------------------------------------------------------------
uint32_t XGeoClass::FindConnection(std::vector<XGeoVector*>* T, XPt2D& P)
{
	XGeoVector* vector;
	T->clear();
	for (uint32_t i = 0; i < m_Vector.size(); i++) {
		vector = m_Vector[i];
		if (vector->IsConnected(P))
			T->push_back(vector);
	}
	return T->size();
}

//-----------------------------------------------------------------------------
// Indique si une classe est une classe raster
//-----------------------------------------------------------------------------
bool XGeoClass::IsRaster()
{
  if (m_Vector.size() < 1)
    return false;
  XGeoVector* V = m_Vector[0];
  if (V->TypeVector() != XGeoVector::Raster)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Indique si une classe est une classe DTM
//-----------------------------------------------------------------------------
bool XGeoClass::IsDTM()
{
  if (m_Vector.size() < 1)
    return false;
  XGeoVector* V = m_Vector[0];
  if (V->TypeVector() != XGeoVector::DTM)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Indique si une classe est une classe LAS
//-----------------------------------------------------------------------------
bool XGeoClass::IsLAS()
{
	if (m_Vector.size() < 1)
		return false;
	XGeoVector* V = m_Vector[0];
	if (V->TypeVector() != XGeoVector::LAS)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Indique si une classe est une classe vectorielle
//-----------------------------------------------------------------------------
bool XGeoClass::IsVector()
{
  if (m_Vector.size() < 1)
    return false;
  XGeoVector* V = m_Vector[0];
  if ((V->TypeVector() != XGeoVector::DTM)&&(V->TypeVector() != XGeoVector::Raster) && (V->TypeVector() != XGeoVector::LAS))
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Zmin de la classe
//-----------------------------------------------------------------------------
double XGeoClass::Zmin() const
{
  double zmin = XGEO_NO_DATA, z = XGEO_NO_DATA;
  for (uint32_t i = 0; i < m_Vector.size(); i++) {
    z = m_Vector[i]->Zmin();
		if (zmin == XGEO_NO_DATA) zmin = z;
    if (z > XGEO_NO_DATA)
      zmin = XMin(zmin, z);
  }
  return zmin;
}

//-----------------------------------------------------------------------------
// Zmax de la classe
//-----------------------------------------------------------------------------
double XGeoClass::Zmax() const
{
  double zmax = XGEO_NO_DATA, z = XGEO_NO_DATA;
  for (uint32_t i = 0; i < m_Vector.size(); i++) {
    z = m_Vector[i]->Zmax();
    if (z > XGEO_NO_DATA)
      zmax = XMax(zmax, z);
  }
  return zmax;
}

//-----------------------------------------------------------------------------
// Import XML
//-----------------------------------------------------------------------------
bool XGeoClass::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML cla = parser->FindSubParser("/xgeoclass", num);
	if (cla.IsEmpty())
		return XErrorError(error, "XGeoClass::XmlRead", XError::eBadFormat);
	m_strName = cla.ReadNode("/xgeoclass/name");
	m_strDescription = cla.ReadNode("/xgeoclass/description");
  m_State = cla.ReadNodeAsInt("/xgeoclass/state");
	XParserXML frame = parser->FindSubParser("/xgeoclass/frame", num);
	m_Frame.XmlRead(&frame, 0, error);
	XParserXML repres = parser->FindSubParser("/xgeoclass/xgeorepres", num);
	m_Repres.XmlRead(&repres, 0, error);
	XParserXML schema = parser->FindSubParser("/xgeoclass/xgeoschema", num);
	m_Schema.XmlRead(&schema, 0, error);

	return true;
}

//-----------------------------------------------------------------------------
// Export XML
//-----------------------------------------------------------------------------
bool XGeoClass::XmlWrite(std::ostream* out)
{
	*out << "<xgeoclass>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	*out << "<description> " << m_strDescription << " </description>" << std::endl;
  *out << "<state> " << m_State << " </state>" << std::endl;
	m_Frame.XmlWrite(out);
	m_Repres.XmlWrite(out);
	m_Schema.XmlWrite(out);
	*out << "</xgeoclass>" << std::endl;

	return out->good();
}
