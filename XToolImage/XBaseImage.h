//-----------------------------------------------------------------------------
//								XBaseImage.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 18/06/2021
//-----------------------------------------------------------------------------

#ifndef XBASEIMAGE_H
#define XBASEIMAGE_H

#include "../XTool/XBase.h"
#include "../XTool/XFile.h"

class XTransfo;
class XInterpol;

class XBaseImage {
protected:
	uint32_t		m_nW;
	uint32_t		m_nH;
	uint16_t		m_nNbBits;
	uint16_t		m_nNbSample;
  uint16_t    m_nSampleFormat;  // 1 : unsigned ; 2 : signed ; 3 : IEEE float; 4 : undefined
  uint16_t*		m_ColorMap;
  uint16_t    m_nColorMapSize;
  uint8_t*     m_ChannelHints;

	// Georeferencement
	double		m_dX0;
	double		m_dY0;
	double		m_dGSD;

public:
  XBaseImage();
	virtual ~XBaseImage();

	// Geometrie de l'image
	inline uint32_t W() { return m_nW; }
	inline uint32_t H() { return m_nH; }
	inline uint16_t NbBits() { return m_nNbBits; }
	inline uint16_t NbSample() { return m_nNbSample; }
  inline uint16_t SampleFormat() { return m_nSampleFormat;}
	virtual uint32_t RowH() { return 1; }		// Renvoie le groupement de ligne optimal pour l'image

  // Metadonnees
  virtual std::string Format() { return "Undefined";}
	virtual std::string Metadata();
  virtual std::string XmlMetadata() { return "";}

  // Palette de couleurs
  uint16_t ColorMapSize() { return m_nColorMapSize;}
  uint16_t* ColorMap() { return m_ColorMap;}
	bool UpdateColorMap(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
  bool ApplyColorMap(uint8_t* in, uint8_t* out, uint32_t w, uint32_t h);

  // Indication des canaux utiles
  virtual void SetChannelHints(uint8_t* hints) { m_ChannelHints = hints;}

	// Georeferencement de l'image
	inline double GSD() { return m_dGSD; }
	inline double X0() { return m_dX0; }
	inline double Y0() { return m_dY0; }
	void GetGeoref(double* xmin, double* ymax, double* gsd) { *xmin = m_dX0; *ymax = m_dY0; *gsd = m_dGSD; }
	void SetGeoref(double xmin, double ymax, double gsd) { m_dX0 = xmin; m_dY0 = ymax; m_dGSD = gsd; }

	virtual uint32_t PixSize();
	virtual uint8_t* AllocArea(uint32_t w, uint32_t h);

	// Acces aux pixels
	virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area) = 0;
	virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area) = 0;
	virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor) = 0;
	virtual bool GetRawArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix,
                          uint32_t* nb_sample, uint32_t factor = 1, bool normalized = false);
  virtual bool GetRawPixel(XFile* file, uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample);
  virtual bool GetStat(XFile* file, double* minVal, double* maxVal, double* meanVal, uint32_t* noData, double no_data = 0.);

  // Passage en 8 bits
  static double MinValue;
  static double MaxValue;
	static double Boost_Hi;
	static double Boost_Lo;
  static bool Uint16To8bits(uint8_t* buffer, uint32_t w, uint32_t h);
  static bool Int16To8bits(uint8_t* buffer, uint32_t w, uint32_t h);

	// Swap pour l'endianess
	static void Swap16bits(uint8_t* buffer, uint32_t nb_value);
	static void Swap32bits(uint8_t* buffer, uint32_t nb_value);

	// Methodes statiques de manipulation de pixels
	static bool CMYK2RGB(uint8_t* buffer, uint32_t w, uint32_t h);
	static bool CMYK2RGBA(uint8_t* buffer, uint32_t w, uint32_t h); 
	static bool YCbCr2RGB(uint8_t* buffer, uint32_t w, uint32_t h);
  static bool MultiSample2RGB(uint8_t* buffer, uint32_t w, uint32_t h, uint16_t nbSample, uint16_t idxR = 0, uint16_t idxG = 1, uint16_t idxB = 2);
	static bool MultiSample2RGB(uint8_t* buffer, uint32_t w, uint32_t h, uint16_t nbSample, uint8_t* paletteRGB);
  static bool ExtractArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout, uint32_t x0, uint32_t y0);
	static bool CopyArea(uint8_t* patch, uint8_t* image, uint32_t wpatch, uint32_t hpatch, uint32_t wimage, uint32_t himage, uint32_t x0, uint32_t y0);
	static bool Crop(uint8_t* buffer, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout, uint32_t x0, uint32_t y0);
	static bool ZoomArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout, uint32_t nbbyte);
	static bool ZoomAreaRGB(uint8_t* R, uint8_t* G, uint8_t* B, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout);
	static void SwitchRGB2BGR(uint8_t* buf, uint32_t nb_pix);
	static void SwitchARGB2BGR(uint8_t* buf, uint32_t nb_pix);
	static void Gray2RGB(uint8_t* buf, uint32_t nb_pix);
	static void Gray2RGBPalette(uint8_t* in, uint8_t* out, uint32_t nb_pix, uint8_t* palette);
	static void RGB2RGBA(uint8_t* buf, uint32_t nb_pix, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t alpha = 255);
	static void RGB2BGRA(uint8_t* buf, uint32_t nb_pix, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t alpha = 255);
	static void Gray2RGBA(uint8_t* buf, uint32_t nb_pix, uint8_t gray = 0, uint8_t alpha = 255);
	static void OffsetArea(uint8_t* buf, uint32_t w, uint32_t h, uint32_t lineW);
	static bool RotateArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t nbbyte, uint32_t rot);
  static void Normalize(uint8_t* pix_in, double* pix_out, uint32_t nb_pixel, double* mean, double* std_dev);
  static double Covariance(double* pix1, double* pix2, uint32_t nb_pixel);
  static bool Correlation(uint8_t* pix1, uint32_t w1, uint32_t h1, uint8_t* pix2, uint32_t w2, uint32_t h2,
                          double* u, double* v, double* pic);
	static bool FastZoomBil(float* in, uint32_t win, uint32_t hin, float* out, uint32_t wout, uint32_t hout);
	static bool Resample(uint8_t* in, uint8_t* out, uint32_t w, uint32_t h, uint16_t nbSample, uint16_t offset, 
											 XTransfo* transfo, XInterpol* interpol, bool noBorder);
	static bool Resample(float* in, float* out, uint32_t w, uint32_t h, uint16_t nbSample, uint16_t offset,
											 XTransfo* transfo, XInterpol* interpol, bool noBorder);

};

#endif //XBASEIMAGE_H
