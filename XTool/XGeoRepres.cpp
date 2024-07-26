//-----------------------------------------------------------------------------
//								XGeoRepres.cpp
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 08/07/2003
//-----------------------------------------------------------------------------

#include "XGeoRepres.h"
#include "XParserXML.h"

XGeoRepres   gDefaultRepres(16581630, 0xFFFFFFFF, 0, 0, 2, 12, 20);

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoRepres::XGeoRepres()
{
  *this = gDefaultRepres;
}

//-----------------------------------------------------------------------------
// Renvoie la couleur sous forme R, G, B
//-----------------------------------------------------------------------------
void XGeoRepres::Color(uint8_t& r, uint8_t& g, uint8_t& b)
{
	uint8_t* ptr = (uint8_t*)&m_nColor;
  r = ptr[2]; // Modification pour QT
	g = ptr[1];
  b = ptr[0];
}

//-----------------------------------------------------------------------------
// Renvoie la couleur de fond sous forme R, G, B
//-----------------------------------------------------------------------------
void XGeoRepres::FillColor(uint8_t& r, uint8_t& g, uint8_t& b)
{
	uint8_t* ptr = (uint8_t*)&m_nFillColor;
  r = ptr[2]; // Modification pour QT
	g = ptr[1];
  b = ptr[0];
}

