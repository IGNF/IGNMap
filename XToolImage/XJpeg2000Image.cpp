//-----------------------------------------------------------------------------
//								XJpeg2000Image.cpp
//								==================
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 21/06/2021
//-----------------------------------------------------------------------------

#ifdef KAKADU_LIB
#include "kdu_file_io.h"
#include "kdu_region_compositor.h"

#include "XJpeg2000Image.h"
#include "../XTool/XParserXML.h"
#include <sstream>
#include "XTiffReader.h"

// Include Kakadu
/*
#undef uint8_t
#include "kdu_file_io.h"
#include "kdu_region_compositor.h"

#ifndef uint8_t
#define uint8_t unsigned char
#endif
*/

class XKduRegionCompositor : public kdu_region_compositor {
public:
  virtual kdu_compositor_buf* allocate_buffer(kdu_coords min_size, kdu_coords& actual_size,
    bool read_access_required);
  virtual void delete_buffer(kdu_compositor_buf* buf) { delete buf; }
};


//-----------------------------------------------------------------------------
// Gestion des messages d'erreur
//-----------------------------------------------------------------------------
class cjp2_message : public kdu_message {
public: // Member classes
  cjp2_message() { ; }
  void put_text(const char* string) { ; }
  void flush(bool end_of_message = false) { if (end_of_message) throw (int)1; }
};

static cjp2_message error_message;
static kdu_message_formatter cjp2_error(&error_message);

