//-----------------------------------------------------------------------------
//								XDxf.cpp
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/05/2004
//-----------------------------------------------------------------------------

#include <algorithm>
#include <cctype>
#include <cstring>

#include "XDxf.h"
#include "../XTool/XPath.h"
#include "../XTool/XGeoBase.h"
#include "../XToolGeod/XGeodConverter.h"


//-----------------------------------------------------------------------------
// Import d'un fichier DXF dans une GeoBase
//-----------------------------------------------------------------------------
bool XDxf::ImportDxf(XGeoBase* base, const char* path, XGeoMap* map)
{
  XDxf* file = new XDxf;
  if (file->Read(path, base)) {
    if (map == NULL)
      base->AddMap(file);
    else
      map->AddObject(file);
  } else {
    delete file;
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier DXF
//-----------------------------------------------------------------------------
bool XDxf::Read(const char* filename, XGeoBase* base, XError* error)
{
	XPath P;
	m_strFilename = filename;
	m_strName = P.Name(filename);
	m_In.open(filename, std::ios_base::in | std::ios_base::binary);
	if (!m_In.good())
		return XErrorError(error, "XDxf::Read", XError::eIOOpen);

	XDxfPoint2D* point2D;
	XDxfPoint3D* point3D;
	XDxfText2D* text2D;
	XDxfText3D* text3D;
	XDxfLine2D* line2D;
	XDxfLine3D* line3D;
	XDxfPoly2D* poly2D;
	XDxfPoly3D* poly3D;

	std::string value, layer, text;
	uint32_t pos = 0, code = 0, nbpt = 0, importance = 0, flagbits = 0, nbpt_spec = 0, nbz = 0;
	bool flag3d, closed, vertex, textflag, noread = false, entities = false, section = false, valid_line;
	double x = 0., y = 0., z = 0.;
	XFrame XYF;
	XPt2D SI, SF;
	XGeoClass* C;

	m_Frame = XFrame();
	XGeoClass* NoLayer = base->AddClass(m_strName.c_str(), "LAYER0");

	while( (!m_In.eof()) && (m_In.good()) ) {
		if (!noread) {
			m_In.getline(m_Line, 1024);
			std::ignore = sscanf(m_Line,"%u", &code);
			m_In.getline(m_Line, 1024);
		}

		if ((code != 0)&&(code != 2))
			continue;

		if (m_Line[strlen(m_Line)- 1] == '\r')
			m_Line[strlen(m_Line) - 1] = 0;
		value = m_Line;
		std::transform(value.begin(), value.end(), value.begin(), tolower);
		pos = (uint32_t)m_In.tellg();

		flag3d = closed = noread = vertex = textflag = false;
		text = layer = "";
		nbpt = importance = flagbits = nbz = 0;
		XYF = XFrame();

		// Debut d'une section
		if (value == "section") {
			section = true;
			continue;
		}
		if (!section) continue;

		// Fin d'une section
		if (value == "endsec") {
			section = false;
			entities = false;
			continue;
		}

		// Debut des entites
		if (value == "entities") {
			entities = true;
			continue;
		}
		if (!entities) continue;

		valid_line = false;

		// Gestion des points et des textes
    if ((value == "point")||(value == "text")||(value == "insert")) {
			if (value == "text")
				textflag = true;
			while( (!m_In.eof()) && (m_In.good()) && (!noread) ) {
				m_In.getline(m_Line, 1024);
				std::ignore = sscanf(m_Line,"%u", &code);
				m_In.getline(m_Line, 1024);
				if (m_Line[strlen(m_Line)- 1] == '\r')
					m_Line[strlen(m_Line) - 1] = 0;
				
				switch(code) {
				case 0 : value = m_Line; noread = true; break;
				case 1 : text = m_Line; break;
				case 8 : layer = m_Line; break;
				case 10 : std::ignore = sscanf(m_Line,"%lf", &x); break;
				case 20 : std::ignore = sscanf(m_Line,"%lf", &y); break;
				case 30 : std::ignore = sscanf(m_Line,"%lf", &z); flag3d = true; break;
				case 40 : std::ignore = sscanf(m_Line,"%u", &importance); break;
				default : value = m_Line;
				}
			}
			
			if (layer != "")
				C = base->AddClass(m_strName.c_str(), layer.c_str());
			else
				C = NoLayer;
				
			if (textflag) {
				if (flag3d) {
					text3D = new XDxfText3D(this);
					text3D->SetFrame(x, y, z);
					text3D->Name(text.c_str());
					text3D->Importance((uint16_t)importance);
					m_Frame += text3D->Frame();
					m_Data.push_back(text3D);
					text3D->Class(C);
					C->Vector(text3D);
				} else {
					text2D = new XDxfText2D(this);
					text2D->SetFrame(x, y);
					text2D->Name(text.c_str());
					text2D->Importance((uint16_t)importance);
					m_Frame += text2D->Frame();
					m_Data.push_back(text2D);
					text2D->Class(C);
					C->Vector(text2D);
				}
			} else {
				if (flag3d) {
					point3D = new XDxfPoint3D(this);
					point3D->SetFrame(x, y, z);
					m_Frame += point3D->Frame();
					m_Data.push_back(point3D);
					point3D->Class(C);
					C->Vector(point3D);
				} else {
					point2D = new XDxfPoint2D(this);
					point2D->SetFrame(x, y);
					m_Frame += point2D->Frame();
					m_Data.push_back(point2D);
					point2D->Class(C);
					C->Vector(point2D);
				}
			}
		}

		// Gestion des lignes type POLYLINE
		if (value == "polyline") {
			while( (!m_In.eof()) && (m_In.good()) ) {
				m_In.getline(m_Line, 1024);
				std::ignore = sscanf(m_Line,"%u", &code);
				m_In.getline(m_Line, 1024);
				if (m_Line[strlen(m_Line)- 1] == '\r')
					m_Line[strlen(m_Line) - 1] = 0;
				switch(code) {
				case 0 : value = m_Line; 
								if (value == "VERTEX")
									vertex = true;
								break;
				case 8 : layer = m_Line; break;
				case 10 : std::ignore = sscanf(m_Line,"%lf", &x); break;
				case 20 : std::ignore = sscanf(m_Line,"%lf", &y);
					if (vertex) {
						XYF += XPt2D(x, y); 
						if (nbpt == 0) SI = XPt2D(x, y); else SF = XPt2D(x,y);
						nbpt++;
						}
					break;
				case 30 : std::ignore = sscanf(m_Line,"%lf", &z);
					if (vertex) {
						if (m_dZmin == XGEO_NO_DATA) m_dZmin = z; else m_dZmin = XMin(m_dZmin, z);
						if (m_dZmax == XGEO_NO_DATA) m_dZmax = z; else m_dZmax = XMax(m_dZmax, z);
						flag3d = true;
					}
					break;
				case 70 : std::ignore = sscanf(m_Line,"%u", &flagbits);
						if ((flagbits % 2) != 0)
							closed = true;
					break;
				default : value = m_Line;
				}
				if (code == 0)
					if (value == "SEQEND") {
						valid_line = true;
						break;
					}
			} // end while
		} // end if

		// Gestion des lignes type LWPOLYLINE
		if (value == "lwpolyline") {
			while( (!m_In.eof()) && (m_In.good()) ) {
				m_In.getline(m_Line, 1024);
				std::ignore = sscanf(m_Line,"%u", &code);
				m_In.getline(m_Line, 1024);
				if (m_Line[strlen(m_Line)- 1] == '\r')
					m_Line[strlen(m_Line) - 1] = 0;
				switch(code) {
					case 0 : value = m_Line; 
									break;
					case 8 : layer = m_Line; break;
					case 10 : std::ignore = sscanf(m_Line,"%lf", &x); break;
					case 20 : std::ignore = sscanf(m_Line,"%lf", &y);
										XYF += XPt2D(x, y); 
										if (nbpt == 0) SI = XPt2D(x, y); else SF = XPt2D(x,y);
										nbpt++;
										break;
					case 30 : std::ignore = sscanf(m_Line,"%lf", &z);
							if (m_dZmin == XGEO_NO_DATA) m_dZmin = z; else m_dZmin = XMin(m_dZmin, z);
							if (m_dZmax == XGEO_NO_DATA) m_dZmax = z; else m_dZmax = XMax(m_dZmax, z);
							flag3d = true;
							nbz++;
						break;
					case 70 : std::ignore = sscanf(m_Line,"%u", &flagbits);
							if ((flagbits % 2) != 0)
								closed = true;
						break;
					case 90 : std::ignore = sscanf(m_Line,"%u", &nbpt_spec);
						break;
					default : value = m_Line;
				}	// end switch
				if (nbpt == nbpt_spec) {
					if (!flag3d)
						valid_line = true;
					if ((flag3d)&&(nbz == nbpt))
						valid_line = true;
					if (valid_line)
						break;
				}
			} // end while
		} // end if

		// Gestion des lignes type LINE
		if (value == "line") {
			while( (!m_In.eof()) && (m_In.good()) ) {
				closed = false;
				m_In.getline(m_Line, 1024);
				std::ignore = sscanf(m_Line,"%u", &code);
				m_In.getline(m_Line, 1024);
				if (m_Line[strlen(m_Line)- 1] == '\r')
					m_Line[strlen(m_Line) - 1] = 0;
				switch(code) {
					case 0 : value = m_Line; 
									break;
					case 8 : layer = m_Line; break;
					case 10 : std::ignore = sscanf(m_Line,"%lf", &x); break;
					case 11 : std::ignore = sscanf(m_Line,"%lf", &x); break;
					case 20 : std::ignore = sscanf(m_Line,"%lf", &y);
										XYF += XPt2D(x, y); 
										SI = XPt2D(x, y);
										nbpt++;
										break;
					case 21 : std::ignore = sscanf(m_Line,"%lf", &y);
										XYF += XPt2D(x, y); 
										SF = XPt2D(x,y);
										nbpt++;
										break;
					case 30 : std::ignore = sscanf(m_Line,"%lf", &z);
							if (m_dZmin == XGEO_NO_DATA) m_dZmin = z; else m_dZmin = XMin(m_dZmin, z);
							if (m_dZmax == XGEO_NO_DATA) m_dZmax = z; else m_dZmax = XMax(m_dZmax, z);
							flag3d = true;
							nbz++;
						break;
					case 31 : std::ignore = sscanf(m_Line,"%lf", &z);
							if (m_dZmin == XGEO_NO_DATA) m_dZmin = z; else m_dZmin = XMin(m_dZmin, z);
							if (m_dZmax == XGEO_NO_DATA) m_dZmax = z; else m_dZmax = XMax(m_dZmax, z);
							flag3d = true;
							nbz++;
						break;
					default : value = m_Line;
				}	// end switch
				if (nbpt == 2) {
					if (!flag3d)
						valid_line = true;
					if ((flag3d)&&(nbz == 2))
						valid_line = true;
					if (valid_line)
						break;
				}
			} // end while
		} // end if

		if (!valid_line)
			continue;

		if (layer != "")
			C = base->AddClass(m_strName.c_str(), layer.c_str());
		else
			C = NoLayer;

		if (closed)
			if (SI != SF)
				nbpt++;

		// Polygone 3D
		if ((closed) && (flag3d)) {
			poly3D = new XDxfPoly3D(this, pos);
			poly3D->SetFrame(&XYF, nbpt);
			m_Frame += poly3D->Frame();
			m_Data.push_back(poly3D);
			poly3D->Class(C);
			C->Vector(poly3D);
		}

		// Polygone 2D
		if ((closed) && (!flag3d)) {
			poly2D = new XDxfPoly2D(this, pos);
			poly2D->SetFrame(&XYF, nbpt);
			m_Frame += poly2D->Frame();
			m_Data.push_back(poly2D);
			poly2D->Class(C);
			C->Vector(poly2D);
		}

		// Ligne 3D
		if ((!closed) && (flag3d)) {
			line3D = new XDxfLine3D(this, pos);
			line3D->SetFrame(&XYF, nbpt);
			m_Frame += line3D->Frame();
			m_Data.push_back(line3D);
			line3D->Class(C);
			C->Vector(line3D);
		}

		// Ligne 2D
		if ((!closed) && (!flag3d)) {
			line2D = new XDxfLine2D(this, pos);
			line2D->SetFrame(&XYF, nbpt);
			m_Frame += line2D->Frame();
			m_Data.push_back(line2D);
			line2D->Class(C);
			C->Vector(line2D);
		}

	}


	m_In.clear();
	m_In.seekg(0);
	return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une geometrie lineaire
//-----------------------------------------------------------------------------
bool XDxf::LoadGeom(uint32_t pos, XPt* P, uint32_t nbpt, double* Z, double* ZRange)
{
	if (!m_In.good())
		return false;
	m_In.seekg(pos);

	uint32_t code = 0;
	uint32_t cmpt = 0;
	bool vertex = false;

	double x = 0., y = 0.;
	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In.getline(m_Line, 1024);
		std::ignore = sscanf(m_Line,"%u", &code);
		m_In.getline(m_Line, 1024);
		switch(code) {
		case 0 : 
			if (strncmp(m_Line, "VERTEX", 6) == 0)
				continue;
			// Emprise Z
			if ((cmpt >= (nbpt - 1)) && (Z != NULL)) {
				if (ZRange != NULL) {
					ZRange[0] = ZRange[1] = Z[0];
					for (uint32_t i = 0; i < cmpt; i++) {
						ZRange[0] = XMin(ZRange[0], Z[i]);
						ZRange[1] = XMax(ZRange[1], Z[i]);
					}
				}
			}
				// Fin de la ligne
			if (cmpt == nbpt)
				return true;
			if (cmpt == (nbpt - 1)) {
				P[cmpt].X = P[0].X;
				P[cmpt].Y = P[0].Y;
				if (Z != NULL)
					Z[cmpt] = Z[0];
				return true;
			}

//			if (strncmp(m_Line, "SEQEND", 6) == 0){
				if (cmpt == (nbpt - 2)) {
					cmpt++;
					P[cmpt].X = P[0].X;
					P[cmpt].Y = P[0].Y;
					if (Z != NULL)
						Z[cmpt] = Z[0];
				}
				if (cmpt == (nbpt - 1)) {
					if ( (Z != NULL) && (ZRange != NULL)) {
						ZRange[0] = ZRange[1] = Z[nbpt-1];
						for (uint32_t i = 0; i < nbpt; i++) {
							ZRange[0] = XMin(ZRange[0], Z[i]);
							ZRange[1] = XMax(ZRange[1], Z[i]);
						}
					}
					return true;
				} else
					return false;
//			}
				break;
		case 10 : std::ignore = sscanf(m_Line, "%lf", &x);	break;
		case 11 : std::ignore = sscanf(m_Line, "%lf", &x);	break;
		case 20 : std::ignore = sscanf(m_Line, "%lf", &y);
			if ((x != 0.)&&(y != 0.)) {
					vertex = true;
					P[cmpt].X = x;
					P[cmpt].Y = y;
					cmpt++;
			}
			break;
		case 21 : std::ignore = sscanf(m_Line, "%lf", &y);
			if ((x != 0.)&&(y != 0.)) {
					vertex = true;
					P[cmpt].X = x;
					P[cmpt].Y = y;
					cmpt++;
			}
			break;
		case 30 : if (Z != NULL) if (vertex) std::ignore = sscanf(m_Line, "%lf", &Z[cmpt-1]); break;
		case 31 : if (Z != NULL) if (vertex) std::ignore = sscanf(m_Line, "%lf", &Z[cmpt-1]); break;
		default : ;
		}
	}

/*
	while( (!m_In.eof()) && (m_In.good()) ) {
		m_In.getline(m_Line, 1024);
		sscanf(m_Line,"%u", &code);
		m_In.getline(m_Line, 1024);
//		if (m_Line[strlen(m_Line)- 1] == '\r')
//			m_Line[strlen(m_Line) - 1] = '\0';
		switch(code) {
		case 0 : 
			if (strncmp(m_Line, "SEQEND", 6) == 0){
				if (cmpt == (nbpt - 2)) {
					cmpt++;
					P[cmpt].X = P[0].X;
					P[cmpt].Y = P[0].Y;
					if (Z != NULL)
						Z[cmpt] = Z[0];
				}
				if (cmpt == (nbpt - 1)) {
					if ( (Z != NULL) && (ZRange != NULL)) {
						ZRange[0] = ZRange[1] = Z[nbpt-1];
						for (uint32_t i = 0; i < nbpt; i++) {
							ZRange[0] = XMin(ZRange[0], Z[i]);
							ZRange[1] = XMax(ZRange[1], Z[i]);
						}
					}
					return true;
				} else
					return false;
			}
			if (strncmp(m_Line, "VERTEX", 6) == 0) {
				if (vertex)
					cmpt++;
				else
					vertex = true;
			}
		case 10 : if (vertex) sscanf(m_Line, "%lf", &P[cmpt].X);	break;
		case 20 : if (vertex) sscanf(m_Line, "%lf", &P[cmpt].Y);	break;
		case 30 : if (Z != NULL) if (vertex) sscanf(m_Line, "%lf", &Z[cmpt]); break;
		default : ;
		}
	}
*/
	return false;
}

//-----------------------------------------------------------------------------
// Conversion geodesique
//-----------------------------------------------------------------------------
bool XDxf::Convert(const char* file_in, const char* file_out, XGeodConverter* L, XError* error)
{
	XPath P;
	m_strFilename = file_in;
	m_strName = P.Name(file_in);
	m_In.open(file_in, std::ios_base::in);
	if (!m_In.good())
		return XErrorError(error, "XDxf::Convert", XError::eIOOpen);


	// Ouverture du fichier de sortie
	std::ofstream dxf;
	dxf.open(file_out);
	if (!dxf.good())
		return XErrorError(error, "XDxf::Convert", XError::eIOOpen);
	dxf.setf(std::ios::fixed);
	dxf.precision(3);
	if (L->EndProjection() == XGeoProjection::RGF93)
		dxf.precision(9);

	char token1[1024], token2[1024];
	std::string value, type;
	uint32_t  code = 0, xcode = 0, ycode = 0, code_max = 0, line = 0;
	double xi, yi, z, xf, yf;
	bool need_code = true, useful_data = false, section = false;

	while( (!m_In.eof()) && (m_In.good()) ) {
		if (need_code) {
			m_In.getline(token1, 1024); line++;
			std::ignore = sscanf(token1,"%u", &code);
			m_In.getline(token2, 1024); line++;
		} else
			need_code = true;

		if (code == 0) {
			value = token2;
			std::transform(value.begin(), value.end(), value.begin(), tolower);
			type = value;
			code_max = 13;
			// Cas particuliers suivant les geometries
			if (type == "ellipse")
				code_max = 10;
			// Debut d'une section
			if (value == "section") {
				section = true;
				useful_data = false;
				dxf << token1 << std::endl;
				dxf << token2 << std::endl;
				continue;
			}
		}

		if (code == 2) {
			value = token2;
			std::transform(value.begin(), value.end(), value.begin(), tolower);
			if ((value == "header")||(value == "tables")||(value == "entities"))
				useful_data = true;
			dxf << token1 << std::endl;
			dxf << token2 << std::endl;
			continue;
		}

		if (!useful_data) {
			dxf << token1 << std::endl;
			dxf << token2 << std::endl;
			continue;
		}

		if ((code >= 10)&&(code <= code_max)) {
			xcode = code;
			std::ignore = sscanf(token2,"%lf", &xi);

			if ((xi < 360.)&&(xi > -360.)&&(xi != 0.))
				dxf.precision(6);

			m_In.getline(token1, 1024); line++;
			std::ignore = sscanf(token1,"%u", &code);
			m_In.getline(token2, 1024); line++;

			if ((code != 20)&&(code != 21)&&(code != 22)&&(code != 23)) {
				dxf << " " << xcode << std::endl;
				dxf << xi << std::endl;
				dxf << token1 << std::endl;
				dxf << token2 << std::endl;
			} else {
				ycode = code;
				std::ignore = sscanf(token2,"%lf", &yi);

				m_In.getline(token1, 1024); line++;
				std::ignore = sscanf(token1,"%u", &code);
				m_In.getline(token2, 1024); line++;
				if ((code == 30)||(code == 31)||(code == 32)||(code == 33))
					std::ignore = sscanf(token2,"%lf", &z);
				else {
					z = 0.;
					need_code = false;
				}

				//if ((code == 10)||(code == 11)||(code == 12)||(code == 13))
				//	need_code = false;

        if ((xi != 0.)&&(yi != 0.)&&(xi != 1.)&&(yi != 1.)&&(fabs(xi) < 1e10)&&(fabs(yi) < 1e10))
          L->ConvertDeg(xi, yi, xf, yf, z);
				else
				{xf = xi ; yf = yi;}

				dxf << " " << xcode << std::endl;
				dxf << xf << std::endl;
				dxf << " " << ycode << std::endl;
				dxf << yf << std::endl;
				if (need_code) {
					dxf << token1 << std::endl;
					dxf << token2 << std::endl;
				}
			}
		} else {
			dxf << token1 << std::endl;
			dxf << token2 << std::endl;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Fixe l'emprise d'une ligne 2D
//-----------------------------------------------------------------------------
void XDxfLine2D::SetFrame(XFrame* XYF, uint32_t nbpt)
{
	m_Frame = *XYF;
	m_nNumPoints = nbpt;
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie d'une ligne DXF 2D
//-----------------------------------------------------------------------------
bool XDxfLine2D::LoadGeom()
{ 
	if (m_File == NULL) 
		return false;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	return m_File->LoadGeom(m_Pos, m_Pt, m_nNumPoints, NULL, NULL);
}

//-----------------------------------------------------------------------------
// Fixe l'emprise d'une ligne 3D
//-----------------------------------------------------------------------------
void XDxfLine3D::SetFrame(XFrame* XYF, uint32_t nbpt)
{
	m_Frame = *XYF;
	m_nNumPoints = nbpt;
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie d'une ligne DXF 2D
//-----------------------------------------------------------------------------
bool XDxfLine3D::LoadGeom()
{ 
	if (m_File == NULL) 
		return false;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete m_Pt; m_Pt = NULL;
		return false;
	}
	m_ZRange = new double[2];
	return m_File->LoadGeom(m_Pos, m_Pt, m_nNumPoints, m_Z, m_ZRange);
}

//-----------------------------------------------------------------------------
// Fixe l'emprise d'un polygone 2D
//-----------------------------------------------------------------------------
void XDxfPoly2D::SetFrame(XFrame* XYF, uint32_t nbpt)
{
	m_Frame = *XYF;
	m_nNumPoints = nbpt;
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie d'un polygone DXF 2D
//-----------------------------------------------------------------------------
bool XDxfPoly2D::LoadGeom()
{ 
	if (m_File == NULL) 
		return false;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	return m_File->LoadGeom(m_Pos, m_Pt, m_nNumPoints, NULL, NULL);
}


//-----------------------------------------------------------------------------
// Fixe l'emprise d'un polygone 3D
//-----------------------------------------------------------------------------
void XDxfPoly3D::SetFrame(XFrame* XYF, uint32_t nbpt)
{
	m_Frame = *XYF;
	m_nNumPoints = nbpt;
}

//-----------------------------------------------------------------------------
// Chargement de la geometrie d'un polygone DXF 3D
//-----------------------------------------------------------------------------
bool XDxfPoly3D::LoadGeom()
{ 
	if (m_File == NULL) 
		return false;
	m_Pt = new XPt[m_nNumPoints];
	if (m_Pt == NULL)
		return false;
	m_Z = new double[m_nNumPoints];
	if (m_Z == NULL) {
		delete m_Pt; m_Pt = NULL;
		return false;
	}
	m_ZRange = new double[2];
	return m_File->LoadGeom(m_Pos, m_Pt, m_nNumPoints, m_Z, m_ZRange);
}
