//-----------------------------------------------------------------------------
//								XGeoJson.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 18/12/2017
//-----------------------------------------------------------------------------

#include "XGeoJson.h"
#include <sstream>
#include <algorithm>

#include "../XTool/XPath.h"
#include "../XToolGeod/XGeoPref.h"
#include "../XTool/XGeoBase.h"

XGeoJson::XGeoJson()
{
  m_Projection = XGeoProjection::Unknown;
}

void XGeoJson::Class(XGeoClass* C)
{
  uint32_t i;
  for (i = 0; i < m_Data.size(); i++){
    XGeoVector* record = (XGeoVector*)m_Data[i];
    record->Class(C);
    C->Vector(record);
  }
}

//-----------------------------------------------------------------------------
// Renvoi la classe d'objets
//-----------------------------------------------------------------------------
XGeoClass* XGeoJson::Class()
{
  if (m_Data.size() < 1)
    return NULL;
  XGeoVector* record = (XGeoVector*)m_Data[0];
  return record->Class();
}

//-----------------------------------------------------------------------------
// Lecture d'un token JSON
//-----------------------------------------------------------------------------
std::string XGeoJson::ReadStringToken(std::istream* in)
{
  char sep;
  std::string token;
  *in >> sep;
  if (sep == '"') {
     do {
     if (sep != '"')
       token += sep;
     sep = in->get();
     } while (sep != '"');
    return token;
  }
  if (sep == '{') {
   int count_open = 1;
     int count_close = 0;
     do {
        token += sep;
        sep = in->get();
        if (sep == '{')
           count_open++;
        if (sep == '}')
           count_close++;
     } while(count_close != count_open);
     token += sep;
     return token;
  }

  if (sep == '[') {
     int count_open = 1;
     int count_close = 0;
     do {
        token += sep;
        sep = in->get();
        if (sep == '[')
           count_open++;
        if (sep == ']')
           count_close++;
     } while(count_close != count_open);
     token += sep;
     return token;
  }
	token += sep;
	do {
		*in >> sep;
		if ((sep == ',')||(sep == '}')||(sep == ']')){
			in->putback(sep);
			break;
		}
		token += sep;
	} while(in->good());
	return token;
}

