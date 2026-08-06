//-----------------------------------------------------------------------------
//								XGeoAnalyst.cpp
//								===============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 26/10/2006
//-----------------------------------------------------------------------------

#include "XGeoAnalyst.h"
#include "../XTool/XGeoRepres.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XGeoVector.h"
#include "../XTool/XParserXML.h"

#include <map>
#include <list>
#include <algorithm>
#include <cmath>

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XGeoAnalyst::XGeoAnalyst()
{
	m_Type = Null;
	m_strLayer = m_strClass = m_strAttrib = "Non defini";
	m_nPlage = m_nFirst = m_nLast = 0;
  m_Base = nullptr;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XGeoAnalyst::~XGeoAnalyst()
{
	DeleteRepres();
}

//-----------------------------------------------------------------------------
// Destruction des representations
//-----------------------------------------------------------------------------
void XGeoAnalyst::DeleteRepres()
{
	if ((m_Repres.size() < 1)||(m_Base == nullptr))
		return;
  for (uint32_t i = 0; i < m_Base->NbLayer(); i++) {
    XGeoLayer* layer = m_Base->Layer(i);
    for (uint32_t j = 0; j < layer->NbClass(); j++) {
      XGeoClass* C = layer->Class(j);
      for (uint32_t k = 0; k < C->NbVector(); k++) {
        XGeoVector* V = C->Vector(k);
        XGeoRepres* R = V->Repres();
        for (uint32_t p = 0; p < m_Repres.size(); p++) {
          if (R == m_Repres[p])
            V->Repres(nullptr);
        }
      }
    }
  }

  for (uint32_t i = 0; i < m_Repres.size(); i++)
		delete m_Repres[i];
	m_Repres.clear();
	m_nPlage = 0;
  m_Visibility.clear();
}

//-----------------------------------------------------------------------------
// Changement de la visibilite
//-----------------------------------------------------------------------------
void XGeoAnalyst::Visibility(uint32_t index, bool flag)
{ 
  if ((index >= m_Visibility.size()) || (index >= m_Repres.size()) || (m_Base == nullptr))
    return;
  if (Type() != All_Value)
    return;
  XGeoRepres* R = m_Repres[index];
  m_Visibility[index] = flag;
  for (uint32_t i = 0; i < m_Base->NbLayer(); i++) {
    XGeoLayer* layer = m_Base->Layer(i);
    for (uint32_t j = 0; j < layer->NbClass(); j++) {
      XGeoClass* C = layer->Class(j);
      for (uint32_t k = 0; k < C->NbVector(); k++) {
        XGeoVector* V = C->Vector(k);
        if (R == V->Repres())
          V->Visible(flag);
      }
    }
  }

}

//-----------------------------------------------------------------------------
// Determine les representations en fonction de deux couleurs (debut, fin)
//-----------------------------------------------------------------------------
void XGeoAnalyst::FillRepres(uint32_t first, uint32_t last)
{
  // Creation des representations
  int red, redF, redL, green, greenF, greenL, blue, blueF, blueL;
  uint8_t* ptr;
  ptr = (uint8_t*)&first;
  redF = (int)ptr[2];
  greenF = (int)ptr[1];
  blueF = (int)ptr[0];
  ptr = (uint8_t*)&last;
  redL = (int)ptr[2];
  greenL = (int)ptr[1];
  blueL = (int)ptr[0];

  for (uint32_t i = 0; i < m_Repres.size(); i++) {
    XGeoRepres* R = m_Repres[i];
    red = (redF * (m_nPlage - i - 1) + redL * i) / (m_nPlage - 1);
    green = (greenF * (m_nPlage - i - 1) + greenL * i) / (m_nPlage - 1);
    blue = (blueF * (m_nPlage - i - 1) + blueL * i) / (m_nPlage - 1);
    R->FillColorARGB(red, green, blue, 128);
    R->Symbol(R->DPBCSymbol(0, 0, i % 6, 0));
    R->ColorARGB(red, green, blue, 255);
    R->Size(1);
  }
}

//-----------------------------------------------------------------------------
// Fixe le type d'analyse en remplissage
//-----------------------------------------------------------------------------
bool XGeoAnalyst::SetFill(uint32_t plage, uint32_t first, uint32_t last)
{
	if (plage < 2)
		return false;

	DeleteRepres();

	m_nPlage = plage;
	m_nFirst = first;
	m_nLast = last;

  // Cas de l'affichage de toutes les valeurs : les representations seront creees a la volee
  if (m_Type == All_Value)
    return true;

	// Creation des representations
  char buf[80];
  for (uint32_t i = 0; i < m_nPlage; i++) {
    XGeoRepres* R = new XGeoRepres();
    sprintf(buf, "Plage %d", (int)i+1);
    R->Name(buf);
    m_Repres.push_back(R);
  }

  FillRepres(first, last);
	return true;
}

//-----------------------------------------------------------------------------
// Valeur de la borne max d'une plage
//-----------------------------------------------------------------------------
double XGeoAnalyst::BorneMax(int num_plage)
{
  if ((num_plage + 1) >= m_Borne.size())
    return 0;
  return m_Borne[num_plage + 1];
}

//-----------------------------------------------------------------------------
// Fait une analyse en remplissage (distribution lineaire)
//-----------------------------------------------------------------------------
uint32_t XGeoAnalyst::RunFillLin(XGeoBase* base)
{
	if (m_Type != Fill_Lin)
		return 0;
	
	XGeoClass* C = base->Class(m_strLayer.c_str(), m_strClass.c_str());
	if (C == NULL)
		return 0;

	// Recherche du min et du max
	std::string Att;
	double min, max, val;
	XGeoVector* V;

	V = C->Vector((uint32_t)0);
	if (V == NULL)
		return 0;
	Att = V->FindAttribute(m_strAttrib);
  try { min = max = std::stod(Att); }
  catch (...) { min = max = 0.; }

	for (uint32_t i = 1; i < C->NbVector(); i++) {
		V = C->Vector(i);
		if (!V->Visible())
			continue;
		Att = V->FindAttribute(m_strAttrib);
		if (Att.size() < 1)
			continue;
    try { val = std::stod(Att); } catch (...) { val = 0.; }
		min = XMin(min, val);
		max = XMax(max, val);
	}
	
	// Affectation des representations
	uint32_t index, nb_vec = 0;
  double delta = (max - min) / m_nPlage;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		if (!V->Visible())
			continue;
		Att = V->FindAttribute(m_strAttrib);
		if (Att.size() < 1)
			continue;
    try { val = std::stod(Att); } catch (...) { val = 0.; }
    index = (uint32_t)floor((val - min) / delta);
    if (index > (m_nPlage - 1)) index = m_nPlage - 1;
		V->Repres(m_Repres[index]);
    nb_vec++;
	}
  // Nom des representations
  char buf[80];
  for (uint32_t i = 0; i < m_nPlage; i++) {
    XGeoRepres* R = m_Repres[i];
    sprintf(buf, "%lf - %lf", min + i * delta, min + (i + 1) * delta);
    R->Name(buf);
  }
  // Valeurs des bornes
  m_Borne.clear();
  for (uint32_t i = 0; i < m_nPlage; i++)
    m_Borne.push_back(min + i * delta);
  m_Borne.push_back(max);

  if (nb_vec > 0) m_Base = base;
	return nb_vec;
}

