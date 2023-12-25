//-----------------------------------------------------------------------------
//								XTiffTileImage.h
//								================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 09/06/2021
//-----------------------------------------------------------------------------

#ifndef XTIFFTILEIMAGE_H
#define XTIFFTILEIMAGE_H

#include "XTiffReader.h"
#include "XBaseImage.h"

class XTiffTileImage : public XBaseImage {
public:
	XTiffTileImage();
	virtual ~XTiffTileImage() { Clear(); }

	virtual uint32_t RowH() { return m_nTileHeight; }		// Renvoie le groupement de ligne optimal pour l'image

  virtual std::string Format() { return "TIFF";}
  virtual std::string Metadata();
  bool SetTiffReader(XBaseTiffReader* reader);

	virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	virtual bool GetLine(XFile* file, uint32_t num, uint8_t* area);
	virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

protected:
	void		Clear();
	bool		AllocBuffer();
	bool		LoadTile(XFile* file, uint32_t x, uint32_t y);
  bool    LoadPlaneTile(XFile* file, uint32_t numTile);
	bool		Decompress();
	bool		PostProcess();
	bool		CopyTile(uint32_t tX, uint32_t tY, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	bool		CopyZoomTile(uint32_t tX, uint32_t tY, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

	uint32_t		m_nTileWidth;
	uint32_t		m_nTileHeight;
	uint32_t		m_nNbTile;
	uint16_t		m_nPixSize;
	uint16_t		m_nPhotInt;
  uint16_t		m_nPlanarConfig;
	uint16_t		m_nCompression;
	uint16_t		m_nPredictor;
	uint64_t*		m_TileOffsets;
	uint64_t*		m_TileCounts;
	uint8_t*			m_JpegTables;
	uint32_t		m_nJpegTablesSize;

	uint8_t*			m_Tile;		// Derniere tile chargee
	uint32_t		m_nLastTile;	// Numero de la derniere tile chargee

  // Gestion de la memoire partagee
  static uint8_t*     m_gBuffer;   // Buffer global de lecture
  static uint32_t    m_gBufSize;  // Taille du buffer
  static uint8_t*     m_gTile;     // Tile globale
  static uint32_t    m_gTileSize; // Taille de la tile globale
  static uint8_t*     m_gPlaneTile;// Tile globale pour les images par plans de couleurs
  static XTiffTileImage* m_gLastImage;  // Derniere image utilisee
};

#endif //XTIFFTILEIMAGE_H
