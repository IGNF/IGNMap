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
#include <cstring>
#include "../XTool/XGeoBase.h"
#include "../XTool/XPath.h"
#include "../XToolGeod/XWebMercator.h"
#include "../XToolAlgo/XJson.h"

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
      std::vector<std::string> types;
      XJson::ReadArrayAsString(geomtypes, types);
    }
  }

  // Lecture des noms des colonnes
  std::vector<int> indices;
  const carquet_schema_t* schema = carquet_reader_schema(m_Reader);
  for (int32_t c = 0; c < m_nNumCols; c++) {
    std::string col_name = carquet_schema_column_name(schema, c);
    if (col_name == m_strGeomCol) { 
      m_nGeomCol = c; continue; 
    }
    if (col_name == "xmin") {
      m_nXminCol = c; continue; 
    }
    if (col_name == "ymin") {
      m_nYminCol = c; continue;
    }
    if (col_name == "xmax") {
      m_nXmaxCol = c; continue;
    }
    if (col_name == "ymax") {
      m_nYmaxCol = c; continue;
    }
    m_Attribut.push_back(col_name);
    indices.push_back(c);
  }

  // Configuraion pour la lecture d'attributs
  if (m_Config.column_indices != nullptr)
    delete[] m_Config.column_indices;
  int32_t* T_indices = new (std::nothrow) int32_t[indices.size()];
  if (T_indices == nullptr)
    return false;
  for (size_t i = 0; i < indices.size(); i++)
    T_indices[i] = indices[i];

  m_Config.column_indices = T_indices;
  m_Config.num_columns = (int32_t)indices.size();

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
        XGeoParquetVector* vector = new XGeoParquetVector(this, id, F);
        vector->XGeoObject::Is3D(geom.Is3D());
        vector->XGeoObject::IsClosed(geom.IsClosed());

        if (vector != nullptr)
          m_Data.push_back(vector);
      }
    
  }
  ClearGeomReader();

  return true;
}

