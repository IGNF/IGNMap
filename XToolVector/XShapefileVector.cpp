//-----------------------------------------------------------------------------
//								XShapefileVector.cpp
//								====================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/09/2003
//-----------------------------------------------------------------------------

#include "XShapefileVector.h"
#include "../XTool/XEndian.h"

XEndian gEndian;


//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XShapefileVector::ReadDBFAttributes(std::vector<std::string>& V)
{
	if (m_File == NULL)
		return false;
	return m_File->ReadAttributes(m_nIndex, V);
}

//-----------------------------------------------------------------------------
// Recherche d'un attribut
//-----------------------------------------------------------------------------
std::string XShapefileVector::FindDBFAttribute(std::string att_name, bool no_case)
{
	if (m_File == NULL)
		return "";
	int pos = m_File->FindField(att_name, no_case);
	if (pos < 0)
		return "";
	std::vector<std::string> V;
	if (!m_File->ReadAttributes(m_nIndex, V))
		return "";
	return V[2*pos + 1];
}

//-----------------------------------------------------------------------------
// Point 2D
//-----------------------------------------------------------------------------
bool XShapefilePoint2D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;

	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;
	gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	m_Frame.Xmax = m_Frame.Xmin;
	m_Frame.Ymax = m_Frame.Ymin;
	return in->good();
}
	
bool XShapefilePoint2D::Write(std::ofstream* out, XError* error)
{
	return false;
}

//-----------------------------------------------------------------------------
// Point 3D
//-----------------------------------------------------------------------------
bool XShapefilePoint3D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();

	gEndian.Read(in, true, &shapeType, 4);	
  if (shapeType == 0)
    return false;
  gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	m_Frame.Xmax = m_Frame.Xmin;
	m_Frame.Ymax = m_Frame.Ymin;
	gEndian.Read(in, true, &m_Z, 8);

	in->seekg(pos + header->Length() * 2L);
		
	return in->good();
}
	
bool XShapefilePoint3D::Write(std::ofstream* out, XError* error)
{
	return false;
}

//-----------------------------------------------------------------------------
// Multi-point 2D
//-----------------------------------------------------------------------------
bool XShapefileMPoint2D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();

	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;
  gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);
	gEndian.Read(in, true, &m_nNumPoints, 4);
	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);
	return in->good();
}
	
bool XShapefileMPoint2D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMPoint2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return true;
}

//-----------------------------------------------------------------------------
// Multi-point 3D
//-----------------------------------------------------------------------------
bool XShapefileMPoint3D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();

	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;
  gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);
	gEndian.Read(in, true, &m_nNumPoints, 4);
	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);
	return in->good();
}
	
bool XShapefileMPoint3D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMPoint3D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	// Lecture des Z
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	gEndian.Read(in, true, &m_ZRange[0], 8);
	gEndian.Read(in, true, &m_ZRange[1], 8);

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Z, 8, m_nNumPoints);

	return in->good();
}

bool XShapefileMPoint3D::LoadGeom2D()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ligne 2D
//-----------------------------------------------------------------------------
bool XShapefileLine2D::Clone(XShapefileMLine2D* P)
{
	if (P == NULL)
		return false;
	if (P->NbPart() != 1)
		return false;
	m_File = P->Shapefile();
	m_nIndex = P->Index();
	m_Pos = P->PosGeom();
	m_nNumPoints = P->NbPt();
	m_Frame = P->Frame();
	return true;
}

bool XShapefileLine2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}

//-----------------------------------------------------------------------------
// Multi-Ligne 2D
//-----------------------------------------------------------------------------
bool XShapefileMLine2D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();
	
	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;

	gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);

	gEndian.Read(in, true, &m_nNumParts, 4);
	gEndian.Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);
	return in->good();
}
	
bool XShapefileMLine2D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMLine2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return true;
}

//-----------------------------------------------------------------------------
// Ligne 3D
//-----------------------------------------------------------------------------
bool XShapefileLine3D::Clone(XShapefileMLine3D* P)
{
	if (P == NULL)
		return false;
	if (P->NbPart() != 1)
		return false;
	m_File = P->Shapefile();
	m_nIndex = P->Index();
	m_Pos = P->PosGeom();
	m_nNumPoints = P->NbPt();
	m_Frame = P->Frame();
	return true;
}

