//-----------------------------------------------------------------------------
//								XShapefileVector.h
//								==================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/09/2003
//-----------------------------------------------------------------------------

#ifndef _XSHAPEFILEVECTOR_H
#define _XSHAPEFILEVECTOR_H

#include "XShapefile.h"
#include "XShapefileRecord.h"


//-----------------------------------------------------------------------------
// XShapefileVector
//-----------------------------------------------------------------------------
class XShapefileVector {
protected :
	XShapefile*			m_File;
	uint32_t					m_nIndex;

public :
	XShapefileVector() { m_File = NULL;}

	inline XShapefile* Shapefile() const { return m_File;}
	inline uint32_t Index() const { return m_nIndex;}
	void Shapefile(XShapefile* shapefile, uint32_t index) { m_File = shapefile; m_nIndex = index;}

	bool ReadDBFAttributes(std::vector<std::string>& V);
	std::string FindDBFAttribute(std::string att_name, bool no_case = false);
};

//-----------------------------------------------------------------------------
// Point
//-----------------------------------------------------------------------------
class XShapefilePoint2D : public XGeoPoint2D, public XShapefileVector {
public:
	XShapefilePoint2D() { m_File = NULL;}

	uint32_t ByteSize() const {return 20;}

	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false) 
																										{ return FindDBFAttribute(att_name, no_case);}
};

class XShapefilePoint3D : public XGeoPoint3D, public XShapefileVector {
public:
	XShapefilePoint3D() { m_File = NULL;}

	uint32_t ByteSize() const {return 36;}

	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
};

//-----------------------------------------------------------------------------
// Multi-Point
//-----------------------------------------------------------------------------
class XShapefileMPoint2D : public XGeoMPoint2D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMPoint2D() { m_Pos = 0; m_File = NULL;}

	uint32_t ByteSize() const {return 36 + 16 * m_nNumPoints;}

	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
};

class XShapefileMPoint3D : public XGeoMPoint3D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMPoint3D() { m_Pos = 0; m_File = NULL;}

	uint32_t ByteSize() const {return 36 + 16 * m_nNumPoints + 16 + m_nNumPoints * 8; ;}

	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
	virtual bool LoadGeom2D();
};

//-----------------------------------------------------------------------------
// Mutli-Ligne 2D
//-----------------------------------------------------------------------------
class XShapefileMLine2D : public XGeoMLine2D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMLine2D() { m_Pos = 0; m_File = NULL;}
	
	uint32_t ByteSize() const { return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;}

	inline uint32_t PosGeom() const { return m_Pos;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Ligne 2D
//-----------------------------------------------------------------------------
class XShapefileLine2D : public XGeoLine2D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileLine2D() { m_Pos = 0; m_File = NULL;}
	
	bool Clone(XShapefileMLine2D* P);

	virtual inline XGeoMap* Map() const { return m_File;}

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Multi-Polyligne 3D
//-----------------------------------------------------------------------------
class XShapefileMLine3D : public XGeoMLine3D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMLine3D() { m_Pos = 0; m_File = NULL;}
	
	uint32_t ByteSize() const { return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8;}

	inline uint32_t PosGeom() const { return m_Pos;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
	virtual bool LoadGeom2D();
};

//-----------------------------------------------------------------------------
// Polyligne 3D
//-----------------------------------------------------------------------------
class XShapefileLine3D : public XGeoLine3D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileLine3D() { m_Pos = 0; m_File = NULL;}
	
	bool Clone(XShapefileMLine3D* P);

	virtual inline XGeoMap* Map() const { return m_File;}

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
	virtual bool LoadGeom2D();
};

//-----------------------------------------------------------------------------
// Multi-Polygone 2D
//-----------------------------------------------------------------------------
class XShapefileMPoly2D : public XGeoMPoly2D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMPoly2D() { m_Pos = 0; m_File = NULL;}

	uint32_t ByteSize() const { return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;}

	inline uint32_t PosGeom() const { return m_Pos;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Polygone 2D
//-----------------------------------------------------------------------------
class XShapefilePoly2D : public XGeoPoly2D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefilePoly2D() { m_Pos = 0; m_File = NULL;}

	bool Clone(XShapefileMPoly2D* P);

	virtual inline XGeoMap* Map() const { return m_File;}

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
};

//-----------------------------------------------------------------------------
// Multi-Polygone 3D
//-----------------------------------------------------------------------------
class XShapefileMPoly3D : public XGeoMPoly3D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefileMPoly3D() { m_Pos = 0; m_File = NULL;}

	uint32_t ByteSize() const { return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8;}

	inline uint32_t PosGeom() const { return m_Pos;}
	virtual inline XGeoMap* Map() const { return m_File;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
	virtual bool LoadGeom2D();
};

//-----------------------------------------------------------------------------
// Polygone 3D
//-----------------------------------------------------------------------------
class XShapefilePoly3D : public XGeoPoly3D, public XShapefileVector {
protected :
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier

public:
	XShapefilePoly3D() { m_Pos = 0; m_File = NULL;}
	
	bool Clone(XShapefileMPoly3D* P);

	virtual inline XGeoMap* Map() const { return m_File;}

	virtual	bool ReadAttributes(std::vector<std::string>& V) { return ReadDBFAttributes(V);}
	virtual std::string FindAttribute(std::string att_name, bool no_case = false)
																										{ return FindDBFAttribute(att_name, no_case);}
	virtual bool LoadGeom();
	virtual bool LoadGeom2D();
};


#endif //_XSHAPEFILEVECTOR_H
