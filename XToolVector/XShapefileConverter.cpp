//-----------------------------------------------------------------------------
//								XShapefileConverter.cpp
//								=======================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 19/09/2003
//-----------------------------------------------------------------------------

#include "XShapefileConverter.h"
#include "../XTool/XGeoBase.h"
#include "XShapefile.h"
#include "XShapefileRecord.h"
#include "XDBase.h"

//-----------------------------------------------------------------------------
// Conversion d'une base
//-----------------------------------------------------------------------------
bool XShapefileConverter::ConvertBase(XGeoBase* base, const char* folder, XWait* wait)
{
	std::string layer_folder;
	uint32_t nb_class = base->NbClass();
	XWaitRange(wait, 0, nb_class);
	for (uint32_t i = 0; i < base->NbLayer(); i++) {
		XGeoLayer* layer = base->Layer(i);
		if (!layer->Visible())
			continue;
    layer_folder = (std::string)folder + "/" + layer->Name();
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			XGeoClass* classe = layer->Class(j);
			XWaitStatus(wait, ("Export de la classe " + classe->Name()).c_str());
			if ((!m_bVisibleOnly)||(classe->Visible()))
				ConvertClass(classe, layer_folder.c_str());
			XWaitStepIt(wait);
			if (XWaitCheckCancel(wait))
				return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'une classe
//-----------------------------------------------------------------------------
bool XShapefileConverter::ConvertClass(XGeoClass* classe, const char* folder)
{
	XShapefile shapefile;
	XGeoVector* vector;
	XDBaseFile dbase;
  std::string filename = (std::string)folder + "/" + classe->Name();

	// Filtrage des classes raster
  if (classe->NbVector() < 1)
    return true;
	vector = classe->Vector((uint32_t)0);
	if (vector->TypeVector() == XGeoVector::Raster)
		return ConvertClassRaster(classe, folder);
  if (vector->TypeVector() == XGeoVector::DTM)
    return ConvertClassRaster(classe, folder);

	// Nombre de vecteurs a exporter
	uint32_t nb_att, nb_vector = classe->NbVector();
	if (m_bVisibleOnly)
		for (uint32_t i = 0; i < classe->NbVector(); i++) {
			vector = classe->Vector(i);
			if (!vector->Visible())
				nb_vector--;
		}
	if (nb_vector < 1)
		return true;

	// Ecriture des attributs
	bool label = false;
	std::vector<std::string> V;
	char buf[256];
	XGeoSchema* schema = classe->Schema();
	if (schema->NbAttribut() < 1) {
		vector = classe->Vector((uint32_t)0);
		vector->ReadAttributes(V);
		nb_att = (uint32_t)(V.size() / 2);
		for (uint32_t i = 0; i < nb_att; i++)
			dbase.AddField(V[i* 2].c_str(), 'C', 80);
		if ((nb_att == 0)&&(vector->Name().size() > 0)) {
			dbase.AddField("NOM", 'C', 80);
			dbase.AddField("IMPORTANCE", 'N', 8);
			label = true;
		}
	} else {
		nb_att = schema->NbAttribut();
		for (uint32_t i = 0; i < nb_att; i++) {
			switch(schema->AttributType(i)) {
				case XGeoAttribut::Bool : dbase.AddField(schema->AttributShortName(i).c_str(), 'C', 8); break;
				case XGeoAttribut::Int16 : dbase.AddField(schema->AttributShortName(i).c_str(), 'N', 8); break;
				case XGeoAttribut::Int32 : dbase.AddField(schema->AttributShortName(i).c_str(), 'N', 12); break;
				case XGeoAttribut::Double : dbase.AddField(schema->AttributShortName(i).c_str(), 'N', 20); break;
				case XGeoAttribut::String : dbase.AddField(schema->AttributShortName(i).c_str(), 'C', schema->AttributLength(i)); break;
				case XGeoAttribut::List : dbase.AddField(schema->AttributShortName(i).c_str(), 'C', schema->AttributLength(i)); break;
				case XGeoAttribut::NumericN : 
					dbase.AddField(schema->AttributShortName(i).c_str(), 'N', schema->AttributLength(i), schema->AttributDecCount(i));
					break;
				case XGeoAttribut::NumericF : 
					dbase.AddField(schema->AttributShortName(i).c_str(), 'F', schema->AttributLength(i), schema->AttributDecCount(i));
					break;
			}
		}
	}
	
	dbase.SetNbRecord(nb_vector);
	dbase.WriteHeader((filename + ".dbf").c_str());
	for (uint32_t i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		vector->ReadAttributes(V);
		if (schema->NbAttribut() > 0)
			schema->SortAttribut(&V);
		if (label) {
			V.clear();
			V.push_back("NOM");
			V.push_back(vector->Name());
			V.push_back("IMPORTANCE");
			snprintf(buf, 256, "%d", vector->Importance());
			V.push_back(buf);
		}
		dbase.WriteRecord(V);
	}

	// Ecriture des geometries
	for (uint32_t i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		if(!LoadVector(vector))
			continue;
		XShapefileRecord* record = new XShapefileRecord;
		if (record->Convert(vector)) {
			shapefile.AddObject(record);
		} else {
			delete record;
		}
		vector->Unload();
	}
	return shapefile.Write(filename.c_str());
}

//-----------------------------------------------------------------------------
// Conversion d'une classe raster
//-----------------------------------------------------------------------------
bool XShapefileConverter::ConvertClassRaster(XGeoClass* classe, const char* folder)
{
	XShapefile shapefile;
	XGeoVector* vector;
	XDBaseFile dbase;
  std::string filename = (std::string)folder + "/" + classe->Name();

	// Nombre de vecteurs a exporter
	uint32_t nb_vector = classe->NbVector();
	if (m_bVisibleOnly)
		for (uint32_t i = 0; i < classe->NbVector(); i++) {
			vector = classe->Vector(i);
			if (!vector->Visible())
				nb_vector--;
		}
	if (nb_vector < 1)
		return true;

	// Ecriture des attributs
	std::vector<std::string> V;
	dbase.AddField("LOCATION", 'C', 255);	
	dbase.SetNbRecord(nb_vector);
	dbase.WriteHeader((filename + ".dbf").c_str());
	for (uint32_t i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
    if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		V.clear();
		V.push_back("LOCATION");
    V.push_back(vector->Filename());
		dbase.WriteRecord(V);
	}

	// Ecriture des geometries
	for (uint32_t i = 0; i < classe->NbVector(); i++) {
		vector = classe->Vector(i);
		if (m_bVisibleOnly)
			if (!vector->Visible())
				continue;
		XShapefileRecord* record = new XShapefileRecord;
		if (record->Convert(vector)) {
			shapefile.AddObject(record);
		} else {
			delete record;
		}
		vector->Unload();
	}
	return shapefile.Write(filename.c_str());
}

//-----------------------------------------------------------------------------
// Chargement d'un vecteur
//-----------------------------------------------------------------------------
bool XShapefileConverter::LoadVector(XGeoVector* vector)
{
	return vector->LoadGeom();
//	vector->LoadGeom();
//	vector->InterpolZ0();
//	return true;
}
