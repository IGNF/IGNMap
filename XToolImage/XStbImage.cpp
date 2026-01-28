//-----------------------------------------------------------------------------
//								XStbImage.cpp
//								=============
//
// Utilisation de la bibliotheque STB pour lire differents formats
// https://github.com/nothings/stb
// 
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 28/01/2026
//-----------------------------------------------------------------------------

#include "XStbImage.h"

#define STBI_NO_JPEG	// Le JPEG est gere par XJpegImage
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb/stb_image.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XStbImage::XStbImage(const char* filename)
{
	m_Data = nullptr;
	m_nDataSize = 0;
	m_bValid = false;
	int x, y, n;
	if (stbi_info(filename, &x, &y, &n) == 0)
		return;
	m_nW = x;
	m_nH = y;
	m_nNbSample = n;
	m_nNbBits = 8;
	m_bValid = true;
	m_strFilename = filename;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XStbImage::~XStbImage()
{
	if (m_Data != nullptr)
		delete[] m_Data;
}

bool XStbImage::GetArea(XFile*, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	if (m_Data == nullptr) {
		int x, y, n;
		m_Data = stbi_load(m_strFilename.c_str(), &x, &y, &n, 0);
		if (m_Data == nullptr)
			return false;
	}
	return XBaseImage::ExtractArea(m_Data, area, m_nW * m_nNbSample, m_nH, w * m_nNbSample, h, x * m_nNbSample, y);
}

bool XStbImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	if (factor == 0) return false;
	if (factor == 1) return GetArea(file, x, y, w, h, area);
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint32_t wout = w / factor;
	uint32_t hout = h / factor;
	uint8_t* pix = new uint8_t[w * h * m_nNbSample];

	bool flag = false;
	if (GetArea(file, x, y, w, h, pix))
		flag = XBaseImage::ZoomArea(pix, area, w, h, wout, hout, m_nNbSample);

	delete[] pix;

	return flag;
}