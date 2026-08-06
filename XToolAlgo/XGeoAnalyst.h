//-----------------------------------------------------------------------------
//								XGeoAnalyst.h
//								=============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/10/2006
//-----------------------------------------------------------------------------

#ifndef _XGEOANALYST_H
#define _XGEOANALYST_H

#include <vector>
#include "../XTool/XBase.h"
#include "../XTool/XGeoObject.h"

class XGeoRepres;
class XGeoVector;
class XGeoBase;

class XGeoAnalyst {
public:
  enum eType { Null = 0, Fill_Lin = 1, Fill_Log = 2, Fill_Const = 3, All_Value = 4};

protected:
	eType						m_Type;					// Type d'analyse
	std::string			m_strName;			// Nom de l'analyse
	std::string			m_strLayer;			// Layer sur lequel porte l'analyse
	std::string			m_strClass;			// Classe sur laquelle porte l'analyse
	std::string			m_strAttrib;		// Attribut sur lequel porte l'analyse

	uint32_t					m_nPlage;
	uint32_t					m_nFirst;				// Premiere couleur
	uint32_t					m_nLast;				// Derniere couleur

	std::vector<XGeoRepres*>	m_Repres;	// Representations
  std::vector<double>       m_Borne;  // Limites des plages
	std::vector<bool>					m_Visibility;
	XGeoBase* m_Base;

	void DeleteRepres();
  void FillRepres(uint32_t first, uint32_t last);

	virtual uint32_t RunFillLin(XGeoBase* base);
	virtual uint32_t RunFillLog(XGeoBase* base);
	virtual uint32_t RunFillConst(XGeoBase* base);
  virtual uint32_t RunAllValue(XGeoBase* base);

public:
	XGeoAnalyst();
	virtual ~XGeoAnalyst();
  void Clear() { DeleteRepres();}

	void Name(const std::string& name) { m_strName = name;}
	void Layer(const std::string& layer) { m_strLayer = layer;}
	void Class(const std::string& classe) { m_strClass = classe;}
	void Attribut(const std::string& att) { m_strAttrib = att;}
	void Type(eType type) { m_Type = type;}

  std::string Name() { return m_strName;}
	std::string Layer() { return m_strLayer;}
	std::string Class() { return m_strClass;}
	std::string Attribut() { return m_strAttrib;}
	eType				Type() { return m_Type;}

	uint32_t NbRepres() { return (uint32_t)m_Repres.size();}
	XGeoRepres* Repres(uint32_t i) { if (i < m_Repres.size()) return m_Repres[i]; return nullptr;}
	bool Visibility(uint32_t i) { if (i < m_Visibility.size()) return m_Visibility[i]; return true; }
	void Visibility(uint32_t i, bool flag);

	bool SetFill(uint32_t plage, uint32_t first, uint32_t last);

	virtual uint32_t Run(XGeoBase* base);

  double BorneMax(int num_plage);
  bool UpdateBorne(XGeoBase* base, int num_plage, double new_max);

  virtual bool XmlRead(XParserXML* parser, uint32_t num = 0, XError* error = NULL);
  virtual bool XmlWrite(std::ostream* out);
};

#endif //_XGEOANALYST_H
