//-----------------------------------------------------------------------------
//								XCogImage.h
//								===========
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 24/06/2021
//-----------------------------------------------------------------------------

#ifndef XCOGIMAGE_H
#define XCOGIMAGE_H

#include "XBaseImage.h"
#include "XTiffTileImage.h"

class XBaseTiffReader;

class XCogImage : public XBaseImage {
protected:
	XBaseTiffReader*							m_Reader;
	std::vector<XTiffTileImage*>	m_TImages;
	std::vector<uint32_t>						m_Factor;

public:
	XCogImage() { m_Reader = NULL; }
	virtual ~XCogImage() { Clear(); }

  virtual bool Open(XFile* file);
	virtual void Clear();
  virtual std::string Format() { return "COG";}
  virtual std::string Metadata();
	virtual uint32_t RowH() { if (m_TImages.size() > 1) return m_TImages[0]->RowH(); return 1;}

	virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area);
	virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
};

#endif //XCOGIMAGE_H
