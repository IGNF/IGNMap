//-----------------------------------------------------------------------------
//								XBigTiffReader.h
//								================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/07/2021
//-----------------------------------------------------------------------------

#ifndef XBIGTIFFREADER_H
#define XBIGTIFFREADER_H

#include "XBaseTiffReader.h"

class XBigTiffReader : public XBaseTiffReader {
protected:
	/* Structure BigTiffTag	: tag BigTIFF de 20 bytes	*/
	typedef struct _BigTiffTag {
		uint16_t		TagId;
		uint16_t		DataType;
		uint64_t		DataCount;
		uint64_t		DataOffset;
	} BigTiffTag;

	/* Structure TiffIFD : Image File Directory	*/
	typedef struct _BigTiffIFD {
		std::vector<BigTiffTag>	TagList;       /* Tableau de Tags   		*/
		uint64_t									NextIFDOffset; /* Offset vers le prochain IFD	*/
	} BigTiffIFD;

	bool ReadHeader(std::istream* in);
	bool ReadIFD(std::istream* in, uint64_t offset, BigTiffIFD* ifd);

	bool FindTag(eTagID id, BigTiffTag* T);
	uint32_t DataSize(BigTiffTag* T);
  uint32_t ReadDataInTag(BigTiffTag* T, uint32_t defaut);
	uint32_t ReadIdTag(eTagID id, uint32_t defaut = 0);
	bool ReadDataArray(std::istream* in, eTagID id, void* V, int size);
	uint32_t* ReadUintArray(std::istream* in, eTagID id, uint32_t* nb_elt);
	uint64_t* ReadUint64Array(std::istream* in, eTagID id, uint32_t* nb_elt);

	uint64_t									m_nIFDOffset;		// Offset du premier IFD
	std::vector<BigTiffIFD>	m_IFD;					// Liste des IFD

public:
	XBigTiffReader() : XBaseTiffReader() { m_nIFDOffset = 0;}
	virtual ~XBigTiffReader() { Clear(); }

	virtual bool Read(std::istream* in);
	virtual uint32_t NbIFD() { return (uint32_t)m_IFD.size(); }
	virtual bool SetActiveIFD(uint32_t i) { if (i >= m_IFD.size()) return false; m_nActiveIFD = i; return true;}
	virtual bool AnalyzeIFD(std::istream* in);
	
	void PrintIFDTag(std::ostream* out);

};


#endif // XTIFFREADER_H
