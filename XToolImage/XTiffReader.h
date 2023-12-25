//-----------------------------------------------------------------------------
//								XTiffReader.h
//								=============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 19/05/2021
//-----------------------------------------------------------------------------

#ifndef XTIFFREADER_H
#define XTIFFREADER_H

#include "XBaseTiffReader.h"

class XTiffReader : public XBaseTiffReader {
protected:
	/* Structure TiffTag	: tag TIFF de 12 bytes	*/
	typedef struct _TiffTag {
		uint16_t		TagId;
		uint16_t		DataType;
		uint32_t		DataCount;
		uint32_t		DataOffset;
	} TiffTag;

	/* Structure TiffIFD : Image File Directory	*/
	typedef struct _TiffIFD {
		std::vector<TiffTag>	TagList;       /* Tableau de Tags   		*/
		uint32_t								NextIFDOffset; /* Offset vers le prochain IFD	*/
	} TiffIFD;

	bool ReadHeader(std::istream* in);
	bool ReadIFD(std::istream* in, uint32_t offset, TiffIFD* ifd);

	bool FindTag(eTagID id, TiffTag* T);
	uint32_t DataSize(TiffTag* T);
  uint32_t ReadDataInTag(TiffTag* T, uint32_t defaut = 0);
	uint32_t ReadIdTag(eTagID id, uint32_t defaut = 0);
	bool ReadDataArray(std::istream* in, eTagID id, void* V, int size);
	uint32_t* ReadUintArray(std::istream* in, eTagID id, uint32_t* nb_elt);

	uint32_t								m_nIFDOffset;		// Offset du premier IFD
	std::vector<TiffIFD>	m_IFD;					// Liste des IFD

public:
	XTiffReader() : XBaseTiffReader() { m_nIFDOffset = 0; }
	virtual ~XTiffReader() { Clear(); }

	virtual bool Read(std::istream* in);
	virtual uint32_t NbIFD() { return (uint32_t)m_IFD.size(); }
	virtual bool SetActiveIFD(uint32_t i) { if (i >= m_IFD.size()) return false; m_nActiveIFD = i; return true;}
	virtual bool AnalyzeIFD(std::istream* in);

	virtual void PrintIFDTag(std::ostream* out);
};


#endif // XTIFFREADER_H
