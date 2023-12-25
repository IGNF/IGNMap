//-----------------------------------------------------------------------------
//								XPredictor.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 24/9/2021
//-----------------------------------------------------------------------------

#ifndef XPREDICTOR_H
#define XPREDICTOR_H

#include "../XTool/XBase.h"

class XPredictor {
public:
  XPredictor() {;}

  bool Decode(uint8_t* Pix, uint32_t W, uint32_t H, uint32_t pixSize, uint32_t nbBits, uint32_t num_algo);
};

#endif // XPREDICTOR_H
