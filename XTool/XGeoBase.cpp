//-----------------------------------------------------------------------------
//								XGeoBase.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/03/2003
//-----------------------------------------------------------------------------

#include "XGeoBase.h"
#include "XGeoLayer.h"
#include "XGeoMap.h"
#include <algorithm>


//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoBase::~XGeoBase()
{
	Clear();
}

//-----------------------------------------------------------------------------
// Nettoyage du contenu de la base
//-----------------------------------------------------------------------------
void XGeoBase::Clear()
{
  uint32_t i;
  for(i = 0; i < m_Map.size(); i++)
		delete m_Map[i];
	m_Map.clear();
	for(i = 0; i < m_Layer.size(); i++)
		delete m_Layer[i];
	m_Layer.clear();
	m_Class.clear();
	m_Selection.clear();
	m_Frame = XFrame();
	m_nVersion = 0;
  m_dZmin = m_dZmax = XGEO_NO_DATA;
}

//-----------------------------------------------------------------------------
// Ajout d'une map
//-----------------------------------------------------------------------------
bool XGeoBase::AddMap(XGeoMap* map)
{
	for (uint32_t i = 0; i < m_Map.size(); i++)
		if (m_Map[i] == map)
			return true;
	m_Frame += map->Frame();
	m_Map.push_back(map);
  if (map->Zmin() > XGEO_NO_DATA){
		if (m_dZmin > XGEO_NO_DATA)
			m_dZmin = XMin(m_dZmin, map->Zmin());
		else
			m_dZmin = map->Zmin();
  }
	if (map->Zmax() > XGEO_NO_DATA)
		m_dZmax = XMax(m_dZmax, map->Zmax());

	return true;
}

//-----------------------------------------------------------------------------
// Destruction d'une map
//-----------------------------------------------------------------------------
bool XGeoBase::RemoveMap(XGeoMap* map)
{
	std::vector<XGeoMap*>::iterator iter;
	for (iter = m_Map.begin(); iter != m_Map.end(); iter++)
		if (*iter == map) {
			map->RemoveAllObjects();
			delete map;
			m_Map.erase(iter);
			return true;
		}

	return false;
}

//-----------------------------------------------------------------------------
// Recherche d'une map par son nom
//-----------------------------------------------------------------------------
XGeoMap* XGeoBase::Map(const char* mapname)
{
	for (uint32_t i = 0; i < NbMap(); i++) {
		XGeoMap* map = Map(i);
		if (map->Name().compare(mapname) == 0)
			return map;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Mise a jour de l'emprise de la base
//-----------------------------------------------------------------------------
bool XGeoBase::UpdateFrame(XFrame *initF)
{
	XGeoMap* map;
  if (initF == NULL)
    m_Frame = XFrame();
  else
    m_Frame = *initF;
	for (uint32_t i = 0; i < m_Map.size(); i++) {
		map = m_Map[i];
		m_Frame += map->Frame();
    if (map->Zmin() > XGEO_NO_DATA){
			if (m_dZmin > XGEO_NO_DATA)
				m_dZmin = XMin(m_dZmin, map->Zmin());
			else
				m_dZmin = map->Zmin();
    }
		if (map->Zmax() > XGEO_NO_DATA)
			m_dZmax = XMax(m_dZmax, map->Zmax());
	}
  m_Frame *= 0.05;
  if ((m_Frame.Width() > 1000)&&(m_Frame.Height() > 1000)) {
    m_Frame.Round(10.);
  }
	return true;
}

//-----------------------------------------------------------------------------
// Ajout d'un layer
//-----------------------------------------------------------------------------
XGeoLayer* XGeoBase::AddLayer(const char* name)
{
	for (uint32_t i = 0; i < m_Layer.size(); i++)
		if (m_Layer[i]->Name().compare(name) == 0)
			return m_Layer[i];
	XGeoLayer* layer = new XGeoLayer;
	layer->Name(name);
	m_Layer.push_back(layer);
	return layer;
}

//-----------------------------------------------------------------------------
// Ajout d'une classe
//-----------------------------------------------------------------------------
XGeoClass* XGeoBase::AddClass(const char* layer_name, const char* class_name)
{
	XGeoLayer* layer = NULL;
	for (uint32_t i = 0; i < m_Layer.size(); i++)
		if (m_Layer[i]->Name().compare(layer_name) == 0) {
			layer = m_Layer[i];
			return layer->AddClass(class_name);
		}
	if (layer == NULL) {
		XGeoLayer* newLayer = new XGeoLayer;
		newLayer->Name(layer_name);
		XGeoClass* C = newLayer->AddClass(class_name);
		m_Layer.push_back(newLayer);
		return C;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Recherche des objets proches d'un point
//-----------------------------------------------------------------------------
uint32_t XGeoBase::Find(std::vector<XGeoVector*>* T, XPt2D& P, double dist)
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;

	T->clear();
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (vector->IsNear2D(P, dist))
					T->push_back(vector);
			}
		}
	}

	return T->size();
}

