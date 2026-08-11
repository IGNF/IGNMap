//-----------------------------------------------------------------------------
//								XGeoLabel.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 24/10/2003
//-----------------------------------------------------------------------------

#ifndef _XGEOLABEL_H
#define _XGEOLABEL_H

#include "XGeoPoint.h"

class XGeoLabel : public XGeoPoint2D {
protected:
	std::string		m_strName;
	uint16_t				m_nImportance;
	uint16_t				m_nRotation;		// Rotation du label (en degre)

public:
	XGeoLabel() : m_nImportance(0), m_nRotation(0)
								{ m_Frame.Xmin = m_Frame.Xmax = m_Frame.Ymin = m_Frame.Ymax = XGEO_NO_DATA;}
	XGeoLabel(double x, double y) : m_nImportance(0), m_nRotation(0)
								{ m_Frame.Xmin = m_Frame.Xmax = x; m_Frame.Ymin = m_Frame.Ymax = y;}
  XGeoLabel(XPt2D P) : m_nImportance(0), m_nRotation(0)
                { m_Frame.Xmin = m_Frame.Xmax = P.X; m_Frame.Ymin = m_Frame.Ymax = P.Y;}

	virtual void Name(std::string name) { m_strName = name;}
	virtual std::string Name() { return m_strName;}

	virtual uint16_t Importance() { return m_nImportance;}
	virtual uint16_t Rotation() { return m_nRotation;}

	void Importance(uint16_t i) { m_nImportance = i;}
	void Rotation(uint16_t r) { m_nRotation = r;}

  virtual	bool ReadAttributes(std::vector<std::string>& V)
  {V.clear();V.push_back("Nom"); V.push_back(m_strName); V.push_back("Importance");
    char buf[8]; sprintf(buf,"%d", m_nImportance);V.push_back(buf); return true;}

};


#endif //_XGEOLABEL_H
