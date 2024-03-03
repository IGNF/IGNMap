//-----------------------------------------------------------------------------
//								XFileImage.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 31/08/2021
//-----------------------------------------------------------------------------

#include <cstring>
#include "XFileImage.h"

#include "XTiffReader.h"
#include "XBigTiffReader.h"
#include "XTiffStripImage.h"
#include "XTiffTileImage.h"
#include "XCogImage.h"
#include "XJpeg2000Image.h"
#include "XOpenJp2Image.h"
#include "XWebPImage.h"
#include "XDtmShader.h"
#include "../XTool/XTransfo.h"
#include "../XTool/XInterpol.h"
#include "XTiffWriter.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XFileImage::XFileImage()
{
  m_Image = nullptr;
  m_RGBChannel[0] = 0;
  m_RGBChannel[1] = 1;
  m_RGBChannel[2] = 2;
  m_Palette = nullptr;
}

//-----------------------------------------------------------------------------
// Fermeture de l'image
//-----------------------------------------------------------------------------
void XFileImage::Close()
{
  m_File.Close();
  if (m_Image != nullptr)
    delete m_Image;
  m_Image = nullptr;
  if (m_Palette != nullptr)
    delete[] m_Palette;
  m_Palette = nullptr;
}

//-----------------------------------------------------------------------------
// Fixe la palette a appliquer aux canaux
//-----------------------------------------------------------------------------
void XFileImage::SetPalette(uint8_t* palette)
{
  if (m_Palette != nullptr)
    delete[] m_Palette;
  m_Palette = palette; // La palette doit etre un tableau de NbSample * 3 valeurs
}

//-----------------------------------------------------------------------------
// Nombre d'octets utilises pour l'affiche de l'image
//-----------------------------------------------------------------------------
int XFileImage::NbByte()
{
  if (m_Image->ColorMapSize() > 0)
    return 3;
  if ((m_Image->NbSample() == 1) && (m_Image->NbBits() == 32))
    return 3;
  if (m_Image->NbSample() == 1)
    return 1;
  return 3;
}
/*
int XFileImage::NbChannel()
{
  if (m_Image->ColorMapSize() > 0)
    return 3;
  if ((m_Image->NbSample() == 1) && (m_Image->NbBits() == 32))
    return 3;
  if (m_Image->NbSample() == 1)
    return 1;
  return 3;
}*/

//-----------------------------------------------------------------------------
// Analyse de l'image
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeImage(std::string path)
{
  m_strFilename = path;
  if (AnalyzeJpeg2000())
    return true;
  if (AnalyzeCog()) // COG avant le TIFF, car le COG est du TIFF
    return true;
  if (AnalyzeTiff())
    return true;
  if (AnalyzeBigTiff())
    return true;
  if (AnalyzeWebP())
    return true;
  return false;
}

