//-----------------------------------------------------------------------------
//								XWKBGeom.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 27/06/2006
//-----------------------------------------------------------------------------

#include "XWKBGeom.h"
#include <cstring>
#include "../XTool/XFrame.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XWKBGeom::XWKBGeom(bool autodelete)
{
	m_Type = Null;
	m_bAutoDelete = autodelete;
	m_nNumPoints = 0;
	m_Pt = NULL;
  m_Z = NULL;
	m_nNumParts = 0;
  m_Parts = NULL;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XWKBGeom::~XWKBGeom()
{
	m_Type = Null;
	if (!m_bAutoDelete)
		return;
	if (m_Pt != NULL)
		delete[] m_Pt;
  if (m_Z != NULL)
    delete[] m_Z;
  if (m_Parts != NULL)
		delete[] m_Parts;
}

//-----------------------------------------------------------------------------
// Lecture
//-----------------------------------------------------------------------------
bool XWKBGeom::Read(uint8_t* geom)
{
	//Byte order
	if (geom[0] > 1)
		return false;

	// Type
	uint32_t* type = (uint32_t*)&geom[1];
	m_Type = (eType)*type;

	switch(m_Type) {
		case wkbPoint :
      m_nNumPoints = 1;
      m_Pt = new XPt[1];
      ::memcpy(m_Pt, &geom[sizeof(uint8_t) + sizeof(uint32_t)], sizeof(XPt));
			return true;
		case wkbLineString :
			return ReadLineString(geom);
		case wkbPolygon :
			return ReadPolygon(geom);
		case wkbMultiPoint :
			return ReadMultiPoint(geom);
		case wkbMultiLineString :
			return ReadMultiLineString(geom);
		case wkbMultiPolygon :
			return ReadMultiPolygon(geom);

    case wkbLineStringZ :
      return ReadLineStringZ(geom);
    case wkbPolygonZ :
      return ReadPolygonZ(geom);
    case wkbMultiPointZ :
      return ReadMultiPointZ(geom);
    case wkbMultiLineStringZ :
      return ReadMultiLineStringZ(geom);
    case wkbMultiPolygonZ :
      return ReadMultiPolygonZ(geom);

    case wkbLineStringZM :
      return ReadLineStringZM(geom);
    case wkbPolygonZM :
      return ReadPolygonZM(geom);
    case wkbMultiPointZM :
      return ReadMultiPointZM(geom);
    case wkbMultiLineStringZM :
      return ReadMultiLineStringZM(geom);
    case wkbMultiPolygonZM :
      return ReadMultiPolygonZM(geom);

    //default : return false;
  }

	return false;
}

//-----------------------------------------------------------------------------
// LineString
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadLineString(uint8_t* geom)
{
	uint32_t* num;
	num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;

  std::memcpy(m_Pt, &geom[sizeof(uint8_t) + 2*sizeof(uint32_t)], m_nNumPoints * sizeof(XPt));
	return true;
}

//-----------------------------------------------------------------------------
// Polygon
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadPolygon(uint8_t* geom)
{
	uint8_t* ptr = &geom[sizeof(uint8_t) + sizeof(uint32_t)];
	m_nNumParts = *((uint32_t*)ptr);
  ptr += sizeof(uint32_t);
  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL)
    return false;
  uint32_t nb = 0;
	m_nNumPoints = 0;
	for (uint32_t i = 0; i < m_nNumParts; i++) {
		nb = *((uint32_t*)ptr);
		m_nNumPoints += nb;
		m_Parts[i] = m_nNumPoints - nb;
		ptr += (sizeof(uint32_t) + nb * 2 * sizeof(double));
  }
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    delete m_Parts;
    m_Parts = NULL;
    m_nNumParts = m_nNumPoints = 0;
    return false;
  }
	ptr = &geom[sizeof(uint8_t) + 2 * sizeof(uint32_t)];
	XPt* P = m_Pt;
  for (uint32_t i = 0; i < m_nNumParts; i++) {
		nb = *((uint32_t*)ptr);
		ptr += sizeof(uint32_t);
		::memcpy(P, ptr, nb * sizeof(XPt));
		ptr += (nb * 2 * sizeof(double));
		P += nb;
	}
	return true;
}

