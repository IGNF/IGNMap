//-----------------------------------------------------------------------------
//								XGeoParquet.cpp
//								===============
//
// Gestion du format GeoParquet
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/11/2026
//-----------------------------------------------------------------------------

#include "XGeoParquet.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XPath.h"
#include "../XToolGeod/XWebMercator.h"
#include "../XToolAlgo/XJson.h"
#include "../XToolVector/XWKBGeom.h"

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoParquetMap::~XGeoParquetMap()
{
  if (m_Reader != nullptr) 
    carquet_reader_close(m_Reader);
  if (m_GeometryBuf != nullptr)
    delete[] m_GeometryBuf;
  carquet_column_reader_free(m_GeomColReader);
  m_nActiveRowGroup = -1;
  m_nNbGeom = 0;
}

//-----------------------------------------------------------------------------
// Vide le buffer de lecture des geometries
//-----------------------------------------------------------------------------
void XGeoParquetMap::ClearGeomReader()
{
  if (m_GeometryBuf != nullptr)
    delete[] m_GeometryBuf;
  m_GeometryBuf = nullptr;
  carquet_column_reader_free(m_GeomColReader);

}

//-----------------------------------------------------------------------------
// Import d'un fichier GeoParquet dans une GeoBase
//-----------------------------------------------------------------------------
XGeoClass* XGeoParquetMap::ImportGeoParquet(XGeoBase* base, const char* path, XGeoMap* map)
{
  XGeoParquetMap* file = new XGeoParquetMap;
  if (!file->OpenFile(path)) {
    delete file;
    return nullptr;
  }
  XGeoClass* C = nullptr;
  XPath P;
  if (file->Read(base)) {
    C = base->AddClass(P.Name(P.Path(path).c_str()).c_str(), P.Name(path, false).c_str());
    file->Class(C);
    if (map == nullptr)
      base->AddMap(file);
    else
      map->AddObject(file);
    base->SortClass();
  }
  else
    delete file;

  return C;
}

//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XGeoParquetMap::Class(XGeoClass* C)
{
  uint32_t i;
  for (i = 0; i < m_Data.size(); i++) {
    XGeoVector* record = (XGeoVector*)m_Data[i];
    record->Class(C);
    C->Vector(record);
  }
}