//-----------------------------------------------------------------------------
// Analyse d'une image TIFF
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeTiff()
{
  if (!m_File.Open(m_strFilename.c_str(), std::ios::in | std::ios::binary))
    return false;

  XTiffReader reader;
  if (!reader.Read(m_File.IStream())) {
    m_File.Close();
    return false;
  }

  if (!reader.AnalyzeIFD(m_File.IStream())) {
    m_File.Close();
    return false;
  }

  if (reader.RowsPerStrip() == 0) { // Tile
    XTiffTileImage* tile_image = new XTiffTileImage;
    if (!tile_image->SetTiffReader(&reader)) {
      delete tile_image;
      m_File.Close();
      return false;
    }
    m_Image = tile_image;
  }
  else {  // Strip
    XTiffStripImage* strip_image = new XTiffStripImage;
    if (!strip_image->SetTiffReader(&reader)) {
      delete strip_image;
      m_File.Close();
      return false;
    }
    m_Image = strip_image;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Analyse d'une image BigTIFF
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeBigTiff()
{
  if (!m_File.Open(m_strFilename.c_str(), std::ios::in | std::ios::binary))
    return false;

  XBigTiffReader reader;
  if (!reader.Read(m_File.IStream())) {
    m_File.Close();
    return false;
  }

  if (!reader.AnalyzeIFD(m_File.IStream())) {
    m_File.Close();
    return false;
  }

  if (reader.RowsPerStrip() == 0) { // Tile
    XTiffTileImage* tile_image = new XTiffTileImage;
    if (!tile_image->SetTiffReader(&reader)) {
      delete tile_image;
      m_File.Close();
      return false;
    }
    m_Image = tile_image;
  }
  else {  // Strip
    XTiffStripImage* strip_image = new XTiffStripImage;
    if (!strip_image->SetTiffReader(&reader)) {
      delete strip_image;
      m_File.Close();
      return false;
    }
    m_Image = strip_image;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Analyse d'une image JPEG2000
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeJpeg2000()
{
#ifdef KAKADU_LIB
  XJpeg2000Image* image_kakadu = new XJpeg2000Image(m_strFilename.c_str());
  if (image_kakadu->IsValid() != true) {
    delete image_kakadu;
    return false;
  }
  m_Image = image_kakadu;
  return true;
#endif
#ifdef OPJ_STATIC
  XOpenJp2Image* image = new XOpenJp2Image(m_strFilename.c_str());
  if (image->IsValid() != true) {
    delete image;
    return false;
  }
  m_Image = image;
  return true;
#endif
  return false;
}

//-----------------------------------------------------------------------------
// Analyse d'une image COG
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeCog()
{
  if (!m_File.Open(m_strFilename.c_str(), std::ios::in | std::ios::binary))
    return false;
  XCogImage* image = new XCogImage;
  if (image == nullptr) {
    m_File.Close();
    return false;
  }
  try {
    if (!image->Open(&m_File)) {
      m_File.Close();
      delete image;
      return false;
    }
  }
  catch (const std::bad_alloc&) {
    m_File.Close();
  }
  m_Image = image;
  return true;
}

//-----------------------------------------------------------------------------
// Analyse d'une image WebP
//-----------------------------------------------------------------------------
bool XFileImage::AnalyzeWebP()
{
  XWebPImage* image = new XWebPImage(m_strFilename.c_str());
  if (!image->IsValid()) {
    delete image;
    return false;
  }
  m_Image = image;
  return true;
}

//-----------------------------------------------------------------------------
// Acces aux pixels
//-----------------------------------------------------------------------------
bool XFileImage::GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
  if (m_Image == nullptr)
    return false;
  if (m_Palette == nullptr)
    m_Image->SetChannelHints(m_RGBChannel);
  else
    m_Image->SetChannelHints(nullptr);

  if (m_Image->ColorMapSize() == 0) {
    if ((m_Image->NbBits() <= 8) && (m_Image->NbSample() == 1)) {  // Niveaux de gris
      return m_Image->GetArea(&m_File, x, y, w, h, area);
    }
    if ((m_Image->NbBits() == 8) && (m_Image->NbSample() == 3)) {  // RGB
      if (m_Image->GetArea(&m_File, x, y, w, h, area))
        return XBaseImage::MultiSample2RGB(area, w, h, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
      else
        return false;
    }
  }
  // Cas necessitant un passage en RGB
  uint8_t* val = m_Image->AllocArea(w, h);
  if (val == NULL)
    return false;
  if (!m_Image->GetArea(&m_File, x, y, w, h, val)) {
    delete[] val;
    return false;
  }

  bool flag = PostProcessRGB(area, val, w, h);
  delete[] val;
  return flag;
}

//-----------------------------------------------------------------------------
// Acces aux pixels avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XFileImage::GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  if (m_Image == nullptr)
    return false;
  if (m_Palette == nullptr)
    m_Image->SetChannelHints(m_RGBChannel);
  else
    m_Image->SetChannelHints(nullptr);

  if (m_Image->ColorMapSize() == 0) {
    if ((m_Image->NbBits() <= 8) && (m_Image->NbSample() == 1)) { // Niveaux de gris
      return m_Image->GetZoomArea(&m_File, x, y, w, h, area, factor);
    }
    if ((m_Image->NbBits() == 8) && (m_Image->NbSample() == 3)) { // RGB
      if (m_Image->GetZoomArea(&m_File, x, y, w, h, area, factor))
        return XBaseImage::MultiSample2RGB(area, w / factor, h / factor, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
      else
        return false;
    }
  }

  // Cas necessitant un passage en RGB
  uint32_t wout = w / factor, hout = h / factor;
  uint8_t* val = m_Image->AllocArea(wout, hout);
  if (val == NULL)
    return false;
  if (!m_Image->GetZoomArea(&m_File, x, y, w, h, val, factor)) {
    delete[] val;
    return false;
  }

  bool flag = PostProcessRGB(area, val, wout, hout, factor);
  delete[] val;
  return flag;
}

//-----------------------------------------------------------------------------
// Transforme les pixels en valeurs RGB
//-----------------------------------------------------------------------------
bool XFileImage::PostProcessRGB(uint8_t* area, uint8_t* val, uint32_t w, uint32_t h, uint32_t factor)
{
  if (m_Image->ColorMapSize() > 0) {  // Image palette
    m_Image->ApplyColorMap(val, area, w, h);
    return true;
  }
  /*
  if ((m_Image->NbBits() == 8) && (m_Image->NbSample() == 4)) {  // CYMK
    XBaseImage::CMYK2RGB(val, w, h);
    ::memcpy(area, val, w * h * 3L);
    return true;
  }*/

  if (m_Image->SampleFormat() == 1) { // Images non signees
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() == 1)) {  // Image 16 bits
      XBaseImage::Uint16To8bits(val, w, h);
      ::memcpy(area, val, w * h);
      return true;
    }
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() == 3)) {  // Image 3 x 16 bits
      XBaseImage::Uint16To8bits(val, w * 3L, h);
      ::memcpy(area, val, w * h * 3L);
      return true;
    }
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() > 3)) {  // Image N x 16 bits
      XBaseImage::Uint16To8bits(val, w * m_Image->NbSample(), h);
      XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
      ::memcpy(area, val, w * h * 3L);
      return true;
    }
  }

  if (m_Image->SampleFormat() == 2) { // Images signees
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() == 1)) {  // Image 16 bits
      XBaseImage::Int16To8bits(val, w, h);
      ::memcpy(area, val, w * h);
      return true;
    }
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() == 3)) {  // Image 3 x 16 bits
      XBaseImage::Int16To8bits(val, w * 3L, h);
      ::memcpy(area, val, w * h * 3L);
      return true;
    }
    if ((m_Image->NbBits() == 16) && (m_Image->NbSample() > 3)) {  // Image N x 16 bits
      XBaseImage::Int16To8bits(val, w * m_Image->NbSample(), h);
      XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
      ::memcpy(area, val, w * h * 3L);
      return true;
    }
  }

  if ((m_Image->NbSample() == 1) && (m_Image->NbBits() == 32)) { // MNT 32 bits
    //XDtmShader dtm(m_Image->GSD() * factor);
    //dtm.ConvertArea((float*)val, w, h, area);
    return true;
  }
  /*
  if ((m_Image->NbSample() > 3) && (m_Image->NbBits() == 8)) { // Images a N canaux, N > 3
    XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
    ::memcpy(area, val, w * h * 3L);
    return true;
  }*/
  if ((m_Image->NbSample() >= 3) && (m_Image->NbBits() == 8)) { // Images a N canaux, N > 3

    if (m_Palette == NULL) {
      XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), m_RGBChannel[0], m_RGBChannel[1], m_RGBChannel[2]);
      ::memcpy(area, val, w * h * 3L);
      return true;
    }
    XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), m_Palette);
    ::memcpy(area, val, w * h * 3L);
    return true;


    /*
    uint8_t palette[60] = { 255, 0, 0, 127, 255, 127, 127, 127, 127, 0, 127, 255, 220, 220, 200, 0, 0, 255, 255, 255, 255, 0, 255, 0, 50, 100, 0,
    0, 255, 200, 0, 120, 30, 125, 0, 127, 0, 255, 255, 60, 40, 20 };

    XBaseImage::MultiSample2RGB(val, w, h, m_Image->NbSample(), palette);
    ::memcpy(area, val, w * h * 3L);
    return true;*/
  }


  return false;
}

