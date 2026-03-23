//-----------------------------------------------------------------------------
//								XStlWriter.cpp
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 3/12/2020
//-----------------------------------------------------------------------------

#include "XStlWriter.h"
#include <iostream>
#include <cstring>

//-----------------------------------------------------------------------------
// Fixe les options
//-----------------------------------------------------------------------------
bool XStlWriter::SetOptions(double scale, double exag, double pas, double zmin)
{
  m_dScale = scale;
  m_dExag = exag;
  m_dPas = pas;
  m_dZmin = zmin;
  return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'un fichier
//-----------------------------------------------------------------------------
bool XStlWriter::WriteStlFile(std::ostream* out, float* T, uint32_t nbW, uint32_t nbH,
                              bool edge, XWait* wait)
{
  char header[80];
  ::memset(header, 0, 80);
  out->write(header, 80);
  uint32_t nbptStl = (nbW -1) * (nbH - 1) * 2;
  if (edge)
    nbptStl += ((nbW - 1) * 4 + (nbH - 1) * 4 + 2);
  out->write((char*)&nbptStl, 4);
  float xA, xB, xC, yA, yB, yC, zA, zB, zC;
  float Ymax = nbH * m_dPas;

  XWaitRange(wait, 0, nbH);
  std::string message;

  for (uint32_t i = 0; i < (nbH - 1); i++) {
    //message = QString("Traitement de la ligne %1 / %2").arg(i).arg(nbH));
    XWaitStatus(wait, message);

    for (uint32_t j = 0; j < (nbW - 1); j++) {

      // A -- C
      // |
      // B
      xA = xB = j * m_dPas;
      xC = (j + 1)* m_dPas;
      yA = yC = Ymax - i * m_dPas;
      yB = Ymax - (i + 1) * m_dPas;
      zA = (T[i * nbW + j] - m_dZmin) * m_dExag / m_dScale;
      zB = (T[(i + 1) * nbW + j] - m_dZmin) * m_dExag / m_dScale;
      zC = (T[i * nbW + j + 1] - m_dZmin) * m_dExag / m_dScale;
      if (zA < 0) zA = 0;
      if (zB < 0) zB = 0;
      if (zC < 0) zC = 0;
      WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);

      //      C
      //      |
      // A -- B
      yA = yB;
      xB = xC;
      zA = zB;
      zB = (T[(i + 1) * nbW + (j + 1)] - m_dZmin) * m_dExag / m_dScale;
      if (zB < 0) zB = 0;
      WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);
    }
    XWaitStepIt(wait);
  }

  if (!edge)
    return out->good();

  // Ecriture des bords Nord et Sud
  for (uint32_t k = 0; k < 2; k++) {
    uint32_t i = 0; // Nord
    if (k == 1)
      i = (nbH - 1); // Sud
    for (uint32_t j = 0; j < (nbW - 1); j++) {
      // A -- C
      // |
      // B
      xA = xB = j * m_dPas;
      xC = (j + 1)* m_dPas;
      yA = yB = yC = Ymax - i * m_dPas;
      zA = (T[i * nbW + j] - m_dZmin) * m_dExag / m_dScale;
      zB = (0.) * m_dExag / m_dScale;
      zC = (T[i * nbW + j + 1] - m_dZmin) * m_dExag / m_dScale;
      if (zA < 0) zA = 0;
      if (zB < 0) zB = 0;
      if (zC < 0) zC = 0;
      if (i == 0)
        WriteTriangle(out, xB, yB, zB, xA, yA, zA, xC, yC, zC);
      else
        WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);

      //      C
      //      |
      // A -- B
      xB = xC;
      zA = zB;
      if (i == 0)
        WriteTriangle(out, xB, yB, zB, xA, yA, zA, xC, yC, zC);
      else
        WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);

    }
  }

  // Ecriture des bords Est et Ouest
  for (uint32_t k = 0; k < 2; k++) {
    uint32_t j = 0;
    if (k == 1)
      j = nbW - 1;
    for (uint32_t i = 0; i < (nbH - 1); i++) {

      // A -- C
      // |
      // B
      xA = xB = xC = j * m_dPas;
      yA = yB = Ymax - i * m_dPas;
      yC = Ymax - (i + 1) * m_dPas;
      zA = (T[i * nbW + j] - m_dZmin) * m_dExag / m_dScale;
      zB = (0.) * m_dExag / m_dScale;
      zC = (T[(i + 1) * nbW + j] - m_dZmin) * m_dExag / m_dScale;
      if (zA < 0) zA = 0;
      if (zB < 0) zB = 0;
      if (zC < 0) zC = 0;
      if (j == 0)
        WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);
      else
        WriteTriangle(out, xB, yB, zB, xA, yA, zA, xC, yC, zC);

      //      C
      //      |
      // A -- B
      yA = yB;
      yB = yC;
      zA = zB;
      if (j == 0)
        WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);
      else
        WriteTriangle(out, xB, yB, zB, xA, yA, zA, xC, yC, zC);
    }
  }

  // Plancher
  xA = 0; yA = Ymax; zA = 0;
  xB = (nbW - 1) * m_dPas; yB = Ymax; zB = 0;
  xC = 0; yC = Ymax - (nbH - 1) * m_dPas; zC = 0;
  WriteTriangle(out, xA, yA, zA, xB, yB, zB, xC, yC, zC);
  xA = xB; yA = yC;
  WriteTriangle(out, xB, yB, zB, xA, yA, zA, xC, yC, zC);

  return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un triangle
//-----------------------------------------------------------------------------
bool XStlWriter::WriteTriangle(std::ostream* out,
                   const float& xA, const float& yA, const float& zA,
                   const float& xB, const float& yB, const float& zB,
                   const float& xC, const float& yC, const float& zC)
{
  static float xN, yN, zN, norm;
  static uint16_t zero = 0;
  xN = (yB - yA)*(zC - zA) - (zB - zA)*(yC - yA);
  yN = (zB - zA)*(xC - xA) - (xB - xA)*(zC - zA);
  zN = (xB - xA)*(yC - yA) - (yB - yA)*(xC - xA);
  norm = sqrt(xN*xN + yN*yN + zN*zN);
  xN /= norm;
  yN /= norm;
  zN /= norm;
  out->write((char*)&xN, 4); out->write((char*)&yN, 4); out->write((char*)&zN, 4);
  out->write((char*)&xA, 4); out->write((char*)&yA, 4); out->write((char*)&zA, 4);
  out->write((char*)&xB, 4); out->write((char*)&yB, 4); out->write((char*)&zB, 4);
  out->write((char*)&xC, 4); out->write((char*)&yC, 4); out->write((char*)&zC, 4);
  out->write((char*)&zero, 2);
  return out->good();
}

