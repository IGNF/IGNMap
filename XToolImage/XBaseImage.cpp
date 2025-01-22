//-----------------------------------------------------------------------------
//								XBaseImage.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 18/06/2021
//-----------------------------------------------------------------------------

#include <cstring>
#include <sstream>
#include <algorithm>
#include "XBaseImage.h"
#include "../XTool/XInterpol.h"

//-----------------------------------------------------------------------------
// Valeurs min, max et boost pour la conversion en 8 bits
//-----------------------------------------------------------------------------
double XBaseImage::MinValue = 0.;
double XBaseImage::MaxValue = 0.;
double XBaseImage::Boost_Hi = 0.;
double XBaseImage::Boost_Lo = 0.;

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XBaseImage::XBaseImage()
{
	m_nW = m_nH = 0;
	m_dX0 = m_dY0 = 0.;
  m_dGSD = 1.;
	m_nNbBits = m_nNbSample = 0;
  m_nSampleFormat = 1;
  m_ColorMap = NULL;
  m_nColorMapSize = 0;
  m_ChannelHints = NULL;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XBaseImage::~XBaseImage()
{
  if (m_ColorMap != NULL)
    delete[] m_ColorMap;
}

//-----------------------------------------------------------------------------
// Renvoie la taille d'un pixel en octet
//-----------------------------------------------------------------------------
uint32_t XBaseImage::PixSize()
{
	if ((m_nNbBits % 8) == 0) // Images 8 bits, 16 bits, ...
		return m_nNbSample * (m_nNbBits / 8);
	if (m_nNbBits == 1)	// Les images 1 bit sont renvoyees en 8 bits
		return m_nNbSample;
	return 0;		// On ne gere pas en standard les images 4 bits, 12 bits ...
}

//-----------------------------------------------------------------------------
// Metadonnees de l'image sous forme de cles / valeurs
//-----------------------------------------------------------------------------
std::string XBaseImage::Metadata()
{
	std::ostringstream out;
  out.setf(std::ios::fixed);
  out.precision(2);
	out << "Largeur:" << m_nW << ";Hauteur:" << m_nH << ";Nb Bits:" << m_nNbBits
		<< ";Nb Sample:" << m_nNbSample << ";Xmin:" << m_dX0 << ";Ymax:" << m_dY0
    << ";GSD:" << m_dGSD <<";";
	return out.str();
}

//-----------------------------------------------------------------------------
// Allocation d'une zone de pixel
//-----------------------------------------------------------------------------
uint8_t* XBaseImage::AllocArea(uint32_t w, uint32_t h)
{
	return new uint8_t[w * h * PixSize()];
}

//-----------------------------------------------------------------------------
// Recuperation des valeurs brutes des pixels sur une ROI
//-----------------------------------------------------------------------------
bool XBaseImage::GetRawArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix,
                            uint32_t* nb_sample, uint32_t factor, bool normalized)
{
  if (factor == 0)
    return false;
  uint32_t wout = w / factor, hout = h / factor;
  uint8_t* area = AllocArea(wout, hout);
  if (area == NULL)
    return false;
  if (!GetZoomArea(file, x, y, w, h, area, factor)) {
    delete[] area;
    return false;
  }
  *nb_sample = NbSample();
  float* pix_ptr = pix;

  if (SampleFormat() == 1) { // Donnees non signees
    if (NbBits() <= 8) {
      uint8_t* pix_area = area;
      for (uint32_t i = 0; i < wout * hout * NbSample(); i++) {
        *pix_ptr = *pix_area;
        if (normalized) *pix_ptr /= 255.;
        pix_ptr++;
        pix_area++;
      }
    }
    if (NbBits() == 16) {
      uint16_t* pix_area = (uint16_t*)area;
      for (uint32_t i = 0; i < wout * hout * NbSample(); i++) {
        *pix_ptr = *pix_area;
        if (normalized) *pix_ptr /= 65535.;
        pix_ptr++;
        pix_area++;
      }
    }
  }

  if (SampleFormat() == 2) { // Donnees signees
    if (NbBits() <= 8) {
      signed char* pix_area = (signed char*)area;
      for (uint32_t i = 0; i < wout * hout * NbSample(); i++) {
        *pix_ptr = *pix_area;
        if (normalized) *pix_ptr /= 128.;
        pix_ptr++;
        pix_area++;
      }
    }
    if (NbBits() == 16) {
      signed short* pix_area = (signed short*)area;
      for (uint32_t i = 0; i < wout * hout * NbSample(); i++) {
        *pix_ptr = *pix_area;
        if (normalized) *pix_ptr /= 32768.;
        pix_ptr++;
        pix_area++;
      }
    }
  }

  if (NbBits() == 32)
    ::memcpy(pix, area, wout * hout * NbSample() * sizeof(float));

  delete[] area;
  return true;
}