//-----------------------------------------------------------------------------
// Classe XKduRegionCompositor : pour allouer les buffers en flottants
//-----------------------------------------------------------------------------
kdu_compositor_buf* XKduRegionCompositor::allocate_buffer(kdu_coords min_size, kdu_coords& actual_size,
                                                          bool read_access_required)
{
  kdu_compositor_buf* buf = new kdu_compositor_buf;
  float* pix = new float[min_size.x * min_size.y * 4L];
  if (pix == NULL)
    return NULL;
  buf->init_float(pix, min_size.x * 4L);
  buf->set_read_accessibility(read_access_required);
  actual_size = min_size;
  return buf;
}

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XJpeg2000Image::XJpeg2000Image(const char* filename)
{
  kdu_customize_errors(&cjp2_error);
  m_strFilename = filename;

  m_Compositor = new kdu_region_compositor;
  m_Jpx_in = new jpx_source;
  m_Src = new jp2_family_src;
  m_Src->open(filename);

  int result;
  try {
    result = m_Jpx_in->open(m_Src, true);
  }
  catch (int) {
    result = -1;
  }

  if (result >= 0)
  { // We have a JP2/JPX-compatible file
    m_Compositor->create(m_Jpx_in);
    //m_Compositor->add_compositing_layer(0,kdu_dims(),kdu_dims());
    m_Compositor->add_ilayer(0, kdu_dims(), kdu_dims());
  }
  else {
    m_bValid = false;
    delete m_Compositor;
    delete m_Jpx_in;
    delete m_Src;
    return;
  }

  m_Compositor->set_surface_initialization_mode(false);
  m_Compositor->set_scale(false, false, false, 1.0F);

  kdu_dims image_dims;
  m_Compositor->get_total_composition_dims(image_dims);
  m_nW = image_dims.size.x;
  m_nH = image_dims.size.y;
  m_bValid = true;
  // Nombre de canaux
  jpx_layer_source jpx_layer = m_Jpx_in->access_layer(0);
  jp2_channels channels = jpx_layer.access_channels();
  if (channels.get_num_colours() == 1)
    m_nNbSample = 1;
  else
    m_nNbSample = 3;
  m_nNbBits = 8;

  jpx_codestream_source code_source = m_Jpx_in->access_codestream(0);
  jp2_dimensions dim = code_source.access_dimensions();
  m_nBitDepth = dim.get_bit_depth(0);
  if ((m_nBitDepth > 8)&&(m_nBitDepth <= 16)){
    m_nNbBits = 16;
  }

  if (m_nBitDepth > 8) {
    m_RawCompositor = new XKduRegionCompositor;
    m_RawCompositor->create(m_Jpx_in);
    m_RawCompositor->add_ilayer(0, kdu_dims(), kdu_dims());
    m_RawCompositor->set_surface_initialization_mode(false);
    m_RawCompositor->set_scale(false, false, false, 1.0F);
    m_FloatCompositor = new XKduRegionCompositor;
    m_FloatCompositor->create(m_Jpx_in);
    m_FloatCompositor->add_ilayer(0, kdu_dims(), kdu_dims());
    m_FloatCompositor->set_surface_initialization_mode(false);
    m_FloatCompositor->set_scale(false, false, false, 1.0F);
    delete m_Compositor;
    m_Compositor = nullptr;
  }
  else
    m_RawCompositor = m_FloatCompositor = nullptr;

  m_dX0 = 0.;	// A revoir avec le XML
  m_dY0 = 0.;
  m_dGSD = 0.;
  // Lecture des metadonnees
  jpx_meta_manager meta = m_Jpx_in->access_meta_manager();
  jpx_metanode root = meta.access_root();
  int count, num_bytes;
  root.count_descendants(count);
  char text[512];
  // Metadonnees XML
  std::string xml;
  for (int i = 0; i < count; i++) {
    jpx_metanode child = root.get_descendant(i);
    if (child.get_box_type() != jp2_xml_4cc)
      continue;
    jp2_input_box box;
    if (!child.open_existing(box))
      continue;
    while ((num_bytes = box.read((kdu_byte*)text, 256)) > 0) {
      for (int j = 0; j < num_bytes; j++)
        if ((text[j] == '\0') || ((text[j] == '\r') || (text[j] == '\t') || (text[j] == '\n'))) text[j] = ' ';
      text[num_bytes] = '\0';
      xml += text;
    }
    break;
  }
  m_strXmlMetadata = xml;
  ReadGeorefXmlOld();

  // Metadonnees Label
  for (int i = 0; i < count; i++) {
    jpx_metanode child = root.get_descendant(i);
    if (child.get_box_type() != jp2_label_4cc)
      continue;
    if (strcmp(child.get_label(), "gml.data") != 0)
      continue;
    int nb_des = 0;
    child.count_descendants(nb_des);
    for (int j = 0; j < nb_des; j++) {
      jpx_metanode son = child.get_descendant(j);
      jpx_metanode link = son.get_descendant(0);
      if (link.get_box_type() != jp2_xml_4cc)
        continue;
      jp2_input_box box;
      if (!link.open_existing(box))
        continue;
      xml = "";
      while ((num_bytes = box.read((kdu_byte*)text, 256)) > 0) {
        for (int j = 0; j < num_bytes; j++)
          if ((text[j] == '\0') || ((text[j] == '\r') || (text[j] == '\t') || (text[j] == '\n'))) text[j] = ' ';
        text[num_bytes] = '\0';
        xml += text;
      }
      break;
    } // endfor j
    m_strXmlMetadata = xml;
    if (ReadGeorefXml())
      break;
  } // endfor i

  // Metadonnees UUID
  for (int i = 0; i < count; i++) {
    jpx_metanode child = root.get_descendant(i);
    if (child.get_box_type() != jp2_uuid_4cc)
      continue;
    jp2_input_box box;
    if (!child.open_existing(box))
      continue;
    int header_length = box.get_box_header_length();
    box.read((kdu_byte*)text, 2 * header_length);
    int box_size = box.get_remaining_bytes();
    if (box_size > 0) {
      char* uuid = new char[box_size];
      if (uuid != NULL) {
        num_bytes = box.read((kdu_byte*)uuid, box_size);
        if (num_bytes > 0) {
          std::string data_uuid(uuid, num_bytes);
          std::istringstream istr(data_uuid);
          ReadGeorefUuid(&istr);
        }
        delete[] uuid;
      }
    }
    break;
  }
  ClearCodec();
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XJpeg2000Image::~XJpeg2000Image()
{
  ClearCodec();
  m_bValid = false;
}

//-----------------------------------------------------------------------------
// Creation du codec de lecture
//-----------------------------------------------------------------------------
bool XJpeg2000Image::CreateCodec()
{
  m_Jpx_in = new jpx_source;
  m_Src = new jp2_family_src;
  m_Src->open(m_strFilename.c_str());

  int result;
  try {
    result = m_Jpx_in->open(m_Src, true);
  }
  catch (int) {
    result = -1;
  }

  if (result < 0) {
    m_bValid = false;
    delete m_Jpx_in;
    delete m_Src;
    m_Src = nullptr;
    m_Jpx_in = nullptr;
    return false;
  }

  if (m_nBitDepth > 8) {
    m_RawCompositor = new XKduRegionCompositor;
    m_RawCompositor->create(m_Jpx_in);
    m_RawCompositor->add_ilayer(0, kdu_dims(), kdu_dims());
    m_RawCompositor->set_surface_initialization_mode(false);
    m_RawCompositor->set_scale(false, false, false, 1.0F);
    m_FloatCompositor = new XKduRegionCompositor;
    m_FloatCompositor->create(m_Jpx_in);
    m_FloatCompositor->add_ilayer(0, kdu_dims(), kdu_dims());
    m_FloatCompositor->set_surface_initialization_mode(false);
    m_FloatCompositor->set_scale(false, false, false, 1.0F);
  }
  else {
    m_Compositor = new kdu_region_compositor;
    m_Compositor->create(m_Jpx_in);
    m_Compositor->add_ilayer(0, kdu_dims(), kdu_dims());
    m_Compositor->set_surface_initialization_mode(false);
    m_Compositor->set_scale(false, false, false, 1.0F);
  }
  return true;
}

//-----------------------------------------------------------------------------
// Fermeture du codec
//-----------------------------------------------------------------------------
void XJpeg2000Image::ClearCodec()
{
  if (m_bValid) {
    if (m_Jpx_in != nullptr) {
      m_Jpx_in->close();
      delete m_Jpx_in;
      m_Jpx_in = nullptr;
    }
    if (m_RawCompositor != nullptr) delete m_RawCompositor;
    if (m_FloatCompositor != nullptr) delete m_FloatCompositor;
    m_RawCompositor = m_FloatCompositor = nullptr;
    if (m_Compositor != nullptr) delete m_Compositor;
    m_Compositor = nullptr;
    if (m_Src != nullptr) {
      m_Src->close();
      delete m_Src;
      m_Src = nullptr;
    }
  }
}

//-----------------------------------------------------------------------------
// Metadonnees de l'image sous forme de cles / valeurs
//-----------------------------------------------------------------------------
std::string XJpeg2000Image::Metadata()
{
  std::ostringstream out;
  out << XBaseImage::Metadata();
  out << "BitDepth:" << m_nBitDepth << ";";
  return out.str();
}

//-----------------------------------------------------------------------------
// Lecture d'une region
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
  return GetJp2Area(x, y, w, h, area);
}

