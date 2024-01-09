//-----------------------------------------------------------------------------
//								XJpegCodec.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 14/06/2021
//-----------------------------------------------------------------------------

#ifndef XJPEGCODEC_H
#define XJPEGCODEC_H

#include <fstream>
#include <iostream>

extern "C" {
#include "../jpeg-9e/jpeglib.h"
}

#include "../XTool/XBase.h"

class XJpegCodec {
protected:
	typedef struct {
		struct jpeg_source_mgr pub;   // public fields
		JOCTET* buffer;              // start of buffer
		boolean start_of_file;        // have we gotten any data yet?
	} my_source_mgr;

	typedef my_source_mgr* my_src_ptr;

	static void jpg_memInitSource(j_decompress_ptr cinfo) {
		my_src_ptr src = (my_src_ptr)cinfo->src;
		src->start_of_file = true;
	}

	static boolean jpg_memFillInputBuffer(j_decompress_ptr cinfo){
		my_src_ptr src = (my_src_ptr)cinfo->src;
		src->start_of_file = FALSE;
		return TRUE;
	}

	static void jpg_memSkipInputData(j_decompress_ptr cinfo, long num_bytes) {
		my_src_ptr src = (my_src_ptr)cinfo->src;
		if (num_bytes > 0) {
			src->pub.next_input_byte += (size_t)num_bytes;
			src->pub.bytes_in_buffer -= (size_t)num_bytes;
		}
	}

	static void jpg_memTermSource(j_decompress_ptr cinfo) { ; }

public:
	XJpegCodec() { ; }
	virtual ~XJpegCodec() { ; }

	bool Decompress(uint8_t* jpeg, uint32_t size_in, uint8_t* out, uint32_t size_out,
									uint8_t* tables = NULL, uint32_t tablesize = 0);
	bool DecompressRaw(uint8_t* jpeg, uint32_t size_in, uint8_t* out, uint32_t size_out,
									uint8_t* tables = NULL, uint32_t tablesize = 0);
};

#endif //XJPEGCODEC_H