//-----------------------------------------------------------------------------
// Fait une analyse en remplissage (distribution logarithmique)
//-----------------------------------------------------------------------------
uint32_t XGeoAnalyst::RunFillLog(XGeoBase* base)
{
	if (m_Type != Fill_Log)
		return 0;
	
	XGeoClass* C = base->Class(m_strLayer.c_str(), m_strClass.c_str());
	if (C == NULL)
		return 0;

	// Recherche du min et du max
	std::string Att;
	double min, max, val;
	XGeoVector* V;

	V = C->Vector((uint32_t)0);
	if (V == NULL)
		return 0;
	Att = V->FindAttribute(m_strAttrib);
  try { min = max = std::stod(Att); }
  catch (...) { min = max = 0.; }

	for (uint32_t i = 1; i < C->NbVector(); i++) {
		V = C->Vector(i);
		if (!V->Visible())
			continue;
		Att = V->FindAttribute(m_strAttrib);
		if (Att.size() < 1)
			continue;
    try { val = std::stod(Att); }
    catch (...) { val = 0.; }
		min = XMin(min, val);
		max = XMax(max, val);
	}
	
	// Affectation des representations
	uint32_t index, nb_vec = 0;
  double delta = log(max - min + 1) / m_nPlage;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		if (!V->Visible())
			continue;
		Att = V->FindAttribute(m_strAttrib);
		if (Att.size() < 1)
			continue;
    try { val = std::stod(Att); }
    catch (...) { val = 0.; }
    index = (uint32_t)floor(log(val - min + 1) / delta);
    if (index > (m_nPlage - 1)) index = (m_nPlage - 1);
		V->Repres(m_Repres[index]);
    nb_vec++;
	}
  // Nom des representations
  char buf[80];
  for (uint32_t i = 0; i < m_nPlage; i++) {
    XGeoRepres* R = m_Repres[i];
    sprintf(buf, "%lf - %lf", min + exp(i * delta) - 1, min + exp((i + 1) * delta) - 1);
    R->Name(buf);
  }
  // Valeurs des bornes
  m_Borne.clear();
  for (uint32_t i = 0; i < m_nPlage; i++)
    m_Borne.push_back(min + exp(i * delta) - 1);
  m_Borne.push_back(max);

  if (nb_vec > 0) m_Base = base;
  return nb_vec;
}