//-----------------------------------------------------------------------------
// Renvoi la classe d'objets
//-----------------------------------------------------------------------------
XGeoClass* XGeoParquetMap::Class()
{
  if (m_Data.size() < 1)
    return NULL;
  XGeoVector* record = (XGeoVector*)m_Data[0];
  return record->Class();
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier
//-----------------------------------------------------------------------------
bool XGeoParquetMap::OpenFile(const char* filename)
{
  carquet_error_t err = CARQUET_ERROR_INIT;
  m_Reader = carquet_reader_open(filename, NULL, &err);
  if (!m_Reader)
    return false;
  m_strFilename = filename;

  m_nTotalRows = carquet_reader_num_rows(m_Reader);
  m_nNumCols = carquet_reader_num_columns(m_Reader);
  m_nNumRgs = carquet_reader_num_row_groups(m_Reader);

  // Metadonnees
  int32_t num_metadata = carquet_reader_num_metadata(m_Reader);
  for (int32_t index = 0; index < num_metadata; index++) {
    const char* key;
    const char* value;
    carquet_reader_get_metadata(m_Reader, index, &key, &value);

    if (strcmp(key, "geo") == 0) {
      std::string source = value;
      std::vector<std::string> find_keys;
      find_keys.push_back("primary_column");
      m_strGeomCol = XJson::FindValue(find_keys, source);
      if (m_strGeomCol.empty())
        return false;

      find_keys.clear();
      find_keys.push_back("columns");
      find_keys.push_back(m_strGeomCol);
      find_keys.push_back("encoding");
      std::string encoding = XJson::FindValue(find_keys, source);
      if (encoding != "WKB")
        return false;

      find_keys.pop_back();
      find_keys.push_back("bbox");
      std::string bbox = XJson::FindValue(find_keys, source);
      std::cout << "Geometry bbox : " << bbox << std::endl;
      std::vector<double> TFrame;
      XJson::ReadArrayAsDouble(bbox, TFrame);
      if (TFrame.size() == 4) {
        m_Frame.Xmin = TFrame[0];
        m_Frame.Ymin = TFrame[1];
        m_Frame.Xmax = TFrame[2];
        m_Frame.Ymax = TFrame[3];
        m_dZmin = m_dZmax = 0.;
      }
      if (TFrame.size() == 6) {
        m_Frame.Xmin = TFrame[0];
        m_Frame.Ymin = TFrame[1];
        m_dZmin = TFrame[2];
        m_Frame.Xmax = TFrame[3];
        m_Frame.Ymax = TFrame[4];
        m_dZmax = TFrame[5];
      }

      XFrame geo(-360., -360., 360., 360.);
      if (m_Frame.IsIn(geo)) {  // Passage en WebMercator si on a des coordonnees geographiques
        m_ProjCode = XGeoProjection::RGF93;
        m_Frame.Xmin = XWebMercator::ConvertLonToXDeg(m_Frame.Xmin);
        m_Frame.Xmax = XWebMercator::ConvertLonToXDeg(m_Frame.Xmax);
        m_Frame.Ymin = XWebMercator::ConvertLatToYDeg(m_Frame.Ymin);
        m_Frame.Ymax = XWebMercator::ConvertLatToYDeg(m_Frame.Ymax);
      }

      find_keys.pop_back();
      find_keys.push_back("geometry_types");
      std::string geomtypes = XJson::FindValue(find_keys, source);
      std::cout << "Geometry types : " << geomtypes << std::endl;
      std::vector<std::string> types;
      XJson::ReadArrayAsString(geomtypes, types);
    }
  }

  // Lecture des noms des colonnes
  const carquet_schema_t* schema = carquet_reader_schema(m_Reader);
  for (int32_t c = 0; c < m_nNumCols; c++) {
    std::string col_name = carquet_schema_column_name(schema, c);
    if (col_name == m_strGeomCol)
      m_nGeomCol = c;
    if (col_name == "xmin")
      m_nXminCol = c;
    if (col_name == "ymin")
      m_nYminCol = c;
    if (col_name == "xmax")
      m_nXmaxCol = c;
    if (col_name == "ymax")
      m_nYmaxCol = c;
    m_Attribut.push_back(col_name);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Lecture du fichier
//-----------------------------------------------------------------------------
bool XGeoParquetMap::Read(XGeoBase* base, XWait* wait)
{
  carquet_error_t err = CARQUET_ERROR_INIT;
 
  int64_t id = -1;
  XFrame F;
  double zmin = 0.;
  uint32_t nbpt = 0;
  uint32_t nbparts = 0;
  int64_t batch_size = m_nTotalRows / m_nNumRgs;
  
  if (m_GeometryBuf == nullptr) {
    m_GeometryBuf = new (std::nothrow) carquet_byte_array_t[batch_size];
    if (m_GeometryBuf == nullptr)
      return false;
  }

  m_GeomColReader = carquet_reader_get_column(m_Reader, 0, m_nGeomCol, &err);
  for (int32_t rg = 0; rg < m_nNumRgs; rg++) {
    if (rg > 0)
      carquet_column_reader_reset(m_GeomColReader, rg, &err);
    m_nActiveRowGroup = rg;

    m_nNbGeom = carquet_column_read_batch(m_GeomColReader, m_GeometryBuf, batch_size, NULL, NULL);

      for (int64_t i = 0; i < m_nNbGeom; i++) {
        id++;
        uint8_t* data = m_GeometryBuf[i].data;
        if (data == nullptr)
          continue;
        XWKBGeom geom(true);
        if (!geom.Read(data))
          continue;
        if (!geom.Frame(&F))
          continue;
        if (m_ProjCode == XGeoProjection::RGF93) {
          F.Xmin = XWebMercator::ConvertLonToXDeg(F.Xmin);
          F.Xmax = XWebMercator::ConvertLonToXDeg(F.Xmax);
          F.Ymin = XWebMercator::ConvertLatToYDeg(F.Ymin);
          F.Ymax = XWebMercator::ConvertLatToYDeg(F.Ymax);
        }
        XGeoVector* vector = nullptr;
        nbpt = geom.NbPt();
        nbparts = geom.NbPart();
        switch (geom.Type()) {
        case XWKBGeom::wkbPoint:
          vector = new XGeoParquetPoint2D(this, id, F);
          break;
        case XWKBGeom::wkbMultiPoint:
          vector = new XGeoParquetMPoint2D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbLineString:
          vector = new XGeoParquetLine2D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbPolygon:
          vector = new XGeoParquetPoly2D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbMultiLineString:
          vector = new XGeoParquetMLine2D(this, id, F, nbpt, nbparts);
          break;
        case XWKBGeom::wkbMultiPolygon:
          vector = new XGeoParquetMPoly2D(this, id, F, nbpt, nbparts);
          break;
        case XWKBGeom::wkbPointZ:
          vector = new XGeoParquetPoint3D(this, id, F, zmin);
          break;
        case XWKBGeom::wkbMultiPointZ:
          vector = new XGeoParquetMPoint3D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbLineStringZ:
          vector = new XGeoParquetLine3D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbPolygonZ:
          vector = new XGeoParquetPoly3D(this, id, F, nbpt);
          break;
        case XWKBGeom::wkbMultiLineStringZ:
          vector = new XGeoParquetMLine3D(this, id, F, nbpt, nbparts);
          break;
        case XWKBGeom::wkbMultiPolygonZ:
          vector = new XGeoParquetMPoly3D(this, id, F, nbpt, nbparts);
          break;
        default:
          continue;
        }
        if (vector != nullptr)
          m_Data.push_back(vector);
      }
    
  }
  ClearGeomReader();

  return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeoParquetMap::ReadAttributes(int64_t offset, std::vector<std::string>& V, XGeoClass* C)
{
  return false;
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie
//-----------------------------------------------------------------------------
bool XGeoParquetMap::LoadGeoParquetGeom(int64_t offset, XGeoParquetVector* V, bool needZ)
{
  carquet_error_t err = CARQUET_ERROR_INIT;

  int64_t batch_size = m_nTotalRows / m_nNumRgs;
  int32_t row_group = offset / batch_size;
  int64_t index = offset - row_group * batch_size;

  if (m_GeometryBuf == nullptr) {
    m_GeometryBuf = new (std::nothrow) carquet_byte_array_t[batch_size];
    if (m_GeometryBuf == nullptr)
      return false;
    m_nActiveRowGroup = row_group;
    m_GeomColReader = carquet_reader_get_column(m_Reader, row_group, m_nGeomCol, &err);
    m_nNbGeom = carquet_column_read_batch(m_GeomColReader, m_GeometryBuf, batch_size, NULL, NULL);
  }

  if (m_nActiveRowGroup != row_group) {
    if (carquet_column_reader_reset(m_GeomColReader, row_group, &err) != CARQUET_OK)
      return false;
    m_nActiveRowGroup = row_group;
    m_nNbGeom = carquet_column_read_batch(m_GeomColReader, m_GeometryBuf, batch_size, NULL, NULL);
  }

  if (index >= m_nNbGeom)
    return false;

  //int64_t skip = carquet_column_skip(m_GeomColReader, index);
  //carquet_byte_array_t geomBuf[1];
  //int64_t nbgeom = carquet_column_read_batch(m_GeomColReader, geomBuf, 1, NULL, NULL);

  double zmin = 0., zmax = 0.;

  uint8_t* data = m_GeometryBuf[index].data;
  if (data != nullptr) {
    XWKBGeom wkb(false);
    if (wkb.Read(data)) {
      if (m_ProjCode == XGeoProjection::RGF93) {
        XPt* Pt = wkb.Pt();
        for (uint32_t i = 0; i < wkb.NbPt(); i++) {
          Pt[i].X = XWebMercator::ConvertLonToXDeg(Pt[i].X);
          Pt[i].Y = XWebMercator::ConvertLatToYDeg(Pt[i].Y);
        }
      }

      V->SetGeom(wkb.NbPt(), wkb.Pt(), wkb.Z(), wkb.NbPart(), (int*)wkb.Parts(), zmin, zmax);
    }
  }

  return true;
}