//-----------------------------------------------------------------------------
// Recuperation d'une zone de pixels avec zoom arriere
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  uint32_t maxW = w, maxH = h;
  if ((x + w) >= m_nW)
    maxW = m_nW - x;
  if ((y + h) >= m_nH)
    maxH = m_nH - y;
  uint32_t wout = maxW / factor;
  uint32_t hout = maxH / factor;

  return GetJp2Area(x, y, maxW, maxH, area, factor, wout, hout);
}

//-----------------------------------------------------------------------------
// Lecture d'une region Jpeg2000
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetJp2Area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area,
                                uint32_t factor, uint32_t wout, uint32_t hout)
{
  if (m_nBitDepth > 8)
    return GetJp2AreaFloat(x, y, w, h, area, factor, wout, hout);
  if (!CreateCodec())
    return false;
  float k_factor = 1;
  if (factor != 1)
    k_factor = 1.F / (float)factor;
  if ((wout == 0) || (hout == 0)) {
    wout = w;
    hout = h;
  }

  kdu_dims image_dims, new_region;

  image_dims.pos.x = (int)(x * k_factor);
  image_dims.pos.y = (int)(y * k_factor);
  image_dims.size.x = (int)(w * k_factor);
  image_dims.size.y = (int)(h * k_factor);

  m_Compositor->refresh();
  m_Compositor->set_scale(false, false, false, (float)k_factor);
  m_Compositor->set_buffer_surface(image_dims);
  kdu_compositor_buf* buf = m_Compositor->get_composition_buffer(image_dims);
  if (buf == NULL) {  // Allocation impossible : on charge une image dezoommee
    k_factor = m_Compositor->find_optimal_scale(image_dims, (float)k_factor, (float)k_factor, 1.0);
    uint32_t newFactor = (uint32_t)(1. / k_factor);
    uint32_t newWout = (uint32_t)(k_factor * w);
    uint32_t newHout = (uint32_t)(k_factor * h);
    uint8_t* tmparea = new uint8_t[newWout * newHout * NbSample()];
    if (tmparea == NULL) {
      ClearCodec();
      return false;
    }
    bool flag = GetJp2Area(x, y, w, h, tmparea, newFactor, newWout, newHout);
    ZoomArea(tmparea, area, newWout, newHout, wout, hout, NbSample());
    delete[] tmparea;
    return flag;
  }
  if ((image_dims.size.x < (int)wout) || (image_dims.size.y < (int)hout)) {
    ClearCodec();
    return false;
  }
  int buffer_row_gap = 0;
  kdu_uint32* buffer = buf->get_buf(buffer_row_gap, false);
  while (m_Compositor->process(100000, new_region));

  kdu_uint32* ptr = buffer;
  unsigned char* data;
  uint32_t nbbyte = NbSample();
  uint32_t count = (nbbyte * wout);
  for (uint32_t i = 0; i < hout; i++) {
    ptr = &buffer[buffer_row_gap * i];
    data = &area[count * i];
    for (uint32_t j = 0; j < wout; j++) {
      ::memcpy(data, ptr, nbbyte);
      ptr++;
      data += nbbyte;
    }
  }
  if (NbSample() == 3)
    SwitchRGB2BGR(area, wout * hout);
  ClearCodec();
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une region Jpeg2000 stockee en float avec passage en 16 bits
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetJp2AreaFloat(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area,
                                     uint32_t factor, uint32_t wout, uint32_t hout)
{
  if (!CreateCodec())
    return false;
  float k_factor = 1;
  if (factor != 1)
    k_factor = 1.F / (float)factor;
  if ((wout == 0) || (hout == 0)) {
    wout = w;
    hout = h;
  }

  kdu_dims image_dims, new_region;

  image_dims.pos.x = (int)(x * k_factor);
  image_dims.pos.y = (int)(y * k_factor);
  image_dims.size.x = (int)(w * k_factor);
  image_dims.size.y = (int)(h * k_factor);

  m_FloatCompositor->refresh();
  m_FloatCompositor->set_scale(false, false, false, (float)k_factor);
  m_FloatCompositor->set_buffer_surface(image_dims);
  kdu_compositor_buf* buf = m_FloatCompositor->get_composition_buffer(image_dims);
  if ((buf == nullptr) || (image_dims.size.x < (int)wout) || (image_dims.size.y < (int)hout)) {
    ClearCodec();
    return false;
  }
  int buffer_row_gap = 0;
  float* buffer = buf->get_float_buf(buffer_row_gap, false);
  while (m_FloatCompositor->process(100000, new_region));

  float* ptr = buffer;
  uint16_t* data = (uint16_t*)area;
  uint32_t nbbyte = NbSample();
  uint32_t count = (nbbyte * wout);
  for (uint32_t i = 0; i < hout; i++) {
    ptr = &buffer[buffer_row_gap * i];
    for (uint32_t j = 0; j < wout; j++) {
      ptr++;  // Canal alpha
      for (uint32_t k = 0; k < nbbyte; k++) {
        data[k] = *ptr * 65535; ptr++;
      }
      data += nbbyte;
      ptr += (3 - nbbyte);  // Les images traitees par Kakadu sont toujours sur 4 octets
    }
  }
  //if (NbSample() == 3)
  //  SwitchRGB2BGR(area, wout* hout);
  ClearCodec();
  return true;
}

//-----------------------------------------------------------------------------
// Recuperation des valeurs brutes des pixels
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetRawPixel(XFile* file, uint32_t x, uint32_t y, uint32_t win, double* pix,
  uint32_t* nb_sample)
{
  if (m_nBitDepth <= 8)
    return XBaseImage::GetRawPixel(file, x, y, win, pix, nb_sample);
  if (!CreateCodec())
    return false;

  if ((win > x) || (win > y))
    return false;
  if ((x + win >= m_nW) || (y + win >= m_nH))
    return false;

  kdu_dims image_dims, new_region;

  image_dims.pos.x = (int)(x - win);
  image_dims.pos.y = (int)(y - win);
  image_dims.size.x = (int)(2 * win + 1);
  image_dims.size.y = (int)(2 * win + 1);

  m_RawCompositor->refresh();
  m_RawCompositor->set_scale(false, false, false, 1.f);
  m_RawCompositor->set_buffer_surface(image_dims);
  kdu_compositor_buf* buf = m_RawCompositor->get_composition_buffer(image_dims);
  if (buf == nullptr) {
    ClearCodec();
    return false;
  }
  int buffer_row_gap = 0;
  float* buffer = buf->get_float_buf(buffer_row_gap, false);
  while (m_RawCompositor->process(100000, new_region));

  double* ptr_pix = pix;
  float* ptr_buf = buffer;
  for (uint32_t j = 0; j < (2 * win + 1); j++) {
    for (uint32_t i = 0; i < (2 * win + 1); i++) {
      ptr_buf++;  // Canal alpha
      for (uint32_t k = 0; k < m_nNbSample; k++) {
        *ptr_pix = (*ptr_buf * 65535);
        ptr_pix++; ptr_buf++;
      }
      ptr_buf += (3 - m_nNbSample);
    }
  }

  *nb_sample = m_nNbSample;
  ClearCodec();
  return true;
}

//-----------------------------------------------------------------------------
// Recuperation des valeurs brutes des pixels
//-----------------------------------------------------------------------------
bool XJpeg2000Image::GetRawArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix,
                                uint32_t* nb_sample, uint32_t factor,  bool normalized)
{
  if (m_nBitDepth <= 8)
    return XBaseImage::GetRawArea(file, x, y, w, h, pix, nb_sample, factor, normalized);
 
  if ((x + w > m_nW) || (y + h > m_nH))
    return false;
  if (!CreateCodec())
    return false;

  float k_factor = 1;
  if (factor != 1)
    k_factor = 1.F / (float)factor;

  kdu_dims image_dims, new_region;

  image_dims.pos.x = (int)(x * k_factor);
  image_dims.pos.y = (int)(y * k_factor);
  image_dims.size.x = (int)(w * k_factor);
  image_dims.size.y = (int)(h * k_factor);

  m_RawCompositor->refresh();
  m_RawCompositor->set_scale(false, false, false, k_factor);
  m_RawCompositor->set_buffer_surface(image_dims);
  while (m_RawCompositor->process(100000, new_region));
  kdu_compositor_buf* buf = m_RawCompositor->get_composition_buffer(image_dims);
  int buffer_row_gap = 0;
  float* buffer = buf->get_float_buf(buffer_row_gap, false);
  float* ptr;
  uint32_t Wout = (uint32_t)(k_factor * w);
  uint32_t Hout = (uint32_t)(k_factor * h);
  for (uint32_t i = 0; i < Hout; i++) {
    ptr = &buffer[buffer_row_gap * i];
    for (uint32_t j = 0; j < Wout; j++) {
      ptr++; // Canal Alpha
      ::memcpy(&pix[i * Wout * m_nNbSample + j * m_nNbSample], ptr, sizeof(float) * m_nNbSample);
      ptr += 3;
    }
  }
  //::memcpy(pix, buffer, w * h * sizeof(float)*4L);

  *nb_sample = m_nNbSample;
  ClearCodec();
  return true;
}

