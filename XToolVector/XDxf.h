//-----------------------------------------------------------------------------
//								XDxf.h
//								======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/05/2004
//-----------------------------------------------------------------------------

#ifndef _XDFX_H
#define _XDFX_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoly.h"

class XGeoBase;
class XGeodConverter;

class XDxf : public XGeoMap {
protected:
	std::string								m_strFilename;
	std::ifstream							m_In;
	char											m_Line[1024];

public:
	XDxf() { m_strFilename = "Inconnu";}

	inline std::ifstream* IStream() { return &m_In;}

	void Class(XGeoClass* C);
	XGeoClass* Class();

	bool Read(const char* filename, XGeoBase* base, XError* error = NULL);
	bool LoadGeom(uint32_t pos, XPt* P, uint32_t nbpt, double* Z, double* ZRange);

	bool Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error = NULL);

  static bool ImportDxf(XGeoBase* base, const char* path, XGeoMap* map = NULL);
};

//-----------------------------------------------------------------------------
// Point 2D
//-----------------------------------------------------------------------------
class XDxfPoint2D : public XGeoPoint2D {
protected :
	XDxf*					m_File;

public:
	XDxfPoint2D(XDxf* file = NULL) { m_File = file;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	void SetFrame(double x, double y) { m_Frame = XFrame(x, y, x, y);}
};

//-----------------------------------------------------------------------------
// Point 3D
//-----------------------------------------------------------------------------
class XDxfPoint3D : public XGeoPoint3D {
protected :
	XDxf*					m_File;

public:
	XDxfPoint3D(XDxf* file = NULL) { m_File = file;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	void SetFrame(double x, double y, double z) { m_Frame = XFrame(x, y, x, y); m_Z = z;}
};

//-----------------------------------------------------------------------------
// Texte 2D
//-----------------------------------------------------------------------------
class XDxfText2D : public XDxfPoint2D {
protected:
	std::string		m_strName;
	uint16_t				m_nImportance;

public:
	XDxfText2D(XDxf* file = NULL) : XDxfPoint2D(file) { m_nImportance = 0;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}
	virtual uint16_t Importance() { return m_nImportance;}

	void Importance(uint16_t i) { m_nImportance = i;}
};

//-----------------------------------------------------------------------------
// Texte 3D
//-----------------------------------------------------------------------------
class XDxfText3D : public XDxfPoint3D {
protected:
	std::string		m_strName;
	uint16_t				m_nImportance;

public:
	XDxfText3D(XDxf* file = NULL) : XDxfPoint3D(file) { m_nImportance = 0;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}
	virtual uint16_t Importance() { return m_nImportance;}

	void Importance(uint16_t i) { m_nImportance = i;}
};

//-----------------------------------------------------------------------------
// Ligne 2D
//-----------------------------------------------------------------------------
class XDxfLine2D : public XGeoLine2D {
protected :
	uint32_t				m_Pos;						// Position de la geometrie dans le fichier
	XDxf*					m_File;

public:
	XDxfLine2D(XDxf* file = NULL, uint32_t pos = 0) { m_File = file; m_Pos = pos;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual bool LoadGeom();
	void SetFrame(XFrame* XYF, uint32_t nbpt);
};

//-----------------------------------------------------------------------------
// Ligne 3D
//-----------------------------------------------------------------------------
class XDxfLine3D : public XGeoLine3D {
protected :
	uint32_t				m_Pos;						// Position de la geometrie dans le fichier
	XDxf*					m_File;

public:
	XDxfLine3D(XDxf* file = NULL, uint32_t pos = 0) { m_File = file; m_Pos = pos;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual bool LoadGeom();
	void SetFrame(XFrame* XYF, uint32_t nbpt);
};

//-----------------------------------------------------------------------------
// Polygone 2D
//-----------------------------------------------------------------------------
class XDxfPoly2D : public XGeoPoly2D {
protected :
	uint32_t				m_Pos;						// Position de la geometrie dans le fichier
	XDxf*					m_File;

public:
	XDxfPoly2D(XDxf* file = NULL, uint32_t pos = 0) { m_File = file; m_Pos = pos;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual bool LoadGeom();
	void SetFrame(XFrame* XYF, uint32_t nbpt);
};

//-----------------------------------------------------------------------------
// Polygone 3D
//-----------------------------------------------------------------------------
class XDxfPoly3D : public XGeoPoly3D {
protected :
	uint32_t				m_Pos;						// Position de la geometrie dans le fichier
	XDxf*					m_File;

public:
	XDxfPoly3D(XDxf* file = NULL, uint32_t pos = 0) { m_File = file; m_Pos = pos;}

	inline XDxf* File() const { return m_File;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual bool LoadGeom();
	void SetFrame(XFrame* XYF, uint32_t nbpt);
};

#endif //_XDFX_H
