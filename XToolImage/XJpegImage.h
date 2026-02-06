//-----------------------------------------------------------------------------
//								XJpegImage.h
//								============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 09/04/2025
//-----------------------------------------------------------------------------

#ifndef XJPEGIMAGE_H
#define XJPEGIMAGE_H

#include <cstdio>
extern "C" {
#include "../jpeg-10/jpeglib.h"
}
#include "XBaseImage.h"


class XJpegImage : public XBaseImage {
protected:
  std::string   m_strFilename;
  std::string   m_strExifData;

  bool ReadExifData();

public:
  XJpegImage() { ; }
  virtual ~XJpegImage() { ; }

  virtual bool Open(std::string filename);
  virtual std::string Format() { return "JPEG"; }
  virtual std::string Metadata() { return m_strExifData; }

  virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
  virtual bool GetLine(XFile* /*file*/, uint32_t /*num*/, uint8_t* /*area*/);

};


#endif // XJPEGIMAGE_H