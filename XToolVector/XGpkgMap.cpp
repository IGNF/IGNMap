//-----------------------------------------------------------------------------
//								XGpkgMap.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 14/12/2020
//-----------------------------------------------------------------------------

#include "XGpkgMap.h"
#include "../XTool/XPath.h"
#include "../XTool/XGeoBase.h"
#include "XWKBGeom.h"
#include <sstream>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGpkgMap::XGpkgMap()
{
  m_DB = NULL;
  m_Class = NULL;
  m_bUTF8 = true;
  m_idxClass = 0;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGpkgMap::~XGpkgMap()
{
  if (m_DB != NULL)
    sqlite3_close(m_DB);
  m_DB = NULL;
  sqlite3_shutdown();
}

//-----------------------------------------------------------------------------
// Import d'un fichier GeoPackage dans une GeoBase
//-----------------------------------------------------------------------------
bool XGpkgMap::ImportGpkg(XGeoBase* base, const char* path, XGeoMap* map)
{
  XGpkgMap* file = new XGpkgMap;
  if (!file->OpenFile(path)) {
    delete file;
    return false;
  }
  if (file->Read(base)) {
    if (map == NULL)
      base->AddMap(file);
    else
      map->AddObject(file);
  } else {
    delete file;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier GeoPackage
//-----------------------------------------------------------------------------
bool XGpkgMap::OpenFile(const char* filename)
{
  // Ouverture de la base
  sqlite3_initialize();
  int rc = sqlite3_open_v2(filename, &m_DB, SQLITE_OPEN_READWRITE, NULL);
  if ( rc != SQLITE_OK) {
    sqlite3_close(m_DB);
    return false;
  }
  m_strFilename = filename;

  m_Table.clear();
  if (!ReadGpkgContents())
    return false;
  if (!ReadGpkgGeomCol())
    return false;
  if (!ReadPrimaryKey())
    return false;

  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la table gpkg_contents : liste des tables geometriques
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadGpkgContents()
{
  if (m_DB == NULL)
    return false;
  // Creation d'un statement
  sqlite3_stmt *stmt = NULL;
  std::string statement = "SELECT table_name, data_type FROM gpkg_contents ;";
  const char* tail;
  sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);

  GpkgTable T;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    T.m_strName = (char*)sqlite3_column_text(stmt, 0);
    T.m_strType = (char*)sqlite3_column_text(stmt, 1);
    m_Table.push_back(T);
  }

  sqlite3_finalize(stmt);
  stmt = NULL;

  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la table gpkg_geometry_columns : nom de la colonne geometrie
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadGpkgGeomCol()
{
  if (m_DB == NULL)
    return false;
  for(uint32_t i = 0; i < m_Table.size(); i++) {
    if (m_Table[i].m_strType != "features")
      continue;

    sqlite3_stmt *stmt = NULL;
    std::string statement = "SELECT column_name, geometry_type_name, z, m ";
    statement += "FROM gpkg_geometry_columns WHERE table_name = '";
    statement += m_Table[i].m_strName;
    statement += "' ;";
    const char* tail;
    int flag;
    sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      m_Table[i].m_strGeomName = (char*)sqlite3_column_text(stmt, 0);
      m_Table[i].m_strGeomType = (char*)sqlite3_column_text(stmt, 1);
      flag = sqlite3_column_int(stmt, 2);
      m_Table[i].m_bZ = true;
      if (flag == 0)
        m_Table[i].m_bZ = false;
      flag = sqlite3_column_int(stmt, 3);
      m_Table[i].m_bM = true;
      if (flag == 0)
        m_Table[i].m_bM = false;
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la clef primaire
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadPrimaryKey()
{
  if (m_DB == NULL)
    return false;
  for(uint32_t i = 0; i < m_Table.size(); i++) {
    sqlite3_stmt *stmt = NULL;
    std::string statement = "SELECT name FROM pragma_table_info('";
    statement += m_Table[i].m_strName;
    statement += "') WHERE pk=1;";
    const char* tail;
    sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      m_Table[i].m_strPrimaryKey = (char*)sqlite3_column_text(stmt, 0);
    }
    sqlite3_finalize(stmt);
    stmt = NULL;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la base
//-----------------------------------------------------------------------------
bool XGpkgMap::Read(XGeoBase* base, XWait* /*wait*/)
{
  if (m_DB == NULL)
    return false;
  XPath path;
  std::string layername = path.Name(m_strFilename.c_str(), false);

  for(uint32_t i = 0; i < m_Table.size(); i++) {
    if (m_Table[i].m_strType != "features")
      continue;
    XGeoClass* C = base->AddClass(layername.c_str(), m_Table[i].m_strName.c_str());
    if (C == NULL)
      continue;
    ReadVectors(i, C);

  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture des vecteurs d'une classe
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadVectors(uint32_t index, XGeoClass* C)
{
  XWKBGeom::eType typeGeom = XWKBGeom::Null;
  bool type_by_object = false;
  XGeoVector* vector;

  if (m_Table[index].m_strGeomType == "GEOMETRY") type_by_object = true;
  if (m_Table[index].m_strGeomType == "POINT") typeGeom = XWKBGeom::wkbPoint;
  if (m_Table[index].m_strGeomType == "LINESTRING") typeGeom = XWKBGeom::wkbLineString;
  if (m_Table[index].m_strGeomType == "POLYGON") typeGeom = XWKBGeom::wkbMultiPolygon;  // On force le typage
  if (m_Table[index].m_strGeomType == "MULTIPOINT") typeGeom = XWKBGeom::wkbMultiPoint;
  if (m_Table[index].m_strGeomType == "MULTILINESTRING") typeGeom = XWKBGeom::wkbMultiLineString;
  if (m_Table[index].m_strGeomType == "MULTIPOLYGON") typeGeom = XWKBGeom::wkbMultiPolygon;
  bool is2D = true;
  if (m_Table[index].m_bZ)
    is2D = false;

  sqlite3_stmt *stmt = NULL;
  std::string statement = "SELECT ";
  statement += m_Table[index].m_strPrimaryKey;
  statement += ", ";
  statement += m_Table[index].m_strGeomName;
  statement += " FROM ";
  statement += m_Table[index].m_strName;

  const char* tail;
  sqlite3_int64 id;
  uint8_t* geom;
  XFrame F;
  double zmin, zmax;
  sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    id = sqlite3_column_int64(stmt, 0);
    geom = (uint8_t*)sqlite3_column_blob(stmt, 1);
    if (!ReadGeomHeader(geom, &F, &zmin, &zmax))
      continue;
    if (type_by_object) { // Cas ou le type geometrique est defini a l'objet
      uint8_t* geom_wkb = &geom[GetGeomHeaderSize(geom)];
      XWKBGeom wkb(false);
      if (wkb.Read(geom_wkb))
        typeGeom = wkb.Type();
    }
    switch(typeGeom) {
    case XWKBGeom::wkbPoint :
      if (is2D) vector = new XGpkgPoint2D(this, id, F);
      else vector = new XGpkgPoint3D(this, id, F, zmin);
      break;
    case XWKBGeom::wkbMultiPoint :
      if (is2D) vector = new XGpkgMPoint2D(this, id, F);
      else vector = new XGpkgMPoint3D(this, id, F);
      break;
    case XWKBGeom::wkbLineString :
      if (is2D) vector = new XGpkgLine2D(this, id, F);
      else vector = new XGpkgLine3D(this, id, F);
      break;
    case XWKBGeom::wkbPolygon :
      if (is2D) vector = new XGpkgPoly2D(this, id, F);
      else vector = new XGpkgPoly3D(this, id, F);
      break;
    case XWKBGeom::wkbMultiLineString :
      if (is2D) vector = new XGpkgMLine2D(this, id, F);
      else vector = new XGpkgMLine3D(this, id, F);
      break;
    case XWKBGeom::wkbMultiPolygon :
      if (is2D) vector = new XGpkgMPoly2D(this, id, F);
      else vector = new XGpkgMPoly3D(this, id, F);
      break;
    case XWKBGeom::wkbPointZ :
      vector = new XGpkgPoint3D(this, id, F, zmin);
      break;
    case XWKBGeom::wkbMultiPointZ :
      vector = new XGpkgMPoint3D(this, id, F);
      break;
    case XWKBGeom::wkbLineStringZ :
      vector = new XGpkgLine3D(this, id, F);
      break;
    case XWKBGeom::wkbPolygonZ :
      vector = new XGpkgPoly3D(this, id, F);
      break;
    case XWKBGeom::wkbMultiLineStringZ :
      vector = new XGpkgMLine3D(this, id, F);
      break;
    case XWKBGeom::wkbMultiPolygonZ :
      vector = new XGpkgMPoly3D(this, id, F);
      break;
    default:
      continue;
    }

    m_Data.push_back(vector);
    vector->Class(C);
    C->Vector(vector);
    m_Frame += F;
  }
  sqlite3_finalize(stmt);
  stmt = NULL;

  return true;
}

//-----------------------------------------------------------------------------
// Lecture de l'entete d'une geometrie
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadGeomHeader(uint8_t* geom, XFrame* F, double* zmin, double* zmax)
{
  if ((geom[0] != 0x47)&&(geom[1] != 0x50)) // Magic Number
    return false;
  uint8_t version = geom[2];
  GpkgFlags* flags = (GpkgFlags*)&geom[3];

  int* srid = (int*)&geom[4];
  double* envelop = (double*)&geom[8];
  if (flags->envelop > 0) {
    F->Xmin = envelop[0];
    F->Xmax = envelop[1];
    F->Ymin = envelop[2];
    F->Ymax = envelop[3];
  } else { // Pas d'enveloppe, il faut donc lire la geometrie
    XWKBGeom wkb;
    if (wkb.Read(&geom[8]))
      wkb.Frame(F);
  }
  if ((flags->envelop == 2)||(flags->envelop == 4)) {
    *zmin = envelop[4];
    *zmax = envelop[5];
  }

  return true;
}

//-----------------------------------------------------------------------------
// Taille de l'entete d'une geometrie
//-----------------------------------------------------------------------------
int XGpkgMap::GetGeomHeaderSize(uint8_t* geom)
{
  GpkgFlags* flags = (GpkgFlags*)&geom[3];
  if (flags->envelop == 0)
    return 8;
  if (flags->envelop == 1)
    return 40;
  if ((flags->envelop == 2)||(flags->envelop == 3))
    return 56;
  if (flags->envelop == 4)
    return 72;
  return 8;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGpkgMap::ReadAttributes(sqlite3_int64 id, std::vector<std::string>& V, XGeoClass* C)
{
  if (!SetAttClass(C))
    return false;

  sqlite3_stmt *stmt = NULL;
  std::stringstream request;
  request << "SELECT ";
  for (uint32_t i = 0; i < m_Att.size()-1; i++)
    request << m_Att[i] << " , ";
  request << m_Att[m_Att.size()-1] << " ";
  request << " FROM "
          << m_Table[m_idxClass].m_strName << " WHERE " << m_Table[m_idxClass].m_strPrimaryKey
          << " = " << id << " ;";
  std::string statement = request.str();

  const char* tail;
  sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    if (sqlite3_column_count(stmt) != m_Att.size())
      break;
    for (uint32_t i = 0; i < m_Att.size(); i++) {
      V.push_back(m_Att[i]);
      std::stringstream data;
      switch(sqlite3_column_type(stmt, i)) {
      case SQLITE_INTEGER: data << sqlite3_column_int(stmt, i); break;
      case SQLITE_FLOAT: data << sqlite3_column_double(stmt, i); break;
      case SQLITE_BLOB: data << "Blob"; break;
      case SQLITE_NULL: data << "Null"; break;
      case SQLITE_TEXT: data << sqlite3_column_text(stmt, i); break;
      default: data << "*";
      }
      V.push_back(data.str());
    }
  }
  sqlite3_finalize(stmt);
  stmt = NULL;

  return true;}

//-----------------------------------------------------------------------------
// Lecture de la geometrie
//-----------------------------------------------------------------------------
bool XGpkgMap::LoadGeom(sqlite3_int64 id, XGpkgVector* V)
{
  if (!SetClass(V->GeoClass()))
    return false;

  sqlite3_stmt *stmt = NULL;
  std::stringstream request;
  request << "SELECT " << m_Table[m_idxClass].m_strGeomName <<  " FROM "
          << m_Table[m_idxClass].m_strName << " WHERE " << m_Table[m_idxClass].m_strPrimaryKey
          << " = " << id << " ;";
  std::string statement = request.str();

  const char* tail;
  uint8_t *geom, *blob;
  bool flag = false;
  XFrame F;
  double zmin, zmax;
  sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    blob = (uint8_t*)sqlite3_column_blob(stmt, 0);
    if (!ReadGeomHeader(blob, &F, &zmin, &zmax))
      continue;
    geom = &blob[GetGeomHeaderSize(blob)];
    XWKBGeom wkb(false);
    if (wkb.Read(geom)) {
      V->SetGeom(wkb.NbPt(), wkb.Pt(), wkb.Z(), wkb.NbPart(), (int*)wkb.Parts(), zmin, zmax);
      flag = true;
      break;
    }
  }
  sqlite3_finalize(stmt);
  stmt = NULL;

  return flag;
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie 2D
//-----------------------------------------------------------------------------
bool XGpkgMap::LoadGeom2D(sqlite3_int64 id, XGpkgVector* V)
{
  return LoadGeom(id, V);
}

//-----------------------------------------------------------------------------
// Fixe une classe par defaut
//-----------------------------------------------------------------------------
bool XGpkgMap::SetClass(XGeoClass* C)
{
  if (C == NULL)
    return false;
  if (m_Class == C)
    return true;
  for (uint32_t i = 0; i < m_Table.size(); i++) {
    if (m_Table[i].m_strName == C->Name()) {
      m_idxClass = i;
       m_Class = C;
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// Fixe les attributs de la classe par defaut
//-----------------------------------------------------------------------------
bool XGpkgMap::SetAttClass(XGeoClass* C)
{
  if (!SetClass(C))
    return false;
  m_Att.clear();
  sqlite3_stmt *stmt = NULL;
  std::string statement = "PRAGMA table_info('";
  statement += m_Table[m_idxClass].m_strName;
  statement += "') ;";
  const char* tail;
  std::string data;
  sqlite3_prepare_v2(m_DB, statement.c_str() , static_cast<int>(statement.size()), &stmt, &tail);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    data = (char*)sqlite3_column_text(stmt, 1);
    if (data == m_Table[m_idxClass].m_strPrimaryKey)
      continue;
    if (data == m_Table[m_idxClass].m_strGeomName)
      continue;
    m_Att.push_back(data);
  }
  sqlite3_finalize(stmt);
  stmt = NULL;
  return true;
}