//-----------------------------------------------------------------------------
// Recuperation des valeurs brutes des pixels autour d'une position
//-----------------------------------------------------------------------------
bool XBaseImage::GetRawPixel(XFile* file, uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample)
{
  if ((win > x)||(win > y))
    return false;
  if ((x+win >= m_nW)||(y+win >= m_nH))
    return false;
  float* area = new float[(2*win+1)*(2*win+1)* NbSample()];
  if (area == NULL)
    return false;
  if (!GetRawArea(file, x - win, y - win, (2*win+1), (2*win+1),area, nb_sample)) {
    delete[] area;
    return false;
  }
  for (uint32_t i = 0; i < (2*win+1) * (2*win+1) * NbSample(); i++)
    pix[i] = area[i];
  delete[] area;
  return true;
}

//-----------------------------------------------------------------------------
// Calcul les statistiques de l'image
//-----------------------------------------------------------------------------
bool XBaseImage::GetStat(XFile* file, double *minVal, double *maxVal, double *meanVal, uint32_t *noData,
                         double no_data)
{
  uint32_t row = RowH();
  uint8_t* line = AllocArea(m_nW, row);
  if (line == NULL)
    return false;
  uint32_t nbRow = m_nH / row;
  if ((m_nH % row) != 0) nbRow++;
  for (uint32_t k = 0; k < NbSample(); k++) {
    noData[k] = 0;
    meanVal[k] = 0.;
    if (NbBits() <= 16)
      maxVal[k] = 0.;
    else
      maxVal[k] = -1.e31;
    if (NbBits() == 8) minVal[k] = 255.;
    if (NbBits() == 16) minVal[k] = 255. * 255.;
    if (NbBits() == 32) minVal[k] = 1.e31;
  }
  for (uint32_t i = 0; i < nbRow; i++) {
    uint32_t rowH = row;
    if ((i == (nbRow - 1)) && (i != 0)) // derniere bande
      rowH = (m_nH % (i * row));
    GetArea(file, 0, i*row, m_nW, rowH, line);
    if (NbBits() <= 8) {
      uint8_t* ptr = line;
      for (uint32_t j = 0; j < m_nW * rowH; j++) {
        for (uint32_t k = 0; k < NbSample(); k++) {
          minVal[k] = XMin(minVal[k], (double)*ptr);
          maxVal[k] = XMax(maxVal[k], (double)*ptr);
          meanVal[k] += (double)*ptr;
          ptr++;
        }
      }
      continue;
    }
    if (NbBits() == 16) {
      uint16_t* ptr = (uint16_t*)line;
      for (uint32_t j = 0; j < m_nW * rowH; j++) {
        for (uint32_t k = 0; k < NbSample(); k++) {
          minVal[k] = XMin(minVal[k], (double)*ptr);
          maxVal[k] = XMax(maxVal[k], (double)*ptr);
          meanVal[k] += (double)*ptr;
          ptr++;
        }
      }
      continue;
    }
    if (NbBits() == 32) {
      float* ptr = (float*)line;
      for (uint32_t j = 0; j < m_nW * rowH; j++) {
        for (uint32_t k = 0; k < NbSample(); k++) {
          if (*ptr > no_data) {
            minVal[k] = XMin(minVal[k], (double)*ptr);
            maxVal[k] = XMax(maxVal[k], (double)*ptr);
            meanVal[k] += (double)*ptr;
          }
          else
            noData[k] += 1;
          ptr++;
        }
      }
      continue;
    }
  }
  for (uint32_t k = 0; k < NbSample(); k++)
    meanVal[k] /= XMax((m_nW * m_nH - noData[k]), (uint32_t)1);
  delete[] line;
  return true;
}

