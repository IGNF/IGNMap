//-----------------------------------------------------------------------------
//								XGeoRaster.cpp
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 20/11/2003
//-----------------------------------------------------------------------------

#include <cstring>
#include "XGeoRaster.h"

//-----------------------------------------------------------------------------
// Constructeur pour les XGeoRasterContext
//-----------------------------------------------------------------------------
XGeoRasterContext::XGeoRasterContext()
{
	m_bActif = false;
	m_nNbChannel = 0;
	m_Gamma = NULL;			// Tableau de gamma pour les canaux
	m_Min = NULL;				// Tableau des niveaux minimums pour les canaux
	m_Max = NULL;				// Tableau des niveaux maximums pour les canaux
	m_Lut = NULL;				// Tableau des LUT pour les canaux
  m_Palette = NULL;
  m_bMedian = false;
  m_nWinMed = 10;
  m_nCenMed = 50;
  m_bImageAdd = false;
  m_bMatrix = false;
  m_bNormGradient = false;
  m_bDirGradient = false;
  for(int i = 0; i < 10; i++)
    m_Matrix[i] = 1;
  m_RGBChannel[0] = 0; m_RGBChannel[1] = 1; m_RGBChannel[2] = 2;
}

//-----------------------------------------------------------------------------
// Destructeur pour les XGeoRasterContext
//-----------------------------------------------------------------------------
XGeoRasterContext::~XGeoRasterContext()
{
	Clear();
}

//-----------------------------------------------------------------------------
// Reinitialise le context
//-----------------------------------------------------------------------------
void XGeoRasterContext::Clear()
{
	if (m_Gamma != NULL)
		delete[] m_Gamma;
	if (m_Min != NULL)
		delete[] m_Min;
	if (m_Max != NULL)
		delete[] m_Max;
	if (m_Lut != NULL) {
		for (uint16_t i = 0; i < m_nNbChannel; i++)
			if (m_Lut[i] != NULL)
				delete[] m_Lut[i];
		delete[] m_Lut;
	}
  if (m_Palette != NULL)
    delete[] m_Palette;
	m_nNbChannel = 0;
	m_Gamma = NULL;
	m_Min = m_Max = NULL;
	m_Lut = NULL;
	m_bActif = false;
  m_bImageAdd = false;
  m_bMedian = false;
  m_nWinMed = 10;
  m_nCenMed = 50;
  m_bNormGradient = false;
  m_bDirGradient = false;
  m_RGBChannel[0] = 0; m_RGBChannel[1] = 1; m_RGBChannel[2] = 2;
}

//-----------------------------------------------------------------------------
// Clone le premier canal uniquement
//-----------------------------------------------------------------------------
void XGeoRasterContext::Clone(XGeoRasterContext* clone)
{
  if (clone == NULL)
    return;
  m_Gamma[0] = clone->Gamma(0);
  m_Min[0] = clone->Min(0);
  m_Max[0] = clone->Max(0);
  m_bMedian = clone->Median();
  m_nWinMed = clone->WinMedian();
  m_nCenMed = clone->CenMedian();
  m_bMatrix = clone->Matrix();
  m_bNormGradient = clone->NormGradient();
  m_bDirGradient = clone->DirGradient();

  ::memcpy(m_Matrix , clone->m_Matrix, 10 * sizeof(int));
  ::memcpy( m_Lut[0], clone->m_Lut[0], 256 * sizeof(uint8_t));
}

