//-----------------------------------------------------------------------------
//								XGeoParquet.h
//								=============
//
// Gestion du format GeoParquet
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/11/2026
//-----------------------------------------------------------------------------

#ifndef XGEOPARQUET_H
#define XGEOPARQUET_H

#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoLine.h"
#include "../XTool/XGeoPoint.h"
#include "../XTool/XGeoPoly.h"
#include "../XTool/XFile.h"

#include "../XToolGeod/XGeoProjection.h"
#include "../XToolVector/XWKBGeom.h"

#include <carquet/carquet.h>

class XGeoBase;
class XGeoParquetVector;

class XGeoParquetMap : public XGeoMapMonoClass {
protected:
  std::string m_strFilename;
  XGeoProjection::XProjCode m_ProjCode;
  uint64_t m_FeatureOffset = 0;
  carquet_reader_t* m_Reader = nullptr;
  int64_t m_nTotalRows = 0;
  int32_t m_nNumCols = 0;
  int32_t m_nNumRgs = 0;
  int64_t m_nMaxBatchSize = 0;
  std::vector<int64_t> m_BatchSize;
  std::string m_strGeomCol;
  int32_t m_nGeomCol = -1;
  int32_t m_nXminCol = -1;
  int32_t m_nXmaxCol = -1;
  int32_t m_nYminCol = -1;
  int32_t m_nYmaxCol = -1;
  std::vector<std::string> m_Attribut;
  std::vector<std::string> m_BatchAtt;
  int32_t m_nBatchAttRowGroup = -1;
  int32_t m_nBatchAttIndex = -1;
  int32_t m_nActiveRowGroup = -1;
  int64_t m_nNbGeom = 0;
  carquet_column_reader_t* m_GeomColReader = nullptr;
  carquet_byte_array_t* m_GeometryBuf = nullptr;
  carquet_batch_reader_config_t m_Config;

  static bool FilterIndex(const carquet_reader_t* reader, int32_t row_group_index, void* user_data);
  int32_t FindRowGroup(const int64_t& offset, int64_t& index);

public:
  XGeoParquetMap() { m_ProjCode = XGeoProjection::WebMercator; carquet_batch_reader_config_init(&m_Config);}
  virtual ~XGeoParquetMap();
  void ClearGeomReader();

  bool OpenFile(const char* filename);
  bool Read(XGeoBase* base, XWait* wait = nullptr);

  bool ReadAttributes(const int64_t& offset, std::vector<std::string>& V);
  bool LoadGeoParquetGeom(const int64_t& offset, XGeoParquetVector* V, bool needZ);
  bool LoadGeom(const int64_t& offset, XGeoParquetVector* V) { return LoadGeoParquetGeom(offset, V, true); }
  bool LoadGeom2D(const int64_t& offset, XGeoParquetVector* V) { return LoadGeoParquetGeom(offset, V, false); }

  static XGeoClass* ImportGeoParquet(XGeoBase* base, const char* path, XGeoMap* map = nullptr);
};

//-----------------------------------------------------------------------------
// Vecteur GeoParquet
//-----------------------------------------------------------------------------
class XGeoParquetVector : public XGeoVector {
protected:
  XGeoParquetMap* m_Map = nullptr;
  int64_t   m_Offset;
  XWKBGeom* m_Geom = nullptr;

public:
  XGeoParquetVector(XGeoParquetMap* map, int64_t offset, XFrame& F) { m_Map = map; m_Offset = offset; m_Frame = F; }
  virtual ~XGeoParquetVector() { if (m_Geom != nullptr) delete m_Geom; }
  virtual inline XGeoClass* GeoClass() const { return Class(); }
  virtual void SetGeom(XWKBGeom* geom) { m_Geom = geom; }

  virtual eTypeVector TypeVector() const;
  virtual uint32_t NbPt() const { if (m_Geom != nullptr) return m_Geom->NbPt();  return 0; }
  virtual XPt2D Pt(uint32_t i) { if (m_Geom != nullptr) return m_Geom->Pt(i); return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA); }
  virtual inline XPt* Pt() { if (m_Geom != nullptr) return m_Geom->Pt(); return nullptr; }
  virtual double Z(uint32_t i) { if (m_Geom != nullptr) return m_Geom->Z(i); return XGEO_NO_DATA; }
  virtual inline double* Z() { if (m_Geom != nullptr) return m_Geom->Z(); return nullptr; }
  virtual inline double* ZRange() { return nullptr; }
  virtual uint32_t NbPart() const { if (m_Geom != nullptr) return m_Geom->NbPart(); return 0; }
  virtual uint32_t Part(uint32_t i) { if (m_Geom != nullptr) return m_Geom->Part(i); return 0; }
  virtual inline int* Parts() { if (m_Geom != nullptr) return (int*)m_Geom->Parts(); return nullptr; }

  virtual inline XGeoMap* Map() const { return m_Map; }
  virtual XGeoClass* Class() const { if (m_Map != nullptr) return m_Map->Class(); return nullptr; }

  virtual bool ReadAttributes(std::vector<std::string>& V)
  {
    if (m_Map != nullptr) return m_Map->ReadAttributes(m_Offset, V); return false;
  }
  virtual bool LoadGeom()
  {
    if (m_Map != nullptr) return m_Map->LoadGeom(m_Offset, this); return false;
  }
  virtual bool LoadGeom2D()
  {
    if (m_Map != nullptr) return m_Map->LoadGeom2D(m_Offset, this); return false;
  }
  virtual void Unload()
  {
    if (m_Geom != nullptr)
      delete m_Geom;
    m_Geom = nullptr;
  }
  virtual bool IsLoaded() 
  { 
    if (m_Geom == nullptr)
      return false; 
    return true;
  }
  virtual double Zmin() const 
  { 
    if (m_Geom == nullptr)
      return XGEO_NO_DATA;
    return m_Geom->ZMin();
  }
  virtual inline double Zmax() const
  {
    if (m_Geom == nullptr)
      return XGEO_NO_DATA;
    return m_Geom->ZMax();
  }

};

#endif //XGEOPARQUET_H
