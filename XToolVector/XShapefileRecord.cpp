//-----------------------------------------------------------------------------
//								XShapefileRecord.cpp
//								====================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 19/09/2003
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>
#include "XShapefileRecord.h"
#include "../XTool/XEndian.h"
#include "../XToolGeod/XLambert.h"
//#include "XPolyContour.h"

XEndian XShapefileRecord::m_Endian;
XEndian XShapefileRecordHeader::m_Endian;

//-----------------------------------------------------------------------------
// Lecture dans un fichier
//-----------------------------------------------------------------------------
bool XShapefileRecordHeader::Read(std::ifstream* in, XError* error)
{
  m_Endian.Read(in, false, &m_nNumber, 4);
  m_Endian.Read(in, false, &m_nLength, 4);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier
//-----------------------------------------------------------------------------
bool XShapefileRecordHeader::Write(std::ofstream* out, XError* error)
{
  m_Endian.Write(out, false, &m_nNumber, 4);
  m_Endian.Write(out, false, &m_nLength, 4);

	return out->good();
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XShapefileRecord::XShapefileRecord()
{
	m_eShapeType = shpNullShape;
	m_nNumPoints = m_nNumParts = 0;
	m_Pt = NULL;
	m_Z = NULL;
	m_M = NULL;
	m_Parts = NULL;
	m_PartTypes = NULL;
	m_ZRange = NULL;
	m_MRange = NULL;
	m_Pos = 0;
	m_nIndex = 0;
	m_File = NULL;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XShapefileRecord::~XShapefileRecord()
{
	Unload();
}

//-----------------------------------------------------------------------------
// Dechargement des donnees chargees dynamiquement
//-----------------------------------------------------------------------------
void XShapefileRecord::Unload()
{ 
	if (m_Pt != NULL)
		delete[] m_Pt;
	if (m_Z != NULL)
		delete[] m_Z;
	if (m_ZRange != NULL)
		delete[] m_ZRange;
	if (m_M != NULL)
		delete[] m_M;
	if (m_MRange != NULL)
		delete[] m_MRange;
	if (m_Parts != NULL)
		delete[] m_Parts;
	if (m_PartTypes != NULL)
		delete[] m_PartTypes;
	m_Pt = NULL;
	m_Z = NULL;
	m_M = NULL;
	m_Parts = NULL;
	m_PartTypes = NULL;
	m_ZRange = NULL;
	m_MRange = NULL;
}

//-----------------------------------------------------------------------------
// Taille en octets
//-----------------------------------------------------------------------------
uint32_t XShapefileRecord::ByteSize() const
{
	switch(m_eShapeType) {
		case shpNullShape : 
			return 4;
		case shpPoint : 
			return 20;
		case shpPolyLine :
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;
		case shpPolygone :
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;
		case shpMultiPoint :
			return 40 + m_nNumPoints * 16;
		case shpPointZ :
			return 36;
		case shpPolyLineZ :
			if (m_M == NULL)
				return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8;
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8 + m_nNumPoints * 8 + 16;
		case shpPolygonZ :
			if (m_M == NULL)
				return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8;
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + 16 + m_nNumPoints * 8 + m_nNumPoints * 8 + 16;
		case shpMultiPointZ :
			if (m_M == NULL)
				return 40 + m_nNumPoints * 16 + 16 + m_nNumPoints * 8;
			return 40 + m_nNumPoints * 16 + 16 + m_nNumPoints * 8 + m_nNumPoints * 8 + 16;
		case shpPointM :
			return 28;
		case shpPolyLineM : 
			if (m_M == NULL)
				return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + m_nNumPoints * 8 + 16;
		case shpPolygonM : 
			if (m_M == NULL)
				return 44 + 4 * m_nNumParts + 16 * m_nNumPoints;
			return 44 + 4 * m_nNumParts + 16 * m_nNumPoints + m_nNumPoints * 8 + 16;
		case shpMultiPointM :
			if (m_M == NULL)
				return 40 + m_nNumPoints * 16;
			return 40 + m_nNumPoints * 16 + m_nNumPoints * 8 + 16;
		case shpMultiPatch :
			if (m_M == NULL)
				return 44 + 4 * m_nNumParts + 4 * m_nNumParts + 16 * m_nNumPoints +16 + m_nNumPoints * 8;
			return 44 + 4 * m_nNumParts + 4 * m_nNumParts + 16 * m_nNumPoints +16 + m_nNumPoints * 8 + m_nNumPoints * 8 + 16;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Lecture
//-----------------------------------------------------------------------------
bool XShapefileRecord::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
  int pos = in->tellg();
  XEndian endian;
  endian.Read(in, true, &m_eShapeType, 4);

  bool flag;
  m_nIndex = header->Number();
  switch(m_eShapeType) {
    case shpNullShape :
      flag = in->good(); break;
    case shpPoint :
      flag = ReadPoint2D(in, &endian, error); break;
    case shpPolyLine :
      flag = ReadPolyLine2D(in, &endian, error); break;
    case shpPolygone :
      flag = ReadPolyLine2D(in, &endian, error); break;
    case shpMultiPoint :
      flag = ReadMultiPoint2D(in, &endian, error); break;
    case shpPointZ :
      flag = ReadPoint3D(in, &endian, header, error); break;
    case shpPolyLineZ :
      flag = ReadPolyLine3D(in, &endian, header, error); break;
    case shpPolygonZ :
      flag = ReadPolyLine3D(in, &endian, header, error); break;
    case shpMultiPointZ :
      flag = ReadMultiPoint3D(in, &endian, header, error); break;
    case shpPointM :
      flag = ReadPoint2DM(in, &endian, header, error); break;
    case shpPolyLineM :
      flag = ReadPolyLine2DM(in, &endian, header, error); break;
    case shpPolygonM :
      flag = ReadPolyLine2DM(in, &endian, header, error); break;
    case shpMultiPointM :
      flag = ReadMultiPoint2DM(in, &endian, header, error); break;
    case shpMultiPatch :
      flag = ReadMultiPatch(in, &endian, header, error); break;
      break;
    default:
      flag = false;
  }
  in->seekg(pos + header->Length() * 2L);
  if (!flag)
    return XErrorAlert(error,"XShapefileRecord::Write", XError::eBadFormat);

  return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des points 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPoint2D(std::ifstream* in, XEndian* endian, XError* error)
{
	endian->Read(in, true, &m_Frame.Xmin, 8);
	endian->Read(in, true, &m_Frame.Ymin, 8);
	m_Frame.Xmax = m_Frame.Xmin;
	m_Frame.Ymax = m_Frame.Ymin;
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des multi-points 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadMultiPoint2D(std::ifstream* in, XEndian* endian, XError* error)
{
	ReadFrame(in, endian, error);
	endian->Read(in, true, &m_nNumPoints, 4);
	m_Pos = in->tellg();
	if (!ReadXYGeom(in, endian, error))
		return false;
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des polylignes et des polygones 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPolyLine2D(std::ifstream* in, XEndian* endian, XError* error)
{
	ReadFrame(in, endian, error);

	endian->Read(in, true, &m_nNumParts, 4);
	endian->Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	if (!ReadParts(in, endian, error))
		return false;
	if (!ReadXYGeom(in, endian, error))
		return false;

	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des points 2D avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPoint2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	if (ReadPoint2D(in, endian, error)) {
		if (in->tellg() < pos + header->Length()) {
			m_MRange = new double[2];
			if (m_MRange == NULL)
				return XErrorError(error, "XShapefileRecord::ReadPoint2DM", XError::eAllocation);
			endian->Read(in, true, &m_MRange[0], 8);
			m_MRange[1] = m_MRange[0];
		}
		return in->good();
	}
	return false;
}

//-----------------------------------------------------------------------------
// Lecture des des polylignes et des polygones 2D avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadMultiPoint2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	if (ReadMultiPoint2D(in, endian, error)) {
		if (in->tellg() < pos + header->Length()) 
			if (!ReadMGeom(in, endian, error))
				return false;
		return in->good();
	}
	return false;
}

//-----------------------------------------------------------------------------
// Lecture des multi-points avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPolyLine2DM(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	if (ReadPolyLine2D(in, endian, error)) {
		if (in->tellg() < pos + header->Length()) {
			m_MRange = new double[2];
			if (m_MRange == NULL)
				return XErrorError(error, "XShapefileRecord::ReadPolyLine2DM", XError::eAllocation);
			endian->Read(in, true, &m_MRange[0], 8);
			endian->Read(in, true, &m_MRange[1], 8);
			m_M = new double[m_nNumPoints];
			if (m_M == NULL) 
				return XErrorError(error, "XShapefileRecord::ReadPolyLine2DM", XError::eAllocation);
			for (int i = 0; i < m_nNumPoints; i++)
				endian->Read(in, true, &m_M[i], 8);
		}
		return in->good();
	}
	return false;
}
//-----------------------------------------------------------------------------
// Lecture des points 3D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPoint3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	endian->Read(in, true, &m_Frame.Xmin, 8);
	endian->Read(in, true, &m_Frame.Ymin, 8);
	m_Frame.Xmax = m_Frame.Xmin;
	m_Frame.Ymax = m_Frame.Ymin;
	m_Pos = in->tellg();
	m_ZRange = new double[2];
	if (m_ZRange == NULL)
		return XErrorError(error, "XShapefileRecord::ReadPoint3D", XError::eAllocation);
	endian->Read(in, true, &m_ZRange[0], 8);
	m_MRange = new double[2];
	if (m_MRange == NULL)
		return XErrorError(error, "XShapefileRecord::ReadPoint3D", XError::eAllocation);
	endian->Read(in, true, &m_MRange[0], 8);

	m_ZRange[1] = m_ZRange[0];
	m_MRange[1] = m_MRange[0];
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des multi-points 3D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadMultiPoint3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	ReadFrame(in, endian, error);
	endian->Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	if (!ReadXYGeom(in, endian, error))
		return false;
	if (!ReadZGeom(in, endian, error))
		return false;

	if (in->tellg() < pos + header->Length())
		if (!ReadMGeom(in, endian, error))
			return false;

	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des polylignes et des polygones 3D
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadPolyLine3D(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	ReadFrame(in, endian, error);

	endian->Read(in, true, &m_nNumParts, 4);
	endian->Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();

	if (!ReadParts(in, endian, error))
		return false;
	if (!ReadXYGeom(in, endian, error))
		return false;
	if (!ReadZGeom(in, endian, error))
		return false;

	if (in->tellg() < pos + header->Length())
		if (!ReadMGeom(in, endian, error))
			return false;

	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture des multi-patch
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadMultiPatch(std::ifstream* in, XEndian* endian, XShapefileRecordHeader* header, XError* error)
{
	int pos = in->tellg();
	ReadFrame(in, endian, error);

	m_Pos = in->tellg();
	endian->Read(in, true, &m_nNumParts, 4);

	m_PartTypes = new ePartType[m_nNumParts];
	if (m_PartTypes == NULL) {
		delete[] m_Parts; m_Parts = NULL;
		return XErrorError(error, "ReadMultiPatch::ReadPolyLine3D (m_PartTypes)", XError::eAllocation);
	}

	endian->Read(in, true, &m_nNumPoints, 4);

	if (!ReadParts(in, endian, error))
		return false;
	for (uint32_t i = 0; i < m_nNumParts; i++)
		endian->Read(in, true, &m_PartTypes[i], 4);

	m_Pos = in->tellg();
	if (!ReadXYGeom(in, endian, error))
		return false;
	if (!ReadZGeom(in, endian, error))
		return false;

	if (in->tellg() < pos + header->Length())
		if (!ReadMGeom(in, endian, error))
			return false;

	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture du rectangle englobant
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadFrame(std::ifstream* in, XEndian* endian, XError* error)
{
	endian->Read(in, true, &m_Frame.Xmin, 8);
	endian->Read(in, true, &m_Frame.Ymin, 8);
	endian->Read(in, true, &m_Frame.Xmax, 8);
	endian->Read(in, true, &m_Frame.Ymax, 8);
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie en XY
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadXYGeom(std::ifstream* in, XEndian* endian, XError* error)
{
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadXYGeom (m_Pt)", XError::eAllocation);
		}

	/*
	for (uint32_t i = 0; i < m_nNumPoints; i++) {
		endian->Read(in, true, &m_Pt[i].X, 8);
		endian->Read(in, true, &m_Pt[i].Y, 8);
	}*/
	endian->ReadArray(in, true, m_Pt, 16, m_nNumPoints);
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie en Z
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadZGeom(std::ifstream* in, XEndian* endian, XError* error)
{
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadZGeom (m_ZRange)", XError::eAllocation);
	}
	endian->Read(in, true, &m_ZRange[0], 8);
	endian->Read(in, true, &m_ZRange[1], 8);
	if (m_nNumPoints < 1)
		return true;

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadZGeom (m_Z)", XError::eAllocation);
	}
	/*
	for (uint32_t i = 0; i < m_nNumPoints; i++)
		endian->Read(in, true, &m_Z[i], 8);
	*/
	endian->ReadArray(in, true, m_Z, 8, m_nNumPoints);
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie en M
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadMGeom(std::ifstream* in, XEndian* endian, XError* error)
{
	m_MRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadMGeom (m_MRange)", XError::eAllocation);
	}
	endian->Read(in, true, &m_MRange[0], 8);
	endian->Read(in, true, &m_MRange[1], 8);

	m_M = new double[m_nNumPoints];
	if (m_M == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadMGeom (m_M)", XError::eAllocation);
	}

	for (uint32_t i = 0; i < m_nNumPoints; i++)
		endian->Read(in, true, &m_M[i], 8);
	return in->good();
}

//-----------------------------------------------------------------------------
//  Lecture du nombre de parties de la geometrie
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadParts(std::ifstream* in, XEndian* endian, XError* error)
{
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return XErrorError(error, "XShapefileRecord::ReadPolyLine3D (m_Parts)", XError::eAllocation);
	}
	/*
	for (int i = 0; i < m_nNumParts; i++)
		endian->Read(in, true, &m_Parts[i], 4);
	*/
	endian->ReadArray(in, true, m_Parts, 4, m_nNumParts);
	return in->good();
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie
//-----------------------------------------------------------------------------
bool XShapefileRecord::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;
	switch(m_eShapeType) {
		case shpNullShape : 
		case shpPoint : 
		case shpPointM :
			return true;

		case shpPointZ :
			in->seekg(m_Pos);
			if (!ReadZGeom(in, &m_Endian, NULL))
				return false;
			m_ZRange[1] = m_ZRange[0];
			return true;

		case shpPolyLine :
		case shpPolygone :
			in->seekg(m_Pos);
			if (!ReadParts(in, &m_Endian, NULL))
				return false;
			if (!ReadXYGeom(in, &m_Endian, NULL))
				return false;
			return true;

		case shpPolyLineZ :
		case shpPolygonZ :
		case shpPolyLineM : 
		case shpPolygonM : 
			in->seekg(m_Pos);
			if (!ReadParts(in, &m_Endian, NULL))
				return false;
			if (!ReadXYGeom(in, &m_Endian, NULL))
				return false;
			if (!ReadZGeom(in, &m_Endian, NULL))
				return false;
			return true;
		case shpMultiPointZ :
			in->seekg(m_Pos);
			if (!ReadXYGeom(in, &m_Endian, NULL))
				return false;
			if (!ReadZGeom(in, &m_Endian, NULL))
				return false;
			return true;

		case shpMultiPoint :
		case shpMultiPointM : 
			return true;
		case shpMultiPatch :
			return true;

			break;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Ecriture
//-----------------------------------------------------------------------------
bool XShapefileRecord::Write(std::ofstream* out, XError* error)
{
	XEndian endian;
	endian.Write(out, true, &m_eShapeType, 4);

	switch(m_eShapeType) {
		case shpNullShape : 
			return out->good();
		case shpPoint : 
			return WritePoint2D(out, &endian, error);
		case shpPolyLine :
			return WritePolyLine2D(out, &endian, error);
		case shpPolygone :
			return WritePolyLine2D(out, &endian, error);
		case shpMultiPoint :
			return WriteMultiPoint2D(out, &endian, error);
		case shpPointZ :
			return WritePoint3D(out, &endian, error);
		case shpPolyLineZ :
			return WritePolyLine3D(out, &endian, error);
		case shpPolygonZ :
			return WritePolyLine3D(out, &endian, error);
		case shpMultiPointZ :
			return WriteMultiPoint3D(out, &endian, error);
		case shpPointM :
			return WritePoint2DM(out, &endian, error);
		case shpPolyLineM : 
			return WritePolyLine2DM(out, &endian, error);
		case shpPolygonM : 
			return WritePolyLine2DM(out, &endian, error);
		case shpMultiPointM : 
			return WriteMultiPoint2DM(out, &endian, error);
		case shpMultiPatch :
			return WriteMultiPatch(out, &endian, error);
			break;
		default:
			return XErrorAlert(error,"XShapefileRecord::Read", XError::eBadFormat);
	}
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des points 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePoint2D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des multi-points 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WriteMultiPoint2D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_Frame.Xmax, 8);
	endian->Write(out, true, &m_Frame.Ymax, 8);
	endian->Write(out, true, &m_nNumPoints, 4);
	for (int i = 0; i < m_nNumPoints; i++) {
		endian->Write(out, true, &m_Pt[i].X, 8);
		endian->Write(out, true, &m_Pt[i].Y, 8);
	}
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des polylignes et des polygones 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePolyLine2D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_Frame.Xmax, 8);
	endian->Write(out, true, &m_Frame.Ymax, 8);

	endian->Write(out, true, &m_nNumParts, 4);
	endian->Write(out, true, &m_nNumPoints, 4);

  int i;
  for (i = 0; i < m_nNumParts; i++)
		endian->Write(out, true, &m_Parts[i], 4);

	for (i = 0; i < m_nNumPoints; i++) {
		endian->Write(out, true, &m_Pt[i].X, 8);
		endian->Write(out, true, &m_Pt[i].Y, 8);
	}
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des points 2D avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePoint2DM(std::ofstream* out, XEndian* endian, XError* error)
{
	if (WritePoint2D(out, endian, error)) {
		endian->Write(out, true, &m_MRange[0], 8);
		return out->good();
	}
	return false;
}

//-----------------------------------------------------------------------------
// Ecriture des des polylignes et des polygones 2D avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::WriteMultiPoint2DM(std::ofstream* out, XEndian* endian, XError* error)
{
	if (WriteMultiPoint2D(out, endian, error)) {
		if (m_M != NULL) {
			endian->Write(out, true, &m_MRange[0], 8);
			endian->Write(out, true, &m_MRange[1], 8);
			for (int i = 0; i < m_nNumPoints; i++)
				endian->Write(out, true, &m_M[i], 8);
			}
		return out->good();
	}
	return false;
}

//-----------------------------------------------------------------------------
// Ecriture des multi-points avec mesure
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePolyLine2DM(std::ofstream* out, XEndian* endian, XError* error)
{
	if (WritePolyLine2D(out, endian, error)) {
		if (m_M != NULL) {
			endian->Write(out, true, &m_MRange[0], 8);
			endian->Write(out, true, &m_MRange[1], 8);
			for (int i = 0; i < m_nNumPoints; i++)
				endian->Write(out, true, &m_M[i], 8);
		}
		return out->good();
	}
	return false;
}

//-----------------------------------------------------------------------------
// Ecriture des points 3D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePoint3D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_ZRange[0], 8);
	if (m_MRange != NULL)
		endian->Write(out, true, &m_MRange[0], 8);
	else {
		double no_data = XGEO_NO_DATA;
		endian->Write(out, true, &no_data, 8);
	}
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des multi-points 2D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WriteMultiPoint3D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_Frame.Xmax, 8);
	endian->Write(out, true, &m_Frame.Ymax, 8);
	endian->Write(out, true, &m_nNumPoints, 4);

  int i;
  for (i = 0; i < m_nNumPoints; i++) {
		endian->Write(out, true, &m_Pt[i].X, 8);
		endian->Write(out, true, &m_Pt[i].Y, 8);
	}
	endian->Write(out, true, &m_ZRange[0], 8);
	endian->Write(out, true, &m_ZRange[1], 8);
	for (i = 0; i < m_nNumPoints; i++)
		endian->Write(out, true, &m_Z[i], 8);

	if (m_M != NULL) {
		endian->Write(out, true, &m_MRange[0], 8);
		endian->Write(out, true, &m_MRange[1], 8);
		for (i = 0; i < m_nNumPoints; i++)
			endian->Write(out, true, &m_M[i], 8);
	}

	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des polylignes et des polygones 3D
//-----------------------------------------------------------------------------
bool XShapefileRecord::WritePolyLine3D(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_Frame.Xmax, 8);
	endian->Write(out, true, &m_Frame.Ymax, 8);

	endian->Write(out, true, &m_nNumParts, 4);
	endian->Write(out, true, &m_nNumPoints, 4);

  int i;
  for (i = 0; i < m_nNumParts; i++)
		endian->Write(out, true, &m_Parts[i], 4);

	for (i = 0; i < m_nNumPoints; i++) {
		endian->Write(out, true, &m_Pt[i].X, 8);
		endian->Write(out, true, &m_Pt[i].Y, 8);
	}

	endian->Write(out, true, &m_ZRange[0], 8);
	endian->Write(out, true, &m_ZRange[1], 8);
	for (i = 0; i < m_nNumPoints; i++) 
		endian->Write(out, true, &m_Z[i], 8);

	if (m_M != NULL) {
		endian->Write(out, true, &m_MRange[0], 8);
		endian->Write(out, true, &m_MRange[1], 8);
		for (i = 0; i < m_nNumPoints; i++)
			endian->Write(out, true, &m_M[i], 8);
	}

	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture des multi-patch
//-----------------------------------------------------------------------------
bool XShapefileRecord::WriteMultiPatch(std::ofstream* out, XEndian* endian, XError* error)
{
	endian->Write(out, true, &m_Frame.Xmin, 8);
	endian->Write(out, true, &m_Frame.Ymin, 8);
	endian->Write(out, true, &m_Frame.Xmax, 8);
	endian->Write(out, true, &m_Frame.Ymax, 8);

	endian->Write(out, true, &m_nNumParts, 4);
	endian->Write(out, true, &m_nNumPoints, 4);

  int i;
  for (i = 0; i < m_nNumParts; i++)
		endian->Write(out, true, &m_Parts[i], 4);
	for (i = 0; i < m_nNumParts; i++)
		endian->Write(out, true, &m_PartTypes[i], 4);

	for (i = 0; i < m_nNumPoints; i++) {
		endian->Write(out, true, &m_Pt[i].X, 8);
		endian->Write(out, true, &m_Pt[i].Y, 8);
	}

	endian->Write(out, true, &m_ZRange[0], 8);
	endian->Write(out, true, &m_ZRange[1], 8);
	for (i = 0; i < m_nNumPoints; i++) 
		endian->Write(out, true, &m_Z[i], 8);

	if (m_M != NULL) {
		endian->Write(out, true, &m_MRange[0], 8);
		endian->Write(out, true, &m_MRange[1], 8);
		for (i = 0; i < m_nNumPoints; i++)
			endian->Write(out, true, &m_M[i], 8);
	}

	return out->good();
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XShapefileRecord::ReadAttributes(std::vector<std::string>& V)
{
	if (m_File == NULL)
		return false;
	return m_File->ReadAttributes(m_nIndex, V);
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeodConverter* L)
{
	double x, y, z;
	XFrame F;

	F.Xmin = F.Ymin = 1e99;
	F.Xmax = F.Ymax = -1e99;

	if (m_Pt != NULL) {
		for (uint32_t i = 0; i < m_nNumPoints; i++) {
			if (m_Z != NULL) z = m_Z[i]; else z = 0.;
			if (z <= XGEO_NO_DATA) z = 0;
			L->ConvertDeg(m_Pt[i].X, m_Pt[i].Y, x, y, z);
			m_Pt[i].X = x; m_Pt[i].Y = y;
			F.Xmin = XMin(F.Xmin, x);
			F.Xmax = XMax(F.Xmax, x);
			F.Ymin = XMin(F.Ymin, y);
			F.Ymax = XMax(F.Ymax, y);
		}
	} else { // Cas des ponctuels
		if (m_ZRange != NULL) z = m_ZRange[0]; else z = 0.;
		if (z <= XGEO_NO_DATA) z = 0;
		L->ConvertDeg(m_Frame.Xmin, m_Frame.Ymin, x, y, z);
		F.Xmin = F.Xmax = x;
		F.Ymin = F.Ymax = y;
	}
	m_Frame = F;
	return true;
}

//-----------------------------------------------------------------------------
// Test si la geometrie est proche d'un point
//-----------------------------------------------------------------------------
bool XShapefileRecord::IsNear2D(XPt2D P, double dist)
{
	if (!Selectable())
		return false;
	if (!m_Class->Selectable())
		return false;
	if (!m_Class->Layer()->Selectable())
		return false;
	if (P.X < m_Frame.Xmin - dist)
		return false;
	if (P.X > m_Frame.Xmax + dist)
		return false;
	if (P.Y < m_Frame.Ymin - dist)
		return false;
	if (P.Y > m_Frame.Ymax + dist)
		return false;

	double d2 = dist * dist;
	double d, am, ab, prod;
	uint32_t i, j, k;
	double alpha;
	int nb_inter = 0;
	XPt A, B, C;
	LoadGeom();
	switch(m_eShapeType) {
		case shpNullShape : 
			break;
		case shpPoint : 
		case shpPointZ :
		case shpPointM :
			d = (m_Frame.Xmin - P.X) * (m_Frame.Xmin - P.X) + (m_Frame.Ymin - P.Y) * (m_Frame.Ymin - P.Y);
			Unload();
			if (d < d2)
				return true;
			return false;

		case shpMultiPoint :
		case shpMultiPointZ :
		case shpMultiPointM : 
			for (i = 0; i < m_nNumPoints; i++) {
				d = m_Pt[i].X * m_Pt[i].X + m_Pt[i].Y * m_Pt[i].Y;
				if (d < d2) {
					Unload();
					return true;
				}
			}
			Unload();
			return false;	

		case shpPolyLine :
		case shpPolyLineM : 
		case shpPolyLineZ :
			for (i = 0; i < m_nNumParts; i++) {
				if (i == m_nNumParts - 1)
					k = m_nNumPoints;
				else
					k = m_Parts[i + 1];
				for (j = m_Parts[i]; j < k - 1; j++) {
					prod = (m_Pt[j+1].X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (m_Pt[j+1].Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
					am = (P.X - m_Pt[j].X)*(P.X - m_Pt[j].X) + (P.Y - m_Pt[j].Y)*(P.Y - m_Pt[j].Y);
					ab = (m_Pt[j+1].X - m_Pt[j].X)*(m_Pt[j+1].X - m_Pt[j].X) + 
						(m_Pt[j+1].Y - m_Pt[j].Y)*(m_Pt[j+1].Y - m_Pt[j].Y);
					if (ab == 0)
						continue;
					d = am - (prod * prod) / ab;
					if (d < d2){
						Unload();
						return true;
					}
				}
			}
			Unload();
			return false;

		case shpPolygone :
		case shpPolygonZ :
		case shpPolygonM : 
			for (i = 0; i < m_nNumParts; i++) {
				if (i == m_nNumParts - 1)
					k = m_nNumPoints;
				else
					k = m_Parts[i + 1];
				for (j = m_Parts[i]; j < k - 1; j++) {
					A = m_Pt[j];
					B = m_Pt[j + 1];
					if ((A.X == P.X)&&(A.Y == P.Y)){
						nb_inter = 0;
						break;
					}
					alpha = ((A.X - A.Y) -  (P.X - P.Y)) / ((A.X - A.Y) -  (B.X - B.Y));
					if ((alpha <= 0.0)||(alpha > 1.0))
						continue;
					C.X = A.X + alpha * (B.X - A.X) - P.X;
					C.Y = A.Y + alpha * (B.Y - A.Y) - P.Y;
					if ((C.X > 0.0)&&(C.Y > 0.0))
						nb_inter++;
					}

				if ((nb_inter % 2) != 0) {	// Nombre pair d'intersection
						Unload();
						return true;
					}
				}
			Unload();
			return false;

		case shpMultiPatch :
			break;
		default:
			break ;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Renvoi le contour de l'objet
//-----------------------------------------------------------------------------
/*
bool XShapefileRecord::GetPolyContour(XPolyContour* P)
{
	switch(m_eShapeType) {
		case shpNullShape : 
		case shpPoint : 
		case shpPointZ :
		case shpPointM :
		case shpMultiPoint :
		case shpMultiPointZ :
		case shpMultiPointM : 
		case shpPolyLine :
		case shpPolyLineM : 
		case shpPolyLineZ :
		case shpMultiPatch :
			return false;
	}
	if (!LoadGeom())
		return false;
	P->Clear();
	XPolygone2D T;
	uint32_t i, j, k;
	for (i = 0; i < m_nNumParts; i++) {
		T.Empty();
		if (i == m_nNumParts - 1)
			k = m_nNumPoints;
		else
			k = m_Parts[i + 1];
		for (j = m_Parts[i]; j < k - 1; j++)
			T.AddPt(m_Pt[j].X, m_Pt[j].Y);
		P->XorPolygon(&T);
	}
	Unload();
	return true;
}
*/

//-----------------------------------------------------------------------------
// Ecriture au format HTML
//-----------------------------------------------------------------------------
bool XShapefileRecord::WriteHtml(std::ostream* out)
{
	*out << "<HTML> <BODY>";
	out->setf(std::ios::fixed);
	out->precision(2);
	*out << "<I>Theme</I> : " << Class()->Layer()->Name() << "<BR>";
	*out << "<I>Classe</I> : <B>" << Class()->Name() << "</B><BR><BR>";
	
	if (LoadGeom()) {
		*out << "<I>Emprise</I> : (" << m_Frame.Xmin << " ; " << m_Frame.Ymin << ") - (" <<
						m_Frame.Xmax << " ; " << m_Frame.Ymax << ")<BR>";
		switch(m_eShapeType) {
		case shpNullShape : *out << "<I>Type</I> : <B>NullShape</B><BR>"; break;
		case shpPoint : *out << "<I>Type</I> : <B>Point</B><BR>"; break;
		case shpPointZ : *out << "<I>Type</I> : <B>Point Z</B><BR>"; break;
		case shpPointM : *out << "<I>Type</I> : <B>Point M</B><BR>"; break;			
		case shpMultiPoint : *out << "<I>Type</I> : <B>MultiPoint</B><BR>"; break;
		case shpMultiPointZ : *out << "<I>Type</I> : <B>MultiPoint Z</B><BR>"; break;
		case shpMultiPointM : *out << "<I>Type</I> : <B>MultiPoint M</B><BR>"; break;
		case shpPolyLine : *out << "<I>Type</I> : <B>PolyLine</B><BR>"; break;
		case shpPolyLineM : *out << "<I>Type</I> : <B>PolyLine M</B><BR>"; break;
		case shpPolyLineZ : *out << "<I>Type</I> : <B>PolyLine Z</B><BR>"; break;
		case shpPolygone : *out << "<I>Type</I> : <B>Polygone</B><BR>"; break;
		case shpPolygonZ : *out << "<I>Type</I> : <B>Polygone Z</B><BR>"; break;
		case shpPolygonM : *out << "<I>Type</I> : <B>Polygone M</B><BR>"; break;
		case shpMultiPatch : *out << "<I>Type</I> : <B>MultiPatch</B><BR>"; break;
		default:
			*out << "<I><B>Type inconnu</B></I>";
		}
		if (m_ZRange != NULL)
			*out << "<I>Emprise alti</I> : (" << m_ZRange[0] << " ; " << m_ZRange[1] << ")<BR>";
		if (m_Pt != NULL) {
			*out << "<I>Nombre de points</I> : " << m_nNumPoints << "<BR>";
			*out << "<I>Nombre de parties</I> : " << m_nNumParts << "<BR><BR>";
			for (uint32_t i = 0; i < m_nNumPoints; i++) {
				*out << m_Pt[i].X << " ; " << m_Pt[i].Y;
				if (m_Z != NULL)
					*out << " ; " << m_Z[i] << "<BR>";
				else
					*out << "<BR>";
			}
		}
		
		Unload();
		*out << "<hr style=\"width: 100%; height: 2px;\">";
		std::vector<std::string> V;
		if (ReadAttributes(V)) {
			for(uint32_t i = 0; i < V.size() / 2; i++)
				*out << "<I>" << V[2 * i] << "</I> : " << V[2*i + 1] << "<BR>";
		}

	} else 
		*out << "Impossible de charger l'objet !";

	*out << "</BODY> </HTML>";

	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoVector en ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoVector* V)
{
	switch(V->TypeVector()) {
		case XGeoVector::Point :
			return Convert((XGeoPoint2D*)V);
		case XGeoVector::PointZ :
			return Convert((XGeoPoint3D*)V);

		case XGeoVector::MPoint :
			return Convert((XGeoMPoint2D*)V);
		case XGeoVector::MPointZ :
			return Convert((XGeoMPoint3D*)V);

		case XGeoVector::Line :
			return Convert((XGeoLine2D*)V);
		case XGeoVector::LineZ :
			return Convert((XGeoLine3D*)V);
		case XGeoVector::MLine :
			return Convert((XGeoMLine2D*)V);
		case XGeoVector::MLineZ :
			return Convert((XGeoMLine3D*)V);

		case XGeoVector::Poly :
			return Convert((XGeoPoly2D*)V);
		case XGeoVector::PolyZ :
			return Convert((XGeoPoly3D*)V);
		case XGeoVector::MPoly :
			return Convert((XGeoMPoly2D*)V);
		case XGeoVector::MPolyZ : 
			return Convert((XGeoMPoly3D*)V);

    default : return ConvertVector(V);
  }
	return false;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoPoint2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoPoint2D* point)
{
	m_eShapeType = shpPoint;
	m_Frame= point->Frame();
	m_Pos = 0;						
	m_File = NULL;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoPoint3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoPoint3D* point)
{
	m_eShapeType = shpPointZ;
	m_Frame= point->Frame();
	m_ZRange = new double[2];
	m_ZRange[0] = point->Zmin();
	m_ZRange[1] = point->Zmax();
	m_Pos = 0;						
	m_File = NULL;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMPoint2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMPoint2D* point)
{
	m_eShapeType = shpPointM;
	m_Frame = point->Frame();
	m_nNumPoints = point->NbPt();
	m_nNumParts = point->NbPart();
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
	}
	memcpy(m_Pt, point->Pt(), m_nNumPoints * sizeof(XPt));
	m_Pos = 0;						
	m_File = NULL;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMPoint3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMPoint3D* point)
{
	if (!Convert((XGeoMPoint2D*)point))
		return false;

	m_eShapeType = shpMultiPointZ;
	m_Z = new double[m_nNumPoints];
	m_ZRange = new double[2];
	if ((m_Z == NULL) || (m_ZRange == NULL)){
		Unload();
		return false;
	}
	m_ZRange[0] = point->Zmin();
	m_ZRange[1] = point->Zmax();
	memcpy(m_Z, point->Z(), m_nNumPoints * sizeof(double));
	m_Pos = 0;						
	m_File = NULL;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoLine2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoLine2D* line)
{
	m_eShapeType = shpPolyLine;
	m_Frame = line->Frame();
	m_nNumPoints = line->NbPt();
	m_nNumParts = line->NbPart();
	m_Pt = new XPt[m_nNumPoints];
	m_Parts = new int[m_nNumParts];
	if ((m_Pt == NULL) || (m_Parts == NULL)) {
		Unload();
		return false;
	}
	memcpy(m_Pt, line->Pt(), m_nNumPoints * sizeof(XPt));
	if (m_nNumParts > 1)
		memcpy(m_Parts, line->Parts(), m_nNumParts * sizeof(int));
	else
		m_Parts[0] = 0;
	m_Pos = 0;						
	m_File = NULL;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoLine3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoLine3D* line)
{
	if (!Convert((XGeoLine2D*)line))
		return false;
	m_eShapeType = shpPolyLineZ;
	m_Z = new double[m_nNumPoints];
	m_ZRange = new double[2];
	if ((m_Z == NULL) || (m_ZRange == NULL)){
		Unload();
		return false;
	}
	m_ZRange[0] = line->Zmin();
	m_ZRange[1] = line->Zmax();
	memcpy(m_Z, line->Z(), m_nNumPoints * sizeof(double));

	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMLine2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMLine2D* line)
{
	return Convert((XGeoLine2D*)line);
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMLine3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMLine3D* line)
{
	if (!Convert((XGeoLine2D*)line))
		return false;
	m_eShapeType = shpPolyLineZ;
	m_Z = new double[m_nNumPoints];
	m_ZRange = new double[2];
	if ((m_Z == NULL) || (m_ZRange == NULL)){
		Unload();
		return false;
	}
	m_ZRange[0] = line->Zmin();
	m_ZRange[1] = line->Zmax();
	memcpy(m_Z, line->Z(), m_nNumPoints * sizeof(double));

	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoPoly2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoPoly2D* poly)
{
	if (!Convert((XGeoLine2D*)poly))
		return false;
	m_eShapeType = shpPolygone;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoPoly3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoPoly3D* poly)
{
	if (!Convert((XGeoLine3D*)poly))
		return false;
	m_eShapeType = shpPolygonZ;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMPoly2D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMPoly2D* poly)
{
	return Convert((XGeoPoly2D*)poly);
}

//-----------------------------------------------------------------------------
// Conversion d'un XGeoMPoly3D vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::Convert(XGeoMPoly3D* poly)
{
	if (!Convert((XGeoMLine3D*)poly))
		return false;
	m_eShapeType = shpPolygonZ;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion par defaut vers un ShapefileRecord
//-----------------------------------------------------------------------------
bool XShapefileRecord::ConvertVector(XGeoVector *vector)
{
	m_eShapeType = shpPolygone;
  m_Frame = vector->Frame();
	m_nNumPoints = 5;
	m_nNumParts = 1;
	m_Pt = new XPt[m_nNumPoints];
	m_Parts = new int[m_nNumParts];
	if ((m_Pt == NULL) || (m_Parts == NULL)) {
		Unload();
		return false;
	}
	m_Pt[0].X = m_Frame.Xmin;
	m_Pt[0].Y = m_Frame.Ymax;
	m_Pt[1].X = m_Frame.Xmax;
	m_Pt[1].Y = m_Frame.Ymax;
	m_Pt[2].X = m_Frame.Xmax;
	m_Pt[2].Y = m_Frame.Ymin;
	m_Pt[3].X = m_Frame.Xmin;
	m_Pt[3].Y = m_Frame.Ymin;
	m_Pt[4].X = m_Frame.Xmin;
	m_Pt[4].Y = m_Frame.Ymax;
	
	m_Parts[0] = 0;
	m_Pos = 0;						
	m_File = NULL;
	return true;
}