//-----------------------------------------------------------------------------
// Lecture du geo-referencement GMLJP2 (ancienne methode)
//-----------------------------------------------------------------------------
bool XJpeg2000Image::ReadGeorefXmlOld()
{
  if (m_strXmlMetadata.length() < 1)
    return false;
  std::istringstream xml;
  xml.str(m_strXmlMetadata.c_str());
  XParserXML parser;
  if (!parser.Parse(&xml))
    return false;
  std::string origin = parser.ReadNode("/JPEG2000_GeoLocation/gml:RectifiedGrid/gml:origin/gml:Point/gml:coordinates");
  if (origin.size() < 1)
    return false;
  std::vector<std::string> vec;
  if (parser.ReadArrayNode("/JPEG2000_GeoLocation/gml:RectifiedGrid/gml:offsetVector", &vec) < 1)
    return false;

  double x0, y0, x, y, dx = 0., dy = 0.;
  sscanf(origin.c_str(), "%lf,%lf", &x0, &y0);
  for (uint32_t i = 0; i < vec.size(); i++) {
    sscanf(vec[i].c_str(), "%lf,%lf", &x, &y);
    dx += x;
    dy += y;
  }
  if (fabs(fabs(dx) - fabs(dy)) > 0.1)
    return false;
  if (dx > 0.)
    m_dX0 = x0;
  else
    m_dX0 = x0 + m_nW * dx;
  if (dy > 0.)
    m_dY0 = y0;
  else
    m_dY0 = y0 + m_nH * dy;

  m_dGSD = fabs(dx);
  return true;
}

//-----------------------------------------------------------------------------
// Lecture du geo-referencement GMLJP2 (nouvelle methode)
//-----------------------------------------------------------------------------
bool XJpeg2000Image::ReadGeorefXml()
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

//-----------------------------------------------------------------------------
// Lecture du geo-referencement GEOJP2
//-----------------------------------------------------------------------------
bool XJpeg2000Image::ReadGeorefUuid(std::istream* uuid)
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
#endif  // KAKADU_LIB