//-----------------------------------------------------------------------------
// MultiPoint
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPoint(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;

  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    double* ptr = (double*)&geom[sizeof(uint8_t) + 2*sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        (sizeof(uint8_t) + sizeof(uint32_t) + 2 * sizeof(double)) * i];
    m_Pt[i].X = *ptr; ptr++;
    m_Pt[i].Y = *ptr; ptr++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// MultiLineString
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiLineString(uint8_t* geom)
{
	uint32_t* num;
	num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
	if (*num == 1) {
    m_nNumParts = 1;
    m_Parts = new uint32_t[m_nNumParts];
    m_Parts[0] = 0;
		return ReadLineString(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);
	}
	return false;
}

//-----------------------------------------------------------------------------
// MultiPolygon
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPolygon(uint8_t* geom)
{
	uint32_t* num;
	num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
	if (*num == 1)
		return ReadPolygon(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);

	XWKBGeom* T = new XWKBGeom[*num];
	if (T == NULL)
		return false;

	// Lecture de tous les polygones
	uint8_t* ptr = &geom[sizeof(uint8_t) + 2*sizeof(uint32_t)];
	for (uint32_t i = 0; i < *num; i++) {
		T[i].ReadPolygon(ptr);
    ptr += (sizeof(uint8_t) + 2*sizeof(uint32_t) + T[i].m_nNumParts * sizeof(uint32_t) +
            T[i].m_nNumPoints * sizeof(XPt));
		m_nNumPoints += T[i].m_nNumPoints;
		m_nNumParts += T[i].m_nNumParts;
	}

  // Allocation des tableaux
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
		delete[] T;
		return false;
	}

  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL) {
    delete[] m_Pt;
    delete[] T;
		return false;
	}

	// Copie des points
	XPt* points = m_Pt;
  for (uint32_t i = 0; i < *num; i++) {
		::memcpy(points, T[i].m_Pt, T[i].m_nNumPoints * sizeof(XPt));
		points += T[i].m_nNumPoints;
	}

	// Copie des parties
	uint32_t* parts = m_Parts;
	uint32_t accu = 0;
  for (uint32_t i = 0; i < *num; i++) {
		for (uint32_t j = 0; j < T[i].m_nNumParts; j++) {
			*parts = T[i].m_Parts[j] + accu;
			parts++;
		}
		accu += T[i].m_nNumPoints;
	}

	delete[] T;

	return true;
}

//-----------------------------------------------------------------------------
// LineStringZ
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadLineStringZ(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    return false;
  }
  double* ptr = (double*)&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)];
  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    m_Pt[i].X = *ptr; ptr++;
    m_Pt[i].Y = *ptr; ptr++;
    m_Z[i] = *ptr; ptr++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Polygon Z
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadPolygonZ(uint8_t* geom)
{
  uint8_t* ptr = &geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumParts = *((uint32_t*)ptr);
  ptr += sizeof(uint32_t);
  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL)
    return false;
  uint32_t nb = 0;
  m_nNumPoints = 0;
  for (uint32_t i = 0; i < m_nNumParts; i++) {
    nb = *((uint32_t*)ptr);
    m_nNumPoints += nb;
    m_Parts[i] = m_nNumPoints - nb;
    ptr += (sizeof(uint32_t) + nb * 3 * sizeof(double));
  }
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    delete m_Parts;
    m_Parts = NULL;
    m_nNumParts = m_nNumPoints = 0;
    return false;
  }
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    m_Pt = NULL;
    delete m_Parts;
    m_Parts = NULL;
    m_nNumParts = m_nNumPoints = 0;
    return false;
  }

  ptr = &geom[sizeof(uint8_t) + 2 * sizeof(uint32_t)];
  XPt* P = m_Pt;
  double* z = m_Z;
  for (uint32_t i = 0; i < m_nNumParts; i++) {
    nb = *((uint32_t*)ptr);
    ptr += sizeof(uint32_t);
    double* zmpt = (double*)ptr;
    for (uint32_t j = 0; j < nb; j++) {
      P[j].X = *zmpt; zmpt++;
      P[j].Y = *zmpt; zmpt++;
      z[j] = *zmpt; zmpt++;
     }
    ptr += (nb * 3 * sizeof(double));
    P += nb;
    z += nb;
  }
  return true;
}

