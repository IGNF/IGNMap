//-----------------------------------------------------------------------------
//								XPackBitsCodec.h
//								================
//
// Auteur : F.Becirspahic - MODSP
//
// 23/08/2009
//
// Classe XPackBitsCodec : compresseur / Decompresseur PackBits
//-----------------------------------------------------------------------------

#ifndef XPACKBITSCODEC_H
#define XPACKBITSCODEC_H

#include "../XTool/XBase.h"

class XPackBitsCodec {
public:
	XPackBitsCodec() { ; }
	virtual ~XPackBitsCodec() { ; }

	bool Decompress(uint8_t* lzw, uint32_t size_in, uint8_t* out, uint32_t size_out);
};

#endif // XPACKBITSCODEC_H

