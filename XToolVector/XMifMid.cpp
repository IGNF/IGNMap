//-----------------------------------------------------------------------------
//								XMifMid.cpp
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 18/09/2003
//-----------------------------------------------------------------------------

#include <algorithm>
#include <cctype>
#include <cstring>
#include "XMifMid.h"
#include "../XTool/XPath.h"
#include "../XToolGeod/XGeodConverter.h"
#include "../XTool/XGeoBase.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XMifMid::XMifMid()
{
	m_nVersion = 0;
	m_Sep = '\t';
	m_strFilename = "Inconnu";
	m_strName = "Inconnu";
	m_nRecord = 0;
	m_nAttPos = 0xFFFFFFFF;
	m_nNbPoint = m_nNbLine = m_nNbPoly = 0;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XMifMid::~XMifMid()
{
	for (uint32_t i = 0; i < m_Repres.size(); i++)
		delete m_Repres[i];
	m_Repres.clear();
}

//-----------------------------------------------------------------------------
// Fixe la classe d'objets
//-----------------------------------------------------------------------------
void XMifMid::Class(XGeoClass* C)
{
  uint32_t i;
  for (i = 0; i < m_Data.size(); i++){
		XGeoVector* record = (XGeoVector*)m_Data[i];
		record->Class(C);
		C->Vector(record);
	}
	// Fixe le schema de la classe a partir du MIF
	std::string name, type;
	int length = 20, dec = 0;
	XGeoAttribut::eType attType = XGeoAttribut::String;
	XGeoSchema schema;
	for (i = 0; i < m_Column.size(); i++) {
		name = m_Column[i];
		type = m_Type[i];
		dec = 0;
		if ((type == "integer")||(type == "Integer")) attType = XGeoAttribut::Int32;
		if ((type == "smallint")||(type == "Smallint")) attType = XGeoAttribut::Int16;
		if ((type == "float")||(type == "Float")) attType = XGeoAttribut::Double;
		if ((strncmp(type.c_str(), "decimal", 7)==0)||(strncmp(type.c_str(), "Decimal", 7)==0)) {
			size_t pos = type.find('(');
			if (pos > 0) {
				(void)sscanf(type.substr(pos+1).c_str(),"%d", &length);
				pos = type.find(',');
				(void)sscanf(type.substr(pos+1).c_str(),"%d", &dec);
				attType = XGeoAttribut::NumericN;
			} else
				length = 20;
		}
		if ((strncmp(type.c_str(), "char", 4)==0)||(strncmp(type.c_str(), "Char", 4)==0)) {
			size_t pos = type.find('(');
			if (pos > 0) {
				sscanf(type.substr(pos+1).c_str(),"%d", &length);
				attType = XGeoAttribut::String;
			} else
				length = 20;
		}

		schema.AddAttribut(name, name, "", attType, length, (uint8_t)dec);
	}
	C->SetSchema(schema);
}
	
//-----------------------------------------------------------------------------
// Renvoi la classe d'objets
//-----------------------------------------------------------------------------
XGeoClass* XMifMid::Class()
{
	if (m_Data.size() < 1)
		return NULL;
	XGeoVector* record = (XGeoVector*)m_Data[0];
	return record->Class();
}

//-----------------------------------------------------------------------------
// Ajout d'une representation
//-----------------------------------------------------------------------------
XGeoRepres* XMifMid::AddRepres(XGeoRepres* repres)
{
	for (uint32_t i = 0; i < m_Repres.size(); i++)
		if ( *m_Repres[i] == *repres )
			return m_Repres[i];
	return repres;
}

//-----------------------------------------------------------------------------
// Lecture de l'entete d'un fichier Mif/Mid
//-----------------------------------------------------------------------------
bool XMifMid::ReadHeader(XError* error)
{
	std::string token;
	char buf[1024];

	m_Sep = '\t';	// Valeur par defaut
	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In >> token;
		std::transform(token.begin(), token.end(), token.begin(), tolower);
		if (token == "version") {
			m_In >> m_nVersion;
			continue;
		}
		if (token == "columns") {
			uint16_t nb;
			m_In >> nb;
			ReadColumns(nb);
			continue;
		}
		if (token == "delimiter") {
			m_In >> token;
			if (token.size() > 2)
				m_Sep = token[1];
			continue;
		}
		if (token == "data")
			return true;

		m_In.get(buf, 1024);
	}
	return XErrorError(error, "XMifMid::ReadHeader", XError::eBadFormat);
}