//-----------------------------------------------------------------------------
// Renvoie la couleur de fond sous forme R, G, B, Alpha
//-----------------------------------------------------------------------------
void XGeoRepres::FillColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& alpha)
{
	uint8_t* ptr = (uint8_t*)&m_nFillColor;
	alpha = ptr[3];
	r = ptr[2]; // Modification pour QT
	g = ptr[1];
	b = ptr[0];
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoRepres::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	XParserXML repres = parser->FindSubParser("/xgeorepres", num);
	if (repres.IsEmpty())
		return XErrorError(error, "XGeoRepres::XmlRead", XError::eBadFormat);

  m_strName = repres.ReadNode("/xgeorepres/name");
	m_strFont = repres.ReadNode("/xgeorepres/font");
	m_nZOrder = repres.ReadNodeAsUInt32("/xgeorepres/zorder");

  m_nMinScale = repres.ReadNodeAsUInt32("/xgeorepres/min_scale");
  m_nMaxScale = repres.ReadNodeAsUInt32("/xgeorepres/max_scale");

	uint32_t data;
	data = repres.ReadNodeAsUInt32("/xgeorepres/size");
	if (data > 255)
		data = 1;
	m_nSize = (uint8_t)data;
	data = repres.ReadNodeAsUInt32("/xgeorepres/font_size");
	if ((data < 1)||(data > 255))
		data = 9;
	m_nFontSize = (uint8_t)data;

	m_nSymbol = repres.ReadNodeAsUInt32("/xgeorepres/symbol");
	if (m_nSymbol == 0) {
    uint32_t pen, brush, cell;
    data = repres.ReadNodeAsUInt32("/xgeorepres/symbol_data");
    pen = repres.ReadNodeAsUInt32("/xgeorepres/symbol_pen");
		brush = repres.ReadNodeAsUInt32("/xgeorepres/symbol_brush");
		cell = repres.ReadNodeAsUInt32("/xgeorepres/symbol_cell");
    m_nSymbol = DPBCSymbol(data, pen, brush, cell);
	}

	if (repres.FindNode("/xgeorepres/pen_color")) // Nouvelle maniere
		m_nColor = repres.ReadNodeAsHexUInt32("/xgeorepres/pen_color");
	else {
		uint32_t r, g, b;
		r = repres.ReadNodeAsUInt32("/xgeorepres/color_r");
		g = repres.ReadNodeAsUInt32("/xgeorepres/color_g");
		b = repres.ReadNodeAsUInt32("/xgeorepres/color_b");
		m_nColor = RGBColor(r, g, b);

		bool transparent = false;
		transparent = repres.ReadNodeAsBool("/xgeorepres/pen_transparent");
		if (transparent)
			m_nColor = 0xFFFFFFFF;
	}

	if (repres.FindNode("/xgeorepres/fill_color")) // Nouvelle maniere
		m_nFillColor = repres.ReadNodeAsHexUInt32("/xgeorepres/fill_color");
	else {
		uint32_t r, g, b;
		r = repres.ReadNodeAsUInt32("/xgeorepres/fill_color_r");
		g = repres.ReadNodeAsUInt32("/xgeorepres/fill_color_g");
		b = repres.ReadNodeAsUInt32("/xgeorepres/fill_color_b");
		m_nFillColor = RGBColor(r, g, b);

		bool transparent = false;
		transparent = repres.ReadNodeAsBool("/xgeorepres/fill_transparent");
		if (transparent)
			m_nFillColor = 0xFFFFFFFF;
	}

	if (repres.FindNode("/xgeorepres/transparency")) {
		uint32_t r = repres.ReadNodeAsUInt32("/xgeorepres/transparency");
		if (r > 100)
			m_nTrans = 100;
		else
			m_nTrans = (uint8_t)r;
	} else {
		m_nTrans = 50;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XGeoRepres::XmlWrite(std::ostream* out)
{
	*out << "\t<xgeorepres>" << std::endl;
  *out << "\t\t<name> " << m_strName << " </name>" << std::endl;

	char buf[12];
	snprintf(buf, 12, "%x", m_nColor);
	*out << "\t\t<pen_color> " << buf << " </pen_color>" << std::endl;
	snprintf(buf, 12, "%x", m_nFillColor);
	*out << "\t\t<fill_color> " << buf << " </fill_color>" << std::endl;

	/* Ancienne version
	uint8_t* ptr;
	if (m_nColor != 0xFFFFFFFF) {
		ptr = (uint8_t*)&m_nColor;
    *out << "\t\t<color_r> " << (int)ptr[2] << " </color_r>" << std::endl;
		*out << "\t\t<color_g> " << (int)ptr[1] << " </color_g>" << std::endl;
    *out << "\t\t<color_b> " << (int)ptr[0] << " </color_b>" << std::endl;
	} else
		*out << "\t\t<pen_transparent>" << (int)true << "</pen_transparent>" << std::endl;

	if (m_nFillColor != 0xFFFFFFFF) {
		ptr = (uint8_t*)&m_nFillColor;
    *out << "\t\t<fill_color_r> " << (int)ptr[2] << " </fill_color_r>" << std::endl;
		*out << "\t\t<fill_color_g> " << (int)ptr[1] << " </fill_color_g>" << std::endl;
    *out << "\t\t<fill_color_b> " << (int)ptr[0] << " </fill_color_b>" << std::endl;
	} else
		*out << "\t\t<fill_transparent>" << (int)true << " </fill_transparent>" << std::endl;
	*/

	*out << "\t\t<symbol> " << m_nSymbol << " </symbol>" << std::endl;
	*out << "\t\t<size> " << (int)m_nSize << " </size>" << std::endl;
	*out << "\t\t<font_size> " << (int)m_nFontSize << " </font_size>" << std::endl;
	*out << "\t\t<zorder> " << m_nZOrder << " </zorder>" << std::endl;
	*out << "\t\t<transparency> " << (int)m_nTrans << " </transparency>" << std::endl;

	if (m_strFont.size() > 0)
		*out << "\t\t<font> " << m_strFont << " </font>" << std::endl;

  *out << "\t\t<min_scale> " << m_nMinScale << " </min_scale>" << std::endl;
  *out << "\t\t<max_scale> " << m_nMaxScale << " </max_scale>" << std::endl;

	*out << "\t</xgeorepres>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Operateurs logiques
//-----------------------------------------------------------------------------
bool operator==(XGeoRepres A, XGeoRepres B)
{
	if (A.m_strName != B.m_strName)
		return false; 
	if (A.m_nColor != B.m_nColor)
		return false; 
	if (A.m_nFillColor != B.m_nFillColor)
		return false; 
	if (A.m_nSymbol != B.m_nSymbol)
		return false; 
	if (A.m_nSize != B.m_nSize)
		return false; 
	if (A.m_nZOrder != B.m_nZOrder)
		return false; 
	if (A.m_nTrans != B.m_nTrans)
		return false; 
  if (A.m_nMinScale != B.m_nMinScale)
    return false;
  if (A.m_nMaxScale != B.m_nMaxScale)
    return false;
  if (A.m_strFont != B.m_strFont)
    return false;
  if (A.m_nFontSize != B.m_nFontSize)
    return false;

	return true;
}

bool operator!=(XGeoRepres A, XGeoRepres B)
{
	return !(A==B);
}
