//-----------------------------------------------------------------------------
//								XCsv.h
//								======
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 5/12/2011
//-----------------------------------------------------------------------------

#ifndef XCSV_H
#define XCSV_H

#include <fstream>
#include <string>
#include <vector>
#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoPoint.h"

class XGeoBase;

//-----------------------------------------------------------------------------
// Fichier CSV
//-----------------------------------------------------------------------------
class XCsvFile :  public XGeoMap {
protected:
  std::ifstream							m_In;
  std::string								m_strFilename;
  char											m_Sep;				// Separateur dans le fichier CSV
  std::vector<std::string>  m_Column;     // Titre des colonnes
  uint32_t                  m_nXPos;      // Position colonne X
  uint32_t                  m_nYPos;      // Position colonne Y
  bool                      m_bGeo;       // Coordonnees geographiques
  uint32_t                  m_nStartLine; // Ligne de debut des objets

  bool ReadLine(char* buf, std::vector<std::string>& V);

public:
  XCsvFile();
  virtual ~XCsvFile() {;}

  void Class(XGeoClass* C);
  XGeoClass* Class();

  bool FindColumns(const char* filename, uint32_t start = 0, char sep = ',', XError* error = nullptr);
  bool Read(const char* filename, uint32_t start, uint32_t xpos, uint32_t ypos, bool geo,
            char sep = ',', uint32_t typecoord = 0, uint32_t zpos = 0, bool zdata = false, XError* error = nullptr);
  bool ReadAttributes(uint32_t pos, std::vector<std::string>& V);
  std::string FindAttribute(uint32_t pos, std::string att_name);

  static XGeoClass* ImportCsv(XGeoBase* base, const char* path, XGeoMap* /*map*/);
};

//-----------------------------------------------------------------------------
// Point dans un fichier CSV
//-----------------------------------------------------------------------------
class XCsvPoint2D : public XGeoPoint2D {
protected :
  uint32_t  m_nPos;	// Position dans le fichier
  XCsvFile*	m_File;

public:
  XCsvPoint2D(XCsvFile* file = nullptr, uint32_t pos = 0) { m_File = file; m_nPos = pos;}

  inline XCsvFile* File() const { return m_File;}
  virtual inline XGeoMap* Map() const { return m_File;}

  void SetXY(double x, double y) { m_Frame.Xmin = m_Frame.Xmax = x; m_Frame.Ymin = m_Frame.Ymax = y; }

  virtual	bool ReadAttributes(std::vector<std::string>& V) { return m_File->ReadAttributes(m_nPos, V);}
  virtual std::string FindAttribute(std::string att_name) { return m_File->FindAttribute(m_nPos, att_name);}
};

class XCsvPoint3D : public XCsvPoint2D {
protected :
  double  m_Z;    // Altitude

public:
  XCsvPoint3D(XCsvFile* file = nullptr, uint32_t pos = 0) :  XCsvPoint2D(file, pos){ m_Z = 0.;}

  void SetZ(double z) { m_Z = z; }

  virtual eTypeVector TypeVector () const { return PointZ;}

  virtual double Z(uint32_t) { return m_Z;}
  virtual bool Is3D() const { return true;}

  virtual inline double Zmin() const { return m_Z;}
  virtual inline double Zmax() const { return m_Z;}
};

#endif // XCSV_H
