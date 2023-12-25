//-----------------------------------------------------------------------------
//								XGeoRaster.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/08/2002
//-----------------------------------------------------------------------------

#ifndef _XGEORASTER_H
#define _XGEORASTER_H

#include "XGeoVector.h"

//-----------------------------------------------------------------------------
// Contexte d'un raster
//-----------------------------------------------------------------------------
class XGeoRasterContext {
protected:
	bool			m_bActif;			// Indique si le contexte est actif
	uint16_t		m_nNbChannel;
	double*		m_Gamma;			// Tableau de gamma pour les canaux
	uint8_t*			m_Min;				// Tableau des niveaux minimums pour les canaux
	uint8_t*			m_Max;				// Tableau des niveaux maximums pour les canaux
	uint8_t**		m_Lut;				// Tableau des LUT pour les canaux
  bool      m_bMedian;    // Filtre median actif
  uint16_t    m_nWinMed;    // Fenetre du filtre median
  uint16_t    m_nCenMed;    // Centile du filtre median
  bool      m_bMatrix;    // Filtre matriciel actif
  int       m_Matrix[10];
  bool      m_bImageAdd;  // Indique que l'on a une image additive
  bool      m_bNormGradient;  // Norme du gradient actif
  bool      m_bDirGradient;
  uint8_t*     m_Palette;    // Palette eventuelle
  uint8_t      m_RGBChannel[3];  // Fixe la relation canaux <-> RGB

public:
	XGeoRasterContext();
	virtual ~XGeoRasterContext();

	void Clear();
  void Clone(XGeoRasterContext* clone);
	bool NbChannel(uint16_t nb);
	inline uint16_t NbChannel() { return m_nNbChannel;}
	bool Channel(uint16_t nb, double gamma, uint8_t min, uint8_t max);
	bool Channel(uint16_t nb, double* gamma, uint8_t* min, uint8_t* max, uint8_t** lut);
  bool NeedLut();
	uint8_t* Lut(uint16_t nb);

  uint8_t Min(uint16_t nb) { if(nb < m_nNbChannel) return m_Min[nb]; return 0;}
  uint8_t Max(uint16_t nb) { if(nb < m_nNbChannel) return m_Max[nb]; return 255;}
  double Gamma(uint16_t nb) { if(nb < m_nNbChannel) return m_Gamma[nb]; return 1.0;}

  void Median(bool flag, uint16_t win, uint16_t cen) { m_bMedian = flag; m_nWinMed = win; m_nCenMed = cen;}
  bool Median() { return m_bMedian;}
  uint16_t WinMedian() { return m_nWinMed;}
  uint16_t CenMedian() { return m_nCenMed;}

  void Matrix(bool flag, int matrix[10]) { m_bMatrix = flag; for (int i=0;i<10;i++) m_Matrix[i]=matrix[i];}
  bool Matrix() { return m_bMatrix;}
  int Denom() { return m_Matrix[0];}
  int* Matrix3x3() {return &m_Matrix[1];}

  void ImageAdd(bool flag) { m_bImageAdd = flag;}
  inline bool ImageAdd() { return m_bImageAdd;}

  void Gradient(bool norm, bool dir) { m_bNormGradient = norm; m_bDirGradient = dir;}
  inline bool NormGradient() { return m_bNormGradient;}
  inline bool DirGradient() { return m_bDirGradient;}

  void Palette(uint8_t* pal);
  uint8_t* Palette() { return m_Palette;}

  void SetRGBChannel(uint8_t r, uint8_t g, uint8_t b) { m_RGBChannel[0] = r; m_RGBChannel[1] = g; m_RGBChannel[2] = b;}
  void GetRGBChannel(uint8_t& r, uint8_t& g, uint8_t& b) { r = m_RGBChannel[0]; g = m_RGBChannel[1]; b = m_RGBChannel[2];}

	void Actif(bool flag) { m_bActif = flag;}
	inline bool Actif() const { return m_bActif;}
};

//-----------------------------------------------------------------------------
// Histogramme d'un raster
//-----------------------------------------------------------------------------
class XGeoRasterHisto {
protected:
	uint16_t		m_nNbChannel;
	double**	m_H;					// Tableau des histogrammes pour les canaux

public:
	XGeoRasterHisto();
	virtual ~XGeoRasterHisto();

