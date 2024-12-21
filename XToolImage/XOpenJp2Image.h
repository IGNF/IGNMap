//-----------------------------------------------------------------------------
//								XOpenJp2Image.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 17/08/2023
//-----------------------------------------------------------------------------

#ifndef XOPENJP2IMAGE_H
#define XOPENJP2IMAGE_H

#ifdef OPJ_STATIC

#include "XBaseImage.h"
#include "../openjpeg/src/lib/openjp2/openjpeg.h"

class XOpenJp2Image : public XBaseImage {
protected:
  bool											m_bValid;	// Indique si l'image est valide
  uint32_t									m_nNumli;	// Numero de la ligne active
  std::string               m_strFilename;
  std::string							  m_strXmlMetadata;
  opj_codec_t*              m_Codec;
  opj_image_t*              m_Image;
  opj_stream_t*             m_Stream;

  //bool ReadGeorefXmlOld();
  //bool ReadGeorefXml();
  bool ReadGeorefUuid(std::istream* uuid);
  bool FindGeorefUuidBox(const char* filename);
  bool FindGeorefXmlBox(const char* filename);

  bool CreateCodec();
  void ClearCodec();

  typedef struct {
    uint32_t box_size, box_type;
    uint64_t box_ext_size, data_size;
  } Jp2Box;
  bool ReadJp2Box(std::istream* in, Jp2Box& box);
  bool ReadGeorefXml();

public:
  XOpenJp2Image(const char* filename);
  virtual ~XOpenJp2Image();

  virtual std::string Format() { return "JP2"; }
  bool IsValid() { return m_bValid; }
  virtual inline bool NeedFile() { return false; }

  virtual bool GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
  virtual bool GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
  virtual bool GetLine(XFile* /*file*/, uint32_t /*num*/, uint8_t* /*area*/) { return false; }

  virtual uint32_t FileSize() { return 0; }

  virtual std::string XmlMetadata() { return m_strXmlMetadata; }
};

#endif

#endif //OPJ_STATIC
