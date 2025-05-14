//-----------------------------------------------------------------------------
//								XSentinelScene.cpp
//								==================
//
// Acces aux images Sentinel 2
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 18/12/2024
//-----------------------------------------------------------------------------

#include "XSentinelScene.h"
#include <sstream>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XSentinelScene::XSentinelScene()
{
	for (int i = 0; i < 16; i++) 
		m_Ima10m[i] = m_Ima20m[i] = m_Ima60m[i] = nullptr;
	m_nResol = 20;
	m_ViewMode = RGB;
	m_CutNDVI = 0.6f;
	m_CutNDWI = 0.f;
	m_Offset = 1000;	// Valeur standard indiquee dans le fichier MTD_MSIL2A.xml
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XSentinelScene::~XSentinelScene()
{
	for (int i = 0; i < 16; i++) {
		if (m_Ima10m[i] != nullptr) delete m_Ima10m[i];
		if (m_Ima20m[i] != nullptr) delete m_Ima20m[i];
		if (m_Ima60m[i] != nullptr) delete m_Ima60m[i];
	}
}


int XSentinelScene::NbByte()
{
	if (m_ViewMode >= RGB)
		return 3;
	if (m_ViewMode == TCI)
		return 3;
	return 1;
}

uint16_t XSentinelScene::NbSample()
{
	if ((m_ViewMode == NDVI) || (m_ViewMode == NDWI))
		return 1;
	if (m_ViewMode >= RGB)
		return 3;
	if (m_ViewMode == TCI)
		return 3;
	return 1;
}

//-----------------------------------------------------------------------------
// Fixe le mode d'affichage
//-----------------------------------------------------------------------------
void XSentinelScene::SetViewMode(ViewMode mode, bool adjustResol)
{
	if (adjustResol) {
		if ((mode == RGB) || (mode == IRC) || (mode == NDVI) || (mode == NDWI))
			m_nResol = 10;
		if ((mode == URBAN) || (mode == SWIR))
			m_nResol = 20;
	}
	m_ViewMode = mode;
}

//-----------------------------------------------------------------------------
// Renvoie le nombre d'images effectivement referencees dans la scene
//-----------------------------------------------------------------------------
int XSentinelScene::NbImages() const
{
	int nb = 0;
	for (int i = 0; i < 16; i++) {
		if (m_Ima10m[i] != nullptr) nb++;
		if (m_Ima20m[i] != nullptr) nb++;
		if (m_Ima60m[i] != nullptr) nb++;
	}
	return nb;
}

//-----------------------------------------------------------------------------
// Verifie que la scene est compatible avec le mode d'affichage
//-----------------------------------------------------------------------------
bool XSentinelScene::CheckViewMode(XFileImage*& imaA, XFileImage*& imaB, XFileImage*& imaC) const
{
	imaA = imaB = imaC = nullptr;
	if (m_ViewMode == RGB) {// B02 B03 B04
		if (m_nResol == 10) { imaA = m_Ima10m[4]; imaB = m_Ima10m[3]; imaC = m_Ima10m[2]; }
		if (m_nResol == 20) { imaA = m_Ima20m[4]; imaB = m_Ima20m[3]; imaC = m_Ima20m[2]; }
		if (m_nResol == 60) { imaA = m_Ima60m[4]; imaB = m_Ima60m[3]; imaC = m_Ima60m[2]; }
		if ((imaA != nullptr) && (imaB != nullptr) && (imaC != nullptr))
			return true;
		else
			return false;
	}
	if (m_ViewMode == IRC) {// B03 B04 B08
		if (m_nResol == 10) { imaA = m_Ima10m[8]; imaB = m_Ima10m[4]; imaC = m_Ima10m[3]; }
		if (m_nResol == 20) { imaA = m_Ima20m[8]; imaB = m_Ima20m[4]; imaC = m_Ima20m[3]; }
		if (m_nResol == 60) { imaA = m_Ima60m[8]; imaB = m_Ima60m[4]; imaC = m_Ima60m[3]; }
		if ((imaA != nullptr) && (imaB != nullptr) && (imaC != nullptr))
			return true;
		else
			return false;
	}
	if (m_ViewMode == URBAN) { // B12 / B11 / B04
		if (m_nResol == 20) { imaA = m_Ima20m[12]; imaB = m_Ima20m[11]; imaC = m_Ima20m[4]; }
		if (m_nResol == 60) { imaA = m_Ima60m[12]; imaB = m_Ima60m[11]; imaC = m_Ima60m[4]; }
		if ((imaA != nullptr) && (imaB != nullptr) && (imaC != nullptr))
			return true;
		else
			return false;
	}
	if (m_ViewMode == SWIR) { // B12 / B8A / B04
		if (m_nResol == 20) { imaA = m_Ima20m[12]; imaB = m_Ima20m[8]; imaC = m_Ima20m[4]; }
		if (m_nResol == 60) { imaA = m_Ima60m[12]; imaB = m_Ima60m[8]; imaC = m_Ima60m[4]; }
		if ((imaA != nullptr) && (imaB != nullptr) && (imaC != nullptr))
			return true;
		else
			return false;
	}

	if (m_ViewMode == NDVI) {// (B8 - B4) / (B8 + B4)
		if (m_nResol == 10) { imaA = m_Ima10m[8]; imaB = m_Ima10m[4]; }
		if (m_nResol == 20) { imaA = m_Ima20m[8]; imaB = m_Ima20m[4]; }
		if (m_nResol == 60) { imaA = m_Ima60m[8]; imaB = m_Ima60m[4]; }
		if ((imaA != nullptr) && (imaB != nullptr))
			return true;
		else
			return false;
	}
	if (m_ViewMode == NDWI) {// (B3 - B8) / (B3 + B8)
		if (m_nResol == 10) { imaA = m_Ima10m[3]; imaB = m_Ima10m[8]; }
		if (m_nResol == 20) { imaA = m_Ima20m[3]; imaB = m_Ima20m[8]; }
		if (m_nResol == 60) { imaA = m_Ima60m[3]; imaB = m_Ima60m[8]; }
		if ((imaA != nullptr) && (imaB != nullptr))
			return true;
		else
			return false;
	}

	// Acces sur un seul canal
	if ((m_ViewMode >= AOT)&&(m_ViewMode <= WVP)) {
		if (m_nResol == 10) imaA = m_Ima10m[m_ViewMode];
		if (m_nResol == 20) imaA = m_Ima20m[m_ViewMode];
		if (m_nResol == 60) imaA = m_Ima60m[m_ViewMode];
		if (imaA != nullptr)
			return true;
		else
			return false;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Renvoie une image active a la resolution de travail
//-----------------------------------------------------------------------------
XFileImage* XSentinelScene::GetActiveImage()
{
	XFileImage* imaA, * imaB, * imaC;
	if (!CheckViewMode(imaA, imaB, imaC))
		return nullptr;
	return imaA;
}

//-----------------------------------------------------------------------------
// Analyse d'un nom d'image
//-----------------------------------------------------------------------------
bool XSentinelScene::AnalyzeFilename(std::string filename, std::string& proj, std::string& name, std::string& date,
	std::string& id, std::string& channel, std::string& resol)
{
	if (filename.size() < 30)
		return false;
	proj = filename.substr(0, 3);
	name = filename.substr(3, 3);
	std::string sep = filename.substr(6, 1);
	if (sep != "_")
		return false;
	date = filename.substr(7, 8);
	id = filename.substr(15, 6);
	sep = filename.substr(22, 1);
	if (sep != "_")
		return false;
	channel = filename.substr(23, 3);
	if (IndexChannel(channel) < 0)
		return false;
	sep = filename.substr(26, 1);
	if (sep != "_")
		return false;
	resol = filename.substr(27, 3);
	if ((resol != "10m") && (resol != "20m") && (resol != "60m"))
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Index des canaux radiometriques
//-----------------------------------------------------------------------------
int XSentinelScene::IndexChannel(std::string channel)
{
	if (channel == "AOT") return 0;
	if (channel == "B01") return 1;
	if (channel == "B02") return 2;
	if (channel == "B03") return 3;
	if (channel == "B04") return 4;
	if (channel == "B05") return 5;
	if (channel == "B06") return 6;
	if (channel == "B07") return 7;
	if (channel == "B08") return 8;
	if (channel == "B8A") return 8;
	if (channel == "B09") return 9;
	if (channel == "B11") return 11;
	if (channel == "B12") return 12;
	if (channel == "SCL") return 13;
	if (channel == "TCI") return 14;
	if (channel == "WVP") return 15;
	return -1;
}

//-----------------------------------------------------------------------------
// Acces aux pixels
//-----------------------------------------------------------------------------
bool XSentinelScene::GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	XFileImage* imaA, * imaB, * imaC;
	if (!CheckViewMode(imaA, imaB, imaC))
		return false;
	// Acces sur un seul canal
	if ((m_ViewMode >= AOT) && (m_ViewMode <= WVP))
		return imaA->GetArea(x, y, w, h, area);
	if ((m_ViewMode == NDVI) || (m_ViewMode == NDWI))
		return BuildIndexImage(x, y, w, h, area, 1, imaA, imaB);
	return BuildRGBImage(x, y, w, h, area, 1, imaA, imaB, imaC);
}

//-----------------------------------------------------------------------------
// Acces aux pixels avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XSentinelScene::GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	XFileImage* imaA, * imaB, * imaC;
	if (!CheckViewMode(imaA, imaB, imaC))
		return false;
	// Acces sur un seul canal
	if ((m_ViewMode >= AOT) && (m_ViewMode <= WVP))
		return imaA->GetZoomArea(x, y, w, h, area, factor);
	if ((m_ViewMode == NDVI) || (m_ViewMode == NDWI))
		return BuildIndexImage(x, y, w, h, area, factor, imaA, imaB);
	return BuildRGBImage(x, y, w, h, area, factor, imaA, imaB, imaC);
}

//-----------------------------------------------------------------------------
// Construction d'une image a partir de 3 bandes
//-----------------------------------------------------------------------------
bool XSentinelScene::BuildRGBImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
																	 XFileImage* imageR, XFileImage* imageG, XFileImage* imageB)
{
	if ((imageR == nullptr) || (imageG == nullptr) || (imageB == nullptr))
		return false;
	uint32_t wout = w / factor, hout = h / factor;
	uint8_t* buf = imageR->AllocArea(wout, hout);
	if (buf == nullptr)
		return false;
	XBaseImage::MinValue = m_Offset;
	XBaseImage::MaxValue = 0xFFFF;
	if (factor == 1)
		imageR->GetArea(x, y, w, h, buf);
	else
		imageR->GetZoomArea(x, y, w, h, buf, factor);
	uint8_t* ptr = area;
	for (uint32_t i = 0; i < wout * hout; i++) {
		*ptr = buf[i]; ptr += 3;
	}
	if (factor == 1)
		imageG->GetArea(x, y, w, h, buf);
	else
		imageG->GetZoomArea(x, y, w, h, buf, factor);
	ptr = area + 1;
	for (uint32_t i = 0; i < wout * hout; i++) {
		*ptr = buf[i]; ptr += 3;
	}
	if (factor == 1)
		imageB->GetArea(x, y, w, h, buf);
	else
		imageB->GetZoomArea(x, y, w, h, buf, factor);
	ptr = area + 2;
	for (uint32_t i = 0; i < wout * hout; i++) {
		*ptr = buf[i]; ptr += 3;
	}
	delete[] buf;
	XBaseImage::MinValue = XBaseImage::MaxValue = 0;
	return true;
}

//-----------------------------------------------------------------------------
// Construction d'une image index a partir de 2 bandes (Ba - Bb) / (Ba + Bb)
//-----------------------------------------------------------------------------
bool XSentinelScene::BuildIndexImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
																		 XFileImage* imageA, XFileImage* imageB)
{
	if ((imageA == nullptr) || (imageB == nullptr))
		return false;
	uint32_t wout = w / factor, hout = h / factor;
	uint32_t nb_sample;
	float* pixA = new float[wout * hout];
	if (!imageA->GetRawArea(x, y, w, h, pixA, &nb_sample, factor)) {
		delete[] pixA;
		return false;
	}
	float* pixB = new float[wout * hout];
	if (!imageB->GetRawArea(x, y, w, h, pixB, &nb_sample, factor)) {
		delete[] pixA;
		delete[] pixB;
		return false;
	}

	float cut_value = m_CutNDWI;
	int chan_index = 2, chan_void1 = 0, chan_void2 = 1;
	if (m_ViewMode == NDVI) {
		cut_value = m_CutNDVI;
		chan_index = 1;
		chan_void2 = 2;
	}

	float index;
	float* ptrA = pixA, * ptrB = pixB;
	uint8_t* ptr = area;
	for (uint32_t i = 0; i < hout; i++) {
		for (uint32_t j = 0; j < wout; j++) {
			if (fabs(*ptrA + *ptrB - 2 * m_Offset) < 1e-6)
				index = -1.f;
			else
				index = (*ptrA - *ptrB) / (*ptrA + *ptrB - 2 * m_Offset);
			ptrA++;
			ptrB++;
			if (index > cut_value) {
				ptr[chan_void1] = (uint8_t)0;
				ptr[chan_void2] = (uint8_t)0;
				ptr[chan_index] = (uint8_t)(127 + index * 127.f);
			}
			else {
				ptr[chan_void1] = (uint8_t)(127 + index * 127.f);
				ptr[chan_void2] = (uint8_t)(127 + index * 127.f);
				ptr[chan_index] = (uint8_t)0;
			}
			ptr += 3;
		}
	}
	delete[] pixA;
	delete[] pixB;

	return true;
}

