//-----------------------------------------------------------------------------
//								XShapefileRecord.h
//								==================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 19/09/2003
//-----------------------------------------------------------------------------

#ifndef _XSHAPEFILERECORD_H
#define _XSHAPEFILERECORD_H

#include "XShapefile.h"
#include "../XTool/XEndian.h"
#include "../XTool/XPt2D.h"
#include "../XTool/XPt3D.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoPoly.h"

//-----------------------------------------------------------------------------
// Entete d'un enregistrement
//-----------------------------------------------------------------------------
class XShapefileRecordHeader {
protected:
  static XEndian	m_Endian;

  int m_nNumber;	// Record Number
	int m_nLength;	// Content Length

public:
	XShapefileRecordHeader() { m_nNumber = 0; m_nLength = 0;}
	XShapefileRecordHeader(int n, int l) { m_nNumber = n; m_nLength = l;}
	~XShapefileRecordHeader() {;}

	inline int Length() const { return m_nLength;}
	inline int Number() const { return m_nNumber;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);
};

//-----------------------------------------------------------------------------
// Enregistrement
//-----------------------------------------------------------------------------
class XShapefileRecord : public XGeoVector {
public:
	enum ePartType { TriangleStrip = 0, TriangleFan = 1, OuterRing = 2, InnerRing = 3, 
		FirstRing = 4, Ring = 5 };

protected:
	static XEndian	m_Endian;

	eShapefileType	m_eShapeType;
	int			m_nNumPoints;
	int			m_nNumParts;
	XPt*		m_Pt;
	double*	m_Z;
	double*	m_ZRange;
	double*	m_M;
	double* m_MRange;
	int*		m_Parts;
	ePartType*	m_PartTypes;
	uint32_t					m_Pos;						// Position de la geometrie dans le fichier
	uint32_t					m_nIndex;					// Index dans le fichier
	XShapefile*			m_File;

	bool ReadPoint2D(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadMultiPoint2D(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadPolyLine2D(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadPoint2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadMultiPoint2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadPolyLine2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadPoint3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadMultiPoint3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadPolyLine3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);
	bool ReadMultiPatch(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error);

	bool ReadFrame(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadXYGeom(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadZGeom(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadMGeom(std::ifstream* in, XEndian* endian, XError* error);
	bool ReadParts(std::ifstream* in, XEndian* endian, XError* error);

	bool WritePoint2D(std::ofstream* in, XEndian* endian, XError* error);
	bool WriteMultiPoint2D(std::ofstream* in, XEndian* endian, XError* error);
	bool WritePolyLine2D(std::ofstream* in, XEndian* endian, XError* error);
	bool WritePoint2DM(std::ofstream* in, XEndian* endian, XError* error);
	bool WriteMultiPoint2DM(std::ofstream* in, XEndian* endian, XError* error);
	bool WritePolyLine2DM(std::ofstream* in, XEndian* endian, XError* error);
	bool WritePoint3D(std::ofstream* in, XEndian* endian, XError* error);
	bool WriteMultiPoint3D(std::ofstream* in, XEndian* endian, XError* error);
	bool WritePolyLine3D(std::ofstream* in, XEndian* endian, XError* error);
	bool WriteMultiPatch(std::ofstream* in, XEndian* endian, XError* error);

	bool Convert(XGeoPoint2D* point);
	bool Convert(XGeoPoint3D* point);

	bool Convert(XGeoMPoint2D* point);
	bool Convert(XGeoMPoint3D* point);
	
	bool Convert(XGeoLine2D* line);
	bool Convert(XGeoLine3D* line);
	bool Convert(XGeoMLine2D* line);
	bool Convert(XGeoMLine3D* line);

	bool Convert(XGeoPoly2D* poly);
	bool Convert(XGeoPoly3D* poly);
	bool Convert(XGeoMPoly2D* poly);
	bool Convert(XGeoMPoly3D* poly);

  bool ConvertVector(XGeoVector* vector);

public:
	XShapefileRecord();
	~XShapefileRecord();

	inline double ZMin() const { if (m_ZRange != NULL) return m_ZRange[0]; return XGEO_NO_DATA;}
	inline double ZMax() const { if (m_ZRange != NULL) return m_ZRange[1]; return XGEO_NO_DATA;}
	inline double MMin() const { if (m_MRange != NULL) return m_MRange[0]; return XGEO_NO_DATA;}
	inline double MMax() const { if (m_MRange != NULL) return m_MRange[1]; return XGEO_NO_DATA;}

	uint32_t ByteSize() const;
	inline eShapefileType ShapefileType() const { return m_eShapeType;}

	inline XShapefile* Shapefile() const { return m_File;}
	void Shapefile(XShapefile* shapefile) { m_File = shapefile;}

	bool Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	void Unload();
	bool LoadGeom();

	bool ReadAttributes(std::vector<std::string>& V);

	virtual bool IsNear2D(XPt2D P, double dist);
  //bool GetPolyContour(XPolyContour* P);

	bool Convert(XGeoVector* V);

	bool Convert(XGeodConverter* L);

	virtual bool WriteHtml(std::ostream* out);
};

#endif //_XSHAPEFILERECORD_H
