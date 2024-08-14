//-----------------------------------------------------------------------------
//								XAnnotation.h
//								=============
//
// Auteur : F.Becirspahic - MODSP
//
// 09/02/2010
//-----------------------------------------------------------------------------

#ifndef XANNOTATION_H
#define XANNOTATION_H

#include "../../XTool/XGeoVector.h"
#include <vector>
#include <string>

class XAnnotation : public XGeoVector
{
public:
  enum ePrimitive { pNull = 0,
                    pPolyline = 10, pArrow = 11,
                    pCurve = 20,
                    pPolygon = 30,
                    pEllipse = 40,
                    pRect = 50,
                    pText = 60};

  XAnnotation();
  XAnnotation(const XAnnotation& A);
  virtual ~XAnnotation() { Clear();}

  void Clear();

  ePrimitive Primitive() const { return m_Primitive;}
  std::string Text() { return m_strText;}
  virtual uint32_t NbPt() const;
  virtual uint32_t NbPart() const { return 1;}
  virtual XPt2D Pt(uint32_t i);
  virtual std::string Name();

  void AddPt(double x, double y);
  void ModPt(double x, double y);
  void MovePt(int& index, double x, double y);
  bool InsertPt(double x, double y, double dist);
  void RemovePt();
  void DeletePt(int index);
  void ComputeFrame();
  bool Close();
  void Text(std::string text) { m_strText = text;}
  int FindClosestEditPoint(XPt2D, double distance);
  void Move(double x, double y);
  void Add(XAnnotation* A);

  void Primitive(ePrimitive prim);

  // Chargement de la geometrie
  virtual bool LoadGeom();
  virtual void Unload();
  virtual bool IsLoaded() { if (m_T == NULL) return false; return true;}
  virtual eTypeVector TypeVector() const;
  virtual inline XPt* Pt() { return m_T;}
  virtual bool IsClosed() const;
  virtual bool IsNear2D(const XPt2D& P, double dist);

  double LengthAltLin();
  double LengthOrtho();

  // Lecture / Ecriture en XML
  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
  virtual bool XmlWrite(std::ostream* out);

  bool KmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
  bool WriteMifMid(std::ostream* mif, std::ostream* mid);
  bool WriteCsv(std::ostream* csv);

  virtual	bool ReadAttributes(std::vector<std::string>& V) { V = m_Att; return true;}
  virtual bool WriteAttributes(std::vector<std::string>& V) { m_Att = V; return true;}

  virtual bool WriteHtml(std::ostream* out);

  // Operateur
  XAnnotation& operator= (const XAnnotation& A);

protected:
  ePrimitive        m_Primitive;
  std::vector<XPt>  m_Pt;
  std::string       m_strText;
  XPt*              m_T;
  std::vector<std::string>  m_Att;
  XGeoRepres        m_R;  // Representation de l'annotation (m_Repres pointe dessus)

  bool IsPtClosed();

};

#endif // XANNOTATION_H