//-----------------------------------------------------------------------------
// Acces aux pixels en valeurs brutes d'une image composite RGB
//-----------------------------------------------------------------------------
bool XSentinelScene::BuildRawPixelRGB(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample,
																			XFileImage* imageR, XFileImage* imageG, XFileImage* imageB)
{
	if ((imageR == nullptr) || (imageG == nullptr) || (imageB == nullptr))
		return false;

	if ((imageR->NbSample() != 1)||(imageG->NbSample() != 1) || (imageB->NbSample() != 1))
		return false;
	double* buf = new double[(2 * win + 1) * (2 * win + 1)];
	if (!imageR->GetRawPixel(x, y, win, buf, nb_sample)) {
		delete[] buf;
		return false;
	}
	double* ptr = pix;
	for (uint32_t i = 0; i < (2 * win + 1) * (2 * win + 1); i++) {
		*ptr = buf[i] - m_Offset; ptr += 3;
	}

	if (!imageG->GetRawPixel(x, y, win, buf, nb_sample)) {
		delete[] buf;
		return false;
	}
	ptr = pix + 1;
	for (uint32_t i = 0; i < (2 * win + 1) * (2 * win + 1); i++) {
		*ptr = buf[i] - m_Offset; ptr += 3;
	}

	if (!imageB->GetRawPixel(x, y, win, buf, nb_sample)) {
		delete[] buf;
		return false;
	}
	ptr = pix + 2;
	for (uint32_t i = 0; i < (2 * win + 1) * (2 * win + 1); i++) {
		*ptr = buf[i] - m_Offset; ptr += 3;
	}

	*nb_sample = 3;
	delete[] buf;
	return true;
}