//-----------------------------------------------------------------------------
// Lecture des colonnes
//-----------------------------------------------------------------------------
bool XMifMid::ReadColumns(uint16_t nb)
{
	std::string token;
	char buf[1024];
	m_Column.clear();
	m_Type.clear();
	for (uint16_t i = 0; i < nb; i++) {
		m_In >> token;
		m_Column.push_back(token);
		m_In >> token;
		m_In.get(buf, 1024);
		if (strlen(buf) > 0) {
			if (buf[strlen(buf) - 1] == 0x0D)		// Line Feed
				buf[strlen(buf) - 1] = '\0';
			token += buf;
		} else
			m_In.clear();
		m_Type.push_back(token);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier MIF/MID
//-----------------------------------------------------------------------------
bool XMifMid::Read(const char* filename, bool extract_repres, XError* error)
{
	XPath P;
	m_strFilename = filename;
	m_strName = P.Name(filename);
	m_In.open(filename, std::ios_base::in | std::ios_base::binary);
	if (!m_In.good())
		return XErrorError(error, "XMifMid::Read", XError::eIOOpen);

	if (!ReadHeader(error))
		return false;
	XMifMidPoint2D* point;
	XMifMidLine2D* line;
	XMifMidMLine2D* lineM;
	XMifMidMPoly2D* poly;

	std::string type, token;
	char sep;
	uint32_t num;
	uint32_t fill, width, pattern, back, symbol;
	XGeoRepres* repres = NULL;
	char buf[1024];
	m_Frame = XFrame();
	m_nNbPoint = m_nNbLine = m_nNbPoly = 0;

	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In >> type;
		m_In.get(sep);
		std::transform(type.begin(), type.end(), type.begin(), tolower);
		if (!m_In.good())
			break;
		if (type == "none")
			return true;

		if (type == "point") {
			point = new XMifMidPoint2D(this, (uint32_t)m_Data.size());
			point->Read(&m_In, error);
			m_Data.push_back(point);
			m_Frame += point->Frame();
			m_nNbPoint++;
			if (extract_repres) {
				if (repres == NULL)
					continue;
				point->Repres(AddRepres(repres));
				if (point->Repres() != repres)
					delete repres;
				repres = NULL;
			}
			continue;
		}

		if (type == "line") {
			line = new XMifMidLine2D(this, (uint32_t)m_Data.size());
			line->Read(&m_In, error);
			m_Data.push_back(line);
			m_Frame += line->Frame();
			m_nNbLine++;
			if (extract_repres) {
				if (repres == NULL)
					continue;
				line->Repres(AddRepres(repres));
				if (line->Repres() != repres)
					delete repres;
				repres = NULL;
			}
			continue;
		}

		if (type == "pline") {
			num = 1;
			if (sep != '\n') {
				std::streamoff pos = m_In.tellg();
				m_In >> token;
				std::transform(token.begin(), token.end(), token.begin(), tolower);
				if (token == "multiple")
					m_In >> num;
				else
					m_In.seekg(pos);
			}
			lineM = new XMifMidMLine2D(this, (uint32_t)m_Data.size());
			lineM->Read(&m_In, num, error);
			m_Data.push_back(lineM);
			m_Frame += lineM->Frame();
			m_nNbLine++;
			if (extract_repres) {
				if (repres == NULL)
					continue;
				lineM->Repres(AddRepres(repres));
				if (lineM->Repres() != repres)
					delete repres;
				repres = NULL;
			}
			continue;
		}

		if (type == "region") {
			poly = new XMifMidMPoly2D(this, (uint32_t)m_Data.size());
			poly->Read(&m_In, error);
			m_Data.push_back(poly);
			m_Frame += poly->Frame();
			m_nNbPoly++;
			if (extract_repres) {
				if (repres == NULL)
					continue;
				poly->Repres(AddRepres(repres));
				if (poly->Repres() != repres)
					delete repres;
				repres = NULL;
			}
			continue;
		}

		/*
		if (type == "center") {
			double x, y;
			m_In >> x >> y;
			poly->Centroide(x, y);
			continue;
		}*/

		if (extract_repres) {
			if (type == "symbol") {
				m_In.get(buf, 1024);
				(void)sscanf(buf,"(%u,%u,%u)", &symbol, &fill, &width);
				if (repres == NULL) repres = new XGeoRepres;
				repres->Size((uint8_t)width);
				repres->Color(MifColor(fill));
				repres->Symbol(symbol);
				continue;
			}

			if (type == "pen") {
				m_In.get(buf, 1024);
				(void)sscanf(buf,"(%u,%u,%u)", &width, &pattern, &fill);
				if (repres == NULL) repres = new XGeoRepres;
				repres->Size((uint8_t)width);
				repres->Color(MifColor(fill));
				continue;
			}

			if (type == "brush") {
				m_In.get(buf, 1024);
				uint16_t nb_elt = 0;
				for (uint32_t k = 0; k < strlen(buf); k++)
          if (buf[k] == ',') nb_elt++;
				back = 0;
				if (nb_elt >= 2)
					(void)sscanf(buf,"(%u,%u,%u)", &pattern, &fill, &back);
				else
					(void)sscanf(buf,"(%u,%u)", &pattern, &fill);
				if (repres == NULL) repres = new XGeoRepres;
				repres->FillColor(MifColor(fill));
				if (pattern == 1)
					repres->FillColor(0xFFFFFFFF);
				continue;
			}
		}

		if ((sep != '\n')&&(sep != '\r'))
			m_In.get(buf, 1024);
	}

	m_In.clear();
	return true;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XMifMid::ReadAttributes(uint32_t num, std::vector<std::string>& V)
{
	V.clear();
	if (num == m_nAttPos) {
		V = m_Att;
		return true;
	}

	if ( (!m_Mid.good()) || (!m_Mid.is_open()) ) {
		m_Mid.clear();
		std::string midfilename = m_strFilename;
		midfilename = midfilename.substr(0, midfilename.rfind('.'));
		midfilename += ".mid";

		m_Mid.open(midfilename.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!m_Mid.good())
			return false;
		m_nRecord = 0;
	}
	
	char buf[1024];
  uint32_t i;

	if (num < m_nRecord) {
		m_Mid.seekg(0);
		m_nRecord = 0;
	}
  for (i = m_nRecord; i < num; i++)
		m_Mid.ignore(1024, '\n');
	m_Mid.getline(buf, 1024);
	m_nRecord = num+1;

	if (!m_Mid.good())
		return false;

	uint32_t n = 0;
	std::string token;
	bool inside_quote = false;
	for (i = 0; i < m_Column.size(); i++) {
		V.push_back(m_Column[i]);
		token = "";
		while(true) {
			if ((buf[n] != m_Sep)&& (n < strlen(buf))){
				if((buf[n] != '"')&&(buf[n] != 0x0A)&&(buf[n] != 0x0D))
					token += buf[n];
				else
					if (buf[n] == '"') inside_quote = !inside_quote;
				n++;
			} else {
				if ((inside_quote)&&(buf[n] == m_Sep)) {
					token += buf[n];
					n++;
				} else {
					n++;
					break;
				}
			}
		}
		V.push_back(token);
	}
	m_Att = V;
	m_nAttPos = num;

	return true;
}

//-----------------------------------------------------------------------------
// Recherche d'un attribut
//-----------------------------------------------------------------------------
std::string XMifMid::FindAttribute(uint32_t num, std::string att_name)
{
	bool flag = false;
  uint32_t i, att_num = 0;
  for (i = 0; i < m_Column.size(); i++)
		if (m_Column[i] == att_name) {
			flag = true;
			att_num = i;
			break;
		}
	if (!flag)
		return "";

	if ( (!m_Mid.good()) || (!m_Mid.is_open()) ) {
		m_Mid.clear();
		std::string midfilename = m_strFilename;
		midfilename = midfilename.substr(0, midfilename.rfind('.'));
		midfilename += ".mid";

		m_Mid.open(midfilename.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!m_Mid.good())
      return "";
		m_nRecord = 0;
	}
	
	char buf[1024];

	if (num < m_nRecord) {
		m_Mid.seekg(0);
		m_nRecord = 0;
	}
	for (i = m_nRecord; i < num; i++)
		m_Mid.ignore(1024, '\n');
	m_Mid.getline(buf, 1024);
	m_nRecord = num+1;

	if (!m_Mid.good())
    return "";

	uint32_t n = 0;
	std::string token;
	bool inside_quote = false;
	for (i = 0; i < m_Column.size(); i++) {
		token = "";
		while(true) {
			if ((buf[n] != m_Sep)&& (n < strlen(buf))){
				if((buf[n] != '"')&&(buf[n] != 0x0A)&&(buf[n] != 0x0D))
					token += buf[n];
				else
					if (buf[n] == '"') inside_quote = !inside_quote;
				n++;
			} else {
				if ((inside_quote)&&(buf[n] == m_Sep)) {
					token += buf[n];
					n++;
				} else {
					n++;
					break;
				}
			}
		}
		if (i == att_num)
			return token;
	}

	return "";
}

//-----------------------------------------------------------------------------
// Lecture de la symbolisation stockee dans le fichier MIF
//-----------------------------------------------------------------------------
bool XMifMid::ReadRepres(XGeoRepres* repres)
{
	if (!m_In.good())
		return false;
	m_In.seekg(0);
	if (!ReadHeader())
		return false;

	std::string type, token;
	char sep;
	char buf[1024];
	uint32_t fill, width, pattern, back, symbol;
	bool pen_flag = false, fill_flag = false;

	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In >> type;
		m_In.get(sep);
		std::transform(type.begin(), type.end(), type.begin(), tolower);

		if (type == "symbol") {
			m_In.get(buf, 1024);
			(void)sscanf(buf,"(%u,%u,%u)", &symbol, &fill, &width);
			repres->Size((uint8_t)width);
			repres->Color(MifColor(fill));
			repres->Symbol(symbol);
			return true;
		}

		if (type == "pen") {
			m_In.get(buf, 1024);
			(void)sscanf(buf,"(%u,%u,%u)", &width, &pattern, &fill);
			repres->Size((uint8_t)width);
			repres->Color(MifColor(fill));
			pen_flag = true;
			if (m_nNbPoly < 1)
				return true;
			if (fill_flag)
				return true;
		}

		if (type == "brush") {
			m_In.get(buf, 1024);
			(void)sscanf(buf,"(%u,%u,%u)", &pattern, &fill, &back);
			repres->FillColor(MifColor(fill));
			if (pattern == 1)
				repres->FillColor(0xFFFFFFFF);
			fill_flag = true;
			if (pen_flag)
				return true;
		}
	}
	m_In.clear();
	return false;
}

//-----------------------------------------------------------------------------
// Conversion couleur MIF/MID <-> couleur Windows
//-----------------------------------------------------------------------------
uint32_t XMifMid::MifColor(uint32_t c)
{
	uint32_t r, g, b;
	uint8_t* ptr = (uint8_t*)&c;
	b = ptr[0];
	g = ptr[1];
	r = ptr[2];
	return (b * 65536 + g * 256 + r);
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XMifMid::Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error)
{
	XPath P;
	m_strFilename = file_in;
	m_strName = P.Name(file_in);
	m_In.open(file_in, std::ios_base::in | std::ios_base::binary);
	if (!m_In.good())
		return XErrorError(error, "XMifMid::Convert", XError::eIOOpen);

	if (!ReadHeader(error))
		return false;

	// Ouverture du fichier de sortie
	std::ofstream mif;
	mif.open(file_out);
	if (!mif.good())
		return XErrorError(error, "XMifMid::Convert", XError::eIOOpen);
	mif.setf(std::ios::fixed);
	mif.precision(3);
	if (L->EndProjection() == XGeoProjection::RGF93)
		mif.precision(9);

	// Entete du MIF
	mif << "VERSION " << m_nVersion << std::endl;
	mif << "Charset \"WindowsLatin1\"" << std::endl;
	mif << "DELIMITER \"" << m_Sep << "\"" << std::endl;
	mif << XGeoProjection::MifProjection(L->EndProjection()) << std::endl;
	mif << "COLUMNS " << m_Column.size() << std::endl;
	for (uint32_t i = 0; i < m_Column.size(); i++)
		mif << m_Column[i] << " " << m_Type[i] << std::endl;

	double xi, yi, xf, yf, a, b;
	std::string type, token;
	char sep;
	uint32_t num, nb;
	std::streampos pos;
	char buf[1024];

	mif << "DATA" << std::endl;
	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In >> type;
		m_In.get(sep);
		token = type;
		std::transform(type.begin(), type.end(), type.begin(), tolower);
		if (!m_In.good())
			break;

		if (type == "none") {
			mif << "NONE" << std::endl;
			continue;
		}

		if (type == "point") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "POINT " <<  xf << " " << yf << std::endl;
			continue;
		}

		if (type == "line") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "LINE " << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			continue;
		}

		if (type == "rect") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "RECT " << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			continue;
		}

		if (type == "roundrect") {
			m_In >> xi >> yi >> a;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "ROUNDRECT " << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			mif << a << std::endl;
			continue;
		}

		if (type == "arc") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "ARC " << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			m_In >> a >> b;
			mif << a << " " << b << std::endl;
			continue;
		}

		if (type == "ellipse") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "ELLIPSE " << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			continue;
		}

		if (type == "text") {
			mif << "TEXT ";
      int nb_quote = 0;
      char c;
      while (nb_quote < 2) {	// Prise en compte du texte (saut de ligne eventuel
        m_In.get(c);					// entre TEXT et le texte
        if (c == '\"') nb_quote++;
        if (nb_quote > 0) mif.put(c);
      }
      mif << std::endl;
      m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << xf << " " << yf;
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << " " << xf << " " << yf << std::endl;
			continue;
		}

		if (type == "pline") {
			num = 1;
			mif << "PLINE";
			if (sep != '\n') {
				pos = m_In.tellg();
				m_In >> token;
				std::transform(token.begin(), token.end(), token.begin(), tolower);
				if (token == "multiple") {
					m_In >> num;
					mif << " MULTIPLE " << num;
				} else
					m_In.seekg(pos);
			}
			mif << std::endl;

			for (uint32_t part = 0; part < num; part++) {
				m_In >> nb;
				mif << nb << std::endl;
				for (uint32_t i = 0; i < nb; i++) {
					m_In >> xi >> yi;
					L->ConvertDeg(xi, yi, xf, yf);
					mif << xf << " " << yf << std::endl;
				}
			}
			continue;
		}

		if (type == "region") {
			m_In >> num;
			mif << "REGION " << num << std::endl;
			for (uint32_t part = 0; part < num; part++) {
				m_In >> nb;
				mif << nb << std::endl;
				for (uint32_t i = 0; i < nb; i++) {
					m_In >> xi >> yi;
					L->ConvertDeg(xi, yi, xf, yf);
					mif << xf << " " << yf << std::endl;
				}
			}
			continue;
		}

		if (type == "center") {
			m_In >> xi >> yi;
			L->ConvertDeg(xi, yi, xf, yf);
			mif << "CENTER " << xf << " " << yf << std::endl;
			continue;
		}

		// Recopie des elements non interpretes
    mif << token << " ";
		if ((sep == 0x0D)||(sep == '\n')) {
			mif << std::endl;
			continue;
		}
		m_In.get(buf, 1024);
		if (buf[strlen(buf) - 1] == 0x0D)		// Line Feed
			buf[strlen(buf) - 1] = '\0';
		mif << buf << std::endl;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Import d'un MIF/MID dans une XGeoBase