//-----------------------------------------------------------------------------
// Recherche des objets dont l'emprise intersecte un cadre
//-----------------------------------------------------------------------------
uint32_t XGeoBase::Find(std::vector<XGeoVector*>* T, XFrame& F)
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;

	T->clear();
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (vector->Frame().Intersect(F))
					T->push_back(vector);
			}
		}
	}

	return T->size();
}

//-----------------------------------------------------------------------------
// Recherche d'un objet en fonction d'un attribut
//-----------------------------------------------------------------------------
XGeoVector* XGeoBase::Find(const char* classname, const char* att_name, const char* att_value)
{
	XGeoClass* C;
	XGeoVector* V;
	std::string name, att;
	for (uint32_t i = 0; i < NbClass(); i++) {
		C = Class(i);
		if (C->NbVector() < 1)
			continue;
		name = C->Name();
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		if (name != classname) continue;

		// L'attribut existe-t-il dans le schema ?
		V = C->Vector((uint32_t)0);
		att = V->FindAttribute(att_name, true);
		if (att.size() < 1)
			continue;

		for (uint32_t j = 0; j < C->NbVector(); j++) {
			V = C->Vector(j);
			att = V->FindAttribute(att_name, true);
			if (att == att_value)
				return V;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Recherche des objets proches d'un point
//-----------------------------------------------------------------------------
XGeoVector* XGeoBase::FindClosest(XPt2D& P)
{
  XGeoLayer* layer = NULL;
  XGeoClass* classe;
  XGeoVector* vector;
  XGeoVector* closest = NULL;
  double distance, dist_min = 1e38;

  for (uint32_t i = 0; i < m_Layer.size(); i++) {
    layer = m_Layer[i];
    for (uint32_t j = 0; j < layer->NbClass(); j++) {
      classe = layer->Class(j);
      for (uint32_t k = 0; k < classe->NbVector(); k++) {
        vector = classe->Vector(k);
        distance = dist2(P, vector->Frame().Center());
        if (distance < dist_min ) {
          dist_min = distance;
          closest = vector;
        }
      }
    }
  }

  return closest;
}

//-----------------------------------------------------------------------------
// Selectionne les objets presents dans un cadre
//-----------------------------------------------------------------------------
uint32_t XGeoBase::SelectFeatures(XFrame* F, bool only_visible)
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	m_Selection.clear();
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if (!layer->Selectable())
			continue;
		if ((only_visible) && (!layer->Visible()))
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if (!classe->Selectable())
				continue;
			if ((only_visible) && (!classe->Visible()))
				continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (!vector->Selectable())
					continue;
				if ((only_visible) && (!vector->Visible()))
					continue;
				if (vector->Frame().Intersect(*F)) {
					if ( (vector->TypeVector() == XGeoVector::DTM) || (vector->TypeVector() == XGeoVector::Raster) || 
							 (vector->TypeVector() == XGeoVector::LAS) ) {
						m_Selection.push_back(vector);
						continue;
					}
					if (vector->Intersect(*F))
						m_Selection.push_back(vector);
				}
			}
		}
	}
	return (uint32_t)m_Selection.size();
}

//-----------------------------------------------------------------------------
// Trie des classes en fonction du ZOrder
//-----------------------------------------------------------------------------
bool ClassZOrder(XGeoClass* A, XGeoClass* B)
{
	return (B->Repres()->ZOrder() > A->Repres()->ZOrder());
}

//-----------------------------------------------------------------------------
// Trie des classes en fonction du ZOrder et renvoi du nombre de classes
//-----------------------------------------------------------------------------
uint32_t XGeoBase::SortClass()
{
	if (NbLayer() < 1)
		return 0;
	m_Class.clear();
	for (uint32_t i = 0; i < NbLayer(); i++) {
		XGeoLayer* layer = Layer(i);
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			XGeoClass* classe = layer->Class(j);
			m_Class.push_back(classe);
		}
	}

	std::stable_sort(m_Class.begin(), m_Class.end(), ClassZOrder);
	return m_Class.size();
}

