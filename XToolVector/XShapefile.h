//-----------------------------------------------------------------------------
//								XShapefile.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/03/2003
//-----------------------------------------------------------------------------

#ifndef _XSHAPEFILE_H
#define _XSHAPEFILE_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoVector.h"
#include "../XTool/XFile.h"

class XGeoBase;
class XGeodConverter;
class XShapefileRecord;
class XDBaseFile;

enum eShapefileType { shpNullShape= 0, shpPoint = 1,
											shpPolyLine = 3, 
											shpPolygone = 5,
											shpMultiPoint = 8, shpPointZ = 11,
											shpPolyLineZ = 13, shpPolygonZ = 15, shpMultiPointZ = 18, shpPointM = 21, shpPolyLineM = 23, 
											shpPolygonM = 25, shpMultiPointM = 28, shpMultiPatch = 31};

//-----------------------------------------------------------------------------
// Entete d'un fichier Shapefile
//-----------------------------------------------------------------------------
class XShapefileHeader {
friend class XShapefile; 
protected:
	int m_nFileCode;	// Byte 0 File Code 9994 Integer Big
	int m_nUnused1;		// Byte 4 Unused 0 Integer Big
	int m_nUnused2;		// Byte 8 Unused 0 Integer Big
	int m_nUnused3;		// Byte 12 Unused 0 Integer Big
	int m_nUnused4;		// Byte 16 Unused 0 Integer Big
	int m_nUnused5;		// Byte 20 Unused 0 Integer Big
	int m_nFileLength;// Byte 24 File Length File Length Integer Big
	int m_nVersion;		// Byte 28 Version 1000 Integer Little
	eShapefileType m_eShapeType;	// Byte 32 Shape Type Shape Type Integer Little
	double m_dXmin;		// Byte 36 Bounding Box Xmin Double Little
	double m_dYmin;		// Byte 44 Bounding Box Ymin Double Little
	double m_dXmax;		// Byte 52 Bounding Box Xmax Double Little
	double m_dYmax;		// Byte 60 Bounding Box Ymax Double Little
	double m_dZmin;		// Byte 68* Bounding Box Zmin Double Little
	double m_dZmax;		// Byte 76* Bounding Box Zmax Double Little
	double m_dMmin;		// Byte 84* Bounding Box Mmin Double Little
	double m_dMmax;		// Byte 92* Bounding Box Mmax Double Little
	//* Unused, with value 0.0, if not Measured or Z type

public:
	XShapefileHeader();
	~XShapefileHeader() {;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);
};

//-----------------------------------------------------------------------------
// Fichier ShapeFile
//-----------------------------------------------------------------------------
class XShapefile : public XGeoMap {
protected:
	std::string											m_strFilename;
	XShapefileHeader								m_Header;
  //std::ifstream										m_In;
  XFile                           m_In;
	XDBaseFile*											m_DBFile;

	virtual XShapefileRecord* CreateShapefileRecord();

	bool OpenDBF();

public:
	XShapefile() {m_DBFile = NULL;}
	~XShapefile();

	eShapefileType ShapeType() const { return m_Header.m_eShapeType;}
	virtual inline double Xmin() const { return m_Header.m_dXmin;}
	virtual inline double Ymin() const { return m_Header.m_dYmin;}
	virtual inline double Zmin() const { return m_Header.m_dZmin;}
	double Mmin() const { return m_Header.m_dMmin;}
	virtual inline double Xmax() const { return m_Header.m_dXmax;}
	virtual inline double Ymax() const { return m_Header.m_dYmax;}
	virtual inline double Zmax() const { return m_Header.m_dZmax;}
	double Mmax() const { return m_Header.m_dMmax;}

	bool Read(const char* filename, XError* error = NULL);
	bool Write(const char* filename, XError* error = NULL);

  //inline std::ifstream* IStream() { return &m_In;}
  std::ifstream* IStream() { return m_In.IStream();}

	void Class(XGeoClass* C);
	XGeoClass* Class();

	bool ReadAttributes(uint32_t index, std::vector<std::string>& V);
	int FindField(std::string fieldname, bool no_case = false);

  bool Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error = NULL);

  static XGeoClass* ImportShapefile(XGeoBase* base, const char* path, XGeoMap* map = NULL);
};


#endif
