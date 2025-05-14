//-----------------------------------------------------------------------------
//								XTiffStripImage.h
//								=================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 15/06/2021
//-----------------------------------------------------------------------------

#ifndef XTIFFSTRIPIMAGE_H
#define XTIFFSTRIPIMAGE_H

#include "XTiffReader.h"
#include "XBaseImage.h"

class XTiffStripImage : public XBaseImage {
public:
	XTiffStripImage();
	virtual ~XTiffStripImage() { Clear(); }

	virtual uint32_t RowH() { return m_nRowsPerStrip; }		// Renvoie le groupement de ligne optimal pour l'image

  virtual std::string Format() { return "TIFF";}
  virtual std::string Metadata();
  bool SetTiffReader(XBaseTiffReader* reader);

	virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area);
	virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

protected:
	void		Clear();
	bool		AllocBuffer();
	bool		LoadStrip(XFile* file, uint32_t num);
  bool    LoadPlaneStrip(XFile* file, uint32_t numStrip);
  bool		Decompress();
	bool		PostProcess();
	bool		CopyStrip(uint32_t numStrip, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);

	uint32_t		m_nRowsPerStrip;
	uint32_t		m_nNbStrip;
	uint16_t		m_nPixSize;
	uint16_t		m_nPhotInt;
	uint16_t		m_nPlanarConfig;
	uint16_t		m_nCompression;
	uint16_t		m_nPredictor;
	uint64_t*		m_StripOffsets;
	uint64_t*		m_StripCounts;
	uint8_t*		m_JpegTables;
	uint32_t		m_nJpegTablesSize;
	bool				m_bNeedSwap;	// Indique qu'il faut swapper les donnees (endianess des donnees)

	uint8_t*		m_Buffer;			// Buffer de lecture
	uint8_t*		m_Strip;			// Derniere strip chargee
	uint32_t		m_nLastStrip;	// Numero de la derniere strip chargee
  uint8_t*    m_PlaneStrip; // Strip pour les images par plans de couleurs
};

#endif //XTIFFSTRIPIMAGE_H