//-----------------------------------------------------------------------------
// Change l'ordre d'une classe dans la liste des classes
//-----------------------------------------------------------------------------
bool XGeoBase::ReorderClass(int oldPosition, int newPosition)
{
	if ((oldPosition >= m_Class.size())||(newPosition >= m_Class.size()))
		return false;
	XGeoClass* C = m_Class[oldPosition];
	m_Class.erase(m_Class.begin() + oldPosition);
	if (newPosition < 0) {
		m_Class.push_back(C);
		return true;
	}
	if (newPosition < oldPosition) {
		m_Class.insert(m_Class.begin() + newPosition, C);
		return true;
	}
	if (newPosition <= m_Class.size()) {
		m_Class.insert(m_Class.begin() + newPosition - 1, C);
		return true;
	}
	m_Class.push_back(C);
	return true;
}

//-----------------------------------------------------------------------------
// Renvoi une classe
//-----------------------------------------------------------------------------
XGeoClass* XGeoBase::Class(uint32_t i)
{
	if (i < m_Class.size())
		return m_Class[i];
	return NULL;
}

//-----------------------------------------------------------------------------
// Recherche d'une classe
//-----------------------------------------------------------------------------
XGeoClass* XGeoBase::Class(const char* layername, const char* classname)
{
	for (uint32_t i = 0; i < NbLayer(); i++) {
		XGeoLayer* layer = Layer(i);
		if (layer->Name().compare(layername) != 0)
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			XGeoClass* classe = layer->Class(j);
			if (classe->Name().compare(classname) == 0)
				return classe;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Recherche d'un layer
//-----------------------------------------------------------------------------
XGeoLayer* XGeoBase::Layer(const char* layername)
{
	for (uint32_t i = 0; i < NbLayer(); i++) {
		XGeoLayer* layer = Layer(i);
		if (layer->Name().compare(layername) == 0)
			return layer;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Rend tous les objets visibles
//-----------------------------------------------------------------------------
void XGeoBase::ShowAll()
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				vector->Visible(true);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Cache les objets en dehors d'un cadre
//-----------------------------------------------------------------------------
void XGeoBase::HideOut(XFrame* F)
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if (!layer->Visible())
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if (!classe->Visible())
				continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (!vector->Visible())
					continue;
				if (!vector->Frame().Intersect(*F))
					vector->Visible(false);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Cache les objets en dehors d'un objet
//-----------------------------------------------------------------------------
void XGeoBase::HideOut(XGeoVector* V)
{
	if (!V->IsClosed())
		return;
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	XFrame F = V->Frame();
	bool flag_inside;
	if (!V->LoadGeom())
		return;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if (!layer->Visible())
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if (!classe->Visible())
				continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (vector == V)
					continue;
				if (!vector->Visible())
					continue;
				flag_inside = false;
				if (!vector->Frame().Intersect(F)) {
					vector->Visible(false);
					continue;
				}
				if (vector->NbPt() < 1)
					continue;
				vector->LoadGeom();
				// Point contenu dans le polygone
        uint32_t p;
        for (p = 0; p < vector->NbPt(); p++) {
					if (V->IsIn2D(vector->Pt(p))) {
						flag_inside = true;
						break;
					}
				} // endfor p
				if (flag_inside) {
					vector->Unload();
					continue;
				}

				// Ligne passant sur le polygone
				for (p = 0; p < vector->NbPt() - 1; p++) {
					if (V->IsIn2D(vector->Pt(p), vector->Pt(p+1))) {
						flag_inside = true;
						break;
					}
				} // endfor p
				if (flag_inside) {
					vector->Unload();
					continue;
				}

				vector->Visible(false);
				vector->Unload();
			} // endfor k
		} // endfor j
	} // endfor i
	V->Unload();
}

//-----------------------------------------------------------------------------
// Cache les objets en dehors ou en partie en dehors d'un objet
//-----------------------------------------------------------------------------
void XGeoBase::HideOutStrict(XGeoVector* V)
{
	if (!V->IsClosed())
		return;
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	XFrame F = V->Frame();
	bool flag_inside;
	if (!V->LoadGeom())
		return;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if (!layer->Visible())
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if (!classe->Visible())
				continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if (vector == V)
					continue;
				if (!vector->Visible())
					continue;
				if (!vector->Frame().Intersect(F)) {
					vector->Visible(false);
					continue;
				}
				if (vector->NbPt() < 1)
					continue;
				vector->LoadGeom();
				// Point contenu dans le polygone
				flag_inside = true;
        uint32_t p;
        for (p = 0; p < vector->NbPt(); p++) {
					if (!V->IsIn2D(vector->Pt(p))) {
						flag_inside = false;
						break;
					}
				} // endfor p
				if (!flag_inside) {
					vector->Visible(false);
					vector->Unload();
					continue;
				}

				// Ligne passant sur le polygone
				for (p = 0; p < vector->NbPt() - 1; p++) {
					if (V->IsIn2D(vector->Pt(p), vector->Pt(p+1))) {
						flag_inside = false;
						break;
					}
				} // endfor p
				if (!flag_inside) {
					vector->Visible(false);
					vector->Unload();
					continue;
				}

				vector->Unload();
			} // endfor k
		} // endfor j
	} // endfor i
	V->Unload();
}