bool XShapefileLine3D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	// Lecture des Z
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	gEndian.Read(in, true, &m_ZRange[0], 8);
	gEndian.Read(in, true, &m_ZRange[1], 8);

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Z, 8, m_nNumPoints);

	return in->good();
}

bool XShapefileLine3D::LoadGeom2D()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}

//-----------------------------------------------------------------------------
// Multi-Ligne 3D
//-----------------------------------------------------------------------------
bool XShapefileMLine3D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();
	
	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;

	gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);

	gEndian.Read(in, true, &m_nNumParts, 4);
	gEndian.Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);

	return in->good();
}
	
bool XShapefileMLine3D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMLine3D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	// Lecture des Z
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	gEndian.Read(in, true, &m_ZRange[0], 8);
	gEndian.Read(in, true, &m_ZRange[1], 8);

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Z, 8, m_nNumPoints);

	return in->good();
}

bool XShapefileMLine3D::LoadGeom2D()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}

//-----------------------------------------------------------------------------
// Polygone 2D
//-----------------------------------------------------------------------------
bool XShapefilePoly2D::Clone(XShapefileMPoly2D* P)
{
	if (P == NULL)
		return false;
	if (P->NbPart() != 1)
		return false;
	m_File = P->Shapefile();
	m_nIndex = P->Index();
	m_Pos = P->PosGeom();
	m_nNumPoints = P->NbPt();
	m_Frame = P->Frame();
	return true;
}

bool XShapefilePoly2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return true;
}

//-----------------------------------------------------------------------------
// Multi-Polygone 2D
//-----------------------------------------------------------------------------
bool XShapefileMPoly2D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();
	
	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;

	gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);

	gEndian.Read(in, true, &m_nNumParts, 4);
	gEndian.Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);
	return in->good();
}
	
bool XShapefileMPoly2D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMPoly2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return true;
}

//-----------------------------------------------------------------------------
// Polygone 3D
//-----------------------------------------------------------------------------
bool XShapefilePoly3D::Clone(XShapefileMPoly3D* P)
{
	if (P == NULL)
		return false;
	if (P->NbPart() != 1)
		return false;
	m_File = P->Shapefile();
	m_nIndex = P->Index();
	m_Pos = P->PosGeom();
	m_nNumPoints = P->NbPt();
	m_Frame = P->Frame();
	return true;
}

bool XShapefilePoly3D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	// Lecture des Z
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	gEndian.Read(in, true, &m_ZRange[0], 8);
	gEndian.Read(in, true, &m_ZRange[1], 8);

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Z, 8, m_nNumPoints);

	return in->good();
}

bool XShapefilePoly3D::LoadGeom2D()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	int parts;
	gEndian.ReadArray(in, true, &parts, 4, 1);
	if (parts != 0)
		return false;

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}

//-----------------------------------------------------------------------------
// Multi-Polygone 3D
//-----------------------------------------------------------------------------
bool XShapefileMPoly3D::Read(std::ifstream* in, XShapefileRecordHeader* header, XError* error)
{
	int shapeType;
	int pos = in->tellg();
	
	gEndian.Read(in, true, &shapeType, 4);
  if (shapeType == 0)
    return false;

	gEndian.Read(in, true, &m_Frame.Xmin, 8);
	gEndian.Read(in, true, &m_Frame.Ymin, 8);
	gEndian.Read(in, true, &m_Frame.Xmax, 8);
	gEndian.Read(in, true, &m_Frame.Ymax, 8);

	gEndian.Read(in, true, &m_nNumParts, 4);
	gEndian.Read(in, true, &m_nNumPoints, 4);

	m_Pos = in->tellg();
	in->seekg(pos + header->Length() * 2L);

	return in->good();
}
	
bool XShapefileMPoly3D::Write(std::ofstream* out, XError* error)
{
	return false;
}

bool XShapefileMPoly3D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	// Lecture des Z
	m_ZRange = new double[2];
	if (m_ZRange == NULL) {
		Unload();
		return false;
	}
	gEndian.Read(in, true, &m_ZRange[0], 8);
	gEndian.Read(in, true, &m_ZRange[1], 8);

	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Z, 8, m_nNumPoints);

	return in->good();
}

bool XShapefileMPoly3D::LoadGeom2D()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);

	// Lecture des parties
	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}
	gEndian.ReadArray(in, true, m_Parts, 4, m_nNumParts);

	// Lecture des points
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}
	gEndian.ReadArray(in, true, m_Pt, 16, m_nNumPoints);

	return in->good();
}
