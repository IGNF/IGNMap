//-----------------------------------------------------------------------------
//								XShapefile.cpp
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/03/2003
//-----------------------------------------------------------------------------

#include "XShapefile.h"
#include "../XTool/XEndian.h"
#include "../XTool/XPath.h"
#include "../XToolGeod/XGeodConverter.h"
#include "XDBase.h"
#include <sstream>
//#include "XPolyContour.h"
#include "XShapefileVector.h"
#include "../XTool/XGeoBase.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XShapefileHeader::XShapefileHeader()
{
	m_nFileCode = 9994;	// Byte 0 File Code 9994 Integer Big
	m_nUnused1 = 0;			// Byte 4 Unused 0 Integer Big
	m_nUnused2 = 0;			// Byte 8 Unused 0 Integer Big
	m_nUnused3 = 0;			// Byte 12 Unused 0 Integer Big
	m_nUnused4 = 0;			// Byte 16 Unused 0 Integer Big
	m_nUnused5 = 0;			// Byte 20 Unused 0 Integer Big
	m_nFileLength = 0;	// Byte 24 File Length File Length Integer Big
	m_nVersion = 1000;	// Byte 28 Version 1000 Integer Little
	m_eShapeType = shpNullShape;		// Byte 32 Shape Type Shape Type Integer Little
	m_dXmin = 0.;		// Byte 36 Bounding Box Xmin Double Little
	m_dYmin = 0.;		// Byte 44 Bounding Box Ymin Double Little
	m_dXmax = 0.;		// Byte 52 Bounding Box Xmax Double Little
	m_dYmax = 0.;		// Byte 60 Bounding Box Ymax Double Little
	m_dZmin = 0.;		// Byte 68* Bounding Box Zmin Double Little
	m_dZmax = 0.;		// Byte 76* Bounding Box Zmax Double Little
	m_dMmin = 0.;		// Byte 84* Bounding Box Mmin Double Little
	m_dMmax = 0.;		// Byte 92* Bounding Box Mmax Double Little
}