//-----------------------------------------------------------------------------
// Acces aux pixels en valeurs brutes avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XFileImage::GetRawArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix, uint32_t* nb_sample, uint32_t factor)
{
  if (m_Image == nullptr)
    return false;
  m_Image->SetChannelHints(nullptr);
  return m_Image->GetRawArea(&m_File, x, y, w, h, pix, nb_sample, factor);
}

bool XFileImage::GetRawPixel(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample)
{
  if (m_Image == nullptr)
    return false;
  m_Image->SetChannelHints(nullptr);
  return m_Image->GetRawPixel(&m_File, x, y, win, pix, nb_sample);
}

//-----------------------------------------------------------------------------
// Statistiques sur l'image
//-----------------------------------------------------------------------------
bool XFileImage::GetStat(double minVal[4], double maxVal[4], double meanVal[4], uint32_t noData[4], double no_data)
{
  if (m_Image == nullptr)
    return false;
  return m_Image->GetStat(&m_File, minVal, maxVal, meanVal, noData, no_data);
}

//-----------------------------------------------------------------------------
// Rotation de l'image
//-----------------------------------------------------------------------------
bool XFileImage::RotateArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t nbbyte, uint32_t rot) const
{
  return XBaseImage::RotateArea(in, out, win, hin, nbbyte, rot);
}