//-----------------------------------------------------------------------------
// Calcul la resolution minimum
//-----------------------------------------------------------------------------
double XGeoBase::ComputeMinResol()
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
  double resol = 10.;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if (!layer->Visible()) continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if (!classe->Visible()) continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				//if ((vector->TypeVector() != XGeoVector::Raster)&&(vector->TypeVector() != XGeoVector::DTM))
				//	continue;
				resol = XMin (resol, vector->Resolution());
			}
		}
	}
	return resol;
}

//-----------------------------------------------------------------------------
// Indique quelle est la classe raster la plus haute (en profondeur d'affichage)
//-----------------------------------------------------------------------------
std::string XGeoBase::FindTopRasterClass()
{
	XGeoClass* classe;
	std::string top_raster;
	for (uint32_t i = 0; i < NbClass(); i++) {
		classe = Class(i);
    if (!classe->IsRaster())
      continue;
		top_raster = classe->Name();
	}
	return top_raster;
}

//-----------------------------------------------------------------------------
// Trouve l'objet raster le plus haut
//-----------------------------------------------------------------------------
XGeoVector* XGeoBase::FindTopRasterObject(XFrame* F)
{
	XGeoVector* top_vector = NULL;
	for (uint32_t i = 0; i < NbClass(); i++) {
		XGeoClass* classe = Class(i);
		if (!classe->Visible())
			continue;
		if (!classe->Layer()->Visible())
			continue;

		for (uint32_t j = 0; j < classe->NbVector(); j++) {
			XGeoVector* V = classe->Vector(j);
			if (!V->Visible())
				continue;
			if (V->Map() != NULL)
				if (!((XGeoObject*)V->Map())->Visible())
					continue;
			if (!V->Frame().Intersect(*F))
				continue;
			if (V->TypeVector() == XGeoVector::Raster)
				top_vector = V;
		}
	}
	return top_vector;
}

//-----------------------------------------------------------------------------
// Regarde le nombre d'objets dans un cadre
//-----------------------------------------------------------------------------
uint32_t XGeoBase::NbObjectFrame(XFrame* F, bool only_visible)
{
	XGeoLayer* layer = NULL;
	XGeoClass* classe;
	XGeoVector* vector;
	uint32_t nb = 0;
	for (uint32_t i = 0; i < m_Layer.size(); i++) {
		layer = m_Layer[i];
		if ((only_visible) && (!layer->Visible()))
			continue;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			classe = layer->Class(j);
			if ((only_visible) && (!classe->Visible()))
				continue;
			for (uint32_t k = 0; k < classe->NbVector(); k++) {
				vector = classe->Vector(k);
				if ((only_visible) && (!vector->Visible()))
					continue;
				if (vector->Frame().Intersect(*F))
					nb++;
			}
		}
	}
	return nb;
}

