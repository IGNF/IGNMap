//-----------------------------------------------------------------------------
//								XGeodGrid.cpp
//								===============
//
// Auteur : F.Becirspahic - MODSP / IGN
//
// Date : 18/11/2013
//-----------------------------------------------------------------------------

#include "XGeodGrid.h"
#include "../XTool/XPt3D.h"

//-----------------------------------------------------------------------------
// Chargement d'une grille
//-----------------------------------------------------------------------------
bool XGeodGrid::LoadGrid(const char* filename, XError* error)
{
  std::ifstream in;
  in.open(filename);
  if (!in.good())
    return XErrorError(error, "XNCGeod::LoadGrid", XError::eIOOpen);

  char buf[1024];
  // Premiere ligne d'entete
  in >> m_dLongMin >> m_dLongMax >> m_dLatMin >> m_dLatMax >> m_dLongStep >> m_dLatStep;
  in.getline(buf, 1023);

  m_nNbLong = XRint((m_dLongMax - m_dLongMin) / m_dLongStep) + 1;
  m_nNbLat = XRint((m_dLatMax - m_dLatMin) / m_dLatStep) + 1;
  uint32_t nb_lig = m_nNbLong * m_nNbLat;
  if (m_Grid != NULL)
    delete[] m_Grid;
  m_Grid = new XPt3[nb_lig];
  if (m_Grid == NULL)
    return XErrorError(error, "XNCGeod::LoadGrid", XError::eAllocation);

  // Chargement de la grille
  double longitude, latitude, Tx, Ty, Tz;
  int precision;
  uint32_t pos;
  for(uint32_t i = 0; i < nb_lig; i++) {
    in >> longitude >> latitude >> Tx >> Ty >> Tz >> precision;
    pos = XRint((longitude - m_dLongMin) / m_dLongStep) * m_nNbLat + XRint((latitude - m_dLatMin) / m_dLatStep);
    m_Grid[pos].X = Tx;
    m_Grid[pos].Y = Ty;
    m_Grid[pos].Z = Tz;
  }
  if (!in.good()) {
    delete[] m_Grid;
    m_Grid = NULL;
    return XErrorError(error, "XNCGeod::LoadGrid", XError::eBadData);
  }

  return true;
}

//-----------------------------------------------------------------------------
// Ecriture d'une grille binaire
//-----------------------------------------------------------------------------
bool XGeodGrid::WriteGrid(const char* filename, XError* error)
{
  std::ofstream out;
  out.open(filename, std::ios::out | std::ios::binary);
  if (!out.good())
    return XErrorError(error, "XNCGeod::WriteGrid", XError::eIOOpen);
  out.write((char*)&m_dLatMin, sizeof(double));
  out.write((char*)&m_dLatMax, sizeof(double));
  out.write((char*)&m_dLongMin, sizeof(double));
  out.write((char*)&m_dLongMax, sizeof(double));
  out.write((char*)&m_dLatStep, sizeof(double));
  out.write((char*)&m_dLongStep, sizeof(double));
  out.write((char*)&m_nNbLat, sizeof(uint32_t));
  out.write((char*)&m_nNbLong, sizeof(uint32_t));

  out.write((char*)m_Grid, sizeof(XPt3) * m_nNbLat * m_nNbLong);
  return out.good();
}

//-----------------------------------------------------------------------------
// Lecture d'une grille binaire
//-----------------------------------------------------------------------------
bool XGeodGrid::ReadGrid(const char* filename, XError* error)
{
  std::ifstream in;
  in.open(filename, std::ios::in | std::ios::binary);
  if (!in.good())
    return XErrorError(error, "XLambert::ReadGrid", XError::eIOOpen);
  in.read((char*)&m_dLatMin, sizeof(double));
  in.read((char*)&m_dLatMax, sizeof(double));
  in.read((char*)&m_dLongMin, sizeof(double));
  in.read((char*)&m_dLongMax, sizeof(double));
  in.read((char*)&m_dLatStep, sizeof(double));
  in.read((char*)&m_dLongStep, sizeof(double));
  in.read((char*)&m_nNbLat, sizeof(uint32_t));
  in.read((char*)&m_nNbLong, sizeof(uint32_t));

  // Allocation de la grille
  if (m_Grid != NULL)
    delete[] m_Grid;
  m_Grid = new XPt3[m_nNbLat * m_nNbLong];
  if (m_Grid == NULL)
    return XErrorError(error, "XLambert::ReadGrid", XError::eAllocation);

  in.read((char*)m_Grid, sizeof(XPt3) * m_nNbLat * m_nNbLong);
  return in.good();
}

//-----------------------------------------------------------------------------
// Interpolation sur la grille
//-----------------------------------------------------------------------------
bool XGeodGrid::Interpol(double lambda, double phi, double* Tx, double* Ty, double* Tz)
{
  if (m_Grid == NULL)
    return false;

  if (lambda < m_dLongMin)
    return false;
  if (lambda > m_dLongMax)
    return false;
  if (phi < m_dLatMin)
    return false;
  if (phi > m_dLatMax)
    return false;

  uint32_t nb_long = (lambda - m_dLongMin) / m_dLongStep;
  uint32_t nb_lat = (phi - m_dLatMin) /  m_dLatStep;
  double x = (lambda - (nb_long * m_dLongStep + m_dLongMin)) / m_dLongStep;
  double y = (phi - (nb_lat * m_dLatStep + m_dLatMin)) / m_dLatStep;

  XPt3D T1(m_Grid[nb_long * m_nNbLat + nb_lat]);
  XPt3D T2(m_Grid[nb_long * m_nNbLat + nb_lat + 1]);
  XPt3D T3(m_Grid[(nb_long + 1) * m_nNbLat + nb_lat]);
  XPt3D T4(m_Grid[(nb_long + 1) * m_nNbLat + nb_lat + 1]);

  XPt3D TM = (1 - x)*(1 - y)*T1 + (1 - x) * y * T2 + x * (1 - y)*T3 + x * y * T4;
  *Tx = TM.X;
  *Ty = TM.Y;
  *Tz = TM.Z;

  return true;
}