//-----------------------------------------------------------------------------
//	Fonction de re-echantillonnage
//-----------------------------------------------------------------------------
bool XFileImage::Resample(std::string file_out, XTransfo* transfo, XInterpol* interpol, XWait* wait)
{
  int lg = interpol->Win();
  double xf, yf, xi, yi, x, y, value;
  int ycur, ytmp, xcur, nb_canal, W, H;

  double* inter;
  double* val;
  uint8_t** buf;
  uint8_t* out;
  uint8_t white = 255;

  XTiffWriter tiff;

  if (m_Image->ColorMapSize() == 256 * 256 * 256) {	// Recherche du blanc pour les images avec palettes
    uint16_t* colormap = m_Image->ColorMap();
    for (int i = 0; i < 256; i++)
      if (colormap[i] == 255)
        if (colormap[i + 256] == 255)
          if (colormap[i + 512] == 255) {
            white = (uint8_t)i;
            break;
          }
    tiff.SetColorMap(colormap);
  }

  transfo->Dimension(Width(), Height(), &W, &H);
  nb_canal = NbByte();

  // Creation de l'entete de l'image en sortie
  double xmin, ymax, gsd;
  uint16_t espg;
  if (transfo->SetGeoref(&xmin, &ymax, &gsd, &espg))
    tiff.SetGeoTiff(xmin, ymax, gsd, espg);
  if (!tiff.Write(file_out.c_str(), W, H, NbByte(), NbBits()))
    return false;

  // Ecriture de la palette ...

  std::ofstream fic_out;
  fic_out.open(file_out.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);
  if (!fic_out.good())
    return false;
  fic_out.seekp(0, std::ios_base::end);

  // Allocation des tableaux
  typedef uint8_t* byteptr;
  buf = new byteptr[2 * lg];
  for (int i = 0; i < 2 * lg; i++)
    buf[i] = new uint8_t[Width() * nb_canal];
  out = new uint8_t[W * nb_canal];
  val = new double[2 * lg];
  inter = new double[2 * lg];

  // Gestion
  if (wait != nullptr)
    wait->SetRange(0, H);

  // Debut de l'iteration ligne par ligne de l'image finale
  for (int line = 0; line < H; line++) {
    xf = 0;
    yf = line;
    transfo->Direct(xf, yf, &xi, &yi);
    ycur = (int)yi;
    int j = 0;
    if ((ycur >= lg - 1) && (ycur < (int)Height() - lg)) {
      for (int i = ycur - lg + 1; i <= ycur + lg; i++)
        //GetLine(i, buf[j++]);
        GetArea(0, i, Width(), 1, buf[j++]);
    }

    for (int col = 0; col < W; col++) {
      xf = col;
      transfo->Direct(xf, yf, &xi, &yi);
      ytmp = (int)yi;
      if (ycur != ytmp) {
        ycur = ytmp;
        j = 0;
        if ((ycur >= lg - 1) && (ycur < (int)Height() - lg))
          for (int i = ycur - lg + 1; i <= ycur + lg; i++)
            //GetLine(i, buf[j++]);
            GetArea(0, i, Width(), 1, buf[j++]);
      }
      xcur = (int)xi;
      for (int canal = 0; canal < nb_canal; canal++) {
        // Gestion des pixels hors zone
        if ((xcur < lg - 1) || (xcur >= (int)Width() - lg) ||
          (ycur < lg - 1) || (ycur >= (int)Height() - lg)) {
          out[nb_canal * col + canal] = white;
          continue;
        }

        x = xi - xcur;
        y = yi - ycur;

        for (int k = 0; k < 2 * lg; k++) {
          j = 0;
          for (int i = xcur - lg + 1; i <= xcur + lg; i++)
            val[j++] = buf[k][i * nb_canal + canal];
          inter[k] = interpol->Compute(val, x);
        }

        value = interpol->Compute(inter, y);
        if ((value - floor(value) < 0.5))
          out[nb_canal * col + canal] = (uint8_t)floor(value);
        else
          out[nb_canal * col + canal] = (uint8_t)ceil(value);

        if (value < 0.1)
          out[nb_canal * col + canal] = 0;
        if (value > 254.9)
          out[nb_canal * col + canal] = 255;
      }
    }
    fic_out.write((char*)out, W * nb_canal * sizeof(uint8_t));
    if (wait != NULL) {
      wait->StepIt();
      if (wait->CheckCancel())
        break;
    }
  }
  // Liberation de la memoire allouee
  for (int i = 0; i < 2 * lg; i++)
    delete[] buf[i];
  delete[] buf;
  delete[] out;
  delete[] val;
  delete[] inter;

  fic_out.close();
  return true;
}