//-----------------------------------------------------------------------------
// Regarde si au moins un objet intersecte un cadre
//-----------------------------------------------------------------------------
bool XGeoBase::IntersectFrame(XFrame* F, bool only_visible)
{
  XGeoLayer* layer = NULL;
  XGeoClass* classe;
  XGeoVector* vector;
  for (uint32_t i = 0; i < m_Layer.size(); i++) {
    layer = m_Layer[i];
    if ((only_visible) && (!layer->Visible()))
      continue;
    for (uint32_t j = 0; j < layer->NbClass(); j++) {
      classe = layer->Class(j);
      if ((only_visible) && (!classe->Visible()))
        continue;
      for (uint32_t k = 0; k < classe->NbVector(); k++) {
        vector = classe->Vector(k);
        if ((only_visible) && (!vector->Visible()))
          continue;
        if (vector->Frame().Intersect(*F))
          if (vector->Intersect(*F))
            return true;
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// Trouve le Z a partir des MNT presents dans la base
//-----------------------------------------------------------------------------
double XGeoBase::Z(const XPt2D& P)
{
  XGeoLayer* layer = Layer("DTM");
  if (layer == NULL)
    return XGEO_NO_DATA;
  XGeoVector* V;
  double z;
  for (uint32_t i = 0; i < layer->NbClass(); i++) {
    XGeoClass* C = layer->Class(i);
    for (uint32_t j = 0; j < C->NbVector(); j++) {
      V = C->Vector(j);
      if (!V->Visible())
        continue;
      if (V->TypeVector() != XGeoVector::DTM)
        continue;
      z = V->Z(P);
      if (z > XGEO_NO_DATA) return z;
    }
  }
  return XGEO_NO_DATA;
}

//-----------------------------------------------------------------------------
// Trouve le Z a partir des LAS presents dans la base
//-----------------------------------------------------------------------------
double XGeoBase::ZLas(const XPt2D& P)
{
  XGeoLayer* layer = Layer("LAS");
  if (layer == NULL)
    return XGEO_NO_DATA;
  double Z = XGEO_NO_DATA, z = XGEO_NO_DATA, dmin = 9e99, d = 0.;
  for (uint32_t i = 0; i < layer->NbClass(); i++) {
    XGeoClass* C = layer->Class(i);
    if (!C->Visible())
      continue;
    for (uint32_t j = 0; j < C->NbVector(); j++) {
      XGeoVector* V = C->Vector(j);
      if (V->ZPos(P, z, d)) {
        if (d < dmin) {
          dmin = d;
          Z = z;
        }
      }
    }
  }
  if (dmin < 9e99)
    return Z;
  return XGEO_NO_DATA;
}

//-----------------------------------------------------------------------------
// Recuperation des Z sur une zone avec un pas donne
//-----------------------------------------------------------------------------
bool XGeoBase::Z(const XPt2D& TL, float* T, double delta, uint32_t w, uint32_t h)
{
  XGeoLayer* layer = Layer("DTM");
  if (layer == NULL)
    return false;
  float* ptr = T;
  XPt2D P;
  for (uint32_t i = 0; i < h; i ++) {
    P.Y = TL.Y - i * delta;
    for (uint32_t j = 0; j < w; j ++) {
      P.X = TL.X + j * delta;
      *ptr = Z(P);
      ptr++;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Retrait d'une classe d'objets
//-----------------------------------------------------------------------------
void XGeoBase::RemoveClass(const char* layername, const char* classname)
{
  XGeoClass* C = Class(layername, classname);
  if (C == NULL)
    return;

  // Retrait des references des objets dans les cartes
  /*
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    XGeoVector* V = C->Vector(i);
    for (uint32_t k = 0; k < m_Map.size(); k++) {
      XGeoMap* map = m_Map[k];
      for (uint32_t p = 0; p < map->NbMap(); p++) {
        map->GeoMap(p)->RemoveObject(V);
      }
      if (map->RemoveObject(V))
        break;
    }
    delete V;
  }*/

  for (uint32_t k = 0; k < m_Map.size(); k++) {
    XGeoMap* map = m_Map[k];
    for (uint32_t p = 0; p < map->NbMap(); p++) {
      map->GeoMap(p)->RemoveClass(C);
    }
    map->RemoveClass(C);
  }
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    XGeoVector* V = C->Vector(i);
    delete V;
  }

  // Retrait des references des objets dans les classes
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    XGeoVector* V = C->Vector(i);
    for (uint32_t p = 0; p < m_Class.size(); p++) {
      XGeoClass* classe = m_Class[p];
      if (V == classe->Mask())
        classe->Mask(NULL);
    }
  }

	// Retrait de la classe dans la liste des classes
	std::vector<XGeoClass*>::iterator iter;
	for (iter = m_Class.begin(); iter != m_Class.end(); iter++) {
		if (*iter == C) {
			m_Class.erase(iter);
			break;
		}
	}

	// Retrait de la classe dans le layer
	XGeoLayer* layer = NULL;
	for (uint32_t k = 0; k < m_Layer.size(); k++)
		if (m_Layer[k]->RemoveClass(C)) {
			layer = m_Layer[k];
			break;
		}
	if (layer != NULL) {
		if (layer->NbClass() < 1) {
			std::vector<XGeoLayer*>::iterator iterL;
			for (iterL = m_Layer.begin(); iterL != m_Layer.end(); iterL++) {
				if (*iterL == layer) {
					m_Layer.erase(iterL);
					break;
				}
			}
		}
	}

  // Retrait des map vides
  std::vector<XGeoMap*> newMap;
  for (uint32_t i = 0; i < m_Map.size(); i++) {
    if (m_Map[i]->NbObject() > 0)
      newMap.push_back(m_Map[i]);
    else
      delete m_Map[i];
  }
  m_Map.clear();
  m_Map = newMap;

	// Recalcul des emprises
	for (uint32_t k = 0; k < m_Map.size(); k++)
		m_Map[k]->UpdateFrame();
	UpdateFrame();

//	delete C;
}

