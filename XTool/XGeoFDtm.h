//-----------------------------------------------------------------------------
//								XGeoFDtm.h
//								==========
//
// Gestion des MNT codes avec des flottants
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 03/03/2009
//-----------------------------------------------------------------------------

#ifndef _XGEOFDTM_H
#define _XGEOFDTM_H

#include "XGeoVector.h"
#include "XFile.h"

class XGeodConverter;

class XGeoFDtm : public XGeoVector {
protected:
	std::string		m_strName;
	std::string		m_strFilename;
	std::string		m_strPath;
  std::string   m_strImageName;
	double				m_dGSD;
	double				m_dZmin;
	double				m_dZmax;
	double				m_dNoData;
	uint32_t				m_nW;
	uint32_t				m_nH;
  uint32_t        m_nOffset;
  uint32_t        m_nNbNoZ;
	bool					m_bValid;
  bool          m_bTmpFile;
  XFile         m_In;
  std::ifstream* m_ActiveStream;

public:
	XGeoFDtm() {
		m_dGSD = 0.; m_dZmin = m_dZmax = XGEO_NO_DATA; m_bValid = m_bTmpFile = false; m_ActiveStream = nullptr;
							m_dNoData = -9999.; m_nOffset=0; m_nNbNoZ = m_nW = m_nH = 0;}
  virtual ~XGeoFDtm() {Close();}
  bool OpenDtm(const char* filename, const char* tmpname);
  virtual void Close();

	virtual eTypeVector TypeVector () const { return DTM;}
	inline bool Valid() const { return m_bValid;}

	virtual inline double Zmin() const { return m_dZmin;}
	virtual inline double Zmax() const { return m_dZmax;}
	virtual inline double NoData() const {return m_dNoData;}
	
	virtual inline uint32_t W() { return m_nW;}
	virtual inline uint32_t H() { return m_nH;}
  virtual inline uint32_t Offset() { return m_nOffset;}
  virtual inline uint32_t NbNoZ() { return m_nNbNoZ;}

	virtual uint32_t NbPt() const { return 5;}
	virtual uint32_t NbPart() const { return 1;}
	virtual bool IsClosed() const { return true;}
  virtual XPt2D Pt(uint32_t i);

	void UpdateGeoref(double xmin, double ymax, double resol) 
										{ m_Frame.Xmin = xmin; m_Frame.Ymax = ymax; m_dGSD = resol;} 
	void UpdateStat(double zmin, double zmax, uint32_t nbNoZ)
										{ m_dZmin = zmin; m_dZmax = zmax; m_nNbNoZ = nbNoZ;}

	virtual inline double Resolution() const { return m_dGSD;}
	void Resolution(double d) { m_dGSD = d;}

  virtual std::string Filename() { return m_strFilename;}
	std::string Path() { return m_strPath;}
  std::string ImageName() { return m_strImageName;}

  virtual void Filename(std::string s) { m_strFilename = s;}
	void Path(std::string s) { m_strPath = s;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

  virtual bool StreamReady();
  virtual bool ReadLine(float* line, uint32_t numLine);
  virtual bool ReadNode(float* node, uint32_t x, uint32_t y);
  virtual bool ReadAll(float* area);

  virtual double Z(const XPt2D& P);
	double Z(double x, double y) { XPt2D P(x, y); return Z(P);}
	double Z(uint32_t x, uint32_t y);
  virtual uint32_t ZFrame(const XFrame& F, double* zmax, double* zmin, double* zmean);
  bool Ground2Pix(double x, double y, uint32_t& u, uint32_t& v);
  //bool Volume(uint32_t& nbLow, uint32_t& nbHigh, float Z0 = 0.);
  bool Volume(std::vector<double>& P, std::vector<uint32_t>& N, double& zmean);
  bool DeltaMax(double Dz, std::vector<XPt3D>& T);
  bool FindContourLine(double Z0, uint8_t* area);
  bool FindThalweg(std::string filename);

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);

	virtual bool Export(std::string filename);
  virtual bool ExportTiff16(std::string filename);
  virtual bool ExportAsc(std::string filename);
	virtual bool ExportXyz(std::string filename);
  virtual bool ExportContour(std::string filename, double equi, double resol = 0.);
  virtual int ExportFlood(std::string filename, double Z0, std::vector<XPt2D>& P,
                          int nb_seed = 0, std::vector<XGeoVector*>* V = NULL);
  virtual bool ExportDiff(std::string filename, XGeoFDtm* dtm);

	bool ImportAsc(std::string file_asc, std::string file_bin);
	bool ImportDis(std::string file_asc, std::string file_bin);
	bool ImportXyz(std::string file_asc, std::string file_bin);
  bool ImportHdr(std::string file_hdr, std::string file_bin);
  virtual bool ImportTif(std::string file_tif, std::string file_bin) { return false;}
  virtual bool ImportTiff16(std::string file_tif, std::string file_bin) { return false;}

	bool WriteAscHeader(std::ostream* out);

	bool ConvertAsc(const char* file_out, XGeodConverter* L, XError* error = NULL);

  virtual	bool ReadAttributes(std::vector<std::string>& V);
  virtual bool WriteHtml(std::ostream* out);

public:
  bool InjectVector(XGeoVector* V, uint8_t* area, uint8_t val);
  bool FindMinMax(XGeoVector* V, XPt3D* Pmin, XPt3D* Pmax, double* zmean, uint32_t* nbNoeud);
  bool FindMinMax(XGeoVector* V, XPt3D* Pmin, XPt3D* Pmax);

};


#endif //_XGEOFDTM_H
