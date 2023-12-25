//-----------------------------------------------------------------------------
//								XWebPCodec.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 19/12/2022
//-----------------------------------------------------------------------------

#ifndef XWEBPCODEC_H
#define XWEBPCODEC_H

#include <fstream>
#include <iostream>

extern "C" {
#include "../libwebp-1.2.4/src/webp/decode.h"
}

#include "../XTool/XBase.h"

class XWebPCodec {

public:
	XWebPCodec() { ; }
	virtual ~XWebPCodec() { ; }

	bool Decompress(uint8_t* webp, uint32_t size_in, uint8_t* out, uint32_t size_out, uint32_t lineW);

};


#endif //XWEBPCODEC_H