//-----------------------------------------------------------------------------
// Fait une analyse en remplissage a effectif constant
//-----------------------------------------------------------------------------
uint32_t XGeoAnalyst::RunFillConst(XGeoBase* base)
{
	if (m_Type != Fill_Const)
		return 0;
	
	XGeoClass* C = base->Class(m_strLayer.c_str(), m_strClass.c_str());
	if (C == NULL)
		return 0;

	// Creation d'une multimap
	std::multimap<double, uint32_t> M;
	std::multimap<double, uint32_t>::iterator iter; 
	std::multimap<double, uint32_t>::key_compare kc1 = M.key_comp();

	std::string Att;
	double val;
	XGeoVector* V;
	uint32_t count = 0;
	for (uint32_t i = 0; i < C->NbVector(); i++) {
		V = C->Vector(i);
		if (!V->Visible())
			continue;
		Att = V->FindAttribute(m_strAttrib);
		if (Att.size() < 1)
			continue;
    try { val = std::stod(Att); }
    catch (...) { val = 0.; }
    M.insert(std::make_pair(val, i));
		count++;
	}
	
	// Affectation des representations
	uint32_t step = 0, nb_vec = 0;
	for (iter = M.begin(); iter != M.end(); iter++) {
		V = C->Vector(iter->second);
		V->Repres(m_Repres[(step * m_nPlage) / count]);
    nb_vec++;
		step++;
	}
  if (nb_vec > 0) m_Base = base;
	return nb_vec;
}

//-----------------------------------------------------------------------------
// Fait une analyse sur toutes les valeurs
//-----------------------------------------------------------------------------
uint32_t XGeoAnalyst::RunAllValue(XGeoBase* base)
{
  if (m_Type != All_Value)
    return 0;

  XGeoClass* C = base->Class(m_strLayer.c_str(), m_strClass.c_str());
  if (C == NULL)
    return 0;

  DeleteRepres();

  bool exist;
  XGeoVector* V;
  std::string Att;
  uint32_t nb_vec = 0;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    V = C->Vector(i);
    if (!V->Visible())
      continue;
    Att = V->FindAttribute(m_strAttrib);
//    if (Att.size() < 1)
//      continue;
    if (Att == " ") Att = "";
    exist = false;
    for(uint32_t j = 0; j < m_Repres.size(); j++) {
      if (m_Repres[j]->Name() == Att) {
        V->Repres(m_Repres[j]);
        nb_vec++;
        exist = true;
        break;
      }
    }
    if ((!exist)&&( m_Repres.size() < 200)) {
      XGeoRepres* newR = new XGeoRepres;
      newR->Name(Att.c_str());
      m_Repres.push_back(newR);
      V->Repres(newR);
      m_Visibility.push_back(true);
      nb_vec++;
    }
  }
  m_nPlage = (uint32_t)m_Repres.size();
  struct { bool operator()(XGeoRepres* a, XGeoRepres* b) const { return *a < *b; } } customLess;
  std::sort(m_Repres.begin(), m_Repres.end(), customLess);
  FillRepres(m_nFirst, m_nLast);

  if (nb_vec > 0) m_Base = base;
  return nb_vec;
}

//-----------------------------------------------------------------------------
// Lancement de l'analyse
//-----------------------------------------------------------------------------
uint32_t XGeoAnalyst::Run(XGeoBase* base)
{
	if ((m_strLayer.size() < 1) || (m_strClass.size() < 1) || (m_strAttrib.size() < 1))
		return 0;
  DeleteRepres();
	if (m_Type == Fill_Lin)
		return RunFillLin(base);
	if (m_Type == Fill_Log)
		return RunFillLog(base);
	if (m_Type == Fill_Const)
		return RunFillConst(base);
  if (m_Type == All_Value)
    return RunAllValue(base);
	return 0;
}