//-----------------------------------------------------------------------------
XGeoClass* XMifMid::ImportMifMid(XGeoBase* base, const char* path, XGeoMap* map)
{
  XPath P;
  XMifMid* file = new XMifMid;
  XGeoClass* C = NULL;

  if (file->Read(path)) {
    C = base->AddClass(P.Name(P.Path(path).c_str()).c_str(), P.Name(path, false).c_str());
    file->Class(C);
    if (map == NULL)
      base->AddMap(file);
    else
      map->AddObject(file);
    base->SortClass();
  } else
    delete file;

  return C;
}

//-----------------------------------------------------------------------------
// Lecture d'un point
//-----------------------------------------------------------------------------
bool XMifMidPoint2D::Read(std::ifstream* in, XError* /*error*/)
{
	*in >> m_Frame.Xmin >> m_Frame.Ymax;
	m_Frame.Xmax = m_Frame.Xmin;
	m_Frame.Ymin = m_Frame.Ymax;
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne simple
//-----------------------------------------------------------------------------
bool XMifMidLine2D::Read(std::ifstream* in, XError* /*error*/)
{
	m_Pos = in->tellg();
	double x1, x2, y1, y2;
	*in >> x1 >> y1 >> x2 >> y2;
	m_Frame.Xmax = XMax(x1, x2);
	m_Frame.Ymax = XMax(y1, y2);
	m_Frame.Xmin = XMin(x1, x2);
	m_Frame.Ymin = XMin(y1, y2);
	m_nNumPoints = 2;
	return in->good();
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour une ligne simple
//-----------------------------------------------------------------------------
bool XMifMidLine2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	m_Pt = new XPt[2];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}

	in->seekg(m_Pos);
	*in >> m_Pt[0].X >> m_Pt[0].Y >> m_Pt[1].X >> m_Pt[1].Y;
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne multiple
//-----------------------------------------------------------------------------
bool XMifMidMLine2D::Read(std::ifstream* in, uint32_t num, XError* /*error*/)
{
	m_Pos = in->tellg();
	m_nNumParts = num;
	uint32_t nb;
	double x, y;
	m_Frame = XFrame();
	m_nNumPoints = 0;
  for (int part = 0; part < m_nNumParts; part++) {
		*in >> nb;
		m_nNumPoints += nb;
		for (uint32_t i = 0; i < nb; i++) {
			*in >> x >> y;
			m_Frame += XPt2D(x, y);
		}
	}
	return in->good();
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour une ligne multiple
//-----------------------------------------------------------------------------
bool XMifMidMLine2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);	

	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}

	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}

	uint32_t nb = 0, cmpt = 0, part = 0;
  for (int i = 0; i < m_nNumParts; i++) {
		m_Parts[i] = part;
		*in >> nb;
		part += nb;
		for (uint32_t j = 0; j < nb; j++) {
			*in >> m_Pt[cmpt].X >> m_Pt[cmpt].Y;
			cmpt++;
		}
	}
		
	return in->good();
}

