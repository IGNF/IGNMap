//-----------------------------------------------------------------------------
//								XCsv.cpp
//								========
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 5/12/2011
//-----------------------------------------------------------------------------

#include <cstring>
#include <cfloat>
#include <sstream>

#include "XCsv.h"
#include "../XTool/XPath.h"
#include "../XTool/XGeoBase.h"
#include "../XToolGeod/XGeoPref.h"

//-----------------------------------------------------------------------------
// Import d'un fichier CSV avec detection automatique des champs
//-----------------------------------------------------------------------------
XGeoClass* XCsvFile::ImportCsv(XGeoBase* base, const char* path, XGeoMap* /*map*/)
{
  XCsvFile commaFile, tabFile, pointFile;
  for (int i = 0; i < 3; i++) {
    char sep = ',';
    if (i == 1) sep = ';';
    if (i == 2) sep = '\t';
    XCsvFile csv;
    if (!csv.FindColumns(path, 0, sep))
      continue;
    int xpos = -1, ypos = -1, zpos = -1, latpos = -1, lonpos = -1;
    for (int col = 0; col < csv.m_Column.size(); col++) {
      if ((csv.m_Column[col] == "X") || (csv.m_Column[col] == "x") || (csv.m_Column[col] == "@X") || (csv.m_Column[col] == "@x"))
        xpos = col;
      if ((csv.m_Column[col] == "Y") || (csv.m_Column[col] == "y") || (csv.m_Column[col] == "@Y") || (csv.m_Column[col] == "@y"))
        ypos = col;
      if ((csv.m_Column[col] == "Z") || (csv.m_Column[col] == "z"))
        zpos = col;
      if ((csv.m_Column[col] == "latitude") || (csv.m_Column[col] == "lat") || (csv.m_Column[col] == "@lat") ||
          (csv.m_Column[col] == "Lat") || (csv.m_Column[col] == "Latitude"))
        latpos = col;
      if ((csv.m_Column[col] == "longitude") || (csv.m_Column[col] == "lon") || (csv.m_Column[col] == "@lon") ||
        (csv.m_Column[col] == "Lon") || (csv.m_Column[col] == "Longitude"))
        lonpos = col;
    }
    if ((xpos < 0) && (ypos < 0) && (lonpos < 0) && (latpos < 0))
      continue;
    bool zdata = false, geo = false;
    if (zpos >= 0) zdata = true;
    if ((xpos < 0) && (ypos < 0) && (lonpos >= 0) && (latpos >= 0)) {
      xpos = lonpos;
      ypos = latpos;
      geo = true;
    }
     
    XCsvFile* newCsv = new XCsvFile;
    newCsv->FindColumns(path, 0, sep);
    if (!newCsv->Read(path, 1, xpos, ypos, geo, sep, 1, zpos, zdata)) {
      delete newCsv;
      continue;
    }
    XPath P;
    XGeoClass* C = base->AddClass("CSV", P.Name(path, false).c_str());
    newCsv->Class(C);
    base->AddMap(newCsv);
    base->SortClass();
    return C;
  }
  return nullptr;
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XCsvFile::XCsvFile()
{
  m_Sep = ',';
  m_nXPos = 1;
  m_nYPos = 2;
  m_nStartLine = 1;
  m_bGeo = false;
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier
//-----------------------------------------------------------------------------
bool XCsvFile::Read(const char* filename, uint32_t start, uint32_t xpos, uint32_t ypos,
                    bool geo, char sep, uint32_t typecoord, uint32_t zpos, bool zdata,
                    XError* error)
{
  XPath P;
  m_Sep = sep;
  m_strFilename = filename;
  m_strName = P.Name(filename);
  m_In.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!m_In.good())
    return XErrorError(error, "XCsvFile::Read", XError::eIOOpen);

  char buf[1024];
  m_Frame = XFrame();

  // Lecture du fichier
  uint32_t numline = 0;
  m_nStartLine = start;
  std::vector<std::string> V;
  double x = 0., y = 0., z = 0., lat = 0., lon = 0.;
  XGeoPref Pref;
  uint32_t pos = 0;

  while( (!m_In.eof()) && (m_In.good()) ) {
    pos = (uint32_t)m_In.tellg();
    m_In.getline(buf, 1024);
    numline++;
    if (numline <= m_nStartLine)
      continue;

    ReadLine(buf, V);

    if ((V.size() <= xpos)||(V.size() <= ypos))
      continue; // Ligne incomplete
    if ((V[xpos] == "")||(V[xpos] == "0"))
      continue;
    if ((V[ypos] == "")||(V[ypos] == "0"))
      continue;
    (void)sscanf(V[xpos].c_str(), "%lf", &x);
    (void)sscanf(V[ypos].c_str(), "%lf", &y);
    if (zdata) {
      z = 0;
      if (V.size() > zpos) {
        (void)sscanf(V[zpos].c_str(), "%lf", &z);
      }
    }
    if (geo) {  // Coordonnees geographiques
      if (typecoord == 0) { // radians
        lat = y * 180. / XPI;
        lon = x * 180. / XPI;
      }
      if (typecoord == 1) { // degres decimaux
        lat = y;
        lon = x;
      }
      if (typecoord == 2) { // degres decimaux
        lat = y / 100000;
        lon = x / 100000;
      }
      if (typecoord == 3) { // degres minutes secondes
        double deg = 0., min = 0., sec = 0., sign = 1.;
        char ori[80], first_car;
        std::istringstream iss;

        iss.str(V[xpos]);
        iss >> std::skipws >> first_car;
        if (first_car == '-') sign = -1;
        (void)sscanf(V[xpos].c_str(), "%lf°%lf'%lf%s", &deg, &min, &sec, ori);
        if (deg < 0) sign = -1.;
        if (strlen(ori) > 0)
          if ((ori[strlen(ori)-1] == 'W')||(ori[strlen(ori)-1] == 'O'))
            sign *= -1.;
        lon = sign * fabs(deg) + sign * min / 60. + sign * sec / 3600.;

        sign = 1.;
        deg = min = sec = 0.0;
        iss.str(V[ypos]);
        iss >> std::skipws >> first_car;
        if (first_car == '-') sign = -1;
        (void)sscanf(V[ypos].c_str(), "%lf°%lf'%lf%s", &deg, &min, &sec, ori);
        if (deg < 0) sign = -1.;
        if (strlen(ori) > 0)
          if ((ori[strlen(ori)-1] == 'S'))
            sign *= -1.;
        lat = sign * fabs(deg) + sign * min / 60. + sign * sec / 3600.;
      }
      if (typecoord == 4) { // degres sexagesimaux
        double deg, min, sec;
        if (y >= 0.)  // Latitude
          deg = floor(y);
        else
          deg = ceil(y);
        sec = modf((y - deg) * 100., &min);
        lat = deg + min / 60. + sec / 36.;
        if (x >= 0.)  // Longitude
          deg = floor(x);
        else
          deg = ceil(x);
        sec = modf((x - deg) * 100., &min);
        lon = deg + min / 60. + sec / 36.;
      }
      Pref.ConvertDeg(XGeoProjection::RGF93,Pref.Projection(), lon, lat, x, y);
    }
    if ((x < XGEO_NO_DATA)||(y < XGEO_NO_DATA))
      continue;
    if ((x > 10e10)||(y > 10e10))
      continue;
    if (std::isnan(x)) continue;
    if (std::isnan(y)) continue;

    if (!zdata) {
      XCsvPoint2D* point = new XCsvPoint2D(this, pos);
      point->SetXY(x, y);
      m_Data.push_back(point);
      m_Frame += point->Frame();
    } else {
      XCsvPoint3D* point = new XCsvPoint3D(this, pos);
      point->SetXY(x, y);
      point->SetZ(z);
      m_Data.push_back(point);
      m_Frame += point->Frame();
    }
  }
  m_In.clear();
  return true;
}

//-----------------------------------------------------------------------------
// Recherche les colonnes
//-----------------------------------------------------------------------------
bool XCsvFile::FindColumns(const char* filename, uint32_t start, char sep, XError* error)
{
  XPath P;
  m_Sep = sep;
  m_strFilename = filename;
  m_strName = P.Name(filename);
  m_In.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!m_In.good())
    return XErrorError(error, "XCsvFile::FindColumns", XError::eIOOpen);

  char buf[1024];

  // Lecture du fichier
  uint32_t numline = 0;
  m_nStartLine = start;
  std::vector<std::string> V;

  while( (!m_In.eof()) && (m_In.good()) ) {
    m_In.getline(buf, 1024);
    numline++;
    if (numline <= start)
      continue;

    ReadLine(buf, m_Column);
    break;
  }
  m_In.close();
  return true;
}

