//-----------------------------------------------------------------------------
//								XCogImage.cpp
//								=============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 24/06/2021
//-----------------------------------------------------------------------------

#include "XCogImage.h"
#include <sstream>

#include "XBigTiffReader.h"
#include "XTiffReader.h"

//-----------------------------------------------------------------------------
// Ouverture d'une image COG
//-----------------------------------------------------------------------------
void XCogImage::Clear()
{
  m_nW = m_nH = 0;
  m_nNbBits = m_nNbSample = 0;
  m_dX0 = m_dY0 = m_dGSD = 0.;
  m_nColorMapSize = 0;
  m_ColorMap = NULL;  // L'image COG ne possede pas sa ColorMap
  m_Factor.clear();
  for (uint32_t i = 0; i < m_TImages.size(); i++)
    delete m_TImages[i];
  m_TImages.clear();
  if (m_Reader != NULL)
    delete m_Reader;
  m_Reader = NULL;
}

//-----------------------------------------------------------------------------
// Metadonnees de l'image sous forme de cles / valeurs
//-----------------------------------------------------------------------------
std::string XCogImage::Metadata()
{
  std::ostringstream out;
  out << XBaseImage::Metadata();
  out << "Nb Images:" << m_TImages.size() << ";";
  for (uint32_t i = 0; i < m_TImages.size(); i++)
    out << m_TImages[i]->Metadata();
  return out.str();
}

//-----------------------------------------------------------------------------
// Ouverture d'une image COG
//-----------------------------------------------------------------------------
bool XCogImage::Open(XFile* file)
{
  Clear();
  XTiffReader* tiffReader = new XTiffReader;
  XBigTiffReader* bigReader = new XBigTiffReader;

  // Ouverture du fichier et analyse de l'entete
  if (!tiffReader->Read(file->IStream())) { // Pas du TIFF classique
    delete tiffReader;
    file->Seek(0);
    if (!bigReader->Read(file->IStream())) { // Pas du BigTIFF
      delete bigReader;
      return false;
    }
    else
      m_Reader = bigReader;
  }
  else
    m_Reader = tiffReader;

  if (m_Reader->NbIFD() < 2) {// Image TIFF standard
    Clear();
    return false;
  }

  // Analyse des IFD
  m_Reader->SetActiveIFD(0);
  if (!m_Reader->AnalyzeIFD(file->IStream())) {
    Clear();
    return false;
  }
  if (m_Reader->RowsPerStrip() > 0) {  // Image en strip => pas du COG
    Clear();
    return false;
  }

  m_nW = m_Reader->Width();
  m_nH = m_Reader->Height();
  m_nNbBits = m_Reader->NbBits();
  m_nNbSample = m_Reader->NbSample();
  m_dX0 = m_Reader->X0();
  m_dY0 = m_Reader->Y0();
  m_dGSD = m_Reader->GSD();

  m_Factor.push_back(1);
  std::vector<uint32_t> T;  // Index des IFD qui nous interessent
  T.push_back(0);
  for (uint32_t i = 1; i < m_Reader->NbIFD(); i++) {
    m_Reader->SetActiveIFD(i);
    if (!m_Reader->AnalyzeIFD(file->IStream()))
      break;
    if ((m_Reader->Width() == 0) || (m_Reader->Height() == 0))
      break;
    double factorW = ((double)m_nW / (double)m_Reader->Width());
    double factorH = ((double)m_nH / (double)m_Reader->Height());
    if (fabs(factorW - factorH) > 10.) {
      break;
    }
    if (m_Reader->IsTransparencyMask()) // Pour l'instant, on ne gere pas les masques de transparence
      continue;
    if (!m_Reader->IsSubResolution()) // Nouvelle image dans le fichier ???
      break;
    T.push_back(i);
    m_Factor.push_back((uint32_t)XRint(factorW));
  }

  // Creation des images
  for (uint32_t i = 0; i < m_Factor.size(); i++) {
    XTiffTileImage* image = new XTiffTileImage;
    if (image == NULL) {
      Clear();
      return false;
    }
    m_Reader->SetActiveIFD(T[i]);
    m_Reader->AnalyzeIFD(file->IStream());
    if (!image->SetTiffReader(m_Reader)) {
      delete image;
      Clear();
      return false;
    }
    m_TImages.push_back(image);
    m_ColorMap = image->ColorMap();
    m_nColorMapSize = image->ColorMapSize();
  }
  return true;
}

//-----------------------------------------------------------------------------
// lecture d'une ROI
//-----------------------------------------------------------------------------
bool XCogImage::GetArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area)
{
  if (m_TImages.size() < 1)
    return false;
  XTiffTileImage* image = m_TImages[0];
  if (image == NULL)
    return false;
  m_ColorMap = image->ColorMap();
  m_nColorMapSize = image->ColorMapSize();
  return image->GetArea(file, x, y, w, h, area);
}

//-----------------------------------------------------------------------------
// Ouverture d'une image COG
//-----------------------------------------------------------------------------
bool XCogImage::GetLine(XFile* file, uint32_t num, uint8_t* area)
{
  if (m_TImages.size() < 1)
    return false;
  XTiffTileImage* image = m_TImages[0];
  if (image == NULL)
    return false;
  return image->GetLine(file, num, area);
}

//-----------------------------------------------------------------------------
// Ouverture d'une image COG
//-----------------------------------------------------------------------------
bool XCogImage::GetZoomArea(XFile* file, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  if (m_TImages.size() < 1)
    return false;
  uint32_t index = 0;
  for (uint32_t i = 0; i < m_Factor.size(); i++) {
    if (factor == m_Factor[i]) {
      index = i;
      break;
    }
    if (factor > m_Factor[i])
      index = i;
  }
  if (index >= m_TImages.size())
    return false;
  XTiffTileImage* image = m_TImages[index];
  if (image == NULL)
    return false;
  m_ColorMap = image->ColorMap();
  m_nColorMapSize = image->ColorMapSize();
  if (factor == m_Factor[index])
    return image->GetArea(file, x / m_Factor[index], y / m_Factor[index], w / m_Factor[index], h / m_Factor[index],
                            area);
  uint8_t* buffer = image->AllocArea(w / m_Factor[index], h / m_Factor[index]);
  if (buffer == NULL)
    return false;
  image->GetArea(file, x / m_Factor[index], y / m_Factor[index], w / m_Factor[index], h / m_Factor[index],
                              buffer);
  ZoomArea(buffer, area, w / m_Factor[index], h / m_Factor[index], w / factor, h / factor, PixSize());
  delete[] buffer;
  return true;
}
