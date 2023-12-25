//-----------------------------------------------------------------------------
//								XWebPCodec.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 19/12/2022
//-----------------------------------------------------------------------------

#include "XWebPCodec.h"

bool XWebPCodec::Decompress(uint8_t* webp, uint32_t size_in, uint8_t* out, uint32_t size_out, uint32_t lineW)
{
	uint8_t* output = WebPDecodeRGBInto(webp, size_in, out, size_out, lineW);
  if (output != NULL)
		return true;
	return false;
}