//-----------------------------------------------------------------------------
// Acces aux pixels en valeurs brutes d'une image composite index
//-----------------------------------------------------------------------------
bool XSentinelScene::BuildRawPixelIndex(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample,
																				XFileImage* imageA, XFileImage* imageB)
{
	if ((imageA == nullptr) || (imageB == nullptr))
		return false;
	uint32_t wout = 2 * win + 1;
	uint32_t nb_sampleA, nb_sampleB;
	double* pixA = new double[wout * wout];
	if (!imageA->GetRawPixel(x, y, win, pixA, &nb_sampleA)) {
		delete[] pixA;
		return false;
	}
	double* pixB = new double[wout * wout];
	if (!imageB->GetRawPixel(x, y, win, pixB, &nb_sampleB)) {
		delete[] pixA;
		delete[] pixB;
		return false;
	}
	for (uint32_t i = 0; i < wout * wout; i++) {
		if (fabs((pixA[i] + pixB[i] - 2 * m_Offset) < 1e-6))
			pix[i] = -1.;
		else
			pix[i] = (pixA[i] - pixB[i]) / (pixA[i] + pixB[i] - 2 * m_Offset);
	}

	delete[] pixA;
	delete[] pixB;
	return true;
}

//-----------------------------------------------------------------------------
// Acces aux pixels en valeurs brutes
//-----------------------------------------------------------------------------
bool XSentinelScene::GetRawPixel(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample)
{
	XFileImage* imaA, * imaB, * imaC;
	if (!CheckViewMode(imaA, imaB, imaC))
		return false;
	// Acces sur un seul canal
	if ((m_ViewMode >= AOT) && (m_ViewMode <= WVP))
		return imaA->GetRawPixel(x, y, win, pix, nb_sample);

	if ((m_ViewMode == NDVI) || (m_ViewMode == NDWI))
		return BuildRawPixelIndex(x, y, win, pix, nb_sample, imaA, imaB);

	return BuildRawPixelRGB(x, y, win, pix, nb_sample, imaA, imaB, imaC);
}

