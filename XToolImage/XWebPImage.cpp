//-----------------------------------------------------------------------------
//								XWebPImage.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 19/12/2022
//-----------------------------------------------------------------------------

#include "XWebPImage.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XWebPImage::XWebPImage(const char* filename) : XBaseImage()
{
	m_Data = NULL;
	m_nDataSize = 0;
	m_bValid = false;
	std::ifstream in;
	in.open(filename, std::ios::in | std::ios::binary);
	if (!in.good())
		return;
	// Verification de l'entete
	char buf[4];
	in.read(buf, 4);
	if ((buf[0] != 'R') || (buf[1] != 'I') || (buf[2] != 'F') || (buf[3] != 'F'))
		return;
	uint32_t size = 0;
	in.read((char*) &size, 4);
	in.read(buf, 4);
	if ((buf[0] != 'W') || (buf[1] != 'E') || (buf[2] != 'B') || (buf[3] != 'P'))
		return;
	// Taille du fichier
	in.seekg(0, std::ios_base::end);
	m_nDataSize = in.tellg();
	m_Data = new uint8_t[m_nDataSize];
	if (m_Data == NULL)
		return;
	in.seekg(0, std::ios_base::beg);
	in.read((char*)m_Data, m_nDataSize);
	// Analyse du fichier
	int w, h;
	if (!WebPGetInfo(m_Data, m_nDataSize, &w, &h)) {
		delete[] m_Data;
		m_Data = NULL;
		return;
	}
	m_nW = w;
	m_nH = h;
	m_nNbBits = 8;
	m_nNbSample = 3;
	m_bValid = true;
}

XWebPImage::~XWebPImage()
{
	if (m_Data != NULL)
		delete[] m_Data;
}

bool XWebPImage::GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	return GetWebPArea(x, y, w, h, area);
}

bool XWebPImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	return GetWebPArea(x, y, w, h, area, factor, w / factor, h / factor);
}

bool XWebPImage::GetWebPArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor, uint32_t wout, uint32_t hout)
{
	WebPDecoderConfig config;
	if (!WebPInitDecoderConfig(&config))
		return false;

	config.options.bypass_filtering = 0;
	config.options.no_fancy_upsampling = 1;
	config.options.use_cropping = 1;
	config.options.crop_left = x;
	config.options.crop_top = y;
	config.options.crop_width = w;
	config.options.crop_height = h;
	if (factor > 1) {
		config.options.use_scaling = 1;
		config.options.scaled_width = wout;
		config.options.scaled_height = hout;
	}

	config.output.colorspace = MODE_RGB;
	config.output.is_external_memory = 1;
	config.output.u.RGBA.rgba = (uint8_t*)area;
	if (factor > 1) {
		config.output.u.RGBA.stride = wout * 3;
		config.output.u.RGBA.size = wout * hout * 3;
	}
	else {
		config.output.u.RGBA.stride = w * 3;
		config.output.u.RGBA.size = w * h * 3;
	}

	if (WebPDecode(m_Data, m_nDataSize, &config) == VP8_STATUS_OK)
		return true;
	return false;
}