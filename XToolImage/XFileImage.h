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
class XFrame;

class XFileImage {
public:
  XFileImage();
  virtual ~XFileImage() { Close(); }

  virtual bool AnalyzeImage(std::string path);
  virtual bool IsValid() { if (m_Image == nullptr) return false; return true; }
  virtual void Close();

  virtual uint32_t Width() { if (m_Image != nullptr) return m_Image->W(); return 0; }
  virtual uint32_t Height() { if (m_Image != nullptr) return m_Image->H(); return 0; }
  virtual bool GetGeoref(double* xmin, double* ymax, double* gsd)
  {
    if (m_Image == nullptr) return false; *xmin = m_Image->X0(); *ymax = m_Image->Y0(); *gsd = m_Image->GSD(); if (*gsd <= 0.) return false; return true;
  }
  virtual void SetGeoref(double xmin, double ymax, double gsd)
  {
    if (m_Image != nullptr) m_Image->SetGeoref(xmin, ymax, gsd);
  }

  virtual int NbByte();
  //int NbChannel();
  virtual uint16_t NbBits() { if (m_Image == nullptr) return 0; return m_Image->NbBits(); }
  virtual uint16_t NbSample() { if (m_Image == nullptr) return 0; return m_Image->NbSample(); }
  virtual std::string Format() { if (m_Image == nullptr) return ""; return m_Image->Format(); }

  virtual std::string Filename() { return m_strFilename; }
  virtual std::string GetMetadata() { if (m_Image == nullptr) return ""; return m_Image->Metadata(); }
  virtual std::string GetXmlMetadata() { if (m_Image == nullptr) return ""; return m_Image->XmlMetadata(); }

  virtual bool GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

  virtual bool GetRawPixel(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample);
  virtual bool GetRawArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix, uint32_t* nb_sample, uint32_t factor = 1);

  // Statistiques sur l'image
  virtual bool GetStat(double minVal[4], double maxVal[4], double meanVal[4], uint32_t noData[4], double no_data = 0.);

  // Rotation d'une zone de pixels
  bool RotateArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t nbbyte, uint32_t rot) const;

  // Allocation d'une ligne de pixels
  uint8_t* AllocArea(uint32_t w, uint32_t h) { return new (std::nothrow) uint8_t[w * h * NbByte()]; }

  // Fixe la relation canaux <-> RGB
  void SetRGBChannel(uint8_t r, uint8_t g, uint8_t b) { m_RGBChannel[0] = r; m_RGBChannel[1] = g; m_RGBChannel[2] = b; }
  void GetRGBChannel(uint8_t& r, uint8_t& g, uint8_t& b) { r = m_RGBChannel[0]; g = m_RGBChannel[1]; b = m_RGBChannel[2]; }
  void SetPalette(uint8_t* palette);

  // Mise a jour de la palette
  bool UpdateColorMap(uint8_t index, uint8_t r, uint8_t g, uint8_t b) { return m_Image->UpdateColorMap(index, r, g, b); }
  bool GetColorMapRGB(uint8_t index, uint8_t& r, uint8_t& g, uint8_t& b);

  // Fonction de reechantillonnage
  bool Resample(std::string file_out, XTransfo* transfo, XInterpol* inter, XWait* wait = nullptr);

  // Preparation pour un dessin
  bool PrepareRasterDraw(XFrame* F, double gsdR, int& U0, int& V0, int& win, int& hin, int& nbBand, int& R0, int& S0, int& wout, int& hout);

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
  bool AnalyzeJpeg();

  bool PostProcessRGB(uint8_t* area, uint8_t* val, uint32_t w, uint32_t h, uint32_t factor = 1);
};

#endif //XFILEIMAGE_H