	void Clear();
	bool NbChannel(uint16_t nb);
	inline uint16_t NbChannel() { return m_nNbChannel;}
	bool Channel(uint16_t nb, double* H);

	bool Analyze(uint16_t nb, uint16_t& min, uint16_t& max, uint16_t& nblevel,
								uint16_t& nbhole, uint16_t& holemax, double& satur,
								double holesize = 0.);
	bool Write(uint16_t nb, std::ostream& out);
};

//-----------------------------------------------------------------------------
// Gestion des objets raster
//-----------------------------------------------------------------------------
class XGeoRaster : public XGeoVector {
protected:
	uint32_t				m_nW;
	uint32_t				m_nH;
	uint16_t				m_nByte;
	double				m_dResolution;
	XGeoVector*		m_Mask;
	std::string		m_strName;
	std::string		m_strFilename;
	std::string		m_strPath;
	uint16_t				m_nRot;						// Indique la rotation de l'image
//  XGeoClass*		m_Class;					// Classe de l'objet
	XGeoRasterContext*	m_Context;
	XGeoRasterHisto*		m_Histo;

public:
	XGeoRaster() : m_dResolution(0.) {m_Histo = NULL; m_Context = NULL; m_nRot = 0; m_Mask = NULL; m_nW = m_nH = 0; m_nByte = 0;}
	virtual ~XGeoRaster() {if (m_Context != NULL) delete m_Context; if (m_Histo != NULL) delete m_Histo;}

	virtual eTypeVector TypeVector () const { return Raster;}

	// Geometrie de l'image
	virtual uint32_t ImageWidth() { return m_nW;}
	virtual uint32_t ImageHeight() { return m_nH;}
	virtual uint16_t NbChannel() { return m_nByte;}

	// Georeferencement de l'image
	virtual void UpdateGeoref(double xmin, double ymax, double resol)
										{ m_Frame.Xmin = xmin; m_Frame.Ymax = ymax; m_dResolution = resol;}
  bool Pix2Ground(uint32_t u, uint32_t v, double& x, double& y);
  bool Ground2Pix(double x, double y, uint32_t& u, uint32_t& v);

	virtual inline double Resolution() const { return m_dResolution;}
	void Resolution(double d) { m_dResolution = d;}

  virtual std::string Filename() { return m_strFilename;}
	std::string Path() { return m_strPath;}

  virtual void Filename(std::string s) { m_strFilename = s;}
	void Path(std::string s) { m_strPath = s;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

	virtual	bool ReadAttributes(std::vector<std::string>& V)
											{ V.clear(); V.push_back("Nom"); V.push_back(m_strName);return true;}
	virtual bool GetHisto(double*, double*, double*) { return false;}

	inline uint16_t Rotation() { return m_nRot;}
	virtual void Rotation(uint16_t rot) { m_nRot = rot;}

	virtual uint32_t NbPt() const { return 5;}
	virtual uint32_t NbPart() const { return 1;}
	virtual bool IsClosed() const { return true;}
	virtual XPt2D Pt(uint32_t i);
  virtual bool Intersect(const XFrame& F) { return m_Frame.Intersect(F);}

	virtual bool CreateContext() {return false;}
	virtual void ClearContext() {if (m_Context != NULL) delete m_Context; m_Context = NULL;}
	virtual XGeoRasterContext* Context() { return m_Context;}
  virtual void ContextChange() {;}
	virtual bool ComputeAutoLevel(double = 0.02) { return false;}
	virtual bool ComputeAutoLevelBalance(double = 0.02) { return false;}
	virtual bool Gamma(double gamma);

	virtual bool CreateHisto() {return false;}
	virtual bool CreateHisto(XFrame&) { return false;}
	virtual void ClearHisto() {if (m_Histo != NULL) delete m_Histo; m_Histo = NULL;}
	virtual bool WriteHisto(uint16_t nb, std::ostream& out);
	virtual bool AnalyzeHisto(uint16_t nb, uint16_t& min, uint16_t& max, uint16_t& nblevel,
														uint16_t& nbhole, uint16_t& holemax, double& satur,
														double holesize = 0.);

};

#endif //_XGEORASTER_H