//-----------------------------------------------------------------------------
// Acces aux pixels en valeurs brutes avec un facteur de zoom
//-----------------------------------------------------------------------------
bool XSentinelScene::GetRawArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix, uint32_t* nb_sample, uint32_t factor)
{
	return false;
}

//-----------------------------------------------------------------------------
// Import d'une image
//-----------------------------------------------------------------------------
bool XSentinelScene::ImportImage(std::string path, std::string filename)
{
	std::string proj, name, date, id, channel, resol;
	if (!AnalyzeFilename(filename, proj, name, date, id, channel, resol))
		return false;
	int index = IndexChannel(channel);
	if (m_strName.empty()) { // La premiere image fixe les attributs
		XFileImage* image = new XFileImage;
		if (!image->AnalyzeImage(path)) {	// L'image n'est pas lisible
			delete image;
			return false;
		}
		m_strName = name;
		m_strDate = date;
		m_strProjection = proj;
		m_strId = id;
		if (resol == "10m")
			m_Ima10m[index] = image;
		if (resol == "20m")
			m_Ima20m[index] = image;
		if (resol == "60m")
			m_Ima60m[index] = image;
		return true;
	}
	if ((m_strName != name) || (m_strDate != date) || (m_strProjection != proj) || (m_strId != id))
		return false;
	XFileImage* image = new XFileImage;
	if (!image->AnalyzeImage(path)) {	// L'image n'est pas lisible
		delete image;
		return false;
	}
	if (resol == "10m")
		m_Ima10m[index] = image;
	if (resol == "20m")
		m_Ima20m[index] = image;
	if (resol == "60m")
		m_Ima60m[index] = image;
	return true;
}

