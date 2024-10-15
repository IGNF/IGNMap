//-----------------------------------------------------------------------------
//								XShapefileConverter.h
//								=====================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 19/09/2003
//-----------------------------------------------------------------------------

#ifndef _XSHAPEFILECONVERTER_H
#define _XSHAPEFILECONVERTER_H

#include "../XTool/XBase.h"

class XGeoBase;
class XGeoClass;
class XGeoVector;

class XShapefileConverter {
protected:
	bool		m_bVisibleOnly;	// Copie des objets visible uniquement

public:
	XShapefileConverter() {	m_bVisibleOnly = false;}

	bool ConvertBase(XGeoBase* base, const char* folder, XWait* wait = NULL);
	bool ConvertClass(XGeoClass* classe, const char* folder);
	bool ConvertClassRaster(XGeoClass* classe, const char* folder);

	void ConvertVisibleOnly(bool flag) { m_bVisibleOnly = flag;}

	bool LoadVector(XGeoVector* vector);
};

#endif //_XSHAPEFILECONVERTER_H
