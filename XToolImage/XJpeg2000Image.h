//-----------------------------------------------------------------------------
//								XJpeg2000Image.h
//								================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/06/2021
//-----------------------------------------------------------------------------

#ifndef XJPEG2000IMAGE_H
#define XJPEG2000IMAGE_H

#ifdef KAKADU_LIB
#include "XBaseImage.h"

class kdu_region_compositor;
class XKduRegionCompositor;
class jpx_source;
class jp2_family_src;

class XJpeg2000Image : public XBaseImage {
protected:
  bool											m_bValid = false;	// Indique si l'image est valide
  jpx_source*               m_Jpx_in = nullptr;
  kdu_region_compositor*    m_Compositor = nullptr;
  jp2_family_src*           m_Src = nullptr;
  std::string							  m_strXmlMetadata;
  int                       m_nBitDepth = 0;
  XKduRegionCompositor*     m_RawCompositor = nullptr;
  XKduRegionCompositor*     m_FloatCompositor = nullptr;

  bool ReadGeorefXmlOld();
  bool ReadGeorefXml();
  bool ReadGeorefUuid(std::istream* uuid);

public:
  XJpeg2000Image(const char* filename);
  virtual ~XJpeg2000Image();

  virtual std::string Format() { return "JP2";}
  virtual std::string Metadata();
  bool IsValid() { return m_bValid; }
  virtual inline bool NeedFile() { return false; }

  virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
  virtual bool GetJp2Area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area,
                          uint32_t factor = 1, uint32_t wout = 0, uint32_t hout = 0);
  virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area) { return false; }
  bool GetJp2AreaFloat(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area,
                       uint32_t factor = 1, uint32_t wout = 0, uint32_t hout = 0);

  virtual uint32_t FileSize() { return 0; }

  // Donnees brutes
  virtual bool GetRawPixel(XFile* file, uint32_t x, uint32_t y, uint32_t win, double* pix,
    uint32_t* nb_sample);
  virtual bool GetRawArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix,
    uint32_t* nb_sample, uint32_t factor = 1, bool normalized = false);

  virtual std::string XmlMetadata() { return m_strXmlMetadata; }
};
#endif // KAKADU_LIB

#endif //XJPEG2000IMAGE_H
