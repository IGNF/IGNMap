//-----------------------------------------------------------------------------
//								XJpegImage.cpp
//								==============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 09/04/2025
//-----------------------------------------------------------------------------

#include "XJpegImage.h"
#include <setjmp.h>
#include <cstring>
#include "../TinyEXIF/TinyEXIF.h"
#include "../XToolGeod/XGeoPref.h"

//-----------------------------------------------------------------------------
// Gestion des erreurs issue de la LibJpeg
//-----------------------------------------------------------------------------
struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

 // Here's the routine that will replace the standard error_exit method:

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}


//-----------------------------------------------------------------------------
// Ouverture d'une image JPEG
//-----------------------------------------------------------------------------
bool XJpegImage::Open(std::string filename)
{
	m_strFilename = filename;
	ReadExifData();
	// Lecture JPEG
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	FILE* infile = fopen(filename.c_str(), "rb");
	if (infile == NULL)
		return false;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, FALSE);

	m_nW = cinfo.image_width;
	m_nH = cinfo.image_height;
	m_nNbBits = 8;
	m_nNbSample = (uint16_t)cinfo.num_components;
	m_strFilename = filename;

	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	//ReadExifData();

	return true;
}

//-----------------------------------------------------------------------------
// lecture d'une ROI
//-----------------------------------------------------------------------------
bool XJpegImage::GetArea(XFile* , uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
	// Lecture JPEG
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	FILE* infile = fopen(m_strFilename.c_str(), "rb");
	if (infile == NULL)
		return false;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, FALSE);

	// Decompression
	(void)jpeg_start_decompress(&cinfo);

	int row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	uint32_t line = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		//if (line >= y + h)
		//	break;
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		if ((line >= y)&&(line < y+h))
			::memcpy(&area[(line - y) * w * m_nNbSample], buffer[0] + x * m_nNbSample, w * m_nNbSample);
		line++;
	}

	(void)jpeg_finish_decompress(&cinfo);

	// Nettoyage
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne
//-----------------------------------------------------------------------------
bool XJpegImage::GetLine(XFile* /*file*/, uint32_t /*num*/, uint8_t* /*area*/)
{
	return false;
}

//-----------------------------------------------------------------------------
// Ouverture d'une image COG
//-----------------------------------------------------------------------------
bool XJpegImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
	if (factor == 0) return false;
	if (factor == 1) return GetArea(file, x, y, w, h, area);
	if ((x + w > m_nW) || (y + h > m_nH))
		return false;

	uint32_t wout = w / factor;
	uint32_t hout = h / factor;
	uint8_t* pix = new uint8_t[w * h * m_nNbSample];

	bool flag = false;
	if (GetArea(file, x, y, w, h, pix))
		flag = XBaseImage::ZoomArea(pix, area, w, h, wout, hout, m_nNbSample);

	delete[] pix;

	return flag;
}

//-----------------------------------------------------------------------------
// Lecture des tags EXIF
//-----------------------------------------------------------------------------
bool XJpegImage::ReadExifData()
{
	// open a stream to read just the necessary parts of the image file
	std::ifstream stream(m_strFilename.c_str(), std::ifstream::in | std::ifstream::binary);
	if (!stream)
		return false;

	// parse image EXIF and XMP metadata
	TinyEXIF::EXIFInfo imageEXIF(stream);
	stream.close();
	if (!imageEXIF.Fields)
		return false;
	m_strExifData = "";
	if (!imageEXIF.ImageDescription.empty())
		m_strExifData += "Description : " + imageEXIF.ImageDescription + " ; ";
	if (!imageEXIF.Make.empty() || !imageEXIF.Model.empty())
		m_strExifData += "CameraModel : " + imageEXIF.Make + " - " + imageEXIF.Model + " ; ";
	if (!imageEXIF.DateTime.empty())
		m_strExifData += "DateTime : " + imageEXIF.DateTime + ";";
	if (imageEXIF.FocalLength > 0.)
		m_strExifData += "FocalLength(mm) : " + std::to_string(imageEXIF.FocalLength) + ";";

	if (imageEXIF.GeoLocation.hasLatLon()) {
		double lat = imageEXIF.GeoLocation.Latitude;
		double lon = imageEXIF.GeoLocation.Longitude;
		XGeoPref pref;
		pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), lon, lat, m_dX0, m_dY0);
		m_dGSD = 0.02;
	}
	return true;
}