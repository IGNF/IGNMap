//-----------------------------------------------------------------------------
//								XGeoMap.cpp
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#include "XGeoMap.h"

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------	
XGeoMap::~XGeoMap()
{
	for (uint32_t i = 0; i < m_Data.size(); i++) 
		delete m_Data[i];
	m_Data.clear();
}

//-----------------------------------------------------------------------------
// Visibilite de la carte
//-----------------------------------------------------------------------------
void XGeoMap::Visible(bool flag)
{
	XGeoObject::Visible(flag);
	for (uint32_t i = 0; i < m_Data.size(); i++) 
		m_Data[i]->Visible(flag);
}

//-----------------------------------------------------------------------------
// Selectionnabilite de la carte
//-----------------------------------------------------------------------------
void XGeoMap::Selectable(bool flag)
{
	XGeoObject::Selectable(flag);
	for (uint32_t i = 0; i < m_Data.size(); i++) 
		m_Data[i]->Selectable(flag);
}

//-----------------------------------------------------------------------------
// Ajout d'un objet dans la carte
//-----------------------------------------------------------------------------
bool XGeoMap::AddObject(XGeoObject* obj)
{
	m_Frame += obj->Frame();
	m_Data.push_back(obj);
	if (obj->Zmin() > XGEO_NO_DATA)
		if (m_dZmin > XGEO_NO_DATA)
				m_dZmin = XMin(m_dZmin, obj->Zmin());
			else
				m_dZmin = obj->Zmin();
	if (obj->Zmax() > XGEO_NO_DATA)
		m_dZmax = XMax(m_dZmax, obj->Zmax());

	return true;
}

//-----------------------------------------------------------------------------
// Nombre de cartes inclues dans la carte
//-----------------------------------------------------------------------------
uint32_t XGeoMap::NbMap()
{
  uint32_t nb_map = 0;
  for (uint32_t i = 0; i < m_Data.size(); i++)
    if (m_Data[i]->Type() == XGeoObject::Map)
      nb_map++;
  return nb_map;
}

//-----------------------------------------------------------------------------
//Renvoi une carte incluse dans la carte
//-----------------------------------------------------------------------------
XGeoMap* XGeoMap::GeoMap(uint32_t num)
{
  uint32_t nb_map = 0;
  for (uint32_t i = 0; i < m_Data.size(); i++) {
    if (m_Data[i]->Type() == XGeoObject::Map) {
      if(nb_map == num)
        return (XGeoMap*)m_Data[i];
      nb_map++;
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
// Mise a jour du cadre
//-----------------------------------------------------------------------------
bool XGeoMap::UpdateFrame()
{
	m_Frame = XFrame();
	for (uint32_t i = 0; i < m_Data.size(); i++) 
		m_Frame += m_Data[i]->Frame();

	return true;
}

//-----------------------------------------------------------------------------
// Recherche si un objet appartient a cette map
//-----------------------------------------------------------------------------
bool XGeoMap::FindObject(XGeoObject* obj)
{
	for (uint32_t i = 0; i < m_Data.size(); i++) 
		if (m_Data[i] == obj)
			return true;
	return false;
}

//-----------------------------------------------------------------------------
// Retire un objet de la map
//-----------------------------------------------------------------------------
bool XGeoMap::RemoveObject(XGeoObject* obj)
{
	std::vector<XGeoObject*>::iterator iter;
	for (iter = m_Data.begin(); iter != m_Data.end(); iter++)
		if (*iter == obj) {
			m_Data.erase(iter);
			return true;
		}
	return false;
}

//-----------------------------------------------------------------------------
// Retire tous les objets de la map
//-----------------------------------------------------------------------------
bool XGeoMap::RemoveAllObjects()
{
	XGeoObject* obj;
	XGeoVector* vec;
	XGeoClass* classe;
	for (uint32_t i = 0; i < m_Data.size(); i++) {
		obj = m_Data[i];
		if (obj->Type() == XGeoObject::Map) {
			((XGeoMap*)obj)->RemoveAllObjects();
			delete obj;
			continue;
		}

		if (obj->Type() == XGeoObject::Vector) {
			vec = (XGeoVector*)obj;
			classe = vec->Class();
			classe->RemoveVector(vec);
			delete vec;
			continue;
		}

	}
	m_Data.clear();
	return true;
}

//-----------------------------------------------------------------------------
// Retire tous les objets d'une classe
//-----------------------------------------------------------------------------
bool XGeoMap::RemoveClass(XGeoClass* C)
{
  std::vector<XGeoObject*> T;
  XGeoObject* obj;
  XGeoVector* V;

  // Test si la classe est presente dans la Map
  bool flag = false;
  for (uint32_t i = 0; i < m_Data.size(); i++) {
    obj = m_Data[i];
    if (obj->Type() != XGeoObject::Vector)
      continue;
    V = (XGeoVector*)obj;
    if (V->Class() == C){
      flag = true;
      break;
    }
  }

  if (!flag)
    return true;

  for (uint32_t i = 0; i < m_Data.size(); i++) {
    obj = m_Data[i];
    if (obj->Type() != XGeoObject::Vector)
      continue;
    V = (XGeoVector*)obj;
    if (V->Class() != C)
      T.push_back(V);
  }
  m_Data.clear();
  m_Data = T;
  return true;
}
	
//-----------------------------------------------------------------------------
// Lecture XML
//-----------------------------------------------------------------------------
bool XGeoMap::XmlRead(XParserXML*, uint32_t, XError*)
{
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture XML
//-----------------------------------------------------------------------------
bool XGeoMap::XmlWrite(std::ostream* out)
{ 
	*out << "<xgeomap>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	for (uint32_t i = 0; i < m_Data.size(); i++)
		m_Data[i]->XmlWrite(out);
	*out << "</xgeomap>" << std::endl;
	return out->good();
}
