//-----------------------------------------------------------------------------
//								XBaseTiffReader.h
//								=================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/07/2021
//-----------------------------------------------------------------------------

#ifndef XBASETIFFREADER_H
#define XBASETIFFREADER_H

#include <istream>
#include <vector>
#include "../XTool/XEndian.h"

class XBaseTiffReader {
public:

	enum eTagID { BITSPERSAMPLE = 258, COLORMAP = 320, COMPRESSION = 259, IMAGEHEIGHT = 257, IMAGEWIDTH	= 256,
								NEWSUBFILETYPE = 254, PHOTOMETRICINTERPRETATION	= 262, PLANARCONFIGURATION = 284,
								RESOLUTIONUNIT = 296, ROWSPERSTRIP = 278, SAMPLESPERPIXEL = 277, STRIPBYTECOUNTS = 279,
								STRIPOFFSETS = 273, TILEBYTECOUNTS = 325, TILELENGTH = 323, TILEOFFSETS = 324, TILEWIDTH = 322,
                XRESOLUTION = 282, YRESOLUTION = 283, PREDICTOR = 317, JPEGTABLES = 347, SAMPLEFORMAT = 339,
                YCBCRSUBSAMPLING = 530, REFERENCEBLACKWHITE = 532,
								ModelPixelScaleTag = 33550, ModelTiepointTag = 33922};

	enum eDataType { BYTE = 1, ASCII = 2, SHORT = 3, LONG = 4, RATIONAL = 5, SBYTE = 6, UNDEFINED = 7, SSHORT = 8,
									 SLONG = 9, SRATIONAL = 10, FLOAT = 11, DOUBLE = 12, 
									 LONG8 = 16, SLONG8 = 17, IFD8 = 18 };

	enum eCompression { UNCOMPRESSED1 = 1, CCITT1D = 2, CCITTGROUP3 = 3, CCITTGROUP4 = 4, LZW = 5, JPEG = 6, JPEGv2 = 7,
											DEFLATE = 8, UNCOMPRESSED2	= 32771, PACKBITS = 32773, DEFLATEv2 = 32946, WEBP = 50001};

	enum ePhotInt { WHITEISZERO = 0, BLACKISZERO = 1, RGBPHOT = 2, RGBPALETTE = 3, TRANSPARENCYMASK = 4,
									CMYKPHOT = 5, YCBCR = 6, CIELAB = 8 };

protected:
	
	void Clear();
	static uint32_t TypeSize(eDataType type);

	XEndian		m_Endian;
	bool			m_bByteOrder;		// Byte order du fichier
	uint32_t	m_nActiveIFD;		// IFD actif

	uint32_t	m_nWidth;
	uint32_t	m_nHeight;
	uint32_t	m_nRowsPerStrip;
	uint32_t	m_nTileWidth;
	uint32_t	m_nTileHeight;
	uint32_t	m_nNbStripOffsets;
	uint32_t	m_nNbStripCounts;
	uint32_t	m_nNbTileOffsets;
	uint32_t	m_nNbTileCounts;
	uint32_t	m_nNewSubFileType;
	uint16_t	m_nNbBits;
	uint16_t	m_nNbSample;
  uint16_t  m_nSampleFormat;
	uint16_t	m_nCompression;
	uint16_t	m_nPredictor;
	uint16_t	m_nPhotInt;
	uint16_t	m_nPlanarConfig;
	uint64_t* m_StripOffsets;
	uint64_t* m_StripCounts;
	uint64_t* m_TileOffsets;
	uint64_t* m_TileCounts;
	uint16_t* m_ColorMap;
  uint16_t  m_nColorMapSize;
	uint8_t*	m_JpegTables;
	uint32_t	m_nJpegTablesSize;
	uint16_t	m_YCbCrSub[2];
	uint32_t	m_ReferenceBlack[12];
	double	m_dX0;
	double	m_dY0;
	double	m_dGSD;

public:
	XBaseTiffReader() {
		m_bByteOrder = true;
		m_nActiveIFD = 0; m_StripOffsets = m_StripCounts = m_TileOffsets = m_TileCounts = NULL;
		m_ColorMap = NULL; m_JpegTables = NULL; m_nJpegTablesSize = 0;
		m_nWidth = m_nHeight = m_nRowsPerStrip = m_nTileWidth = m_nTileHeight = m_nNewSubFileType = 0;
		m_nNbStripOffsets = m_nNbStripCounts = m_nNbTileOffsets = m_nNbTileCounts = 0;
    m_nNbBits = m_nNbSample = m_nSampleFormat = m_nCompression = m_nPredictor = m_nPhotInt = m_nPlanarConfig = 0;
		m_nColorMapSize = 0;
		m_dX0 = m_dY0 = m_dGSD = 0.0;
		m_YCbCrSub[0] = m_YCbCrSub[1] = 0;
		for (int i = 0; i < 12; i++) m_ReferenceBlack[i] = 0;
	}
	virtual ~XBaseTiffReader() { Clear(); }

	virtual bool Read(std::istream* in) = 0;
	virtual uint32_t NbIFD() = 0;
	virtual bool SetActiveIFD(uint32_t i) = 0;
	virtual bool AnalyzeIFD(std::istream* in) = 0;
	virtual void PrintIFDTag(std::ostream* out) = 0;

	inline uint32_t Width() { return m_nWidth; }
	inline uint32_t Height() { return m_nHeight; }
	inline uint32_t RowsPerStrip() { return m_nRowsPerStrip; }
	inline uint32_t TileWidth() { return m_nTileWidth; }
	inline uint32_t TileHeight() { return m_nTileHeight; }
	inline uint32_t NewSubFileType() { return m_nNewSubFileType; }
	bool GetStripInfo(uint32_t* nbStrip, uint64_t** offset, uint64_t** count);
	bool GetTileInfo(uint32_t* nbTile, uint64_t** offset, uint64_t** count);
  bool GetColorMap(uint16_t** map, uint16_t* mapsize);
	inline uint16_t	NbBits() { return m_nNbBits; }
	inline uint16_t	NbSample() { return m_nNbSample; }
  inline uint16_t SampleFormat() { return m_nSampleFormat;}
	inline uint16_t	Compression() { return m_nCompression; }
	inline uint16_t	Predictor() { return m_nPredictor; }
	inline uint16_t	PhotInt() { return m_nPhotInt; }
	inline uint16_t	PlanarConfig() { return m_nPlanarConfig; }
	inline double X0() { return m_dX0; }
	inline double Y0() { return m_dY0; }
	inline double GSD() { return m_dGSD; }
	bool GetJpegTablesInfo(uint8_t** tables, uint32_t* size);

	bool IsTransparencyMask() { if ((m_nNewSubFileType & (uint32_t)4) != 0) return true; return false; }
	bool IsSubResolution() { if ((m_nNewSubFileType & (uint32_t)1) != 0) return true; return false; }
	bool NeedSwap() { if (m_Endian.ByteOrder() != m_bByteOrder) return true; return false; }

	void PrintInfo(std::ostream* out);

  static std::string CompressionString(uint16_t compression);
  static std::string PhotIntString(uint16_t photint);

};


#endif // XTIFFREADER_H
