//-----------------------------------------------------------------------------
//								XFileImage.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 31/08/2021
//-----------------------------------------------------------------------------

#ifndef XFILEIMAGE_H
#define XFILEIMAGE_H

#include "XBaseImage.h"
#include "../XTool/XFile.h"

class XTransfo;
class XInterpol;

class XFileImage {
public:
  XFileImage();
  ~XFileImage() { Close(); }

  virtual bool AnalyzeImage(std::string path);
  virtual bool IsValid() { if (m_Image == nullptr) return false; return true; }
  void Close();

  uint32_t Width() { if (m_Image != nullptr) return m_Image->W(); return 0; }
  uint32_t Height() { if (m_Image != nullptr) return m_Image->H(); return 0; }
  bool GetGeoref(double* xmin, double* ymax, double* gsd)
  {
    if (m_Image == nullptr) return false; *xmin = m_Image->X0(); *ymax = m_Image->Y0(); *gsd = m_Image->GSD(); if (*gsd <= 0.) return false; return true;
  }
  void SetGeoref(double xmin, double ymax, double gsd)
  {
    if (m_Image != nullptr) m_Image->SetGeoref(xmin, ymax, gsd);
  }

  int NbByte();
  //int NbChannel();
  uint16_t NbBits() { if (m_Image == nullptr) return 0; return m_Image->NbBits(); }
  uint16_t NbSample() { if (m_Image == nullptr) return 0; return m_Image->NbSample(); }
  std::string Format() { if (m_Image == nullptr) return ""; return m_Image->Format(); }

  std::string GetMetadata() { if (m_Image == NULL) return ""; return m_Image->Metadata(); }
  std::string GetXmlMetadata() { if (m_Image == NULL) return ""; return m_Image->XmlMetadata(); }

  virtual bool GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

  virtual bool GetRawPixel(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample);
  virtual bool GetRawArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix, uint32_t* nb_sample, uint32_t factor = 1);

  // Statistiques sur l'image
  virtual bool GetStat(double minVal[4], double maxVal[4], double meanVal[4], uint32_t noData[4], double no_data = 0.);

  // Rotation d'une zone de pixels
  bool RotateArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t nbbyte, uint32_t rot) const;

  // Allocation d'une ligne de pixels
  uint8_t* AllocArea(uint32_t w, uint32_t h) { return new uint8_t[w * h * NbByte()]; }

  // Fixe la relation canaux <-> RGB
  void SetRGBChannel(uint8_t r, uint8_t g, uint8_t b) { m_RGBChannel[0] = r; m_RGBChannel[1] = g; m_RGBChannel[2] = b; }
  void GetRGBChannel(uint8_t& r, uint8_t& g, uint8_t& b) { r = m_RGBChannel[0]; g = m_RGBChannel[1]; b = m_RGBChannel[2]; }
  void SetPalette(uint8_t* palette);

  // Fonction de reechantillonnage
  bool Resample(std::string file_out, XTransfo* transfo, XInterpol* inter, XWait* wait = nullptr);

protected:
  XBaseImage*   m_Image;
  std::string   m_strFilename;
  XFile         m_File;
  uint8_t          m_RGBChannel[3];
  uint8_t*         m_Palette;  // Palette utilisateur

  bool AnalyzeTiff();
  bool AnalyzeBigTiff();
  bool AnalyzeCog();
  bool AnalyzeJpeg2000();
  bool AnalyzeWebP();

  bool PostProcessRGB(uint8_t* area, uint8_t* val, uint32_t w, uint32_t h, uint32_t factor = 1);
};

#endif //XFILEIMAGE_H