//-----------------------------------------------------------------------------
// MultiPoint Z
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPointZ(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    return false;
  }
  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    double* ptr = (double*)&geom[sizeof(uint8_t) + 2*sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        (sizeof(uint8_t) + sizeof(uint32_t) + 3 * sizeof(double)) * i];
    m_Pt[i].X = *ptr; ptr++;
    m_Pt[i].Y = *ptr; ptr++;
    m_Z[i] = *ptr; ptr++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// MultiLineString Z
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiLineStringZ(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  if (*num == 1) {
    m_nNumParts = 1;
    m_Parts = new uint32_t[m_nNumParts];
    m_Parts[0] = 0;
    return ReadLineStringZ(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);
  }
  return false;
}

//-----------------------------------------------------------------------------
// MultiPolygon Z
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPolygonZ(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  if (*num == 1)
    return ReadPolygonZ(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);

  XWKBGeom* T = new XWKBGeom[*num];
  if (T == NULL)
    return false;

  // Lecture de tous les polygones
  uint8_t* ptr = &geom[sizeof(uint8_t) + 2*sizeof(uint32_t)];
  for (uint32_t i = 0; i < *num; i++) {
    T[i].ReadPolygonZ(ptr);
    ptr += (sizeof(uint8_t) + 2*sizeof(uint32_t) + T[i].m_nNumParts * sizeof(uint32_t) +
            T[i].m_nNumPoints * 3 * sizeof(double));
    m_nNumPoints += T[i].m_nNumPoints;
    m_nNumParts += T[i].m_nNumParts;
  }

  // Allocation des tableaux
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    delete[] T;
    return false;
  }

  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete[] m_Pt; m_Pt = NULL;
    delete[] T;
    return false;
  }

  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL) {
    delete[] m_Z; m_Z = NULL;
    delete[] m_Pt; m_Pt = NULL;
    delete[] T;
    return false;
  }

  // Copie des points
  XPt* points = m_Pt;
  double* z = m_Z;
  for (uint32_t i = 0; i < *num; i++) {
    ::memcpy(points, T[i].m_Pt, T[i].m_nNumPoints * sizeof(XPt));
    points += T[i].m_nNumPoints;
    ::memcpy(z, T[i].m_Z, T[i].m_nNumPoints * sizeof(double));
    z += T[i].m_nNumPoints;
  }

  // Copie des parties
  uint32_t* parts = m_Parts;
  uint32_t accu = 0;
  for (uint32_t i = 0; i < *num; i++) {
    for (uint32_t j = 0; j < T[i].m_nNumParts; j++) {
      *parts = T[i].m_Parts[j] + accu;
      parts++;
    }
    accu += T[i].m_nNumPoints;
  }

  delete[] T;

  return true;
}

//-----------------------------------------------------------------------------
// LineStringZM
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadLineStringZM(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    return false;
  }
  double* ptr = (double*)&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)];
  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    m_Pt[i].X = *ptr; ptr++;
    m_Pt[i].Y = *ptr; ptr++;
    m_Z[i] = *ptr; ptr++;
    ptr++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Polygon
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadPolygonZM(uint8_t* geom)
{
  uint8_t* ptr = &geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumParts = *((uint32_t*)ptr);
  ptr += sizeof(uint32_t);
  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL)
    return false;
  uint32_t nb = 0;
  m_nNumPoints = 0;
  for (uint32_t i = 0; i < m_nNumParts; i++) {
    nb = *((uint32_t*)ptr);
    m_nNumPoints += nb;
    m_Parts[i] = m_nNumPoints - nb;
    ptr += (sizeof(uint32_t) + nb * 4 * sizeof(double));
  }
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    delete m_Parts;
    m_Parts = NULL;
    m_nNumParts = m_nNumPoints = 0;
    return false;
  }
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    m_Pt = NULL;
    delete m_Parts;
    m_Parts = NULL;
    m_nNumParts = m_nNumPoints = 0;
    return false;
  }

  ptr = &geom[sizeof(uint8_t) + 2 * sizeof(uint32_t)];
  XPt* P = m_Pt;
  double* z = m_Z;
  for (uint32_t i = 0; i < m_nNumParts; i++) {
    nb = *((uint32_t*)ptr);
    ptr += sizeof(uint32_t);
    double* zmpt = (double*)ptr;
    for (uint32_t j = 0; j < nb; j++) {
      P[j].X = *zmpt; zmpt++;
      P[j].Y = *zmpt; zmpt++;
      z[j] = *zmpt; zmpt++;
      zmpt++;
    }
    ptr += (nb * 4 * sizeof(double));
    P += nb;
    z += nb;
  }
  return true;
}

