//-----------------------------------------------------------------------------
//								XGeoRepres.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/03/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOREPRES_H
#define _XGEOREPRES_H

#include "XBase.h"

class XParserXML;

class XGeoRepres {
protected:
	std::string	m_strName;		// Nom de la representation
	std::string	m_strFont;		// Nom de la police
	uint32_t			m_nColor;			// Couleur du trait
	uint32_t			m_nFillColor;	// Couleur de remplissage
	uint32_t			m_nSymbol;		// Numero du symbole ponctuelle ou du pattern
	uint32_t			m_nZOrder;		// Ordre d'affichage
	uint8_t				m_nSize;			// Dimension du symbole ou epaisseur du trait
	uint8_t				m_nFontSize;	// Taille de la police
	uint8_t				m_nTrans;			// Pourcentage de transparence
  bool        m_bDeletable; // Indique si la representation peut etre detruite
  uint32_t      m_nMinScale;  // Echelle minimum d'affichage
  uint32_t      m_nMaxScale;  // Echelle minimum d'affichage
public:
  XGeoRepres();
  XGeoRepres(uint32_t color, uint32_t fill, uint32_t symbol, uint32_t zorder, uint8_t size, uint8_t font, uint8_t trans)
  { m_nColor = color; m_nFillColor = fill; m_nSymbol = symbol; m_nZOrder = zorder;
    m_nSize = size; m_nFontSize = font; m_nTrans = trans; m_bDeletable = false;
    m_nMinScale = 0; m_nMaxScale = 0;}
	virtual ~XGeoRepres() {;}

	inline uint32_t Color() const { return m_nColor;}
	inline uint32_t FillColor() const { return m_nFillColor;}
	inline uint32_t Symbol() const { return m_nSymbol;}
	inline uint32_t ZOrder() const { return m_nZOrder;}
	inline uint8_t Size() const { return m_nSize;}
	inline uint8_t FontSize() const { return m_nFontSize;}
	inline uint8_t Transparency() const { return m_nTrans;}
  inline bool Deletable() const { return m_bDeletable;}
  inline uint32_t MinScale() const { return m_nMinScale;}
  inline uint32_t MaxScale() const { return m_nMaxScale;}
	std::string Name() { return m_strName;}
	std::string Font() { return m_strFont;}
	void Color(uint8_t& r, uint8_t& g, uint8_t& b);
	void FillColor(uint8_t& r, uint8_t& g, uint8_t& b);
	void FillColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& alpha);

  //static inline uint32_t RGBColor(uint32_t r, uint32_t g, uint32_t b)
  //															{uint32_t m = 255; return XMin(b,m)*256*256 + XMin(g,m)*256 + XMin(r, m);}
  // Modification pour QT
  static inline uint32_t RGBColor(uint32_t r, uint32_t g, uint32_t b)
                                {uint32_t m = 255; return XMin(r,m)*256*256 + XMin(g,m)*256 + XMin(b, m);}
  static inline uint32_t DPBCSymbol(uint32_t d, uint32_t p, uint32_t b, uint32_t s)
          {uint32_t m = 255; return XMin(d,m)*256*256*256 + XMin(p,m)*256*256 + XMin(b,m)*256 + XMin(s, m);}

  uint8_t Data() { uint8_t* ptr = (uint8_t*)&m_nSymbol; return ptr[3];}
  uint8_t Pen() { uint8_t* ptr = (uint8_t*)&m_nSymbol; return ptr[2];}
	uint8_t Brush() { uint8_t* ptr = (uint8_t*)&m_nSymbol; return ptr[1];}
	uint8_t Cell() { uint8_t* ptr = (uint8_t*)&m_nSymbol; return ptr[0];}

	void Color(uint32_t c) { m_nColor = c;}
	void Color(uint32_t r, uint32_t g, uint32_t b) { m_nColor = RGBColor(r, g, b);}
	void FillColor(uint32_t c) { m_nFillColor = c;}
	void FillColor(uint32_t r, uint32_t g, uint32_t b) { m_nFillColor = RGBColor(r, g, b);}
	void Symbol(uint32_t s) { m_nSymbol = s;}
	void Size(uint8_t s) { m_nSize = s;}
	void FontSize(uint8_t s) { m_nFontSize = s;}
	void ZOrder(uint32_t z) { m_nZOrder = z;}
	void Name(const char* name) { m_strName = name;}
	void Font(const char* name) { m_strFont = name;}
	void Transparency(uint8_t t) { if (t > 100) m_nTrans = 100; else m_nTrans = t;}
  void Deletable(bool flag) { m_bDeletable = flag;}
  void Scale(uint32_t min, uint32_t max) { m_nMinScale = min; m_nMaxScale = max;}

	virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
	virtual bool XmlWrite(std::ostream* out);

	// Operateurs logiques
	friend bool operator==(XGeoRepres, XGeoRepres);
	friend bool operator!=(XGeoRepres, XGeoRepres);
};

extern XGeoRepres   gDefaultRepres;

#endif //_XGEOREPRES_H