//-----------------------------------------------------------------------------
// Fixe le nombre de canaux geres par le context
//-----------------------------------------------------------------------------
bool XGeoRasterContext::NbChannel(uint16_t nb)
{
	Clear();
	if (nb == 0) 
		return true;

	m_nNbChannel = nb;
	m_Gamma = new double[nb];
	m_Min = new uint8_t[nb];
	m_Max = new uint8_t[nb];
	m_Lut = new uint8_t*[nb];
	if ((m_Gamma == NULL)||(m_Min == NULL)||(m_Max == NULL)||(m_Lut == NULL)) {
		Clear();
		return false;
	}
  uint16_t i;
  for (i = 0; i < nb; i++) {
		m_Gamma[i] = 1.0;
		m_Min[i] = 0;
		m_Max[i] = 255;
		m_Lut[i] = NULL;
	}
	for (i = 0; i < nb; i++) {
		m_Lut[i] = new uint8_t[256];      
		if (m_Lut[i] == NULL) {
			Clear();
			return false;
		}
    for(uint16_t j = 0; j < 256; j++)
      m_Lut[i][j] = (uint8_t)j;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Fixe les valeurs d'un canal
//-----------------------------------------------------------------------------
bool XGeoRasterContext::Channel(uint16_t nb, double gamma, uint8_t min, uint8_t max)
{
	if (nb >= m_nNbChannel)
		return false;
	m_Gamma[nb] = gamma;
	m_Min[nb] = min;
	m_Max[nb] = max;
	if (max == 0) {
		for (uint16_t i = 0; i < 256; i++)
			m_Lut[nb][i] = 0;
		return true;
	}
	if (min == 255) {
		for (uint16_t i = 0; i < 256; i++)
			m_Lut[nb][i] = 255;
		return true;
	}
	if (min >= max) {
		for (uint16_t i = 0; i < 256; i++)
			m_Lut[nb][i] = 0;
		return true;
	}	
  uint16_t i;
  for (i = 0; i < min; i++)
		m_Lut[nb][i] = 0;
	for (i = min; i < max; i++)
		m_Lut[nb][i] = (uint8_t)(pow((double)(i - min) / (double)(max - min), gamma) * 255.0);
	for (i = max; i < 256; i++)
		m_Lut[nb][i] = 255;

	return true;
}

//-----------------------------------------------------------------------------
// Renvoi les valeurs d'un canal
//-----------------------------------------------------------------------------
bool XGeoRasterContext::Channel(uint16_t nb, double* gamma, uint8_t* min, uint8_t* max, uint8_t** lut)
{
	if (nb >= m_nNbChannel)
		return false;
	*gamma = m_Gamma[nb];
	*min = m_Min[nb];
	*max = m_Max[nb];
	*lut = m_Lut[nb];
	return true;
}

//-----------------------------------------------------------------------------
// Test s'il est utile d'appliquer la LUT
//-----------------------------------------------------------------------------
bool XGeoRasterContext::NeedLut()
{
  for (uint16_t i = 0; i < m_nNbChannel; i++) {
    if (m_Min[i] != 0) return true;
    if (m_Max[i] != 255) return true;
    if (m_Gamma[i] != 1.0) return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
// Renvoie la LUT associee a un canal
//-----------------------------------------------------------------------------
uint8_t* XGeoRasterContext::Lut(uint16_t nb)
{
	if (nb >= m_nNbChannel)
		return NULL;
	return m_Lut[nb];
}

//-----------------------------------------------------------------------------
// Fixe la palette
//-----------------------------------------------------------------------------
void XGeoRasterContext::Palette(uint8_t* pal)
{
  if (pal == NULL) {  // On detruit la palette existante s'il y en a une
    if (m_Palette != NULL)
      delete[] m_Palette;
    m_Palette = NULL;
    return;
  }
  if (m_Palette == NULL) {
    m_Palette = new uint8_t[256 * 3];
    if (m_Palette == NULL) return;
  }
  ::memcpy(m_Palette, pal, 256 * 3 * sizeof(uint8_t));
}

//-----------------------------------------------------------------------------
// Constructeur pour les XGeoRasterHisto
//-----------------------------------------------------------------------------
XGeoRasterHisto::XGeoRasterHisto()
{
	m_nNbChannel = 0;
	m_H = NULL;					// Tableau des histogrammes pour les canaux
}

//-----------------------------------------------------------------------------
// Destructeur pour les XGeoRasterHisto
//-----------------------------------------------------------------------------
XGeoRasterHisto::~XGeoRasterHisto()
{
	Clear();
}

//-----------------------------------------------------------------------------
// Reinitialise l'objet XGeoRasterHisto
//-----------------------------------------------------------------------------
void XGeoRasterHisto::Clear()
{
	if (m_H != NULL) {
		for (uint16_t i = 0; i < m_nNbChannel; i++)
			if (m_H[i] != NULL)
				delete[] m_H[i];
		delete[] m_H;
	}
	m_H = NULL;
}

//-----------------------------------------------------------------------------
// Fixe le nombre de canaux geres par le XGeoRasterHisto
//-----------------------------------------------------------------------------
bool XGeoRasterHisto::NbChannel(uint16_t nb)
{
	Clear();
	if (nb == 0) 
		return true;

	m_nNbChannel = nb;
	m_H = new double*[nb];
	if (m_H == NULL) {
		Clear();
		return false;
	}
  uint16_t i;
  for (i = 0; i < nb; i++)
		m_H[i] = NULL;

	for (i = 0; i < nb; i++) {
		m_H[i] = new double[256];
		if (m_H[i] == NULL) {
			Clear();
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Fixe l'histogramme d'un canal
//-----------------------------------------------------------------------------
bool XGeoRasterHisto::Channel(uint16_t nb, double* H)
{
	if (nb >= m_nNbChannel)
		return false;
	::memcpy(m_H[nb], H, 256 * sizeof(double));
	return true;
}

//-----------------------------------------------------------------------------
// Analyse d'un histogramme
//-----------------------------------------------------------------------------
bool XGeoRasterHisto::Analyze(uint16_t nb, uint16_t& min, uint16_t& max, uint16_t& nblevel,
															uint16_t& nbhole, uint16_t& holemax, double& satur, double holesize)
{
	if (nb >= m_nNbChannel)
		return false;

	double* H = m_H[nb];

	// Niveau minimum et niveau maximum
	double nbpix = 0.;	// Nombre de pixels de l'image
	min = 255; max = 0;
  uint16_t i;
  for (i = 0; i < 256; i++) {
		if ((H[i] > 0.0)&&(min == 255))
			min = i;
		if (H[i] > 0.0)
			max = i;
		nbpix += H[i];
	}
	if (min > max)
		return false;

	// Trous et nombre de niveaux utilises
	nblevel = 0;
	uint16_t hole = 0;
	holemax = nbhole = 0;
	for (i = min; i <= max; i++) {
		if (H[i] > holesize * nbpix) {
			nblevel++;
			holemax = XMax(holemax, hole);
			hole = 0;
		} else {
			hole++;
			nbhole++;
		}
	}

	// Saturation en debut ou en fin d'histogramme
	satur = XMax(H[min] / nbpix, H[max] / nbpix);

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'un histogramme dans un flux
//-----------------------------------------------------------------------------
bool XGeoRasterHisto::Write(uint16_t nb, std::ostream& out)
{
	if (nb >= m_nNbChannel)
		return false;

	double* H = m_H[nb];
	for (uint32_t i = 0; i < 256; i++)
		out << XRint(H[i] * 1000.) << "\t";

	return out.good();
}

//-----------------------------------------------------------------------------
// Points du cadre de l'image
//-----------------------------------------------------------------------------
XPt2D XGeoRaster::Pt(uint32_t i)
{
	switch(i) {
		case 0 : return XPt2D(m_Frame.Xmin, m_Frame.Ymax); 
		case 1 : return XPt2D(m_Frame.Xmax, m_Frame.Ymax); 
		case 2 : return XPt2D(m_Frame.Xmax, m_Frame.Ymin); 
		case 3 : return XPt2D(m_Frame.Xmin, m_Frame.Ymin); 
		case 4 : return XPt2D(m_Frame.Xmin, m_Frame.Ymax); 
	}
	return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Fixe un gamma general a l'image
//-----------------------------------------------------------------------------
bool XGeoRaster::Gamma(double gamma)
{
	if (!CreateContext())
		return false;

	double gam;
  uint8_t min = 0, max = 0;
	uint8_t* LUT;
	for (uint16_t num = 0; num < m_Context->NbChannel(); num++) {
		m_Context->Channel(num, &gam, &min, &max, &LUT);
		m_Context->Channel(num, gamma, min, max);
	}
	m_Context->Actif(true);
	return true;
}

//-----------------------------------------------------------------------------
// Analyse des histogrammes
//-----------------------------------------------------------------------------
bool XGeoRaster::AnalyzeHisto(uint16_t nb, uint16_t& min, uint16_t& max, uint16_t& nblevel, 
															uint16_t& nbhole, uint16_t& holemax, double& satur, double holesize)
{ 
	if (m_Histo == NULL) {
		min = max = nblevel = nbhole = holemax = 0;
		return false;
	}
	return m_Histo->Analyze(nb, min, max, nblevel, nbhole, holemax, satur, holesize);
}

//-----------------------------------------------------------------------------
// Ecriture d'un histogramme dans un flux
//-----------------------------------------------------------------------------
bool XGeoRaster::WriteHisto(uint16_t nb, std::ostream& out)
{
	if (m_Histo == NULL)
		return false;

	return m_Histo->Write(nb, out);
}

//-----------------------------------------------------------------------------
// Conversion Image -> Terrain
//-----------------------------------------------------------------------------
bool XGeoRaster::Pix2Ground(uint32_t u, uint32_t v, double& x, double& y)
{
  if ((u >= m_nW)||(v >= m_nH))
    return false;
  x = m_Frame.Xmin + u * m_dResolution;
  y = m_Frame.Ymax - v * m_dResolution;
  return true;
}

//-----------------------------------------------------------------------------
// Conversion Terrain -> Image
//-----------------------------------------------------------------------------
bool XGeoRaster::Ground2Pix(double x, double y, uint32_t& u, uint32_t& v)
{
  if (!m_Frame.IsIn(XPt2D(x, y)))
    return false;
  u = XRint((x - m_Frame.Xmin) / m_dResolution);
  v = XRint((m_Frame.Ymax - y) / m_dResolution);
  return true;
}