//-----------------------------------------------------------------------------
// Lecture d'un polygone multiple
//-----------------------------------------------------------------------------
bool XMifMidMPoly2D::Read(std::ifstream* in, XError* /*error*/)
{
	m_Pos = in->tellg();
	*in >> m_nNumParts;
	uint32_t nb;
	double x, y;
	m_nNumPoints = 0;
	m_Frame = XFrame();
  for (int i = 0; i < m_nNumParts; i++) {
		*in >> nb;
		m_nNumPoints += nb;
		for (uint32_t j = 0; j < nb; j++) {
			*in >> x >> y;
			m_Frame += XPt2D(x, y);
		}
	}
	return in->good();
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie pour un polygone multiple
//-----------------------------------------------------------------------------
bool XMifMidMPoly2D::LoadGeom()
{
	if (m_File == NULL)
		return false;
	std::ifstream* in = m_File->IStream();
	if (!in->good())
		return false;

	in->seekg(m_Pos);	

	*in >> m_nNumParts;

	m_Parts = new int[m_nNumParts];
	if (m_Parts == NULL) {
		Unload();
		return false;
	}

	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL) {
		Unload();
		return false;
		}

	uint32_t nb = 0, cmpt = 0, part = 0;
  for (int i = 0; i < m_nNumParts; i++) {
		m_Parts[i] = part;
		*in >> nb;
		part += nb;
		for (uint32_t j = 0; j < nb; j++) {
			*in >> m_Pt[cmpt].X >> m_Pt[cmpt].Y;
			cmpt++;
		}
	}

	return in->good();
}