//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XCsvFile::Class(XGeoClass* C)
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
XGeoClass* XCsvFile::Class()
{
  if (m_Data.size() < 1)
    return NULL;
  XGeoVector* record = (XGeoVector*)m_Data[0];
  return record->Class();
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XCsvFile::ReadAttributes(uint32_t pos, std::vector<std::string>& V)
{
  if (!m_In.good())
    return false;
  m_In.seekg(pos);
  char buf[1024];
  m_In.getline(buf, 1024);
  std::vector<std::string> Att;
  if(!ReadLine(buf, Att))
    return false;
  V.clear();
  for (uint32_t i = 0; i < m_Column.size(); i++) {
    V.push_back(m_Column[i]);
    if (i < Att.size())
      V.push_back(Att[i]);
    else
      V.push_back("");
  }
  return true;
}

//-----------------------------------------------------------------------------
// Recherche d'un attribut
//-----------------------------------------------------------------------------
std::string XCsvFile::FindAttribute(uint32_t pos, std::string att_name)
{
  bool flag = false;
  uint32_t i, att_num = 0;
  for (i = 0; i < m_Column.size(); i++)
    if (m_Column[i] == att_name) {
      flag = true;
      att_num = i;
      break;
    }
  if (!flag)
    return "";
  std::vector<std::string> Att;
  if (ReadAttributes(pos, Att))
    return Att[att_num * 2 + 1];
  return "";
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne
//-----------------------------------------------------------------------------
bool XCsvFile::ReadLine(char* buf, std::vector<std::string>& V)
{
  uint32_t n = 0;
  std::string token;
  bool inside_quote = false;
  V.clear();
  /*
  do {
    token = "";
    while(true) {
      if ((buf[n] != m_Sep)&& (n < strlen(buf))){
        if((buf[n] != '"')&&(buf[n] != 0x0A)&&(buf[n] != 0x0D))
          token += buf[n];
        else
          if (buf[n] == '"') inside_quote = !inside_quote;
        n++;
      } else {
        if ((inside_quote)&&(buf[n] == m_Sep)) {
          token += buf[n];
          n++;
        } else {
          n++;
          break;
        }
      }
    }
    V.push_back(token);
  } while(n < strlen(buf));
  */
  do {
    token = "";
    while(true) {
      if ((buf[n] != m_Sep)&& (n < strlen(buf))){
        if ((token.size() == 0)&&(buf[n] == '"')) {
          inside_quote = !inside_quote;
        } else {
          if((buf[n] != '"')&&(buf[n] != 0x0A)&&(buf[n] != 0x0D))
            token += buf[n];
          else {
            if (buf[n] == '"') {
              if (inside_quote)
                inside_quote = !inside_quote;
              else
                token += buf[n];
            }
          }
        }
        n++;
      } else {
        if ((inside_quote)&&(buf[n] == m_Sep)) {
          token += buf[n];
          n++;
        } else {
          n++;
          inside_quote = false;
          break;
        }
      }
    }
    V.push_back(token);
  } while(n < strlen(buf));
  return true;
}
