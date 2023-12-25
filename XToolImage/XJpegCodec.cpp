//-----------------------------------------------------------------------------
//								XJpegCodec.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 14/06/2021
//-----------------------------------------------------------------------------

#include "XJpegCodec.h"

//-----------------------------------------------------------------------------
// Decompression standard
//-----------------------------------------------------------------------------
bool XJpegCodec::Decompress(uint8_t* jpeg, uint32_t size_in, uint8_t* out, uint32_t size_out, 
														uint8_t* tables, uint32_t tablesize)
{
	// Lecture JPEG
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* Initialize the JPEG decompression object with default error handling. */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// Fixe les tables JPEG si necessaire
	if (tables != NULL) {
		jpeg_mem_src(&cinfo, tables, tablesize);
		(void)jpeg_read_header(&cinfo, FALSE);
	}
	jpeg_mem_src(&cinfo, jpeg, size_in);
	
	// Read file header, set default decompression parameters
	(void)jpeg_read_header(&cinfo, TRUE);
	
	// Start decompressor
	(void)jpeg_start_decompress(&cinfo);

	// Process data
	JSAMPROW* row = new JSAMPROW[cinfo.output_height];
	for (uint32_t i = 0; i < cinfo.output_height; i++)
		row[i] = (JSAMPROW)(&out[cinfo.output_width * cinfo.output_components * i]);

	int num_scanlines = 0, total = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		num_scanlines = jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&row[total], cinfo.output_height);
		total += num_scanlines;
	}
	delete[] row;

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return true;
}

//-----------------------------------------------------------------------------
// Decompression brute
//-----------------------------------------------------------------------------
bool XJpegCodec::DecompressRaw(uint8_t* jpeg, uint32_t size_in, uint8_t* out, uint32_t size_out,
															 uint8_t* tables, uint32_t tablesize)
{
	// Lecture JPEG
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* Initialize the JPEG decompression object with default error handling. */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* Specify data source for decompression */
	
	// Fixe les tables JPEG si necessaire
	if (tables != NULL) {
		jpeg_mem_src(&cinfo, tables, tablesize);
		(void)jpeg_read_header(&cinfo, FALSE);
	}

	jpeg_mem_src(&cinfo, jpeg, size_in);

	// Read file header, set default decompression parameters
	(void)jpeg_read_header(&cinfo, TRUE);
	cinfo.jpeg_color_space = JCS_UNKNOWN;
	cinfo.out_color_space = JCS_UNKNOWN;
	cinfo.raw_data_out = TRUE;
	cinfo.do_fancy_upsampling = FALSE;
	
	// Start decompressor
	(void)jpeg_start_decompress(&cinfo);

	// Raw data
	uint32_t max_lines = cinfo.max_v_samp_factor * DCTSIZE;
	
	JSAMPARRAY* bufferraw = new JSAMPARRAY[cinfo.output_components];
	JSAMPROW* bufferraw2 = new JSAMPROW[max_lines * 2];
	bufferraw[0] = &bufferraw2[0]; // Y channel rows (8 or 16)
	bufferraw[1] = &bufferraw2[max_lines]; // U channel rows (8)
	bufferraw[2] = &bufferraw2[max_lines + max_lines / 2]; // V channel rows (8)
	uint8_t* raw_buf = new uint8_t[max_lines * cinfo.max_h_samp_factor * cinfo.output_width];
	for (uint32_t i = 0; i < max_lines * cinfo.max_h_samp_factor; i++)
		bufferraw2[i] = (JSAMPROW)&raw_buf[i * cinfo.output_width];

	int num_scanlines = 0;
	uint8_t* ptr = out;

	while (cinfo.output_scanline < cinfo.output_height) {
		//num_scanlines = jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&row[total], cinfo.output_height);
		num_scanlines = jpeg_read_raw_data(&cinfo, bufferraw, max_lines);

		for (uint32_t i = 0; i < num_scanlines; i++) {
			for (uint32_t j = 0; j < cinfo.output_width; j++) {
				*ptr = bufferraw2[i][j]; ptr++;
				*ptr = bufferraw2[i / 2 + 16][j / 2]; ptr++;
				*ptr = bufferraw2[i / 2 + 24][j / 2]; ptr++;
			}
		}
	}

	delete[] raw_buf;
	delete[] bufferraw2;
	delete[] bufferraw;

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return true;
}