//-----------------------------------------------------------------------------
// Attributs Sentinel
//-----------------------------------------------------------------------------
bool XSentinelScene::SentinelAttributes(std::vector<std::string>& V)
{
	V.push_back("Name"); V.push_back(m_strName);
	V.push_back("ID"); V.push_back(m_strId);
	V.push_back("Date"); V.push_back(m_strDate);
	V.push_back("Projection"); V.push_back(m_strProjection);
	int nb_images = 0;
	for (int i = 0; i < 16; i++) if (m_Ima10m[i] != nullptr) nb_images++;
	V.push_back("10m");
	std::ostringstream out;
	out << nb_images;
	V.push_back(out.str());
	nb_images = 0;
	for (int i = 0; i < 16; i++) if (m_Ima20m[i] != nullptr) nb_images++;
	V.push_back("20m");
	out.str(""); out.clear();
	out << nb_images;
	V.push_back(out.str());
	nb_images = 0;
	for (int i = 0; i < 16; i++) if (m_Ima60m[i] != nullptr) nb_images++;
	V.push_back("60m");
	out.str(""); out.clear();
	out << nb_images;
	V.push_back(out.str());
	return true;
}

//-----------------------------------------------------------------------------
// Calcul du NDVI en un point
//-----------------------------------------------------------------------------
double XSentinelScene::GetIndex(double X, double Y, ViewMode indexMode)
{
	if ((indexMode != NDVI) && (indexMode != NDWI))
		return -1.;
	ViewMode mode = GetViewMode();
	SetViewMode(indexMode);
	XFileImage *imaA = nullptr, *imaB = nullptr, *imaC = nullptr;
	if (!CheckViewMode(imaA, imaB, imaC)) {
		SetViewMode(mode);
		return -1.;
	}
	SetViewMode(mode);
	int W = (int)imaA->Width();
	int H = (int)imaA->Height();
	double Xmin = 0., Ymax = 0., gsd = 1.;
	imaA->GetGeoref(&Xmin, &Ymax, &gsd);
	int U0 = XRint((X - Xmin) / gsd);
	int V0 = XRint((Ymax - Y) / gsd);
	if ((U0 <= 0) || (V0 <= 0) || (U0 >= (W - 1)) || (V0 >= (H - 1))) 
		return -1.;
	double pixA[9], pixB[9];
	uint32_t nb_sample;
	imaA->GetRawPixel(U0, V0, 1, pixA, &nb_sample);
	imaB->GetRawPixel(U0, V0, 1, pixB, &nb_sample);
	if ((pixA[4] + pixB[4] - 2 * m_Offset) < 1e-6)
		return -1.;
	return ((pixA[4] - pixB[4]) / (pixA[4] + pixB[4] - 2 * m_Offset));
}