//-----------------------------------------------------------------------------
// Lecture d'un objet JSON
//-----------------------------------------------------------------------------
bool XGeoJson::ReadKeyValue(std::istream* in, std::string& key, std::string& value)
{
 key = ReadStringToken(in);
 if (key.size() < 1)
    return false;
 char sep;
 *in >> sep;
 if (sep != ':')
    return false;
 value = ReadStringToken(in);
 if (value.size() < 1)
    return false;
 return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un objet JSON
//-----------------------------------------------------------------------------
std::string XGeoJson::ReadObject(std::istream* in)
{
  char sep;
  std::string token;
  *in >> sep;
  if (sep == '{') {
   int count_open = 1;
     int count_close = 0;
     do {
        token += sep;
        sep = in->get();
        if (sep == '{')
           count_open++;
        if (sep == '}')
           count_close++;
     } while(count_close != count_open);
     token += sep;
     return token;
  }
  return "";
}

//-----------------------------------------------------------------------------
// Lecture d'un point 2D ou 3D
//-----------------------------------------------------------------------------
bool XGeoJson::ReadPoint(std::istream* in, double &x, double &y, double &z)
{
  char sep;
  *in >> sep;
  if (sep != '[')
    return false;
  *in >> x;
  *in >> sep;
  if (sep != ',')
    return false;
  *in >> y;
  *in >> sep;
  if (sep == ',') {
    *in >> z;
    *in >> sep;
  }
  while (sep != ']')
    *in >> sep;
  return true;
}

//-----------------------------------------------------------------------------
// Analyze d'une LineString
//-----------------------------------------------------------------------------
bool XGeoJson::AnalyzeLineString(std::istream* in, XFrame* F, uint32_t* nbPt, XPt* P)
{
  char sep;
  double x, y, z;
  *F = XFrame();
  uint32_t numPt = 0;
  *in >> sep;
  if (sep != '[')
    return false;
  do {
    if (!ReadPoint(in, x, y , z))
      return false;
    if ((P != NULL)&&(numPt < *nbPt)){
        P[numPt].X = x;
        P[numPt].Y = y;
    }
    numPt++;
    *F += XPt2D(x, y);
    *in >> sep;
    if (sep == ']')
      break;
    if (sep != ',')
      return false;
  } while (true);
  if (P == NULL)
    *nbPt = numPt;

  return true;
}


//-----------------------------------------------------------------------------
// Analyze d'un polygone
//-----------------------------------------------------------------------------
bool XGeoJson::AnalyzePolygon(std::istream* in, XFrame* F, uint32_t* nbPt, uint32_t* nbPart,
                               XPt* P, int *Parts)
{
  char sep;
  double x, y, z;
  *F = XFrame();
  uint32_t numPt = 0;
  uint32_t numPart = 1;
  if (Parts != NULL)
    Parts[0] = 0;
  *in >> sep;
  if (sep != '[')
    return false;
  do {
    *in >> sep;
    if (sep != '[')
      return false;
    do {
      if (!ReadPoint(in, x, y , z))
        return false;
      if ((P != NULL)&&(numPt < *nbPt)){
          P[numPt].X = x;
          P[numPt].Y = y;
      }
      numPt++;
      *F += XPt2D(x, y);
      *in >> sep;
      if (sep == ']')
        break;
      if (sep != ',')
        return false;
    } while (true);
    *in >> sep;
    if (sep == ']')
      break;
    if (sep != ',')
      return false;
    if ((Parts != NULL)&&(numPart < *nbPart))
      Parts[numPart] = numPt;
    numPart++;
  } while (true);
  if (P == NULL)
    *nbPt = numPt;
  if (Parts == NULL)
    *nbPart = numPart;

  return true;
}

//-----------------------------------------------------------------------------
// Analyze d'un multi-polygone
//-----------------------------------------------------------------------------
bool XGeoJson::AnalyzeMultiPolygon(std::istream* in, XFrame* F, uint32_t* nbPt, uint32_t* nbPart,
                                    XPt* P, int *Parts)
{
  char sep;
  double x, y, z;
  *F = XFrame();
  uint32_t numPt = 0;
  uint32_t numPart = 1;
  if (Parts != NULL)
    Parts[0] = 0;
  *in >> sep;
  if (sep != '[')
    return false;
  do {
    *in >> sep;
    if (sep != '[')
      return false;
    do {
      *in >> sep;
      if (sep != '[')
        return false;
      do {
        if (!ReadPoint(in, x, y , z))
          return false;
        if ((P != NULL)&&(numPt < *nbPt)){
          P[numPt].X = x;
          P[numPt].Y = y;
        }
        numPt++;
        *F += XPt2D(x, y);
        *in >> sep;
        if (sep == ']')
          break;
        if (sep != ',')
          return false;
      } while (true);
      *in >> sep;
      if (sep == ']')
        break;
      if (sep != ',')
        return false;
      if ((Parts != NULL)&&(numPart < *nbPart))
        Parts[numPart] = numPt;
      numPart++;
    } while (true);
    *in >> sep;
    if (sep == ']')
      break;
    if (sep != ',')
      return false;
    if ((Parts != NULL)&&(numPart < *nbPart)) // Bug sur les polygones en plusieurs parties disjointes
      Parts[numPart] = numPt;
    numPart++;
  } while (true);
  if (P == NULL)
    *nbPt = numPt;
  if (Parts == NULL)
    *nbPart = numPart;

  return true;
}

//-----------------------------------------------------------------------------
// Analyze d'une geometrie
//-----------------------------------------------------------------------------
bool XGeoJson::AnalyzeGeometry(std::istream* in, std::string &type, XFrame* F,
                               uint32_t* nbPt, uint32_t* nbPart, XPt* P, int *Parts)
{
  char sep;
  *in >> sep;
  if (sep != '{')
     return false;
  bool flag;
  std::string key, value, coord;
  do {
     flag = ReadKeyValue(in, key, value);
     if (!flag)
        return false;
     if (key == "coordinates")
        coord = value;
     if (key == "type")
        type = value;

     *in >> sep;
     if (sep == '}')
         break;
  } while(true);
  std::istringstream in_coord;
  in_coord.str(coord);
  if (type == "Point") {
    double x, y, z;
    ReadPoint(&in_coord, x, y, z);
    F->Xmin = F->Xmax = x;
    F->Ymin = F->Ymax = y;
    return true;
  }
  if ((type == "LineString") || (type == "MultiPoint"))
     return AnalyzeLineString(&in_coord, F, nbPt, P);
  if ((type == "Polygon") || (type == "MultiLineString"))
     return AnalyzePolygon(&in_coord, F, nbPt, nbPart, P, Parts);
  if (type == "MultiPolygon")
     return AnalyzeMultiPolygon(&in_coord, F, nbPt, nbPart, P, Parts);

  return false;
}

//-----------------------------------------------------------------------------
// Analyse d'un feature GEOJSON
//-----------------------------------------------------------------------------
bool XGeoJson::AnalyzeFeature(std::istream* in)
{
  uint32_t pos = in->tellg();
  char sep;
  *in >> sep;
  if (sep != '{') {
    in->putback(sep);
    return false;
  }

  bool flag;
  std::string key, value, type;
  XFrame F;
  uint32_t nbPt, nbPart;
  XGeoPref pref;
  do {
    flag = ReadKeyValue(in, key, value);
    if (!flag)
      return false;
    if (key == "geometry") {
      std::istringstream in_geom;
      in_geom.str(value);
      if(AnalyzeGeometry(&in_geom, type, &F, &nbPt, &nbPart)) {
        if (m_Projection == XGeoProjection::Unknown) {
          if ((fabs(F.Xmin) <= 180.)&&(fabs(F.Xmax) <= 180.)&&(fabs(F.Ymin) <= 180.)&&(fabs(F.Ymax) <= 180.))
            m_Projection = XGeoProjection::RGF93;
          else
            m_Projection = pref.Projection();
        }
        if (m_Projection == XGeoProjection::RGF93) {
          pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), F.Xmin, F.Ymin, F.Xmin, F.Ymin);
          pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), F.Xmax, F.Ymax, F.Xmax, F.Ymax);
        }

        XGeoObject* obj = NULL;
        if (type == "Point")
          obj = new XGeoJsonPoint2D(this, pos, &F);
        if (type == "MultiPoint")
          obj = new XGeoJsonMPoint2D(this, pos, &F, &nbPt);
        if (type == "LineString")
          obj = new XGeoJsonLine2D(this, pos, &F, &nbPt);
        if (type == "Polygon") {
          if (nbPart == 1)
            obj = new XGeoJsonPoly2D(this, pos, &F, &nbPt);
          else
            obj = new XGeoJsonMPoly2D(this, pos, &F, &nbPt, &nbPart);
        }
        if (type == "MultiLineString")
          obj = new XGeoJsonMLine2D(this, pos, &F, &nbPt, &nbPart);
        if (type == "MultiPolygon")
          obj = new XGeoJsonMPoly2D(this, pos, &F, &nbPt, &nbPart);

        if (obj != NULL)
          m_Data.push_back(obj);
          m_Frame += F;
      }
    }
    *in >> sep;
    if (sep == '}')
      break;
  } while(true);
  return true;
}