//-----------------------------------------------------------------------------
// Lecture dans un fichier
//-----------------------------------------------------------------------------
bool XShapefileHeader::Read(std::ifstream* in, XError* error)
{
	XEndian endian;

	endian.Read(in, false, &m_nFileCode, 4);
	if (m_nFileCode != 9994)
		return false;
	endian.Read(in, false, &m_nUnused1, 4);
	endian.Read(in, false, &m_nUnused2, 4);
	endian.Read(in, false, &m_nUnused3, 4);
	endian.Read(in, false, &m_nUnused4, 4);
	endian.Read(in, false, &m_nUnused5, 4);
	endian.Read(in, false, &m_nFileLength, 4);

	endian.Read(in, true, &m_nVersion, 4);
	if (m_nVersion != 1000)
		return false;
	endian.Read(in, true, &m_eShapeType, 4);
	endian.Read(in, true, &m_dXmin, 8);
	endian.Read(in, true, &m_dYmin, 8);
	endian.Read(in, true, &m_dXmax, 8);
	endian.Read(in, true, &m_dYmax, 8);
	endian.Read(in, true, &m_dZmin, 8);
	endian.Read(in, true, &m_dZmax, 8);
	endian.Read(in, true, &m_dMmin, 8);
	endian.Read(in, true, &m_dMmax, 8);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier
//-----------------------------------------------------------------------------
bool XShapefileHeader::Write(std::ofstream* out, XError* error)
{
	XEndian endian;

	endian.Write(out, false, &m_nFileCode, 4);
	endian.Write(out, false, &m_nUnused1, 4);
	endian.Write(out, false, &m_nUnused2, 4);
	endian.Write(out, false, &m_nUnused3, 4);
	endian.Write(out, false, &m_nUnused4, 4);
	endian.Write(out, false, &m_nUnused5, 4);
	endian.Write(out, false, &m_nFileLength, 4);

	endian.Write(out, true, &m_nVersion, 4);
	endian.Write(out, true, &m_eShapeType, 4);
	endian.Write(out, true, &m_dXmin, 8);
	endian.Write(out, true, &m_dYmin, 8);
	endian.Write(out, true, &m_dXmax, 8);
	endian.Write(out, true, &m_dYmax, 8);
	endian.Write(out, true, &m_dZmin, 8);
	endian.Write(out, true, &m_dZmax, 8);
	endian.Write(out, true, &m_dMmin, 8);
	endian.Write(out, true, &m_dMmax, 8);

	return out->good();
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XShapefile::~XShapefile()
{
	if (m_DBFile != NULL)
		delete m_DBFile;
}

//-----------------------------------------------------------------------------
// Creation d'un ShapefileRecord
//-----------------------------------------------------------------------------
XShapefileRecord* XShapefile::CreateShapefileRecord()
{ 
	return new XShapefileRecord;
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier Shapefile
//-----------------------------------------------------------------------------
bool XShapefile::Read(const char* filename, XError* error)
{
	XPath P;
	m_strFilename = filename;
	m_strName = P.Name(filename);
  m_In.Open(filename, std::ios_base::in| std::ios_base::binary);
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return XErrorError(error, "XShapefile::Read", XError::eIOOpen);
  if (!in->good())
		return XErrorError(error, "XShapefile::Read", XError::eIOOpen);

  if (!m_Header.Read(in, error))
		return XErrorError(error, "XShapefile::Read (m_Header)", XError::eIORead);
	m_Frame = XFrame(m_Header.m_dXmin, m_Header.m_dYmin, m_Header.m_dXmax, m_Header.m_dYmax);

	// Lecture des enregistrements
	/*
	while( (!m_In.eof()) && (m_In.good()) ) {
		XShapefileRecordHeader header;
		if (header.Read(&m_In, error)) {
			XShapefileRecord* record = CreateShapefileRecord();
			record->Read(&m_In, &header, error);
			record->Shapefile(this);
			m_Data.push_back(record);
			record->Unload();
		}
		else {
			XErrorError(error, "XShapefile::Read", XError::eBadFormat);
			break;
		}
		int pos = m_In.tellg();
		if (pos >= (m_Header.m_nFileLength * 2))
			return m_In.good();
	}
	*/
	XShapefilePoint2D* point2D;
	XShapefilePoint3D* point3D;
	XShapefileMPoint2D* pointM2D;
	XShapefileMPoint3D* pointM3D;
	XShapefileLine2D* line2D;
	XShapefileLine3D* line3D;
	XShapefilePoly2D* poly2D;
	XShapefilePoly3D* poly3D;
	XShapefileMLine2D* lineM2D;
	XShapefileMLine3D* lineM3D;
	XShapefileMPoly2D* polyM2D;
	XShapefileMPoly3D* polyM3D;
  uint32_t index = 0, pos;
  XShapefileRecordHeader header;
  while( (!in->eof()) && (in->good()) ) {
    if (header.Read(in, error)) {
			switch(ShapeType()) {
			case shpPoint : 
      case shpPointM :
        point2D = new XShapefilePoint2D;
        if (point2D->Read(in, &header, error)) {
          point2D->Shapefile(this, index);
          m_Data.push_back(point2D);
        } else delete point2D;
				break;
			case shpPointZ : 
				point3D = new XShapefilePoint3D;
        if (point3D->Read(in, &header, error)) {
          point3D->Shapefile(this, index);
          m_Data.push_back(point3D);
        } else delete point3D;
				break;
			case shpMultiPoint : 
      case shpMultiPointM :
        pointM2D = new XShapefileMPoint2D;
        if (pointM2D->Read(in, &header, error)) {
          pointM2D->Shapefile(this, index);
          m_Data.push_back(pointM2D);
        } else delete pointM2D;
				break;
			case shpMultiPointZ : 
				pointM3D = new XShapefileMPoint3D;
        if (pointM3D->Read(in, &header, error)) {
          pointM3D->Shapefile(this, index);
          m_Data.push_back(pointM3D);
        } else delete pointM3D;
				break;
			case shpPolyLine : 
      case shpPolyLineM :
        lineM2D = new XShapefileMLine2D;
        if (!lineM2D->Read(in, &header, error)) {
          delete lineM2D;
          break;
        }
				lineM2D->Shapefile(this, index);
				if (lineM2D->NbPart() == 1) {
					line2D = new XShapefileLine2D;
					if (line2D->Clone(lineM2D))
						m_Data.push_back(line2D);
					delete lineM2D;
				} else
					m_Data.push_back(lineM2D);
				break;
			case shpPolyLineZ : 
				lineM3D = new XShapefileMLine3D;
        if (!lineM3D->Read(in, &header, error)) {
          delete lineM3D;
          break;
        }
				lineM3D->Shapefile(this, index);
				if (lineM3D->NbPart() == 1) {
					line3D = new XShapefileLine3D;
					if (line3D->Clone(lineM3D))
						m_Data.push_back(line3D);
					delete lineM3D;
				} else
					m_Data.push_back(lineM3D);
				break;
      case shpPolygone :
      case shpPolygonM :
        polyM2D = new XShapefileMPoly2D;
        if (!polyM2D->Read(in, &header, error)) {
          delete polyM2D;
          break;
        }
				polyM2D->Shapefile(this, index);
				if (polyM2D->NbPart() == 1) {
					poly2D = new XShapefilePoly2D;
					if (poly2D->Clone(polyM2D))
						m_Data.push_back(poly2D);
					delete polyM2D;
				} else
					m_Data.push_back(polyM2D);
				break;
      case shpPolygonZ :
        polyM3D = new XShapefileMPoly3D;
        if (!polyM3D->Read(in, &header, error)) {
          delete polyM3D;
          break;
        }
				polyM3D->Shapefile(this, index);
				if (polyM3D->NbPart() == 1) {
					poly3D = new XShapefilePoly3D;
					if (poly3D->Clone(polyM3D))
						m_Data.push_back(poly3D);
					delete polyM3D;
				} else
					m_Data.push_back(polyM3D);
				break;
				default: return false;
			} // end switch
			index++;
		}
		else {
			XErrorError(error, "XShapefile::Read", XError::eBadFormat);
			break;
		}
    pos = (uint32_t)in->tellg() / 2L;
    if (pos >= m_Header.m_nFileLength)
      return in->good();
      //return true;
	}
  return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un fichier Shapefile
//-----------------------------------------------------------------------------
bool XShapefile::Write(const char* filename, XError* error)
{
	if (m_Data.size() < 1)
		return true;
	std::ofstream shp, shx;
	std::string file_shp = (std::string)filename + ".shp";
	std::string file_shx = (std::string)filename + ".shx";

	shp.open(file_shp.c_str(), std::ios_base::out| std::ios_base::binary);
	if (!shp.good())
		return XErrorError(error, "XShapefile::Write", XError::eIOOpen);
	shx.open(file_shx.c_str(), std::ios_base::out| std::ios_base::binary);
	if (!shx.good())
		return XErrorError(error, "XShapefile::Write", XError::eIOOpen);

	uint32_t length = 100;
	XFrame F;
	double zmin = -XGEO_NO_DATA, zmax = XGEO_NO_DATA;
	XShapefileRecord* record = (XShapefileRecord*)m_Data[0];
	eShapefileType shapeType = record->ShapefileType();
  uint32_t i;
  for (i = 0; i < m_Data.size(); i++){
		record = (XShapefileRecord*)m_Data[i];
		length += (record->ByteSize() + 8);
    if (record->ShapefileType() == shpNullShape)
      continue;
    F += record->Frame();
    if (shapeType != record->ShapefileType())
      return XErrorError(error, "XShapefile::Write", XError::eBadData);
		if (record->ZMin() > XGEO_NO_DATA)
			zmin = XMin(zmin, record->ZMin());
		if (record->ZMax() > XGEO_NO_DATA)
			zmax = XMax(zmax, record->ZMax());
	}
	if (zmin >= -XGEO_NO_DATA)
		zmin = XGEO_NO_DATA;

	m_Header.m_eShapeType = shapeType;
	m_Header.m_nFileLength = length / 2;
	m_Header.m_dXmin = F.Xmin;
	m_Header.m_dXmax = F.Xmax;
	m_Header.m_dYmin = F.Ymin;
	m_Header.m_dYmax = F.Ymax;
	m_Header.m_dZmin = zmin;
	m_Header.m_dZmax = zmax;
	m_Header.m_dMmin = XGEO_NO_DATA;
	m_Header.m_dMmax = XGEO_NO_DATA;

	if (!m_Header.Write(&shp, error))
		return XErrorError(error, "XShapefile::Write (m_Header)", XError::eIOWrite);

	m_Header.m_nFileLength = (int)(50 + m_Data.size() * 4);
	if (!m_Header.Write(&shx, error))
		return XErrorError(error, "XShapefile::Write (m_Header)", XError::eIOWrite);

	// Ecriture des enregistrements
	XShapefileRecordHeader header;
	for (i = 0; i < m_Data.size(); i++) {
		record = (XShapefileRecord*)m_Data[i];
		header = XShapefileRecordHeader (i + 1, record->ByteSize() / 2);
		uint32_t pos = (uint32_t)shp.tellp();
		header.Write(&shp, error);
		record->Write(&shp, error);
		header = XShapefileRecordHeader (pos / 2, record->ByteSize() / 2);
		header.Write(&shx, error);
	}
	return shp.good();
}

//-----------------------------------------------------------------------------
// Conversion geodesique d'un fichier
//-----------------------------------------------------------------------------
bool XShapefile::Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error)
{
	XPath P;
	m_strFilename = file_in;
	m_strName = P.Name(file_in);
  m_In.Open(file_in, std::ios_base::in| std::ios_base::binary);
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return XErrorError(error, "XShapefile::Read", XError::eIOOpen);
  if (!in->good())
		return XErrorError(error, "XShapefile::Read", XError::eIOOpen);

  if (!m_Header.Read(in, error))
		return XErrorError(error, "XShapefile::Read (m_Header)", XError::eIORead);
	m_Frame = XFrame(m_Header.m_dXmin, m_Header.m_dYmin, m_Header.m_dXmax, m_Header.m_dYmax);

	// Lecture des enregistrements
  while( (!in->eof()) && (in->good()) ) {
		XShapefileRecordHeader header;
    if (header.Read(in, error)) {
			XShapefileRecord* record = CreateShapefileRecord();
      record->Read(in, &header, error);
			record->Shapefile(this);
			m_Data.push_back(record);
		}
		else {
			XErrorError(error, "XShapefile::Read", XError::eBadFormat);
			break;
		}
    int pos = (int)in->tellg();
    if (pos >= (m_Header.m_nFileLength * 2))
			break;
	}

	// Conversion
	for (uint32_t i = 0; i < m_Data.size(); i++) {
		XShapefileRecord* record = (XShapefileRecord*)m_Data[i];
		record->Convert(L);
	}

	// Ecriture
	return Write(P.PathName(file_out).c_str(), error);
}

//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XShapefile::Class(XGeoClass* C)
{
  uint32_t i;
  for (i = 0; i < m_Data.size(); i++){
		XGeoVector* record = (XGeoVector*)m_Data[i];
		record->Class(C);
		C->Vector(record);
	}
	// Fixe le schema de la classe a partir du DBF
  if (!OpenDBF())
    return;
	std::string name;
	uint16_t length;
	uint8_t dec;
	XDBaseFieldDescriptor::eType dbType;
	XGeoAttribut::eType attType;
	XGeoSchema schema;
	for (i = 0; i < m_DBFile->NbField(); i++) {
		if (!m_DBFile->GetFieldDesc(i, name, dbType, length, dec))
			continue;
		switch(dbType) {
			case XDBaseFieldDescriptor::Short : attType = XGeoAttribut::Int16; break;
			case XDBaseFieldDescriptor::Long : attType = XGeoAttribut::Int32; break;
			case XDBaseFieldDescriptor::Double : attType = XGeoAttribut::Double; break;
			case XDBaseFieldDescriptor::NumericN : attType = XGeoAttribut::NumericN; break;
			case XDBaseFieldDescriptor::NumericF : attType = XGeoAttribut::NumericF; break;
			default: attType = XGeoAttribut::String;
		}
		schema.AddAttribut(name, name, "", attType, length, dec);
	}
	C->SetSchema(schema);
}
	
//-----------------------------------------------------------------------------
// Renvoi la classe d'objets
//-----------------------------------------------------------------------------
XGeoClass* XShapefile::Class()
{
	if (m_Data.size() < 1)
		return NULL;
	XGeoVector* record = (XGeoVector*)m_Data[0];
	return record->Class();
}

//-----------------------------------------------------------------------------
// Renvoi les attributs d'un record
//-----------------------------------------------------------------------------
bool XShapefile::ReadAttributes(uint32_t index, std::vector<std::string>& V)
{
	V.clear();
	if (!OpenDBF()) 
		return false;
	return m_DBFile->ReadRecord(index, V);
}

//-----------------------------------------------------------------------------
// Recherche de l'existence d'un champ dans le DBF
//-----------------------------------------------------------------------------
int XShapefile::FindField(std::string fieldname, bool no_case)
{ 
	if (!OpenDBF()) 
		return false;
	return m_DBFile->FindField(fieldname, no_case);
}

//-----------------------------------------------------------------------------
// Ouverture du fichier DBF
//-----------------------------------------------------------------------------
bool XShapefile::OpenDBF()
{
	if (m_DBFile != NULL)
		return true;
	m_DBFile = new XDBaseFile;
	if (m_DBFile == NULL)
		return false;
	std::string dbfilename = m_strFilename;
	dbfilename = dbfilename.substr(0, dbfilename.rfind('.'));
	dbfilename += ".dbf";
	if (!m_DBFile->ReadHeader(dbfilename.c_str())) {
		delete m_DBFile;
		m_DBFile = NULL;
		return false;
	}
  // Presence d'un fichier CPG d'encodage
  std::string cpgfilename = m_strFilename;
  cpgfilename = cpgfilename.substr(0, cpgfilename.rfind('.'));
  cpgfilename += ".cpg";
  std::ifstream in;
  in.open(cpgfilename.c_str(), std::ios_base::in);
  if (in.good()) {
    std::string code;
    in >> code;
    if ((code == "UTF-8")||(code == "utf-8"))
      m_bUTF8 = true;
  }
	return true;
}

//-----------------------------------------------------------------------------
// Import d'un Shapefile dans une XGeoBase
//-----------------------------------------------------------------------------
XGeoClass* XShapefile::ImportShapefile(XGeoBase* base, const char* path, XGeoMap* map)
{
  XPath P;
  XShapefile* file = new XShapefile;
  XGeoClass* C = NULL;

  if (file->Read(path)) {
    C = base->AddClass(P.Name(P.Path(path).c_str()).c_str(), P.Name(path, false).c_str());
    file->Class(C);
    if (map == NULL)
      base->AddMap(file);
    else
      map->AddObject(file);
    base->SortClass();
  }
  else
    delete file;

  return C;
}
