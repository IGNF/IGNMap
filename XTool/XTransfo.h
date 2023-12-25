//-----------------------------------------------------------------------------
//								XTransfo.h
//								==========
//
// Auteur : F.Becirspahic - MODSP
//
// 30/08/2010
//-----------------------------------------------------------------------------
#ifndef XTRANSFO_H
#define XTRANSFO_H

class XTransfo
{
public:
  XTransfo() {;}
    virtual ~XTransfo() {;}

    virtual void Direct(double x, double y, double *u, double *v) { *u = x; *v = y;}
    virtual void Indirect(double u, double v, double *x, double *y) {*x = u; *y = v;}

    virtual void Dimension(int w, int h, int* wout, int* hout) { *wout = w; *hout = h;}
    virtual bool SetGeoref(double* , double* , double* , unsigned short* ) { return false;}
};

#endif // XTRANSFO_H
