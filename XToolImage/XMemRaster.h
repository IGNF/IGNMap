//-----------------------------------------------------------------------------
//								MemRaster.h
//								===========
//
// Chargement d'image avec bufferisation
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/09/2025
//-----------------------------------------------------------------------------

#ifndef XMEMRASTER_H
#define XMEMRASTER_H

#include "XFileImage.h"

class XMemRaster : public XFileImage {
public:
  XMemRaster();
  ~XMemRaster();

  virtual void Close();

  void SetRotation(uint16_t rot) { m_nRot = rot % 4; }
  uint16_t GetRotation() const { return m_nRot; }
  uint32_t NbPixelX();
  uint32_t NbPixelY();
  int OriX() const { return m_nOriX; }
  int OriY() const { return m_nOriY; }

  //bool GetPixel(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

  void SetOrigin(int Ox, int Oy) { m_nOriX = Ox; m_nOriY = Oy; }
  void MoveOrigin(int Tx, int Ty) { m_nOriX += Tx; m_nOriY += Ty; }
  bool SetViewport(int x0, int y0, int x1, int y1, uint32_t factor);
  void ViewPort2Image(int& X, int& Y);
  void Image2Viewport(int& X, int& Y);

  uint8_t* GetLinePointer(int y0, int* startX, uint32_t* w);

protected:
  uint32_t m_memU0; // Origine pixel de la zone chargee en memoire
  uint32_t m_memV0;
  uint32_t m_memW;  // Dimension en pixel de la zone chargee en memoire
  uint32_t m_memH;
  uint32_t m_memFactor; // Facteur utilise lors du chargement
  uint32_t m_nGrow;
  uint8_t* m_memArea;
  uint32_t m_memAreaSize;
  uint16_t m_nRot;
  uint8_t* m_memRotArea;  // Zone pixels avec la rotation
  uint32_t m_memRotSize;


  int m_nOriX;
  int m_nOriY;

  bool LoadPixelInMemory(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint32_t factor);
  bool ReadPixel(uint32_t startX, uint32_t startY, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);
};

#endif //XMEMRASTER_H