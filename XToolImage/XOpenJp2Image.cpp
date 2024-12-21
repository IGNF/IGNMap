//-----------------------------------------------------------------------------
//								XOpenJp2Image.cpp
//								=================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 17/08/2023
//-----------------------------------------------------------------------------

#ifdef OPJ_STATIC

#include <fstream>
#include <sstream>
#include <cstring>
#include "../XTool/XEndian.h"
#include "../XTool/XParserXML.h"
#include "XOpenJp2Image.h"
#include "XTiffReader.h"

//-----------------------------------------------------------------------------
// Gestion des messages d'erreur
//-----------------------------------------------------------------------------

// sample error callback expecting a FILE* client object
static void error_callback(const char* /*msg*/, void* client_data)
{
  (void)client_data;
  //fprintf(stdout, "[ERROR] %s", msg);
}
// sample warning callback expecting a FILE* client object
static void warning_callback(const char* /*msg*/, void* client_data)
{
  (void)client_data;
  //fprintf(stdout, "[WARNING] %s", msg);
}
// sample debug callback expecting no client object
static void info_callback(const char* /*msg*/, void* client_data)
{
  (void)client_data;
  //fprintf(stdout, "[INFO] %s", msg);
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XOpenJp2Image::XOpenJp2Image(const char* filename)
{
  m_bValid = false;
  m_strFilename = filename;

  if (!CreateCodec())
    return;

  m_nW = m_Image->x1 - m_Image->x0;
  m_nH = m_Image->y1 - m_Image->y0;

  m_nNbSample = (uint16_t)m_Image->numcomps;
  m_nNbBits = (uint16_t)m_Image->comps[0].prec;
  if ((m_nNbBits > 8) && (m_nNbBits <= 16))
    m_nNbBits = 16;

  m_bValid = true;
  ClearCodec();

  FindGeorefUuidBox(filename);
  FindGeorefXmlBox(filename);
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XOpenJp2Image::~XOpenJp2Image()
{
  ClearCodec();
}

//-----------------------------------------------------------------------------
// Reinitialisation du codec
//-----------------------------------------------------------------------------
void XOpenJp2Image::ClearCodec()
{
  if (m_bValid) {
    opj_destroy_codec(m_Codec);
    m_Codec = nullptr;
    opj_stream_destroy(m_Stream);
    m_Stream = nullptr;
    opj_image_destroy(m_Image);
    m_Image = nullptr;
  }
}

//-----------------------------------------------------------------------------
// Creation d'un codec de decompression
//-----------------------------------------------------------------------------
bool XOpenJp2Image::CreateCodec()
{
  ClearCodec();
  m_Stream = opj_stream_create_default_file_stream(m_strFilename.c_str(), OPJ_TRUE);
  if (m_Stream == nullptr)
    return false;

  m_Codec = opj_create_decompress(OPJ_CODEC_JP2);
  if (m_Codec == nullptr) {
    opj_stream_destroy(m_Stream);
    m_Stream = nullptr;
    return false;
  }
  //register callbacks
  opj_set_info_handler(m_Codec, info_callback, 00);
  opj_set_warning_handler(m_Codec, warning_callback, 00);
  opj_set_error_handler(m_Codec, error_callback, 00);

  opj_dparameters_t parameters;
  opj_set_default_decoder_parameters(&parameters);
  if (!opj_setup_decoder(m_Codec, &parameters)) {
    opj_destroy_codec(m_Codec);
    m_Codec = nullptr;
    opj_stream_destroy(m_Stream);
    m_Stream = nullptr;
    return false;
  }
  opj_codec_set_threads(m_Codec, 4);
  opj_decoder_set_strict_mode(m_Codec, OPJ_TRUE);

  if (!opj_read_header(m_Stream, m_Codec, &m_Image)) {
    opj_destroy_codec(m_Codec);
    m_Codec = nullptr;
    opj_stream_destroy(m_Stream);
    m_Stream = nullptr;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une region
//-----------------------------------------------------------------------------
bool XOpenJp2Image::GetArea(XFile* , uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
  if (!m_bValid)
    return false;
  //if (m_Image->comps[0].factor != 0) {  // Le codec est a reconstruire
  //  ClearCodec();
   if (!CreateCodec())
    return false;
  //}
  opj_set_decoded_resolution_factor(m_Codec, 0);
  opj_set_decode_area(m_Codec, m_Image, x, y, x+w, y+h);
  if (!opj_decode(m_Codec, m_Stream, m_Image))
    return false;

  int nb_byte = m_nNbBits / 8;
  uint8_t* ptr = area;
  for (uint32_t i = 0; i < w * h; i++) {
    for (uint32_t j = 0; j < m_Image->numcomps; j++) {
      //*ptr = m_Image->comps[j].data[i];
      memcpy(ptr, &(m_Image->comps[j].data[i]), nb_byte);
      ptr += nb_byte;
    }
  }
  
  //ClearCodec();
  return true;
}

//-----------------------------------------------------------------------------
// Recuperation d'une zone de pixels avec zoom arriere
//-----------------------------------------------------------------------------
bool XOpenJp2Image::GetZoomArea(XFile* , uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  if (!m_bValid)
    return false;

  uint32_t maxW = w, maxH = h;
  if ((x + w) >= m_nW)
    maxW = m_nW - x;
  if ((y + h) >= m_nH)
    maxH = m_nH - y;
  uint32_t wout = maxW / factor;
  uint32_t hout = maxH / factor;
  uint32_t opj_factor = 0, fac = factor;
  do {
    opj_factor++;
    fac /= 2;
  } while (fac > 1);

   if (!CreateCodec())
     return false;
  
   bool flag = false;
  for (uint32_t i = 0; i < opj_factor; i++) {
    flag = opj_set_decoded_resolution_factor(m_Codec, opj_factor);
    if (flag) break;// Le niveau de zoom existe bien
    opj_factor -= 1;
  }
  if (!flag)
    return false;
  if (!opj_set_decode_area(m_Codec, m_Image, x, y, x + maxW, y + maxH))
    return false;
  if (!opj_decode(m_Codec, m_Stream, m_Image))
    return false;

  uint32_t* lut = new uint32_t[wout];
  for (uint32_t i = 0; i < wout; i++)
    lut[i] = (i * m_Image->comps[0].w / wout);

  uint8_t* ptr = area;
  OPJ_INT32** srcPix = new OPJ_INT32*[m_nNbSample];
  int nb_byte = m_nNbBits / 8;

  for (uint32_t i = 0; i < hout; i++) {
    uint32_t pos = m_Image->comps[0].w * (i * m_Image->comps[0].h / hout);
    for (uint32_t j = 0; j < m_nNbSample; j++)
      srcPix[j] = &m_Image->comps[j].data[pos];
    for (uint32_t k = 0; k < wout; k++) {
      for (uint32_t j = 0; j < m_nNbSample; j++) {
        //*ptr = srcPix[j][lut[k]]; ptr++;
        memcpy(ptr, &(srcPix[j][lut[k]]), nb_byte);
        ptr += nb_byte;
      }
    }
  }
  delete[] lut;
  delete[] srcPix;

  ClearCodec(); 
  
  return true;
}

//-----------------------------------------------------------------------------
// Lecture du geo-referencement GEOJP2
//-----------------------------------------------------------------------------
bool XOpenJp2Image::ReadGeorefUuid(std::istream* uuid)
{
  XTiffReader reader;
  if (!reader.Read(uuid))
    return false;
  reader.AnalyzeIFD(uuid);
  m_dX0 = reader.X0();
  m_dY0 = reader.Y0();
  m_dGSD = reader.GSD();
  return true;
}

//-----------------------------------------------------------------------------
// Recherche de la box UUID contenant du geo-referencement GEOJP2
//-----------------------------------------------------------------------------
bool XOpenJp2Image::FindGeorefUuidBox(const char* filename)
{
  // Identifiant MrSID des box GeoJP2
  static const char mrsid_uuid[16] = { (char)0xb1, (char)0x4b, (char)0xf8, (char)0xbd, (char)0x08, (char)0x3d,
                                       (char)0x4b, (char)0x43, (char)0xa5, (char)0xae, (char)0x8c, (char)0xd7,
                                       (char)0xd5, (char)0xa6, (char)0xce, (char)0x03 };

  std::ifstream in;
  in.open(filename, std::ios_base::binary | std::ifstream::in);
  if (!in.good())
    return false;

  do {
    Jp2Box box;
    if (!ReadJp2Box(&in, box))
      return false;

    if (box.box_type == 0x75756964) { // uuid box
      char ID[16];
      in.read(ID, 16);
      if (strncmp(mrsid_uuid, ID, 16) != 0) {
        in.seekg(box.data_size - 16, std::ios_base::cur);
        continue;
      }
      
      char* buf = new char[box.data_size - 16];
      in.read(buf, box.data_size - 16);
      std::string data_uuid(buf, box.data_size - 16);
      std::istringstream istr(data_uuid);
      bool flag = ReadGeorefUuid(&istr);
      delete[] buf;
      return flag;
    }
    in.seekg(box.data_size, std::ios_base::cur);
    if (!in.good())
      break;
  } while (true);
  return false;
}

//-----------------------------------------------------------------------------
// Recherche de la box XML contenant du geo-referencement GEOJP2
//-----------------------------------------------------------------------------
bool XOpenJp2Image::FindGeorefXmlBox(const char* filename)
{
  std::ifstream in;
  in.open(filename, std::ios_base::binary | std::ifstream::in);
  if (!in.good())
    return false;

  do {
    Jp2Box root;
    if (!ReadJp2Box(&in, root))
      return false;
    if (root.box_type != 0x61736f63) { // jp2_association_4cc box
      in.seekg(root.data_size, std::ios_base::cur);
      continue;
    }
    Jp2Box label_root;
    if (!ReadJp2Box(&in, label_root))
      return false;
    if (label_root.box_type != 0x6c626c20) { //jp2_label_4cc box
      in.seekg(label_root.data_size, std::ios_base::cur);
      continue;
    }
    char* label = new char[label_root.data_size];
    in.read(label, label_root.data_size);
    int result = strncmp("gml.data", label, 8);
    delete[] label;
    if (result != 0)
      continue;

    Jp2Box assoc_gml;
    if (!ReadJp2Box(&in, assoc_gml))
      return false;
    if (assoc_gml.box_type != 0x61736f63) { // jp2_association_4cc box
      in.seekg(assoc_gml.data_size, std::ios_base::cur);
      continue;
    }

    Jp2Box label_gml;
    if (!ReadJp2Box(&in, label_gml))
      return false;
    if (label_gml.box_type != 0x6c626c20) { //jp2_label_4cc box
      in.seekg(label_gml.data_size, std::ios_base::cur);
      continue;
    }
    label = new char[label_gml.data_size];
    in.read(label, label_gml.data_size);
    result = strncmp("gml.root-instance", label, 17);
    delete[] label;
    if (result != 0)
      continue;

    Jp2Box xml_box;
    if (!ReadJp2Box(&in, xml_box))
      return false;
    if (xml_box.box_type != 0x786d6c20) { //jp2_xml_4cc box
      in.seekg(xml_box.data_size, std::ios_base::cur);
      continue;
    }

    char* buf = new char[xml_box.data_size];
    in.read(buf, xml_box.data_size);
    m_strXmlMetadata = std::string(buf, xml_box.data_size);
    delete[] buf;
    return ReadGeorefXml();
  } while (in.good());
  return false;
}

//-----------------------------------------------------------------------------
// Lecture d'une box JPEG2000
//-----------------------------------------------------------------------------
bool XOpenJp2Image::ReadJp2Box(std::istream* in, Jp2Box& box)
{
  XEndian endian;
  endian.Read(in, false, &box.box_size, 4);
  endian.Read(in, false, &box.box_type, 4);
  if (box.box_size == 0)
    return false;
  if (box.box_size == 1) {  // Presence d'une taille etendue
    endian.Read(in, false, &box.box_ext_size, 8);
    box.data_size = box.box_ext_size - 16;
  }
  else
    box.data_size = (uint64_t)box.box_size - 8;
  return true;
}

//-----------------------------------------------------------------------------
// Lecture du geo-referencement GMLJP2 (nouvelle methode)
//-----------------------------------------------------------------------------
bool XOpenJp2Image::ReadGeorefXml()
{
  if (m_strXmlMetadata.length() < 1)
    return false;
  std::istringstream xml;
  xml.str(m_strXmlMetadata.c_str());
  XParserXML parser;
  if (!parser.Parse(&xml))
    return false;
  std::string origin = parser.ReadNode("/gml:FeatureCollection/gml:featureMember/gml:FeatureCollection/gml:featureMember/gml:RectifiedGridCoverage/gml:rectifiedGridDomain/gml:RectifiedGrid/gml:origin/gml:Point/gml:pos");
  if (origin.size() < 1)
    return false;

  std::vector<std::string> vec;
  if (parser.ReadArrayNode("/gml:FeatureCollection/gml:featureMember/gml:FeatureCollection/gml:featureMember/gml:RectifiedGridCoverage/gml:rectifiedGridDomain/gml:RectifiedGrid/gml:offsetVector", &vec) < 1)
    return false;

  double x0, y0, x, y, dx = 0., dy = 0.;
  sscanf(origin.c_str(), "%lf %lf", &x0, &y0);
  for (uint32_t i = 0; i < vec.size(); i++) {
    sscanf(vec[i].c_str(), "%lf %lf", &x, &y);
    dx += x;
    dy += y;
  }
  if (fabs(fabs(dx) - fabs(dy)) > 0.1)
    return false;
  if (dx > 0.)
    m_dX0 = x0;
  else
    m_dX0 = x0 + m_nW * dx;
  if (dy < 0.)
    m_dY0 = y0;
  else
    m_dY0 = y0 + m_nH * dy;

  m_dGSD = fabs(dx);
  m_dX0 -= m_dGSD * 0.5;  // On rajoute un demi-pixel
  m_dY0 += m_dGSD * 0.5;
  return true;
}

#endif //OPJ_STATIC