bool XGeoParquetMap::FilterIndex(const carquet_reader_t* reader, int32_t row_group_index, void* user_data)
{
  int32_t* ptr = (int32_t*)user_data;
  if (*ptr == row_group_index)
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeoParquetMap::ReadAttributes(int64_t offset, std::vector<std::string>& V)
{
  carquet_error_t err = CARQUET_ERROR_INIT;

  int64_t batch_size = m_nTotalRows / m_nNumRgs;
  int32_t row_group = offset / batch_size;
  int64_t index = offset - row_group * batch_size;
  int32_t att_batch_size = 100;

  if ((row_group == m_nBatchAttRowGroup) && (index < m_nBatchAttIndex) && (index >= (m_nBatchAttIndex - att_batch_size))) {
    int32_t tab_index = m_Config.batch_size - (m_nBatchAttIndex - index);
    for (int32_t i = 0; i < m_Config.num_columns; i++) {
      V.push_back(m_Attribut[i]);
      V.push_back(m_BatchAtt[i * m_Config.batch_size + tab_index]);
    }
    return true;
  }
  m_BatchAtt.clear();
  m_nBatchAttIndex = m_nBatchAttRowGroup = -1;

  m_Config.row_group_filter = FilterIndex;
  m_Config.row_group_filter_ctx = &row_group;
  m_Config.batch_size = att_batch_size;
  carquet_batch_reader_t* br = carquet_batch_reader_create(m_Reader, &m_Config, &err);
  carquet_row_batch_t* batch = nullptr;
  const carquet_schema_t* schema = carquet_reader_schema(m_Reader);

  bool flag = false;
  int64_t num_val = 0;
  while (carquet_batch_reader_next(br, &batch) == CARQUET_OK && batch) {
    num_val += m_Config.batch_size;
    if (index >= num_val) {
      carquet_row_batch_free(batch);
      batch = nullptr;
      continue;
    }
    const void* data; const uint8_t* null_bitmap; int64_t n;
    for (int32_t i = 0; i < m_Config.num_columns; i++) {
      carquet_row_batch_column(batch, m_Config.column_indices[i], &data, &null_bitmap, &n);
      const carquet_byte_array_t* s = (const carquet_byte_array_t *)data;

      for (int64_t k = 0; k < n; k++) {
        bool is_null = null_bitmap && !(null_bitmap[k / 8] & (1 << (k % 8)));
        if (is_null) {
          m_BatchAtt.push_back("");
          continue;
        }

        std::ostringstream out;
        out.precision(6);
        out.setf(std::ios::fixed);
        switch (carquet_schema_column_type(schema, m_Config.column_indices[i])) {
        case 0: // CARQUET_PHYSICAL_BOOLEAN
          out << ((const bool*)data)[k];
          m_BatchAtt.push_back(out.str());
          break;
        case 1: // CARQUET_PHYSICAL_INT32
          out << ((const int32_t*)data)[k];
          m_BatchAtt.push_back(out.str());
          break;
        case 2: // CARQUET_PHYSICAL_INT64
          out << ((const int64_t*)data)[k];
          m_BatchAtt.push_back(out.str());
          break;
        case 3: // CARQUET_PHYSICAL_INT96
          m_BatchAtt.push_back("");
          break;
        case 4: // CARQUET_PHYSICAL_FLOAT
          out << ((const float*)data)[k];
          m_BatchAtt.push_back(out.str());
          break;
        case 5: // CARQUET_PHYSICAL_DOUBLE
          out << ((const double*)data)[k];;
          m_BatchAtt.push_back(out.str());
          break;
        case 6: // CARQUET_PHYSICAL_BYTE_ARRAY
          m_BatchAtt.push_back(std::string((const char*)s[k].data, (size_t)s[k].length));
          break;
        case 7: // CARQUET_PHYSICAL_FIXED_LEN_BYTE_ARRAY
          m_BatchAtt.push_back("");
          break;
        default: m_BatchAtt.push_back("");
        }

      } //endfor k
    } //endfor i
    carquet_row_batch_free(batch);
    batch = nullptr;
    flag = true;
    m_nBatchAttRowGroup = row_group;
    m_nBatchAttIndex = num_val;
    break;
  }
  carquet_batch_reader_free(br);

  int32_t tab_index = m_Config.batch_size - (num_val - index);
  for (int32_t i = 0; i < m_Config.num_columns; i++) {
    V.push_back(m_Attribut[i]);
    V.push_back(m_BatchAtt[i * m_Config.batch_size + tab_index]);
  }

  return flag;
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
    XWKBGeom* wkb = new XWKBGeom(true);
    if (wkb->Read(data)) {
      if (m_ProjCode == XGeoProjection::RGF93) {
        XPt* Pt = wkb->Pt();
        for (uint32_t i = 0; i < wkb->NbPt(); i++) {
          Pt[i].X = XWebMercator::ConvertLonToXDeg(Pt[i].X);
          Pt[i].Y = XWebMercator::ConvertLatToYDeg(Pt[i].Y);
        }
      }

      V->SetGeom(wkb);
    }
    else
      delete wkb;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Conversion type vecteur
//-----------------------------------------------------------------------------
XGeoVector::eTypeVector XGeoParquetVector::TypeVector() const
{ 
  if (m_Geom == nullptr) 
    return Null; 
  switch (m_Geom->Type()) {
  case XWKBGeom::wkbPoint: return Point;
  case XWKBGeom::wkbPointZ: return PointZ;
  case XWKBGeom::wkbMultiPoint: return MPoint;
  case XWKBGeom::wkbMultiPointZ: return MPointZ;
  case XWKBGeom::wkbLineString: return Line;
  case XWKBGeom::wkbLineStringZ: return LineZ;
  case XWKBGeom::wkbMultiLineString: return MLine;
  case XWKBGeom::wkbMultiLineStringZ: return MLineZ;
  case XWKBGeom::wkbPolygon: return Poly;
  case XWKBGeom::wkbPolygonZ: return PolyZ;
  case XWKBGeom::wkbMultiPolygon: return MPoly;
  case XWKBGeom::wkbMultiPolygonZ: return MPolyZ;
  }
  return Null;
}