//-----------------------------------------------------------------------------
// Mise à jour d'une borne
//-----------------------------------------------------------------------------
bool XGeoAnalyst::UpdateBorne(XGeoBase *base, int num_plage, double new_max)
{
  if ((m_Type != Fill_Lin)&&(m_Type != Fill_Log))
    return false;

  // Valeurs des bornes
  m_Borne[num_plage+1] = new_max;

  XGeoClass* C = base->Class(m_strLayer.c_str(), m_strClass.c_str());
  if (C == nullptr)
    return false;

  // Recherche du min et du max
  std::string Att;
  double val;
  XGeoVector* V;

  // Affectation des representations
  uint32_t index;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    V = C->Vector(i);
    if (!V->Visible())
      continue;
    Att = V->FindAttribute(m_strAttrib);
    if (Att.size() < 1)
      continue;
    try { val = std::stod(Att); }
    catch (...) { val = 0.; }
    index = 0;
    if (val >= m_Borne[m_Borne.size()-1])
      index = (uint32_t)m_Borne.size()- 1;
    for (uint32_t j = 1; j < m_Borne.size(); j++) {
      if ((val >= m_Borne[j - 1])&&(val < m_Borne[j])) {
          index = j - 1;
          break;
      }
    }
    if (index > (m_nPlage - 1)) index = m_nPlage - 1;
    V->Repres(m_Repres[index]);
  }
  // Nom des representations
  char buf[80];
  for (uint32_t i = 0; i < m_nPlage; i++) {
    XGeoRepres* R = m_Repres[i];
    sprintf(buf, "%lf - %lf", m_Borne[i],  m_Borne[i+1]);
    R->Name(buf);
  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture au format XML
//-----------------------------------------------------------------------------
bool XGeoAnalyst::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
  DeleteRepres();
  XParserXML analyst = parser->FindSubParser("/xgeoanalyst", num);
  if (analyst.IsEmpty())
    return XErrorError(error, "XGeoAnalyst::XmlRead", XError::eBadFormat);

  m_strName = analyst.ReadNode("/xgeoanalyst/name");
  m_strLayer = analyst.ReadNode("/xgeoanalyst/layer");
  m_strClass = analyst.ReadNode("/xgeoanalyst/class");
  m_strAttrib = analyst.ReadNode("/xgeoanalyst/attribut");
  m_Type = (eType)analyst.ReadNodeAsInt("/xgeoanalyst/type");
  m_nFirst = analyst.ReadNodeAsUInt32("/xgeoanalyst/first_color");
  m_nLast = analyst.ReadNodeAsUInt32("/xgeoanalyst/last_color");
  m_nPlage = analyst.ReadNodeAsUInt32("/xgeoanalyst/nb_plage");

  for (uint32_t i = 0; i < m_nPlage; i++) {
    XParserXML repres = analyst.FindSubParser("/xgeoanalyst/xgeorepres", i);
    if (!repres.IsEmpty()) {
      XGeoRepres* R = new XGeoRepres;
      if (!R->XmlRead(&repres)) {
        delete R;
        return XErrorError(error, "XGeoAnalyst::XmlRead", XError::eBadFormat);
      }
      m_Repres.push_back(R);
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Ecriture au format XML
//-----------------------------------------------------------------------------
bool XGeoAnalyst::XmlWrite(std::ostream* out)
{
  *out << "<xgeoanalyst>" << std::endl;
  *out << "<name> " << m_strName << " </name>" << std::endl;
  *out << "<layer> " << m_strLayer << " </layer>" << std::endl;
  *out << "<class> " << m_strClass << " </class>" << std::endl;
  *out << "<attribut> " << m_strAttrib << " </attribut>" << std::endl;
  *out << "<type> " << m_Type << " </type>" << std::endl;
  *out << "<first_color> " << m_nFirst << " </first_color>" << std::endl;
  *out << "<last_color> " << m_nLast << " </last_color>" << std::endl;
  *out << "<nb_plage> " << m_Repres.size() << " </nb_plage>" << std::endl;

  for (uint32_t i = 0; i < m_Repres.size(); i++)
    m_Repres[i]->XmlWrite(out);

  *out << "</xgeoanalyst>" << std::endl;

  return out->good();
}

