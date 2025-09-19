//-----------------------------------------------------------------------------
//								XMemRaster.cpp
//								==============
//
// Chargement d'image avec bufferisation
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/09/2025
//-----------------------------------------------------------------------------

#include "XMemRaster.h"
#include <cstring>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XMemRaster::XMemRaster()
{
  m_memArea = m_memRotArea = nullptr;
  m_memU0 = m_memV0 = 0;
  m_memW = m_memH = 0;
  m_memFactor = 1;
  m_nGrow = 1024;
  m_memArea = m_memRotArea = nullptr;
  m_memAreaSize = m_memRotSize = 0;
  m_nRot = 0;
  m_nOriX = m_nOriY = 0;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XMemRaster::~XMemRaster()
{
  if (m_memArea != nullptr)
    delete[] m_memArea;
  if (m_memRotArea != nullptr)
    delete[] m_memRotArea;
}

//-----------------------------------------------------------------------------
// Fermeture
//-----------------------------------------------------------------------------
void XMemRaster::Close()
{
  if (m_memArea != nullptr)
    delete m_memArea;
  if (m_memRotArea != nullptr)
    delete[] m_memRotArea;
  m_memArea = m_memRotArea = nullptr;
  m_memAreaSize = m_memRotSize = 0;
  m_memU0 = m_memV0 = 0;
  m_memW = m_memH = 0;
  m_memFactor = 1;
  m_nGrow = 1024;
  m_nRot = 0;
  m_nOriX = m_nOriY = 0;
  XFileImage::Close();
}

//-----------------------------------------------------------------------------
// Nombre de pixels sur l'axe des X (axe Ouest -> Est)
//-----------------------------------------------------------------------------
uint32_t XMemRaster::NbPixelX()
{
  if ((m_nRot == 1) || (m_nRot == 3))
    return Height();
  return Width();
}

//-----------------------------------------------------------------------------
// Nombre de pixels sur l'axe des Y (axe Sud -> Nord)
//-----------------------------------------------------------------------------
uint32_t XMemRaster::NbPixelY()
{
  if ((m_nRot == 1) || (m_nRot == 3))
    return Width();
  return Height();
}

//-----------------------------------------------------------------------------
// Conversion Viewport -> Image
//-----------------------------------------------------------------------------
void XMemRaster::ViewPort2Image(int& X, int& Y)
{ 
  X -= m_nOriX;
  Y -= m_nOriY;
  int u = X, v = Y;
  if (m_nRot == 1) {
    u = Width() - Y;
    v = X;
  }
  if (m_nRot == 2) {
    u = Width() - X;
    v = Height() - Y;
  }
  if (m_nRot == 3) {
    u = Y;
    v = Height() - X;
  }
  X = u;
  Y = v;
}

//-----------------------------------------------------------------------------
// Conversion Image -> Viewport
//-----------------------------------------------------------------------------
void XMemRaster::Image2Viewport(int& X, int& Y)
{ 
  int u = X, v = Y;
  if (m_nRot == 1) {
    u = Y;
    v = Width() - X;
  }
  if (m_nRot == 2) {
    u = Width() - X;
    v = Height() - Y;
  }
  if (m_nRot == 3) {
    u = Height() - Y;
    v = X;
  }
  X = u + m_nOriX;
  Y = v + m_nOriY;
}

//-----------------------------------------------------------------------------
// Charge les pixels dans le cache si necessaire
//-----------------------------------------------------------------------------
bool XMemRaster::LoadPixelInMemory(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint32_t factor)
{
  bool reload = false;
  uint32_t winMem, hinMem;

  if (m_memArea == nullptr) reload = true;
  if (startX < m_memU0) reload = true;
  if (startY < m_memV0) reload = true;
  if (factor != m_memFactor) reload = true;
  if ((startX + w) > (m_memU0 + m_memW)) reload = true;
  if ((startY + h) > (m_memV0 + m_memH)) reload = true;

  if (reload) {
    m_memFactor = factor;
    if (((int)startX - (int)m_nGrow) < 0)
      m_memU0 = 0;
    else
      m_memU0 = startX - m_nGrow;
    if (((int)startY - (int)m_nGrow) < 0)
      m_memV0 = 0;
    else
      m_memV0 = startY - m_nGrow;

    m_memW = (startX - m_memU0) + w + m_nGrow;
    if ((m_memU0 + m_memW) >= NbPixelX())
      m_memW = NbPixelX() - m_memU0;
    m_memH = (startY - m_memV0) + h + m_nGrow;
    if ((m_memV0 + m_memH) >= NbPixelY())
      m_memH = NbPixelY() - m_memV0;

    winMem = m_memW / factor;
    hinMem = m_memH / factor;
    if ((winMem < 1) || (hinMem < 1))
      return false;

    if ((m_memAreaSize < (winMem * hinMem * NbByte())) || (m_memArea == nullptr)) {
      if (m_memArea != nullptr)
        delete[] m_memArea;
      m_memArea = AllocArea(winMem, hinMem);
      if (m_memArea == nullptr)
        return false;
      m_memAreaSize = winMem * hinMem * NbByte();
    }

    bool flag = ReadPixel(m_memU0, m_memV0, m_memW, m_memH, m_memArea, m_memFactor);
    if (!flag)
      return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Recuperation des pixels
//-----------------------------------------------------------------------------
/*
bool MemRaster::GetPixel(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  uint32_t winMem, hinMem, win, hin;

  if (!LoadPixelInMemory(startX, startY, w, h, factor))
    return false;

  winMem = m_memW / factor;
  hinMem = m_memH / factor;
  win = w / factor;
  hin = h / factor;
  uint32_t Yin = (startY - m_memV0) / factor;
  uint32_t Xin = (startX - m_memU0) / factor;

  uint8_t* ptr = &m_memArea[Yin * winMem * NbByte()];
  for (uint32_t i = 0; i < hin; i++) {
    memcpy(&area[i * (win * NbByte())], &ptr[Xin * NbByte()], win * NbByte());
    ptr += winMem * NbByte();
  }
  return true;
}*/

//-----------------------------------------------------------------------------
// lecture des pixels dans le fichier
//-----------------------------------------------------------------------------
bool XMemRaster::ReadPixel(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor)
{
  if (m_nRot == 0) {
    if (factor > 1)
      return GetZoomArea(startX, startY, w, h, area, factor);
    else
      return GetArea(startX, startY, w, h, area);
  }

  bool flag = false;
  uint32_t win = w / factor;
  uint32_t hin = h / factor;
  uint32_t count = (win * NbByte()) * hin;
  if ((m_memRotArea == nullptr) || (m_memRotSize < count)) {
    if (m_memRotArea != nullptr)
      delete[] m_memRotArea;
    m_memRotArea = new (std::nothrow) uint8_t[count];
    if (m_memRotArea == nullptr)
      return false;
  }

  if (m_nRot == 2) {
    if (factor > 1)
      flag = GetZoomArea(NbPixelX() - w - startX, NbPixelY() - h - startY, w, h, area, factor);
    else
      flag = GetArea(NbPixelX() - w - startX, NbPixelY() - h - startY, w, h, area);

    RotateArea(area, m_memRotArea, win, hin, NbByte(), m_nRot);
    memcpy(area, m_memRotArea, count);
    return flag;
  }

  if (m_nRot == 1) {
    if (factor > 1)
      flag = GetZoomArea(Width() - h - startY, startX, h, w, area, factor);
    else
      flag = GetArea(Width() - h - startY, startX, h, w, area);

    RotateArea(area, m_memRotArea, hin, win, NbByte(), m_nRot);
    memcpy(area, m_memRotArea, count);
    return flag;
  }

  if (m_nRot == 3) {
    if (factor > 1)
      flag = GetZoomArea(startY, Height() - w - startX, h, w, area, factor);
    else
      flag = GetArea(startY, Height() - w - startX, h, w, area);

    RotateArea(area, m_memRotArea, hin, win, NbByte(), m_nRot);
    memcpy(area, m_memRotArea, count);
    return flag;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Definition d'une zone a visualiser (x0 ; y0) : topLeft | (x1 ; y1) : bottomRight
//-----------------------------------------------------------------------------
bool XMemRaster::SetViewport(int x0, int y0, int x1, int y1, uint32_t factor)
{
  if ((x1 < m_nOriX) || (y1 < m_nOriY))
    return false; // Pas d'intersection avec le Viewport

  uint32_t startU = 0, startV = 0;  // Position dans l'image
  if (x0 > m_nOriX)
    startU = (x0 - m_nOriX);
  if (y0 > m_nOriY)
    startV = (y0 - m_nOriY);
  if ((startU > NbPixelX())||(startV > NbPixelY()))
    return false; // Pas d'intersection avec le Viewport

  uint32_t endU = x1 - m_nOriX, endV = y1 - m_nOriY;
  if (endU > NbPixelX())
    endU = NbPixelX();
  if (endV > NbPixelY())
    endV = NbPixelY();

  uint32_t w = endU - startU;
  uint32_t h = endV - startV;

  return LoadPixelInMemory(startU, startV, w, h, factor);
}

//-----------------------------------------------------------------------------
// Renvoie un pointeur sur la zone de chargement
//-----------------------------------------------------------------------------
uint8_t* XMemRaster::GetLinePointer(int y0, int* startX, uint32_t* w)
{
  if (y0 < m_nOriY + (int)m_memV0)
    return nullptr;
  if (y0 >= m_nOriY + (int)(m_memV0 + m_memH))
    return nullptr;

  uint8_t* ptr = &m_memArea[(m_memW / m_memFactor) * NbByte() * ((y0 - (m_nOriY + m_memV0))/m_memFactor)];
  *w = m_memW;
  *startX = m_nOriX + m_memU0;
  return ptr;
}
