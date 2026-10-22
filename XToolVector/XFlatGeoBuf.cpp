//-----------------------------------------------------------------------------
//								XFlatGeoBuf.h
//								=============
//
// Gestion du format Flatgeobuf
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/10/2026
//-----------------------------------------------------------------------------

#include <sstream>
#include "XFlatGeoBuf.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XPath.h"
#include "../XToolGeod/XWebMercator.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XFlatGeoBufMap::XFlatGeoBufMap()
{
  m_ProjCode = XGeoProjection::WebMercator;

}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XFlatGeoBufMap::~XFlatGeoBufMap()
{
	if (m_HeaderData != nullptr)
		delete[] m_HeaderData;
  if (m_FeatureData != nullptr)
    delete[] m_FeatureData;
  m_In.Close();
}

//-----------------------------------------------------------------------------
// Import d'un fichier FlatGeoBuf dans une GeoBase
//-----------------------------------------------------------------------------
XGeoClass* XFlatGeoBufMap::ImportFlatGeoBuf(XGeoBase* base, const char* path, XGeoMap* map)
{
  XFlatGeoBufMap* file = new XFlatGeoBufMap;
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
void XFlatGeoBufMap::Class(XGeoClass* C)
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
XGeoClass* XFlatGeoBufMap::Class()
{
  if (m_Data.size() < 1)
    return NULL;
  XGeoVector* record = (XGeoVector*)m_Data[0];
  return record->Class();
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::OpenFile(const char* filename)
{
  m_In.Open(filename, std::ios_base::in | std::ios_base::binary);
  std::ifstream* infile = m_In.IStream();
  if (infile == nullptr)
    return false;
  infile->seekg(0, std::ios::beg);

  // Lecture du magic number 0x6667620366676201
  char magic[8];
  infile->read(magic, 8);
  if ((magic[0] != 0x66) && (magic[1] != 0x67) && (magic[2] != 0x62) && (magic[3] != 0x03) &&
    (magic[4] != 0x66) && (magic[5] != 0x67) && (magic[6] != 0x62) && (magic[7] != 0x01))
    return false;

  // Lecture de l'entete
  uint32_t length = 0;
  infile->read((char*)&length, sizeof(uint32_t));
  if (m_HeaderData != nullptr) delete[] m_HeaderData;
  m_HeaderData = new(std::nothrow) uint8_t[length];
  if (m_HeaderData == nullptr)
    return false;

  infile->read((char*)m_HeaderData, length);
  auto header = FlatGeobuf::GetHeader(m_HeaderData);

  m_strName = header->name()->str();

  // Lecture du CRS
  int32_t crscode = header->crs()->code();
  if (crscode == 2154) m_ProjCode = XGeoProjection::Lambert93;
  if (crscode == 4171) m_ProjCode = XGeoProjection::RGF93;
  if (crscode == 4326) m_ProjCode = XGeoProjection::RGF93;

  // Lecture du cadre
  auto enveloppe = header->envelope();
  if (enveloppe != nullptr) {
    if (enveloppe->size() == 4) {
      m_Frame.Xmin = (*enveloppe)[0];
      m_Frame.Ymin = (*enveloppe)[1];
      m_Frame.Xmax = (*enveloppe)[2];
      m_Frame.Ymax = (*enveloppe)[3];
    }
  }
  XFrame geo(-360., -360., 360., 360.);
  if (m_Frame.IsIn(geo)) {  // Passage en WebMercator si on a des coordonnees geographiques
    m_ProjCode = XGeoProjection::RGF93;
    m_Frame.Xmin = XWebMercator::ConvertLonToXDeg(m_Frame.Xmin);
    m_Frame.Xmax = XWebMercator::ConvertLonToXDeg(m_Frame.Xmax);
    m_Frame.Ymin = XWebMercator::ConvertLatToYDeg(m_Frame.Ymin);
    m_Frame.Ymax = XWebMercator::ConvertLatToYDeg(m_Frame.Ymax);
  }
  
  // Lecture de l'index
  uint64_t spatialIndexSizeInBytes = 0;
  if (header->index_node_size() > 0) {
    // Calcul de la taille de l'index
    spatialIndexSizeInBytes = FlatGeobuf::PackedRTree::size(header->features_count(), header->index_node_size());
    /*
    char* data_tree = new(std::nothrow) char[spatialIndexSizeInBytes];
    infile.read(data_tree, spatialIndexSizeInBytes);
    FlatGeobuf::PackedRTree T(data_tree, header->features_count(), header->index_node_size());
    */
    infile->seekg(spatialIndexSizeInBytes, std::ios::cur);
  }
  m_FeatureOffset = infile->tellg();
  // Lecture des features

  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la base
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::Read(XGeoBase* base, XWait*)
{
  if (m_HeaderData == nullptr)
    return false;
  auto header = FlatGeobuf::GetHeader(m_HeaderData);
  if (header == nullptr)
    return false;

  // Ouverture du fichier
  std::ifstream* infile = m_In.IStream();
  if (infile == nullptr)
    return false;
  infile->seekg(m_FeatureOffset, std::ios::beg);

  // Lecture des features
  uint32_t length = 0, cache_length = 0;
  char* data_feature = nullptr;
  bool is3D = header->has_z();
  uint64_t offset = 0;
  XFrame F;
  double zmin = 0., zmax = 0.;
  uint32_t nbPoint = 0, nbPart = 0;

  for (uint64_t num_feature = 0; num_feature < header->features_count(); num_feature++) {
    offset = infile->tellg();
    infile->read((char*)&length, sizeof(uint32_t));
    if (length > cache_length) {
      if (data_feature != nullptr)
        delete[] data_feature;
      data_feature = new(std::nothrow) char[length];
      if (data_feature == nullptr)
        return false;
      cache_length = length;
    }

    infile->read(data_feature, length);
    auto feature = FlatGeobuf::GetFeature(data_feature);
    auto geometry = feature->geometry();
    if (is3D)
      AnalyzeGeometry3D(geometry, &F, &zmin, &zmax, &nbPoint, &nbPart);
    else
      AnalyzeGeometry2D(geometry, &F, &nbPoint, &nbPart);

    FlatGeobuf::GeometryType geomType = geometry->type();
    if (geomType == FlatGeobuf::GeometryType::Unknown)
      geomType = header->geometry_type();

    XGeoVector* vector = nullptr;
    switch (geomType) {
      case FlatGeobuf::GeometryType::Unknown :
        break;
      case FlatGeobuf::GeometryType::Point:
        if (is3D) vector = new XFlatGeoBufPoint3D(this, offset, F, zmin);
        else vector = new XFlatGeoBufPoint2D(this, offset, F);
        break;
      case FlatGeobuf::GeometryType::LineString:
        if (is3D) vector = new XFlatGeoBufLine3D(this, offset, F, nbPoint);
        else vector = new XFlatGeoBufLine2D(this, offset, F, nbPoint);
        break;
      case FlatGeobuf::GeometryType::Polygon:
        if (is3D) vector = new XFlatGeoBufPoly3D(this, offset, F, nbPoint);
        else vector = new XFlatGeoBufPoly2D(this, offset, F, nbPoint);
        break;
      case FlatGeobuf::GeometryType::MultiPoint:
        if (is3D) vector = new XFlatGeoBufMPoint3D(this, offset, F, nbPoint);
        else vector = new XFlatGeoBufMPoint2D(this, offset, F, nbPoint);
        break;
      case FlatGeobuf::GeometryType::MultiLineString:
        if (is3D) vector = new XFlatGeoBufMLine3D(this, offset, F, nbPoint, nbPart);
        else vector = new XFlatGeoBufMLine2D(this, offset, F, nbPoint, nbPart);
        break;
      case FlatGeobuf::GeometryType::MultiPolygon:
        if (is3D) vector = new XFlatGeoBufMPoly3D(this, offset, F, nbPoint, nbPart);
        else vector = new XFlatGeoBufMPoly2D(this, offset, F, nbPoint, nbPart);
        break;
      default:;
    }
    if (vector != nullptr)
      m_Data.push_back(vector);

  }
  if (m_FeatureData != nullptr)
    delete[] m_FeatureData;
  m_FeatureData = (uint8_t*)data_feature;
  return infile->good();
}

//-----------------------------------------------------------------------------
// Analyse d'une geometrie 3D
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::AnalyzeGeometry3D(const FlatGeobuf::Geometry* geometry, XFrame* F, double* Zmin, double* Zmax,
                                      uint32_t* nbPoint, uint32_t* nbPart) const
{
  auto parts = geometry->parts();
  double xmin = std::numeric_limits<double>::max();
  double xmax = std::numeric_limits<double>::lowest();
  double ymin = xmin, ymax = xmax, zmin = xmin, zmax = xmax;
  *nbPoint = *nbPart = 0;
  if (parts == nullptr) {
    auto xy = geometry->xy();
    auto zvalue = geometry->z();
    if ((xy == nullptr)||(zvalue == nullptr))
      return false;
    *nbPoint = (uint32_t)xy->size();
    auto ends = geometry->ends();
    if (ends != nullptr)
      *nbPart = (uint32_t)ends->size();
    else
      *nbPart = 1;

    for (flatbuffers::uoffset_t i = 0; i < xy->size(); i+=2) {
      xmin = XMin(xmin, (*xy)[i]);
      xmax = XMax(xmax, (*xy)[i]);
      ymin = XMin(ymin, (*xy)[i+1]);
      ymax = XMax(ymax, (*xy)[i+1]);
    }
    for (flatbuffers::uoffset_t i = 0; i < zvalue->size(); i++) {
      zmin = XMin(zmin, (*zvalue)[i]);
      zmax = XMax(zmax, (*zvalue)[i]);
    }
  }
  else {
    for (flatbuffers::uoffset_t j = 0; j < parts->size(); j++) {
      auto geopart = (*parts)[j];
      auto xy = geopart->xy();
      auto zvalue = geopart->z();
      if ((xy == nullptr) || (zvalue == nullptr))
        return false;
      *nbPoint += (uint32_t)xy->size();
      auto ends = geopart->ends();
      if (ends != nullptr)
        *nbPart += (uint32_t)ends->size();
      else
        *nbPart += 1;

      for (flatbuffers::uoffset_t i = 0; i < xy->size(); i += 2) {
        xmin = XMin(xmin, (*xy)[i]);
        xmax = XMax(xmax, (*xy)[i]);
        ymin = XMin(ymin, (*xy)[i + 1]);
        ymax = XMax(ymax, (*xy)[i + 1]);
      }
      for (flatbuffers::uoffset_t i = 0; i < zvalue->size(); i++) {
        zmin = XMin(zmin, (*zvalue)[i]);
        zmax = XMax(zmax, (*zvalue)[i]);
      }
    }
  }

  if (m_ProjCode == XGeoProjection::RGF93) {
    xmin = XWebMercator::ConvertLonToXDeg(xmin);
    xmax = XWebMercator::ConvertLonToXDeg(xmax);
    ymin = XWebMercator::ConvertLatToYDeg(ymin);
    ymax = XWebMercator::ConvertLatToYDeg(ymax);
  }

  *F = XFrame(xmin, ymin, xmax, ymax);
  *Zmin = zmin;
  *Zmax = zmax;
  return true;
}

//-----------------------------------------------------------------------------
// Analyse d'une geometrie 2D
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::AnalyzeGeometry2D(const FlatGeobuf::Geometry* geometry, XFrame* F, uint32_t* nbPoint, uint32_t* nbPart) const
{
  auto parts = geometry->parts();
  double xmin = std::numeric_limits<double>::max();
  double xmax = std::numeric_limits<double>::lowest();
  double ymin = xmin, ymax = xmax;
  *nbPoint = *nbPart = 0;
  if (parts == nullptr) {
    auto xy = geometry->xy();
    if (xy == nullptr) 
      return false;
    *nbPoint = (uint32_t)xy->size();
    auto ends = geometry->ends();
    if (ends != nullptr)
      *nbPart = (uint32_t)ends->size();
    else
      *nbPart = 1;
    for (flatbuffers::uoffset_t i = 0; i < xy->size(); i += 2) {
      xmin = XMin(xmin, (*xy)[i]);
      xmax = XMax(xmax, (*xy)[i]);
      ymin = XMin(ymin, (*xy)[i + 1]);
      ymax = XMax(ymax, (*xy)[i + 1]);
    }
  }
  else {
    for (flatbuffers::uoffset_t j = 0; j < parts->size(); j++) {
      auto geopart = (*parts)[j];
      auto xy = geopart->xy();
      if (xy == nullptr)
        return false;
      *nbPoint += (uint32_t)xy->size();
      auto ends = geopart->ends();
      if (ends != nullptr)
        *nbPart += (uint32_t)ends->size();
      else
        *nbPart += 1;

      for (flatbuffers::uoffset_t i = 0; i < xy->size(); i += 2) {
        xmin = XMin(xmin, (*xy)[i]);
        xmax = XMax(xmax, (*xy)[i]);
        ymin = XMin(ymin, (*xy)[i + 1]);
        ymax = XMax(ymax, (*xy)[i + 1]);
      }
    }
  }

  if (m_ProjCode == XGeoProjection::RGF93) {
    xmin = XWebMercator::ConvertLonToXDeg(xmin);
    xmax = XWebMercator::ConvertLonToXDeg(xmax);
    ymin = XWebMercator::ConvertLatToYDeg(ymin);
    ymax = XWebMercator::ConvertLatToYDeg(ymax);
  }

  *F = XFrame(xmin, ymin, xmax, ymax);
  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une geometrie
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::LoadFlatGeoBufGeom(uint64_t offset, XFlatGeoBufVector* V, bool needZ)
{
  // Ouverture du fichier
  std::ifstream* infile = m_In.IStream();
  if (infile == nullptr)
    return false;
  infile->seekg(offset, std::ios::beg);

  uint32_t length = 0;
  infile->read((char*)&length, sizeof(uint32_t));
  infile->read((char*)m_FeatureData, length);
  auto feature = FlatGeobuf::GetFeature(m_FeatureData);
  auto geometry = feature->geometry();

  int nb = 0;
  std::vector<int> Parts;
  XPt* P = nullptr;
  double zmin = std::numeric_limits<double>::max(), zmax = std::numeric_limits<double>::lowest();
  double* Z = nullptr;

  auto parts = geometry->parts();
  // Single Part
  if (parts == nullptr) {
    auto xy = geometry->xy();
    if (xy == nullptr)
      return false;
    nb = (uint32_t)(xy->size() / 2);
    P = new(std::nothrow) XPt[nb];
    if (P == nullptr)
      return false;
    for (uint32_t i = 0; i < nb; i++) {
      P[i].X = (*xy)[2 * i];
      P[i].Y = (*xy)[2 * i + 1];
    }
    if (m_ProjCode == XGeoProjection::RGF93) {
      for (uint32_t i = 0; i < nb; i++) {
        P[i].X = XWebMercator::ConvertLonToXDeg(P[i].X);
        P[i].Y = XWebMercator::ConvertLatToYDeg(P[i].Y);
      }
    }
    auto ends = geometry->ends();
    if (ends != nullptr) {
      Parts.push_back(0);
      for (uint32_t i = 1; i < ends->size() - 1; i++)
        Parts.push_back((*ends)[i - 1]);
    }

    if (needZ) {
      auto zvalues = geometry->z();
      if (zvalues != nullptr) {
        Z = new (std::nothrow) double[zvalues->size()];
        if (Z == nullptr) {
          delete[] P;
          return false;
        }
        for (flatbuffers::uoffset_t i = 0; i < zvalues->size(); i++) {
          Z[i] = (*zvalues)[i];
          zmin = XMin(zmin, Z[i]);
          zmax = XMax(zmax, Z[i]);
        }
      }
    }
  
    V->SetGeom(nb, P, Z, 0, nullptr, zmin, zmax);
    return true;
  }

  // Multi-parts
  for (int j = 0; j < parts->size(); j++) {
    auto geopart = (*parts)[j];
    auto xy = geopart->xy();
    if (xy == nullptr)
      return false;
    nb += (uint32_t)(xy->size() / 2);
  }
  P = new(std::nothrow) XPt[nb];
  if (P == nullptr) 
    return false;
  
  XPt* ptrP = P;
  int cmptPt = 0;
  for (flatbuffers::uoffset_t j = 0; j < parts->size(); j++) {
    Parts.push_back(cmptPt);
    auto geopart = (*parts)[j];
    auto xy = geopart->xy();
    for (uint32_t i = 0; i < xy->size() / 2; i++) {
      ptrP->X = (*xy)[2 * i];
      ptrP->Y = (*xy)[2 * i + 1];
      if (m_ProjCode == XGeoProjection::RGF93) {
        ptrP->X = XWebMercator::ConvertLonToXDeg(ptrP->X);
        ptrP->Y = XWebMercator::ConvertLatToYDeg(ptrP->Y);
      }
      ptrP++;
    }
    auto ends = geopart->ends();
    if (ends != nullptr) {
      for (uint32_t i = 1; i <= ends->size() - 1; i++)
        Parts.push_back(cmptPt + (int)(*ends)[i - 1]);
    }
    cmptPt += (int)(xy->size() / 2);
  }

  if (needZ) {
    Z = new (std::nothrow) double[nb];
    if (Z == nullptr) {
      delete[] P;
      return false;
    }
    double* ptrZ = Z;
    for (flatbuffers::uoffset_t j = 0; j < parts->size(); j++) {
      auto geopart = (*parts)[j];
      auto z = geopart->z();
      if (z == nullptr)
        continue;
      for (uint32_t i = 0; i < z->size(); i++) {
        *ptrZ = (*z)[i];
        zmin = XMin(zmin, *ptrZ);
        zmax = XMax(zmax, *ptrZ);
        ptrZ++;
      }
    }
  }

  int* T = new int[Parts.size()];
  memcpy(T, Parts.data(), Parts.size() * sizeof(int));
  V->SetGeom(nb, P, Z, Parts.size(), T, zmin, zmax);

  return true;
}

//-----------------------------------------------------------------------------
// Chargement d'une geometrie
//-----------------------------------------------------------------------------
bool XFlatGeoBufMap::ReadAttributes(uint64_t offset, std::vector<std::string>& V, XGeoClass* C)
{
  // Ouverture de l'entete
  if (m_HeaderData == nullptr)
    return false;
  auto header = FlatGeobuf::GetHeader(m_HeaderData);
  if (header == nullptr)
    return false;

  // Ouverture du fichier
  std::ifstream* infile = m_In.IStream();
  if (infile == nullptr)
    return false;
  infile->seekg(offset, std::ios::beg);

  uint32_t length = 0;
  infile->read((char*)&length, sizeof(uint32_t));
  infile->read((char*)m_FeatureData, length);
  auto feature = FlatGeobuf::GetFeature(m_FeatureData);

  V.clear();

  auto columns = feature->columns();
  if (columns == nullptr)
    columns = header->columns();
  if (columns == nullptr)
    return false;

  // Nom des colonnes
  for (auto iter = columns->begin(); iter != columns->end(); iter++) {
    V.push_back((*iter)->name()->str());
    V.push_back("");
  }
  
  if (feature->properties() != nullptr) {
    for (auto iter = feature->properties()->begin(); iter != feature->properties()->end(); ) {
      uint16_t index = 0;
      uint8_t* ptr = (uint8_t*)&index;
      ptr[0] = *iter; iter++;
      ptr[1] = *iter; iter++;
      if (index >= columns->size())
        continue;

      int strlength = 0;
      std::ostringstream out;

      switch ((*columns)[index]->type()) {
      case FlatGeobuf::ColumnType::Byte :
        out << *iter;
        iter++;
        break;
      case FlatGeobuf::ColumnType::UByte:
        out << *iter;
        iter++;
        break;
      case FlatGeobuf::ColumnType::Bool:
        //out << *iter;
        if (*iter == 0) out << "False"; else out << "True";
        iter++;
        break;
      case FlatGeobuf::ColumnType::Short:
        int16_t s;
        ptr = (uint8_t*)&s;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        out << s;
        break;
      case FlatGeobuf::ColumnType::UShort:
        uint16_t us;
        ptr = (uint8_t*)&us;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        out << us;
        break;
      case FlatGeobuf::ColumnType::Int:
        int32_t n;
        ptr = (uint8_t*)&n;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        out << n;
        break;
      case FlatGeobuf::ColumnType::UInt:
        uint32_t un;
        ptr = (uint8_t*)&un;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        out << un;
        break;
      case FlatGeobuf::ColumnType::Long:
        int64_t l;
        ptr = (uint8_t*)&l;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        ptr[4] = *iter; iter++;
        ptr[5] = *iter; iter++;
        ptr[6] = *iter; iter++;
        ptr[7] = *iter; iter++;
        out << l;
        break;
      case FlatGeobuf::ColumnType::ULong:
        uint64_t ul;
        ptr = (uint8_t*)&ul;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        ptr[4] = *iter; iter++;
        ptr[5] = *iter; iter++;
        ptr[6] = *iter; iter++;
        ptr[7] = *iter; iter++;
        out << ul;
        break;
      case FlatGeobuf::ColumnType::Float:
        float f;
        ptr = (uint8_t*)&f;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        out << f;
        break;
      case FlatGeobuf::ColumnType::Double:
        double d;
        ptr = (uint8_t*)&d;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        ptr[4] = *iter; iter++;
        ptr[5] = *iter; iter++;
        ptr[6] = *iter; iter++;
        ptr[7] = *iter; iter++;
        out << d;
        break;
      case FlatGeobuf::ColumnType::String:
        ptr = (uint8_t*)&strlength;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        for (int cmpt = 0; cmpt < strlength; cmpt++) {
          out << *iter;
          iter++;
        }
        break;
      case FlatGeobuf::ColumnType::Json:
        ptr = (uint8_t*)&strlength;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        for (int cmpt = 0; cmpt < strlength; cmpt++) {
          out << *iter;
          iter++;
        }
        break;

      case FlatGeobuf::ColumnType::DateTime:
        ptr = (uint8_t*)&strlength;
        ptr[0] = *iter; iter++;
        ptr[1] = *iter; iter++;
        ptr[2] = *iter; iter++;
        ptr[3] = *iter; iter++;
        for (int cmpt = 0; cmpt < strlength; cmpt++) {
          out << *iter;
          iter++;
        }
        break;
      case FlatGeobuf::ColumnType::Binary:

        break;
      }

      V[index * 2 + 1] = out.str();

    }
  }
  return true;
}
