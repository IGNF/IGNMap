//-----------------------------------------------------------------------------
//								XGeoObject.h
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 23/08/2002
//-----------------------------------------------------------------------------

#ifndef _XGEOOBJECT_H
#define _XGEOOBJECT_H

#include "XFrame.h"

#ifndef XGEO_NO_DATA
#define XGEO_NO_DATA	-1e38
#endif

class XDeviceContext;
class XGeoRepres;

class XGeoObject {
public:
	enum eType { None, Map, Vector, Layer, Class};
	enum eState { sNone = 0, sActif = 2, sVisible = 4, sSelected = 8, sSelectable = 16, sEditable = 32};
protected:
	XFrame			m_Frame;					// Rectangle englobant
	int					m_State;					// Etat
//	std::string	m_strName;

public:
	XGeoObject() {m_State = sActif | sVisible | sSelectable;}
	virtual ~XGeoObject() {;}

	virtual inline eType Type() const { return None;}
	inline bool Actif() const { if (m_State & sActif) return true; return false;}
	inline bool Visible() const { if (m_State & sVisible) return true; return false;}
	inline bool Selected() const { if (m_State & sSelected) return true; return false;}
	inline bool Selectable() const { if (m_State & sSelectable) return true; return false;}
	inline bool Editable() const { if (m_State & sEditable) return true; return false;}

	virtual bool IsSelectable() const { return Selectable();}

  virtual void Actif(bool flag) { if (flag) m_State |= sActif; else m_State &= ~sActif;}
  virtual void Visible(bool flag) { if (flag) m_State |= sVisible; else m_State &= ~sVisible;}
  virtual void Selected(bool flag) { if (flag) m_State |= sSelected; else m_State &= ~sSelected;}
  virtual void Selectable(bool flag) { if (flag) m_State |= sSelectable; else m_State &= ~sSelectable;}
  virtual void Editable(bool flag) { if (flag) m_State |= sEditable; else m_State &= ~sEditable;}

	virtual XFrame Frame() const { return m_Frame;}
	virtual bool IntersectFrame(XFrame& r) const { 	if (m_Frame.Xmin > r.Xmax) return false;
																									if (m_Frame.Xmax < r.Xmin) return false;
																									if (m_Frame.Ymin > r.Ymax) return false;
																									if (m_Frame.Ymax < r.Ymin)	return false;
																									return true; }


  inline double FrameW() { return m_Frame.Width();}
  inline double FrameH() { return m_Frame.Height();}
	inline double X0() const { return m_Frame.Xmin;}
	inline double Y0() const { return m_Frame.Ymax;}

//	virtual inline XPt2D Centroide() const { return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);}

	virtual inline double Xmin() const { return m_Frame.Xmin;}
	virtual inline double Xmax() const { return m_Frame.Xmax;}
	virtual inline double Ymin() const { return m_Frame.Ymin;}
	virtual inline double Ymax() const { return m_Frame.Ymax;}
	virtual inline double Zmin() const { return XGEO_NO_DATA;}
	virtual inline double Zmax() const { return XGEO_NO_DATA;}

	virtual void Name(std::string) { ;}
	virtual std::string Name() { return "";}
  virtual void Filename(std::string) { ;}
  virtual std::string Filename() { return "";}

	virtual void Repres(XGeoRepres*) {;}
	virtual inline XGeoRepres* Repres() { return NULL;}

	virtual void WriteInfo(const char*) {;}

	virtual bool XmlRead(XParserXML*, uint32_t = 0, XError* = NULL) {return true;}
	virtual bool XmlWrite(std::ostream*) { return true;}


	virtual bool IsNear2D(const XPt2D&, double) { return false;}

	virtual bool WriteHtml(std::ostream*) { return false;}

};

#endif //_XGEOOBJECT_H
