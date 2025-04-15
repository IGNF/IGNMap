//-----------------------------------------------------------------------------
//								XPredictor.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 24/9/2021
//-----------------------------------------------------------------------------

#include "XPredictor.h"
#include <cstring>

bool XPredictor::Decode(uint8_t* Pix, uint32_t W, uint32_t H, uint32_t pixSize, uint32_t nbBits, uint32_t num_algo)
{
  if (num_algo == 1) return true;
  if (num_algo == 2) { // PREDICTOR_HORIZONTAL
    uint32_t lineW = W * pixSize;
    if (nbBits == 8){
      for (uint32_t i = 0; i < H; i++)
        for (uint32_t j = i * lineW; j < (i + 1) * lineW - pixSize; j++)
          Pix[j + pixSize] += Pix[j];
    }
    if (nbBits == 16){
      for (uint32_t i = 0; i < H; i++){
        uint16_t* ptr = (uint16_t*)&Pix[lineW*i];
        for (uint32_t j = 0; j < (W - 1); j++) {
          ptr[1] += ptr[0];
          ptr++;
        }
      }
    }
    if (nbBits == 32){
      for (uint32_t i = 0; i < H; i++){
        uint32_t* ptr = (uint32_t*)&Pix[lineW*i];
        for (uint32_t j = 0; j < (W - 1); j++) {
          ptr[1] += ptr[0];
          ptr++;
        }
      }
    }
    return true;
  }
  if (num_algo == 3) { // PREDICTOR_FLOATINGPOINT
    uint32_t lineW = W * pixSize;
    uint8_t* buf = new uint8_t[lineW];
    for (uint32_t i = 0; i < H; i++) {
      for (uint32_t j = i * lineW; j < (i + 1) * lineW - 1; j++)
        Pix[j + 1] += Pix[j];
      std::memcpy(buf, &Pix[i * lineW], lineW);
      uint8_t* ptr_0 = buf;
      uint8_t* ptr_1 = &buf[W];
      uint8_t* ptr_2 = &buf[W*2];
      uint8_t* ptr_3 = &buf[W*3];
      for (uint32_t j = i * lineW; j < (i + 1) * lineW; j += 4) {
        Pix[j] = *ptr_3; ptr_3++;
        Pix[j + 1] = *ptr_2; ptr_2++;
        Pix[j + 2] = *ptr_1; ptr_1++;
        Pix[j + 3] = *ptr_0; ptr_0++;
      }
    }
    delete[] buf;
    return true;
  }
  return false;
}