//-----------------------------------------------------------------------------
// MultiPoint
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPointZM(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  m_nNumPoints = *num;
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL)
    return false;
  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete m_Pt;
    return false;
  }
  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    double* ptr = (double*)&geom[sizeof(uint8_t) + 2*sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        (sizeof(uint8_t) + sizeof(uint32_t) + 4 * sizeof(double)) * i];
    m_Pt[i].X = *ptr; ptr++;
    m_Pt[i].Y = *ptr; ptr++;
    m_Z[i] = *ptr; ptr++;
    ptr++;
  }
  return true;
}

//-----------------------------------------------------------------------------
// MultiLineString
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiLineStringZM(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  if (*num == 1) {
    m_nNumParts = 1;
    m_Parts = new uint32_t[m_nNumParts];
    m_Parts[0] = 0;
    return ReadLineStringZM(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);
  }
  return false;
}

//-----------------------------------------------------------------------------
// MultiPolygon
//-----------------------------------------------------------------------------
bool XWKBGeom::ReadMultiPolygonZM(uint8_t* geom)
{
  uint32_t* num;
  num = (uint32_t*)&geom[sizeof(uint8_t) + sizeof(uint32_t)];
  if (*num == 1)
    return ReadPolygonZM(&geom[sizeof(uint8_t) + 2*sizeof(uint32_t)]);

  XWKBGeom* T = new XWKBGeom[*num];
  if (T == NULL)
    return false;

  // Lecture de tous les polygones
  uint8_t* ptr = &geom[sizeof(uint8_t) + 2*sizeof(uint32_t)];
  for (uint32_t i = 0; i < *num; i++) {
    T[i].ReadPolygonZM(ptr);
    ptr += (sizeof(uint8_t) + 2*sizeof(uint32_t) + T[i].m_nNumParts * sizeof(uint32_t) +
            T[i].m_nNumPoints * 4 * sizeof(double));
    m_nNumPoints += T[i].m_nNumPoints;
    m_nNumParts += T[i].m_nNumParts;
  }

  // Allocation des tableaux
  m_Pt = new XPt[m_nNumPoints];
  if (m_Pt == NULL) {
    delete[] T;
    return false;
  }

  m_Z = new double[m_nNumPoints];
  if (m_Z == NULL) {
    delete[] m_Pt; m_Pt = NULL;
    delete[] T;
    return false;
  }

  m_Parts = new uint32_t[m_nNumParts];
  if (m_Parts == NULL) {
    delete[] m_Z; m_Z = NULL;
    delete[] m_Pt; m_Pt = NULL;
    delete[] T;
    return false;
  }

  // Copie des points
  XPt* points = m_Pt;
  double* z = m_Z;
  for (uint32_t i = 0; i < *num; i++) {
    ::memcpy(points, T[i].m_Pt, T[i].m_nNumPoints * sizeof(XPt));
    points += T[i].m_nNumPoints;
    ::memcpy(z, T[i].m_Z, T[i].m_nNumPoints * sizeof(double));
    z += T[i].m_nNumPoints;
  }

  // Copie des parties
  uint32_t* parts = m_Parts;
  uint32_t accu = 0;
  for (uint32_t i = 0; i < *num; i++) {
    for (uint32_t j = 0; j < T[i].m_nNumParts; j++) {
      *parts = T[i].m_Parts[j] + accu;
      parts++;
    }
    accu += T[i].m_nNumPoints;
  }

  delete[] T;

  return true;
}

//-----------------------------------------------------------------------------
// Cadre de la geometrie
//-----------------------------------------------------------------------------
bool XWKBGeom::Frame(XFrame* F)
{
  if (m_nNumPoints < 1)
    return false;
  F->Xmin = F->Xmax = m_Pt[0].X;
  F->Ymin = F->Ymax = m_Pt[0].Y;
  for (uint32_t i = 0; i < m_nNumPoints; i++) {
    F->Xmin = XMin(F->Xmin, m_Pt[i].X);
    F->Xmax = XMax(F->Xmax, m_Pt[i].X);
    F->Ymin = XMin(F->Ymin, m_Pt[i].Y);
    F->Ymax = XMax(F->Ymax, m_Pt[i].Y);
  }
  return true;
}