//-----------------------------------------------------------------------------
// Lecture de la geometrie d'une feature GEOJSON
//-----------------------------------------------------------------------------
bool XGeoJson::ReadFeatureGeom(uint32_t pos, uint32_t nbPt, XPt* P, uint32_t nbPart, int* Parts)
{
  if (pos >= 2147483647L) {// Fichier > 2GB
    m_In.seekg(2147483647L);
    m_In.seekg((pos - 2147483647L), std::ios_base::cur);
  } else
    m_In.seekg(pos);
  if (!m_In.good())
    return false;
  char sep;
  m_In >> sep;
  if (sep != '{')
    return false;

  bool flag;
  std::string key, value, type;
  XFrame F;
  XGeoPref pref;
  do {
    flag = ReadKeyValue(&m_In, key, value);
    if (!flag)
      return false;
    if (key == "geometry") {
      std::istringstream in_geom;
      in_geom.str(value);
      if(AnalyzeGeometry(&in_geom, type, &F, &nbPt, &nbPart, P, Parts)) {
        for(uint32_t i = 0; i < nbPt; i++)
          if (m_Projection == XGeoProjection::RGF93)
            pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), P[i].X, P[i].Y, P[i].X, P[i].Y);
       }
    }
    m_In >> sep;
    if (sep == '}')
      break;
  } while(true);
  return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeoJson::ReadAttributes(uint32_t pos, std::vector<std::string>& V)
{
  if (pos >= 2147483647L) {// Fichier > 2GB
    m_In.seekg(2147483647L);
    m_In.seekg((pos - 2147483647L), std::ios_base::cur);
  } else
    m_In.seekg(pos);
  if (!m_In.good())
    return false;
  char sep;
  m_In >> sep;
  if (sep != '{')
    return false;

  bool flag;
  std::string key, value;
  do {
    flag = ReadKeyValue(&m_In, key, value);
    if (!flag)
      return false;
    if (key == "properties") {
      std::istringstream in_att;
      in_att.str(value);

      in_att >> sep;
      if (sep == '{') {
        do {
         flag = ReadKeyValue(&in_att, key, value);
         if (!flag)
            break;
         V.push_back(key);
         V.push_back(value);
         in_att >> sep;
         if (sep == '}')
             break;
        } while(true);
      }
    } else
      ReadForeignMembers(key, value, V);
    m_In >> sep;
    if (sep == '}')
      break;
  } while(true);
  return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs d'un fichier GEOJSON
//-----------------------------------------------------------------------------
bool XGeoJson::Read(const char* filename, XError* error)
{
  XPath P;
  m_strFilename = filename;
  m_strName = P.Name(filename);
  m_Frame = XFrame();
  m_In.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!m_In.good())
    return XErrorError(error, "XGeoJson::Read", XError::eIOOpen);

  char sep;
  std::string token;
  m_In >> sep;
  if (sep != '{')
    return false;

  while( (!m_In.eof()) && (m_In.good()) ) {
    token = ReadStringToken(&m_In);
    m_In >> sep;
    if (sep != ':')
       return false;
    std::transform(token.begin(), token.end(), token.begin(), tolower);
    if (token == "features") {
      m_In >> sep;
       if (sep != '[')
          return false;
       do {
          AnalyzeFeature(&m_In);
          m_In >> sep;
      } while(sep != ']');
    } else
      ReadStringToken(&m_In);

    m_In >> sep;
    if (sep == '}')
      break;
    if (sep != ',')
      return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Import d'un GeoJSON dans une XGeoBase
//-----------------------------------------------------------------------------
XGeoClass* XGeoJson::ImportGeoJson(XGeoBase* base, const char* path, XGeoMap* map)
{
  XPath P;
  XGeoJson* geoFile = new XGeoJson;
  XGeoClass* C = NULL;

  if (geoFile->Read(path)) {
    C = base->AddClass(P.Name(P.Path(path).c_str()).c_str(), P.Name(path, false).c_str());
    geoFile->Class(C);
    if (map == NULL)
      base->AddMap(geoFile);
    else
      map->AddObject(geoFile);
    base->SortClass();
  } else
    delete geoFile;

  return C;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeoJsonVector::ReadGeoJsonAttributes(std::vector<std::string>& V)
{
  if (m_File == NULL)
    return false;
  return m_File->ReadAttributes(m_Pos, V);
}

//-----------------------------------------------------------------------------
// Recherche d'un attribut
//-----------------------------------------------------------------------------
std::string XGeoJsonVector::FindGeoJsonAttribute(std::string att_name, bool no_case)
{
  if (m_File == NULL)
    return "";
  std::vector<std::string> V;
  if (!m_File->ReadAttributes(m_Pos, V))
    return "";
  for (uint32_t i = 0; i < V.size(); i+=2) {
    if (no_case)
      std::transform(V[i].begin(), V[i].end(), V[i].begin(), tolower);
    if (V[i] == att_name)
      return V[i+1];
  }
  return "";
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour un multi-point 2D
//-----------------------------------------------------------------------------
bool XGeoJsonMPoint2D::LoadGeom()
{
  if (m_File == NULL)
    return false;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    Unload();
    return false;
    }
  return m_File->ReadFeatureGeom(m_Pos, m_nNumPoints, m_Pt, 1, NULL);
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour une polyligne 2D
//-----------------------------------------------------------------------------
bool XGeoJsonLine2D::LoadGeom()
{
  if (m_File == NULL)
    return false;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    Unload();
    return false;
    }
  return m_File->ReadFeatureGeom(m_Pos, m_nNumPoints, m_Pt, 1, NULL);
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour un multi-polyligne 2D
//-----------------------------------------------------------------------------
bool XGeoJsonMLine2D::LoadGeom()
{
  if (m_File == NULL)
    return false;

  m_Parts = new int[m_nNumParts];
  if (m_Parts == NULL) {
    Unload();
    return false;
  }

  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    Unload();
    return false;
    }
  return m_File->ReadFeatureGeom(m_Pos, m_nNumPoints, m_Pt, m_nNumParts, m_Parts);
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour un polygone
//-----------------------------------------------------------------------------
bool XGeoJsonPoly2D::LoadGeom()
{
  if (m_File == NULL)
    return false;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    Unload();
    return false;
    }
  return m_File->ReadFeatureGeom(m_Pos, m_nNumPoints, m_Pt, 1, NULL);
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour un multi-polygone
//-----------------------------------------------------------------------------
bool XGeoJsonMPoly2D::LoadGeom()
{
  if (m_File == NULL)
    return false;

  m_Parts = new int[m_nNumParts];
  if (m_Parts == NULL) {
    Unload();
    return false;
  }

  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    Unload();
    return false;
    }
  return m_File->ReadFeatureGeom(m_Pos, m_nNumPoints, m_Pt, m_nNumParts, m_Parts);
}