//-----------------------------------------------------------------------------
// Application d'une palette de couleurs
//-----------------------------------------------------------------------------
bool XBaseImage::ApplyColorMap(uint8_t* in, uint8_t* out, uint32_t w, uint32_t h)
{
  if ((m_ColorMap == NULL)||(m_nColorMapSize < 3*256))
    return false;
  uint8_t *ptr_in = in, *ptr_out = out;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      ptr_out[0] = m_ColorMap[*ptr_in] / 256;
      ptr_out[1] = m_ColorMap[*ptr_in + 256] / 256;
      ptr_out[2] = m_ColorMap[*ptr_in + 512] / 256;
      ptr_out += 3;
      ptr_in++;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Conversion CMYK -> RGB
//-----------------------------------------------------------------------------
bool XBaseImage::CMYK2RGB(uint8_t* buffer, uint32_t w, uint32_t h)
{
  double C, M, Y, K;
  uint8_t *ptr_in = buffer, *ptr_out = buffer;
	for (uint32_t i = 0; i < h; i++) {
		for (uint32_t j = 0; j < w; j++) {
			C = *ptr_in / 255.; ptr_in++;
			M = *ptr_in / 255.; ptr_in++;
			Y = *ptr_in / 255.; ptr_in++;
			K = *ptr_in / 255.; ptr_in++;
			C = (C * (1. - K) + K);
			M = (M * (1. - K) + K);
			Y = (Y * (1. - K) + K);
			*ptr_out = (uint8_t)((1 - C)*255); ptr_out++;
			*ptr_out = (uint8_t)((1 - M)*255); ptr_out++;
			*ptr_out = (uint8_t)((1 - Y)*255); ptr_out++;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Conversion CMYK -> RGBA
//-----------------------------------------------------------------------------
bool XBaseImage::CMYK2RGBA(uint8_t* buffer, uint32_t w, uint32_t h)
{
  double C, M, Y, K;
  uint8_t* ptr_in = buffer, * ptr_out = buffer;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      C = *ptr_in / 255.; ptr_in++;
      M = *ptr_in / 255.; ptr_in++;
      Y = *ptr_in / 255.; ptr_in++;
      K = *ptr_in / 255.; ptr_in++;
      C = (C * (1. - K) + K);
      M = (M * (1. - K) + K);
      Y = (Y * (1. - K) + K);
      *ptr_out = (uint8_t)((1 - C) * 255); ptr_out++;
      *ptr_out = (uint8_t)((1 - M) * 255); ptr_out++;
      *ptr_out = (uint8_t)((1 - Y) * 255); ptr_out++;
      *ptr_out = 255; ptr_out++;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Conversion YCbCr -> RGB
//-----------------------------------------------------------------------------
bool XBaseImage::YCbCr2RGB(uint8_t* buffer, uint32_t w, uint32_t h)
{
  double R, G, B, Y, Cb, Cr;
  uint8_t* ptr_in = buffer, * ptr_out = buffer;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      Y = *ptr_in; ptr_in++;
      Cb = *ptr_in; ptr_in++;
      Cr = *ptr_in; ptr_in++;
      R = XMin(XMax(Y + 1.402 * (Cr - 128.), 0.), 255.);
      G = XMin(XMax(Y - 0.34414 * (Cb - 128.) - 0.71414 * (Cr - 128.), 0.), 255.);
      B = XMin(XMax(Y + 1.772 * (Cb - 128.), 0.), 255.);
      *ptr_out = (uint8_t)R; ptr_out++;
      *ptr_out = (uint8_t)G; ptr_out++;
      *ptr_out = (uint8_t)B; ptr_out++;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Conversion N Samples -> RGB
//-----------------------------------------------------------------------------
bool XBaseImage::MultiSample2RGB(uint8_t* buffer, uint32_t w, uint32_t h, uint16_t nbSample, uint16_t idxR, uint16_t idxG, uint16_t idxB)
{
  if ((idxR >= nbSample)||(idxG >= nbSample)||(idxB >= nbSample)||(nbSample < 3))
    return false;
  if ((nbSample == 3)&&(idxR == 0)&&(idxG == 1)&&(idxB == 2))
    return true;
  uint8_t *ptr_in = buffer, *ptr_out = buffer, r, g, b;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      r = ptr_in[idxR]; g = ptr_in[idxG]; b = ptr_in[idxB];
      *ptr_out = r; ptr_out++;
      *ptr_out = g; ptr_out++;
      *ptr_out = b; ptr_out++;
      ptr_in += nbSample;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Conversion N Samples -> RGB via une palette
//-----------------------------------------------------------------------------
bool XBaseImage::MultiSample2RGB(uint8_t* buffer, uint32_t w, uint32_t h, uint16_t nbSample, uint8_t* paletteRGB)
{
  if (nbSample < 3)
    return false;
  uint8_t* ptr_in = buffer, * ptr_out = buffer;
  double r, g, b;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      r = g = b = 0.;
      for (uint32_t k = 0; k < nbSample; k++) {
        r += ((double)ptr_in[k] * (double)paletteRGB[k * 3]) / 255.;
        g += ((double)ptr_in[k] * (double)paletteRGB[k * 3 + 1]) / 255.;
        b += ((double)ptr_in[k] * (double)paletteRGB[k * 3 + 2]) / 255.;
      }
      *ptr_out = r; ptr_out++;
      *ptr_out = g; ptr_out++;
      *ptr_out = b; ptr_out++;
      ptr_in += nbSample;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Conversion 16bits -> 8bits non signes
//-----------------------------------------------------------------------------
bool XBaseImage::Uint16To8bits(uint8_t* buffer, uint32_t w, uint32_t h)
{
	// Recherche du min / max
  uint16_t val_min = (uint16_t)MinValue, val_max = (uint16_t)MaxValue;
  double val_moy = 0.;
  if ((val_min == 0) && (val_max == 0)) {
		val_min = 0xFFFF;
		val_max = 0;
		uint16_t* ptr = (uint16_t*)buffer;
		for (uint32_t i = 0; i < w * h; i++) {
			val_min = XMin(*ptr, val_min);
			val_max = XMax(*ptr, val_max);
      val_moy += (*ptr);
			ptr++;
		}
    val_moy /= (w * h);
	}

  if (Boost_Hi > 0.)  // Application du boost
    val_max = (uint16_t)XMin(val_max - (val_max - val_moy) * Boost_Hi, 65535.);
  if (Boost_Lo > 0.)
    val_min = (uint16_t)XMax(val_min + (val_moy - val_min) * Boost_Lo, 0.);
  if ((val_max - val_min) == 0)
    return true;

  // Application de la transformation
	uint16_t* ptr_val = (uint16_t*)buffer;
	uint8_t* ptr_buf = buffer;
	for (uint32_t i = 0; i < w * h; i++) {
    if (*ptr_val < val_min) {
      *ptr_buf = 0;
    } else {
      if (*ptr_val > val_max)
        *ptr_buf = 255;
      else
        *ptr_buf = (*ptr_val - val_min) * 255 / (val_max - val_min);
    }
    ptr_buf++;
		ptr_val++;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Conversion 16bits -> 8bits signes
//-----------------------------------------------------------------------------
bool XBaseImage::Int16To8bits(uint8_t* buffer, uint32_t w, uint32_t h)
{
  // Recherche du min / max
  signed short val_min = (signed short)MinValue, val_max = (signed short)MaxValue;
  double val_moy = 0.;
  if ((val_min == 0) && (val_max == 0)) {
    val_min = 32767;
    val_max = -32767;
    signed short* ptr = (signed short*)buffer;
    for (uint32_t i = 0; i < w * h; i++) {
      val_min = XMin(*ptr, val_min);
      val_max = XMax(*ptr, val_max);
      val_moy += (*ptr);
      ptr++;
    }
    val_moy /= (w * h);
  }
  if (Boost_Hi > 0.)  // Application du boost
    val_max = (uint16_t)XMin(val_max - (val_max - val_moy) * Boost_Hi, 32767.);
  if (Boost_Lo > 0.)
    val_min = (uint16_t)XMax(val_min + (val_moy - val_min) * Boost_Lo, -32767.);
  if ((val_max - val_min) == 0)
    return true;
  // Application de la transformation
  signed short* ptr_val = (signed short*)buffer;
  uint8_t* ptr_buf = buffer;
  for (uint32_t i = 0; i < w * h; i++) {
    if (*ptr_val < val_min) {
      *ptr_buf = 0;
    } else {
      if (*ptr_val > val_max)
        *ptr_buf = 255;
      else
        *ptr_buf = (*ptr_val - val_min) * 255 / (val_max - val_min);
    }
    ptr_buf++;
    ptr_val++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Extraction d'une zone de pixels
//-----------------------------------------------------------------------------
bool XBaseImage::ExtractArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout,
                             uint32_t x0, uint32_t y0)
{
  if (x0 + wout > win) return false;
  if (y0 + hout > hin) return false;

  uint8_t* buf_in = &in[y0 * win + x0];
  uint8_t* buf_out= out;
  for (uint32_t i = 0; i < hout; i++) {
    ::memcpy(buf_out, buf_in, wout);
    buf_in += win;
    buf_out+= wout;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Copie d'une zone de pixels au sein d'une image
//-----------------------------------------------------------------------------
bool XBaseImage::CopyArea(uint8_t* patch, uint8_t* image, uint32_t wpatch, uint32_t hpatch, uint32_t wimage, uint32_t himage,
                          uint32_t x0, uint32_t y0)
{
  if (x0 + wpatch > wimage) return false;
  if (y0 + hpatch > himage) return false;
  for (uint32_t i = 0; i < hpatch; i++)
    ::memcpy(&image[(y0 + i) * wimage + x0], &patch[i * wpatch], wpatch);
  return true;
}

//-----------------------------------------------------------------------------
// Zoom sur un buffer de pixels
//-----------------------------------------------------------------------------
bool XBaseImage::ZoomArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout, uint32_t nbbyte)
{
	uint32_t* lut = new uint32_t[wout];
	for (uint32_t i = 0; i < wout; i++)
		lut[i] = nbbyte * (i * win / wout);

	for (uint32_t i = 0; i < hout; i++) {
		uint8_t* line = &out[i * (nbbyte * wout)];
		uint8_t* src = &in[nbbyte * win * (i * hin / hout)];
		for (uint32_t k = 0; k < wout; k++) {
      ::memcpy(line, &src[lut[k]], nbbyte);
			line += nbbyte;
		}
	}

	delete[] lut;
	return true;
}

//-----------------------------------------------------------------------------
// Zoom sur un buffer de pixels a partir de 3 buffers R, G, B
//-----------------------------------------------------------------------------
bool XBaseImage::ZoomAreaRGB(uint8_t* R, uint8_t* G, uint8_t* B, uint8_t* out, uint32_t win, uint32_t hin, uint32_t wout, uint32_t hout)
{
  uint32_t* lut = new uint32_t[wout];
  for (uint32_t i = 0; i < wout; i++)
    lut[i] = 3 * (i * win / wout);

  uint8_t* ptr = out;
  for (uint32_t i = 0; i < hout; i++) {
    uint32_t pos = win * (i * hin / hout);
    uint8_t* srcR = &R[pos];
    uint8_t* srcG = &G[pos];
    uint8_t* srcB = &B[pos];
    for (uint32_t k = 0; k < wout; k++) {
      *ptr = srcR[lut[k]]; ptr++;
      *ptr = srcG[lut[k]]; ptr++;
      *ptr = srcB[lut[k]]; ptr++;
    }
  }
  delete[] lut;
  return true;
}

//-----------------------------------------------------------------------------
// Inversion de triplet RGB en triplet BGR : Windows travaille en BGR
//-----------------------------------------------------------------------------
void XBaseImage::SwitchRGB2BGR(uint8_t* buf, uint32_t nb_pix)
{
	uint8_t r;
	for (uint32_t i = 0; i < nb_pix * 3; i += 3) {
		r = buf[i + 2];
		buf[i + 2] = buf[i];
		buf[i] = r;
	}
}

//-----------------------------------------------------------------------------
// Inversion de triplet ARGB en triplet BGR : Windows travaille en BGR
//-----------------------------------------------------------------------------
void XBaseImage::SwitchARGB2BGR(uint8_t* buf, uint32_t nb_pix)
{
	uint8_t *ptr_bgr = buf, *ptr_argb = buf;
	for (uint32_t i = 0; i < nb_pix; i++) {
		ptr_argb[3] = ptr_argb[0]; // pour sauvegarder la valeur
		ptr_bgr[0] = ptr_argb[2];
		ptr_bgr[1] = ptr_argb[1];
		ptr_bgr[2] = ptr_argb[3];
		ptr_bgr+=3;
		ptr_argb+=4;
	}
}

//-----------------------------------------------------------------------------
// Conversion niveau de gris -> RGB
//-----------------------------------------------------------------------------
void XBaseImage::Gray2RGB(uint8_t* buf, uint32_t nb_pix)
{
  uint8_t* ptr = &buf[nb_pix-1];
  for (uint32_t i = 1; i <= nb_pix; i++) {
    memset(&buf[3 * nb_pix - i * 3], *ptr, 3);
    ptr--;
  }
}

//-----------------------------------------------------------------------------
// Conversion RGB -> RGBA
//-----------------------------------------------------------------------------
void XBaseImage::RGB2RGBA(uint8_t* buf, uint32_t nb_pix, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
  uint8_t* ptr_alpha = &buf[nb_pix * 4 - 4];
  uint8_t* ptr_rgb = &buf[nb_pix * 3 - 3];
  if (alpha == 255) { // Pas de couleur transparente
    for (uint32_t i = 0; i < nb_pix; i++) {
      memcpy(ptr_alpha, ptr_rgb, 3);
      ptr_alpha[3] = 255;
      ptr_alpha -= 4;
      ptr_rgb -= 3;
    }
    return;
  }
  // Couleur transparente
  uint8_t trans[3];
  trans[0] = r; trans[1] = g; trans[2] = b;
  for (uint32_t i = 0; i < nb_pix; i++) {
    memcpy(ptr_alpha, ptr_rgb, 3);
    if (memcmp(trans, ptr_rgb, 3) == 0)
      ptr_alpha[3] = alpha;
    else
      ptr_alpha[3] = 255;
    ptr_alpha -= 4;
    ptr_rgb -= 3;
  }
}

//-----------------------------------------------------------------------------
// Conversion RGB -> BGRA
//-----------------------------------------------------------------------------
void XBaseImage::RGB2BGRA(uint8_t* buf, uint32_t nb_pix, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
  uint8_t* ptr_alpha = &buf[nb_pix * 4 - 4];
  uint8_t* ptr_rgb = &buf[nb_pix * 3 - 3];
  if (alpha == 255) { // Pas de couleur transparente
    for (uint32_t i = 0; i < nb_pix; i++) {
      ptr_alpha[0] = ptr_rgb[2];
      ptr_alpha[1] = ptr_rgb[1];
      ptr_alpha[2] = ptr_rgb[0];
      ptr_alpha[3] = 255;
      ptr_alpha -= 4;
      ptr_rgb -= 3;
    }
    return;
  }
  // Couleur transparente
  uint8_t trans[3];
  trans[0] = r; trans[1] = g; trans[2] = b;
  for (uint32_t i = 0; i < nb_pix; i++) {
    ptr_alpha[0] = ptr_rgb[2];
    ptr_alpha[1] = ptr_rgb[1];
    ptr_alpha[2] = ptr_rgb[0];
    if (memcmp(trans, ptr_rgb, 3) == 0)
      ptr_alpha[3] = alpha;
    else
      ptr_alpha[3] = 255;
    ptr_alpha -= 4;
    ptr_rgb -= 3;
  }
}

//-----------------------------------------------------------------------------
// Conversion niveau de gris -> RGB
//-----------------------------------------------------------------------------
void XBaseImage::Gray2RGBA(uint8_t* buf, uint32_t nb_pix, uint8_t gray, uint8_t alpha)
{
  uint8_t* ptr = &buf[nb_pix - 1];
  uint8_t* ptr_rgba = &buf[nb_pix * 4 - 4];
  if (alpha == 255) { // Pas de couleur transparente
    for (uint32_t i = 0; i < nb_pix; i++) {
      memset(ptr_rgba, *ptr, 3);
      ptr_rgba[3] = 255;
      ptr--;
      ptr_rgba -= 4;
    }
    return;
  }
  // Couleur transparente
  for (uint32_t i = 0; i < nb_pix; i++) {
    memset(ptr_rgba, *ptr, 3);
    if (*ptr == gray)
      ptr_rgba[3] = alpha;
    else
      ptr_rgba[3] = 255;
    ptr--;
    ptr_rgba -= 4;
  }
}

//-----------------------------------------------------------------------------
// Ajout d'un offset pour que le buffer ait une largeur de lineW octets
//-----------------------------------------------------------------------------
void XBaseImage::OffsetArea(uint8_t* buf, uint32_t w, uint32_t h, uint32_t lineW)
{
  if (lineW == w)
    return;
  for (uint32_t i = h - 1; i > 0; i--) {
    uint8_t* src = &buf[i * w];
    uint8_t* dst = &buf[i * lineW];
    memmove(dst, src, w);
  }
}

//-----------------------------------------------------------------------------
// Rotation d'une zone de pixels : 0->0° , 1->90°, 2->180°, 3->270°
//-----------------------------------------------------------------------------
bool XBaseImage::RotateArea(uint8_t* in, uint8_t* out, uint32_t win, uint32_t hin, uint32_t nbbyte, uint32_t rot)
{
  if (rot == 0) {		// Pas de rotation
    ::memcpy(out, in, win * hin * nbbyte);
    return true;
  }
	if (rot > 3)
		return false;

	uint8_t* forw;
	uint8_t* back;
	uint32_t lineW = win * nbbyte;

	if (rot == 2) {	// Rotation a 180 degres
		for (uint32_t i = 0; i < hin; i++) {
			forw = &in[i * lineW];
			back = &out[(hin - 1 - i) * lineW + (win - 1) * nbbyte];
			for (uint32_t j = 0; j < win; j++) {
        ::memcpy(back, forw, nbbyte);
				forw += nbbyte;
				back -= nbbyte;
			}
		}
		return true;
	}

	if (rot == 1) { // Rotation a 90 degres
		for (uint32_t i = 0; i < win; i++) {
			forw = &in[i * nbbyte];
			back = &out[(win - 1 - i) * hin * nbbyte];
			for (uint32_t j = 0; j < hin; j++) {
        ::memcpy(back, forw, nbbyte);
				back += nbbyte;
				forw += lineW;
			}
		}
		return true;
	}

	if (rot == 3) { // Rotation a 270 degres
		for (uint32_t i = 0; i < win; i++) {
			forw = &in[i * nbbyte];
			back = &out[i * hin * nbbyte + nbbyte * (hin - 1)];
			for (uint32_t j = 0; j < hin; j++) {
        ::memcpy(back, forw, nbbyte);
				forw += lineW;
				back -= nbbyte;
			}
		}
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Normalisation d'une zone de pixels et recuperation des statistiques
//-----------------------------------------------------------------------------
void XBaseImage::Normalize(uint8_t* pix_in, double* pix_out, uint32_t nb_pixel, double* mean, double* std_dev)
{
  *mean = 0.0;
  *std_dev = 0.0;
  for (uint32_t i = 0;  i < nb_pixel; i++)
    (*mean) += (double)pix_in[i];
  (*mean) /= (double)nb_pixel;
  for (uint32_t i = 0;  i < nb_pixel; i++) {
    pix_out[i] = (double)pix_in[i] - (*mean);
    (*std_dev) += (pix_out[i] * pix_out[i]);
  }
  (*std_dev) /= (double)nb_pixel;
}

//-----------------------------------------------------------------------------
// Covariance entre deux zones de pixels
//-----------------------------------------------------------------------------
double XBaseImage::Covariance(double* pix1, double* pix2, uint32_t nb_pixel)
{
  double cov = 0.0;
  for (uint32_t i = 0; i < nb_pixel; i++)
    cov += (pix1[i] * pix2[i]);
  return (cov / (double)nb_pixel);
}

//-----------------------------------------------------------------------------
// Fonction de correlation
//-----------------------------------------------------------------------------
bool XBaseImage::Correlation(uint8_t* pix1, uint32_t w1, uint32_t h1,
                             uint8_t* pix2, uint32_t w2, uint32_t h2,
                             double* u, double* v, double* pic)
{
  double *win1, *win2;	// Fenetres de correlation
  double *correl;				// Valeurs de correlation
  double mean1, dev1, mean2, dev2, cov, a, b;
  uint32_t k = 0, n = 0, lin = 0, col = 0;
  uint8_t *buf;

  win1 = new double[w1 * h1];
  win2 = new double[w1 * h1];
  correl = new double[(h2 - h1 + 1)*(w2 - w1 + 1)];
  buf = new uint8_t[w1 * h1];
  if ((win1 == NULL)||(win2 == NULL)||(correl == NULL)||(buf == NULL)){
    delete[] win1; delete[] win2; delete[] correl; delete[] buf;
    return false;
  }
  Normalize(pix1, win1, w1 * h1, &mean1, &dev1);
  if (dev1 == 0.0) {
    delete[] win1; delete[] win2; delete[] correl; delete[] buf;
    return false;
  }

  *pic = 0.0;
  for (uint32_t i = 0; i < h2 - h1; i++)
    for (uint32_t j = 0; j < w2 - w1; j++) {
      ExtractArea(pix2, buf, w2, h2, w1, h1, j, i);
      Normalize(buf, win2,  w1 * h1, &mean2, &dev2);
      if (dev2 == 0.0)
        continue;
      cov = Covariance(win1, win2, w1 * h1);
      correl[n] = cov / sqrt(dev1 * dev2);
      if (correl[n] > *pic) {
        k = n;
        *pic = correl[n];
        lin = i;
        col = j;
      }
      n++;
    }

  delete[] win1;
  delete[] win2;
  delete[] buf;

  if ((k == 0)||(k == ((h2 - h1 + 1)*(w2 - w1 + 1)-1))) {
    delete[] correl;
    return false;
  }

  a = correl[k-1] - *pic;
  b = correl[k+1] - *pic;
  a = (a - b) / (2.0 * (a + b));
  *u = (double)col + a + (double)w1 * 0.5;
  uint32_t wK = w2 - w1 + 1;
  a = correl[k-wK] - *pic;
  b = correl[k+wK] - *pic;
  a = (a - b) / (2.0 * (a + b));
  *v = (double)lin + a + (double)h1 * 0.5;

  delete[] correl;
  return true;
}

//-----------------------------------------------------------------------------
// Fonction de zoom rapide bilineaire
//-----------------------------------------------------------------------------
bool XBaseImage::FastZoomBil(float* in, uint32_t win, uint32_t hin, float* out, uint32_t wout, uint32_t hout)
{
  double x_factor = (double)win / (double)wout;
  double y_factor = (double)hin / (double)hout;
  double val[4];
  XInterLin interpol;

  for (uint32_t y = 0; y < hout; y++) {
    double v = y * y_factor;
    if (ceil(v) >= hin) v = hin - 1;
    for (uint32_t x = 0; x < wout; x++) {
      double u = x * x_factor;
      if (ceil(u) >= win) u = win - 1;

      val[0] = in[(int)(floor(v) * win + floor(u))];
      val[1] = in[(int)(floor(v) * win + ceil(u))];
      val[2] = in[(int)(ceil(v) * win + floor(u))];
      val[3] = in[(int)(ceil(v) * win + ceil(u))];

      out[y * wout + x] = (float)interpol.BiCompute(val, u - floor(u), v - floor(v));
    }

  }
  return true;
}
