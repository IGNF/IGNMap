//-----------------------------------------------------------------------------
//								XZlibCodec.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 10/06/2021
//-----------------------------------------------------------------------------

#ifndef XZLIBCODEC_H
#define XZLIBCODEC_H

#include "../XTool/XBase.h"

class XZlibCodec {
public:
	XZlibCodec() { ; }
	virtual ~XZlibCodec() { ; }

	bool Decompress(uint8_t* lzw, uint32_t size_in, uint8_t* out, uint32_t size_out);
};

#endif //XZLIBCODEC_H