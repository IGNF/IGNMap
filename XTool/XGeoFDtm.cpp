//-----------------------------------------------------------------------------
//								XGeoFDtm.cpp
//								============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 03/03/2009
//-----------------------------------------------------------------------------

#include <cstring>
#include <algorithm>

#include "XGeoFDtm.h"
#include "XPath.h"
#include "../XToolGeod/XGeodConverter.h"
#include "../XToolImage/XTiffWriter.h"
#include "XPath.h"
#include "XInterpol.h"


//-----------------------------------------------------------------------------
// Ouverture d'un MNT
//-----------------------------------------------------------------------------
bool XGeoFDtm::OpenDtm(const char* filename, const char* tmpname)
{
  XPath P;
  std::string ext = P.Ext(filename);
  std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
  bool flag = false;
  m_bTmpFile = false;
  if (ext == ".asc")
    flag = ImportAsc(filename, tmpname);
  if (ext == ".xyz")
    flag = ImportXyz(filename, tmpname);
  if (ext == ".hdr")
    flag = ImportHdr(filename, tmpname);
  if (flag){
    m_strImageName = tmpname;
    m_bTmpFile = true;
  }

  if (ext == ".tif") {
    flag = ImportTif(filename, tmpname);
    if (!flag)
      flag = ImportTiff16(filename, tmpname);
  }

  if (ext == ".xml") {
    XParserXML parser;
    if (parser.Parse(filename))
      flag = XmlRead(&parser);
  }

  return flag;
}

//-----------------------------------------------------------------------------
// Fermeture du MNT
//-----------------------------------------------------------------------------
void XGeoFDtm::Close()
{
	m_strName = m_strFilename = m_strPath = "";
	m_dGSD = 0.;
	m_dZmin = m_dZmax = XGEO_NO_DATA;
	m_nW = m_nH = 0;
  m_nNbNoZ = 0;
	m_dNoData = -9999.;
	m_bValid = false;
  m_In.Close();
  m_ActiveStream = NULL;
}

//-----------------------------------------------------------------------------
// Indique si la lecture est possible
//-----------------------------------------------------------------------------
bool XGeoFDtm::StreamReady()
{
  m_ActiveStream = m_In.IStream();
  if (m_ActiveStream == NULL)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'une ligne de noeuds
//-----------------------------------------------------------------------------
bool XGeoFDtm::ReadLine(float* line, uint32_t numLine)
{
  m_ActiveStream->seekg((numLine * m_nW)* sizeof(float)+m_nOffset, std::ios::beg);
  m_ActiveStream->read((char*)line, m_nW * sizeof(float));
  return true;
}

//-----------------------------------------------------------------------------
// Lecture d'un noeud
//-----------------------------------------------------------------------------
bool XGeoFDtm::ReadNode(float* node, uint32_t x, uint32_t y)
{
  m_ActiveStream->seekg((y * m_nW + x)* sizeof(float)+m_nOffset, std::ios::beg);
  m_ActiveStream->read((char*)node, sizeof(float));
  return true;
}

//-----------------------------------------------------------------------------
// Lecture de tout le MNT
//-----------------------------------------------------------------------------
bool XGeoFDtm::ReadAll(float* area)
{
  m_ActiveStream->seekg(m_nOffset, std::ios::beg);
  m_ActiveStream->read((char*)area, m_nW * m_nH * sizeof(float));
  return true;
}

//-----------------------------------------------------------------------------
// Renvoi un noeud de la grille
//-----------------------------------------------------------------------------
double XGeoFDtm::Z(uint32_t x, uint32_t y)
{
	if (!m_bValid)
		return 0;
	if ((x >= m_nW)||(y >= m_nH))
		return 0;
  if (!StreamReady())
    return m_dNoData;
	float val;
  ReadNode(&val, x, y);

	return (double)val;
}

//-----------------------------------------------------------------------------
// Points du cadre de l'image
//-----------------------------------------------------------------------------
XPt2D XGeoFDtm::Pt(uint32_t i)
{
  switch(i) {
    case 0 : return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
    case 1 : return XPt2D(m_Frame.Xmax, m_Frame.Ymax);
    case 2 : return XPt2D(m_Frame.Xmax, m_Frame.Ymin);
    case 3 : return XPt2D(m_Frame.Xmin, m_Frame.Ymin);
    case 4 : return XPt2D(m_Frame.Xmin, m_Frame.Ymax);
  }
  return XPt2D(XGEO_NO_DATA, XGEO_NO_DATA);
}

//-----------------------------------------------------------------------------
// Calcul d'un Z sur le MNT
//-----------------------------------------------------------------------------
double XGeoFDtm::Z(const XPt2D& P)
{
	if (!m_bValid)
    return XGEO_NO_DATA;
  if (P.X < (m_Frame.Xmin - m_dGSD * 0.5)) return XGEO_NO_DATA;
  if (P.X > (m_Frame.Xmax + m_dGSD * 0.5)) return XGEO_NO_DATA;
  if (P.Y < (m_Frame.Ymin - m_dGSD * 0.5)) return XGEO_NO_DATA;
  if (P.Y > (m_Frame.Ymax + m_dGSD * 0.5)) return XGEO_NO_DATA;

	double dx = (P.X - m_Frame.Xmin) / m_dGSD;
	double dy = (m_Frame.Ymax - P.Y) / m_dGSD;
	int nbx = (int)floor(dx);
	int nby = (int)floor(dy);

	if ((nbx < -1)||(nby < -1))
    return XGEO_NO_DATA; // m_dNoData;
	if ((nbx >= (int)m_nW)||(nby >= (int)m_nH))
    return XGEO_NO_DATA; // m_dNoData;

  if (!StreamReady())
    return m_dNoData;
  float val[4], w[4];
	float *ptr = val, *wptr = w;
	double distance;
	XPt2D M;
	for (int i = nby; i < (nby+2); i++) {
		for (int j = nbx; j < (nbx+2); j++) {
			if ( (j >= 0) && (j < (int)m_nW) && (i >= 0) && (i < (int)m_nH)) {
        ReadNode(ptr, j, i);
				if (*ptr <= m_dNoData)
					*wptr = 0.;
				else {
					M.X = m_Frame.Xmin + j * m_dGSD;
					M.Y = m_Frame.Ymax - i * m_dGSD;
					distance = dist2(P, M);
					if (distance < 1.e-6)
						return *ptr;
					*wptr = 1. / distance;
				}
			} else {
				*ptr = (float)m_dNoData;
				*wptr = 0.;
			}

			ptr++;
			wptr++;
		}	// endfor j
	} // endfor i

	double sumw = w[0] + w[1] + w[2] + w[3];
	if (sumw <= 0.)
    return XGEO_NO_DATA;

	return (w[0]*val[0] + w[1]*val[1] + w[2]*val[2] + w[3]*val[3]) / sumw;
}

//-----------------------------------------------------------------------------
// Donne le ZMax, ZMin, ZMean sur un cadre et renvoie le nombre de noeuds
//-----------------------------------------------------------------------------
uint32_t XGeoFDtm::ZFrame(const XFrame &F, double* zmax, double* zmin, double* zmean)
{
  *zmax = *zmin = *zmean = XGEO_NO_DATA;
  if (!m_Frame.Intersect(F))
    return 0;
  uint32_t startX = 0, endX = m_nW - 1, startY = 0, endY = m_nH - 1;
  if (F.Xmin > m_Frame.Xmin)
    startX = (uint32_t)ceil((F.Xmin - m_Frame.Xmin)/m_dGSD);
  if (F.Xmax < m_Frame.Xmax)
    endX = (uint32_t)floor((F.Xmax - m_Frame.Xmin)/m_dGSD);
  if (F.Ymax < m_Frame.Ymax)
    startY = (uint32_t)ceil((m_Frame.Ymax - F.Ymax)/m_dGSD);
  if (F.Ymin > m_Frame.Ymin)
    endY = (uint32_t)floor(( m_Frame.Ymax - F.Ymin)/m_dGSD);

  if (!StreamReady())
    return 0;
  float z;
  double newZ;
  uint32_t nbz = 0;
  *zmean = 0.;
  *zmin = 1e31;
  *zmax = -1e31;
  for (uint32_t i = startY; i <= endY; i++) {
    for (uint32_t j = startX; j <= endX; j++) {
      ReadNode(&z, j, i);
      if (z > m_dNoData) {
        newZ = z;
        nbz++;
        *zmean += newZ;
        *zmin = XMin(*zmin, newZ);
        *zmax = XMax(*zmax, newZ);
      }
    }
  }
  if (nbz > 0)
    *zmean = *zmean / nbz;
  else
    *zmax = *zmin = *zmean = XGEO_NO_DATA;

  return nbz;
}

//-----------------------------------------------------------------------------
// Conversion Terrain -> Image
//-----------------------------------------------------------------------------
bool XGeoFDtm::Ground2Pix(double x, double y, uint32_t& u, uint32_t& v)
{
  if (!m_Frame.IsIn(XPt2D(x, y)))
    return false;
  u = XRint((x - m_Frame.Xmin) / m_dGSD);
  v = XRint((m_Frame.Ymax - y) / m_dGSD);
  return true;
}

//-----------------------------------------------------------------------------
// Lecture des meta-donnees en XML
//-----------------------------------------------------------------------------
bool XGeoFDtm::XmlRead(XParserXML* parser, uint32_t num, XError* error)
{
	m_bValid = false;
	m_strName = parser->ReadNode("/xgeofdtm/name");
	m_dZmin = parser->ReadNodeAsDouble("/xgeofdtm/zmin");
	m_dZmax = parser->ReadNodeAsDouble("/xgeofdtm/zmax");
	m_dNoData = parser->ReadNodeAsInt("/xgeofdtm/nodata");
  m_nNbNoZ = parser->ReadNodeAsUInt32("/xgeofdtm/nb_noz");
	m_dGSD = parser->ReadNodeAsDouble("/xgeofdtm/resolution");
  m_nOffset = parser->ReadNodeAsUInt32("/xgeofdtm/offset");
  m_strFilename = parser->ReadNode("/xgeofdtm/datafile");
	if (m_dGSD < 0.01)
		return false;

	XParserXML frame = parser->FindSubParser("/xgeofdtm/frame");
	if (frame.IsEmpty())
		return false;
	if(!m_Frame.XmlRead(&frame))
		return false;

	m_nW = XRint(m_Frame.Width() / m_dGSD) + 1;
	m_nH = XRint(m_Frame.Height() / m_dGSD) + 1;

	XPath P;
	std::string filename = parser->Filename();
	m_strPath = P.Path(filename.c_str());
  m_strFilename = m_strPath + "/" + m_strFilename;
  m_strImageName = m_strFilename;

  m_In.Close();
  m_bValid = m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
	return m_bValid;
}

//-----------------------------------------------------------------------------
// Ecriture des meta-donnees en XML
//-----------------------------------------------------------------------------
bool XGeoFDtm::XmlWrite(std::ostream* out)
{
	out->setf(std::ios::fixed);
	out->precision(2);

	*out << "<xgeofdtm>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	*out << "<zmin> " << m_dZmin << " </zmin>" << std::endl;
	*out << "<zmax> " << m_dZmax << " </zmax>" << std::endl;
	*out << "<nodata> " << m_dNoData << " </nodata>" << std::endl;
  *out << "<nb_noz> " << m_nNbNoZ << " </nb_noz>" << std::endl;
	*out << "<resolution> " << m_dGSD << " </resolution>" << std::endl;
  *out << "<offset> " << m_nOffset << " </offset>" << std::endl;
  *out << "<datafile> " << m_strImageName << " </datafile>" << std::endl;

	m_Frame.XmlWrite(out);

	*out << "</xgeofdtm>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture d'une entete ASC
//-----------------------------------------------------------------------------
bool XGeoFDtm::WriteAscHeader(std::ostream* out)
{
	out->setf(std::ios::fixed);
	out->precision(2);

	*out << "ncols         " << m_nW << std::endl;
	*out << "nrows         " << m_nH << std::endl;
	*out << "xllcorner     " << m_Frame.Xmin - m_dGSD * 0.5 << std::endl;
	*out << "yllcorner     " << m_Frame.Ymin - m_dGSD * 0.5 << std::endl;
	*out << "cellsize      " << m_dGSD << std::endl;
	*out << "NODATA_value  " << m_dNoData << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Import d'un fichier ASC
//-----------------------------------------------------------------------------
bool XGeoFDtm::ImportAsc(std::string file_asc, std::string file_bin)
{
  Close();
  // Ouverture du fichier ASC
  std::ifstream in;
  in.open(file_asc.c_str(), std::ios_base::in| std::ios_base::binary);
  if (!in.good())
    return false;

  // Recuperation des caracteristiques du MNT
  char buf[1024], token[1024];
  std::string keyword;
  uint32_t w, h;
  double x, y, step, no_data;
  bool flag_center = false;

  in.getline(buf, 1024);  // Nombre de colonnes
  sscanf(buf, "%s %u", token, &w);
  keyword = token;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
  if (keyword != "ncols")
    return false;

  in.getline(buf, 1024);  // Nombre de lignes
  sscanf(buf, "%s %u", token, &h);
  keyword = token;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
  if (keyword != "nrows")
    return false;

  in.getline(buf, 1024);  // Origine X
  sscanf(buf, "%s %lf", token, &x);
  keyword = token;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
  if (keyword != "xllcorner") {
    if (keyword != "xllcenter")
      return false;
    flag_center = true;
  }

  in.getline(buf, 1024);  // Origine Y
  sscanf(buf, "%s %lf", token, &y);
  keyword = token;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
  if (keyword != "yllcorner") {
    if ( (!flag_center) || (keyword != "yllcenter") )
      return false;
  }

  in.getline(buf, 1024);  // GSD
  sscanf(buf, "%s %lf", token, &step);
  keyword = token;
  std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
  if (keyword != "cellsize")
    return false;

  char c = in.peek();
  if ((c == 'n')||(c == 'N')) {
    in.getline(buf, 1024);  // NODATA
    sscanf(buf, "%s %lf", token, &no_data);
    keyword = token;
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), tolower);
    if (keyword != "nodata_value")
      return false;
  } else
    no_data = -9999;

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  if (!tiff.Write(file_bin.c_str(),w, h, 1, 32))
    return false;
  std::ofstream out;
  out.open(file_bin.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  m_nOffset = out.tellp();

  // Conversion des donnees
  float *line, *ptr;
  line = new float[w];
  if (line == NULL)
    return false;
  float zmax = -9e9, zmin = 9e9;
  m_nNbNoZ = 0;
  for (uint32_t i = 0; i < h; i++) {
    ptr = line;
    for (uint32_t j = 0; j < w; j++) {
      if (j < (w-1)) {
        in.getline(buf, 1024, ' ');
        while((strlen(buf)<1)&&(in.good()))
          in.getline(buf, 1024, ' ');
      } else
        in.getline(buf, 1024);
      sscanf(buf,"%f", ptr);
      if (*ptr > no_data) {
        zmax = XMax(zmax, *ptr);
        zmin = XMin(zmin, *ptr);
      } else
        m_nNbNoZ++;
      ptr++;
    }
    out.write((char*)line, w * sizeof(float));
  }
  delete[] line;
  out.close();
  in.close();

  // Affectation des parametres
  m_nH = h;
  m_nW = w;
  m_dGSD = step;
  m_dNoData = no_data;
  m_strFilename = file_bin;
  XPath P;
  m_strPath = P.Path(file_asc.c_str());
  m_strName = P.Name(file_asc.c_str());
  m_Frame.Xmin = x + step * 0.5;
  m_Frame.Ymin = y + step * 0.5;
  if (flag_center) {
    m_Frame.Xmin = x;
    m_Frame.Ymin = y;
  }
  m_Frame.Xmax = m_Frame.Xmin + step * (m_nW - 1);
  m_Frame.Ymax = m_Frame.Ymin + step * (m_nH - 1);
  m_dZmax = zmax;
  m_dZmin = zmin;
  if (zmin == 9e9) {  // MNT vide ?
    m_dZmax = m_dNoData;
    m_dZmin = m_dNoData;
  }

  m_bValid = m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
  return m_bValid;
}
//-----------------------------------------------------------------------------
// Import d'un fichier DIS
//-----------------------------------------------------------------------------
bool XGeoFDtm::ImportDis(std::string file_asc, std::string file_bin)
{
	Close();
	// Ouverture des fichiers
	std::ifstream in;
	in.open(file_asc.c_str());
	if (!in.good())
		return false;
	std::ofstream out;
	out.open(file_bin.c_str(), std::ios_base::out| std::ios_base::binary);
	if (!out.good())
		return false;

	// Recuperation des caracteristiques du MNT
	uint32_t w, h;
	int nb_car;
	double x, y, dx, dy, zmin, zmax, step;
	std::string token;
	char buf[512], data[80];

	in.get(buf, 416);
	token = buf;

	strncpy(data, &buf[160], 15); data[15] = 0;
	sscanf(data, "%lf", &x);
	strncpy(data, &buf[175], 15); data[15] = 0;
	sscanf(data, "%lf", &y);

	strncpy(data, &buf[280], 10); data[10] = 0;
	sscanf(data, "%lf", &dx);
	strncpy(data, &buf[290], 10); data[10] = 0;
	sscanf(data, "%lf", &dy);
	if (dx != dy)
		return false;
	step = dx;

	strncpy(data, &buf[320], 10); data[10] = 0;
	sscanf(data, "%u", &w);
	strncpy(data, &buf[330], 10); data[10] = 0;
	sscanf(data, "%u", &h);

	strncpy(data, &buf[380], 10); data[10] = 0;
	sscanf(data, "%lf", &zmin);
	strncpy(data, &buf[390], 10); data[10] = 0;
	sscanf(data, "%lf", &zmax);

	strncpy(data, &buf[410], 5); data[5] = 0;
	sscanf(data, "%d", &nb_car);

	while(in.peek() != '\n') {
		in.ignore();
	}
	in.ignore();
		
	// Conversion des donnees
	float *line, *ptr;
	line = new float[w];
	if (line == NULL)
		return false;
	data[nb_car] = 0;
	nb_car++;
	for (uint32_t i = 0; i < h; i++) {
		ptr = line; 
		for (uint32_t j = 0; j < w; j++) {
			in.get(data, nb_car);
			sscanf(data, "%f", ptr);
			ptr++;
		}
		out.write((char*)line, w * sizeof(float));
		while(in.peek() != '\n')
			in.ignore();
		in.ignore();
		in.clear();
	}
	delete[] line;
	out.close();
	in.close();

	// Affectation des parametres
	m_nH = h;
	m_nW = w;
	m_dGSD = step;
	m_strFilename = file_bin;
	XPath P;
	m_strPath = P.Path(m_strFilename.c_str());
	m_strName = P.Name(file_asc.c_str());
	m_Frame.Xmin = x + step * 0.5;
	m_Frame.Ymin = y + step * 0.5;
	m_Frame.Xmax = m_Frame.Xmin + step * (m_nW - 1);
	m_Frame.Ymax = m_Frame.Ymin + step * (m_nH - 1);
	m_dZmin = zmin;
	m_dZmax = zmax;

  m_bValid = 	m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
	return m_bValid;
}

//-----------------------------------------------------------------------------
// Import d'un fichier XYZ
//-----------------------------------------------------------------------------
bool XGeoFDtm::ImportXyz(std::string file_asc, std::string file_bin)
{
	Close();
	// Ouverture des fichiers
	std::ifstream in;
	in.open(file_asc.c_str());
	if (!in.good())
		return false;

	// Recuperation des caracteristiques du MNT
	double x, y, z, xmin, xmax, ymin, ymax, zmin, zmax, lastx, lasty, stepx, stepy;
	in >> x >> y >> z;
	xmin = xmax = lastx = x;
	ymin = ymax = lasty = y;
	zmin = zmax = z;
	stepx = stepy = 1e100;
	while(!in.eof()) {
		in >> x >> y >> z;
		xmin = XMin(x, xmin);
		xmax = XMax(x, xmax);
		ymin = XMin(y, ymin);
		ymax = XMax(y, ymax);
		zmin = XMin(z, zmin);
		zmax = XMax(z, zmax);
		if (x != lastx)
			stepx = XMin(stepx, fabs(x - lastx));
		if (y != lasty)
			stepy = XMin(stepy, fabs(y - lasty));
	}
	if ((stepx < 0.1)||(stepy < 0.1))
		return false;
	if (fabs(stepx - stepy) > 0.1)
		return false;

	m_dZmin = zmin;
	m_dZmax = zmax;
	m_nH = (ymax - ymin) / stepy + 1;
	m_nW = (xmax - xmin) / stepx + 1;

	m_Frame.Xmin = xmin;
	m_Frame.Ymin = ymin;
	m_Frame.Xmax = m_Frame.Xmin + stepx * (m_nW - 1);
	m_Frame.Ymax = m_Frame.Ymin + stepx * (m_nH - 1);

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  if (!tiff.Write(file_bin.c_str(), m_nW, m_nH, 1, 32))
    return false;
  std::ofstream out;
  out.open(file_bin.c_str(), std::ios_base::out| std::ios_base::binary | std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  m_nOffset = out.tellp();

	// Conversion des donnees
	in.clear();
	in.close();
	in.open(file_asc.c_str());

	float *area;
	area = new float[m_nW * m_nH];
	if (area == NULL)
		return false;
	for (uint32_t k = 0; k < m_nW * m_nH; k++)
		area[k] = m_dNoData;
  m_nNbNoZ = m_nW * m_nH;
	uint32_t i, j;
	while(!in.eof()) {
		in >> x >> y >> z;
		i = XRint((x - m_Frame.Xmin) / stepx);
		j = XRint((m_Frame.Ymax - y) / stepx);
		area[j * m_nW + i] = z;
    m_nNbNoZ--;
	}
	out.write((char*)area, m_nW * m_nH * sizeof(float));
	delete[] area;
	out.close();
	in.close();

	// Affectation des parametres
	m_dGSD = stepx;
	m_strFilename = file_bin;
	XPath P;
	m_strPath = P.Path(m_strFilename.c_str());
	m_strName = P.Name(file_asc.c_str());

  m_bValid = 	m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
	return m_bValid;
}

//-----------------------------------------------------------------------------
// Import d'un fichier HDR
//-----------------------------------------------------------------------------
bool XGeoFDtm::ImportHdr(std::string file_hdr, std::string file_bin)
{
  Close();
  // Ouverture des fichiers
  std::ifstream in;
  in.open(file_hdr.c_str());
  if (!in.good())
    return false;

  std::string token, ext;
  char buf[1024];
  double xmin = 0., ymax = 0., stepx = 0., stepy = 0.;
  uint32_t w = 0, h = 0, nbbits = 0, skipbytes = 0;
  bool esri = true;
  while( (!in.eof()) && (in.good())) {
    in >> token;
    if (token == "!+") {
      in.getline(buf, 1024);
      continue;
    }

    if (token == "ULXMAP")
      in >> xmin;
    if (token == "ULYMAP")
      in >> ymax;
    if (token == "XDIM")
      in >> stepx;
    if (token == "YDIM")
      in >> stepy;
    if (token == "NCOLS")
      in >> w;
    if (token == "NROWS")
      in >> h;
    if (token == "LAYOUT")
      in >> ext;
    if (token == "NBITS")
      in >> nbbits;
    if (token == "PROJECTION")
      esri = false;
    if (token == "SKIPBYTES")
      in >> skipbytes;
  }
  in.clear();
  in.close();
  if (stepx <= 0.)
    return false;

  m_nH = h;
  m_nW = w;
  m_nNbNoZ = m_nW * m_nH;
  m_dNoData = -999.;
  m_dZmin = 1e32;
  m_dZmax = -1e32;

  m_dGSD = stepx; // On considere que XDIM == YDIM ...
  if (esri) {
    m_Frame.Xmin = xmin;
    m_Frame.Ymax = ymax;
  } else { // Convention GeoView
    m_Frame.Xmin = xmin + m_dGSD * 0.5;
    m_Frame.Ymax = ymax - m_dGSD * 0.5;
  }
  m_Frame.Xmax = m_Frame.Xmin + m_dGSD * (m_nW - 1);
  m_Frame.Ymin = m_Frame.Ymax - m_dGSD * (m_nH - 1);

  // Nom du fichier des donnees
  std::string file_data;
  file_data = file_hdr.substr(0, file_hdr.rfind('.'));
  file_data += ".";
  file_data += ext;

  std::ifstream data;
  data.open(file_data.c_str(), std::ios_base::in| std::ios_base::binary);
  if (!data.good())
    return false;
  data.seekg(skipbytes);

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  if (!tiff.Write(file_bin.c_str(), m_nW, m_nH, 1, 32))
    return false;
  std::ofstream out;
  out.open(file_bin.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  m_nOffset = out.tellp();

  // Cas des fichiers BIL en 32 bits -> lecture directe
  if (nbbits == 32) {
    float *line;
    line = new float[m_nW];
    if (line == NULL)
      return false;
    for (uint32_t k = 0; k < m_nH; k++) {
      data.read((char*)line, m_nW * sizeof(float));
      for (uint32_t i = 0; i < m_nW; i++) {
        if (line[i] <= m_dNoData)
          continue;
        m_nNbNoZ--;
        m_dZmin = XMin(line[i], (float)m_dZmin);
        m_dZmax = XMax(line[i], (float)m_dZmax);
      }
      out.write((char*)line, m_nW * sizeof(float));
     }
    delete[] line;
    out.close();
    data.close();
  }

  // Cas des fichiers BIL en 16 bits -> Conversion des donnees
  if (nbbits == 16) {
    short int *line_in;
    line_in = new short int[m_nW];
    if (line_in == NULL)
      return false;
    float *line_out;
    line_out = new float[m_nW];
    if (line_out == NULL)
      return false;

    for (uint32_t k = 0; k < m_nH; k++) {
      data.read((char*)line_in, m_nW * sizeof(short int));
      for (uint32_t i = 0; i < m_nW; i++) {
        if (line_in[i] <= m_dNoData) {
          line_out[i] = m_dNoData;
        } else {
          m_nNbNoZ--;
          m_dZmin = XMin((double)line_in[i], m_dZmin);
          m_dZmax = XMax((double)line_in[i], m_dZmax);
          line_out[i] = line_in[i];
        }
      }
      out.write((char*)line_out, m_nW * sizeof(float));
    }

    delete[] line_in;
    delete[] line_out;
    out.close();
    data.close();
  }

  // Affectation des parametres
  m_strFilename = file_bin;
  XPath P;
  m_strPath = P.Path(m_strFilename.c_str());
  m_strName = P.Name(file_hdr.c_str());
  m_bValid = 	m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
  return m_bValid;
}

//-----------------------------------------------------------------------------
// Export binaire natif du MNT
//-----------------------------------------------------------------------------
bool XGeoFDtm::Export(std::string filename)
{
	if (!m_bValid)
		return false;

  XPath P;
  std::string xmlfile = P.PathName(filename.c_str()) + ".xml";
  std::string tifffile = P.PathName(filename.c_str()) + ".tif";

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax  + m_dGSD * 0.5, m_dGSD);
  if (!tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 32, NULL, 3))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  uint32_t offset = out.tellp();

	// Ecriture des donnees
  if (!StreamReady())
    return false;
	float* line = new float[m_nW];
	if (line == NULL)
		return false;
	for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
		out.write((char*)line, m_nW * sizeof(float));
	}
	out.close();
	delete[] line;

	// Ecriture du fichier XML
  std::string name = P.Name(filename.c_str(), false);
	std::string oldname = m_strName;
  std::string oldimage = m_strImageName;
	m_strName = name;
  m_strImageName = name + ".tif";
  uint32_t old_offset = m_nOffset;
  m_nOffset = offset;
	
	std::ofstream xml;
	xml.open(xmlfile.c_str());
  bool flag = false;
  if (xml.good())
    flag = XmlWrite(&xml);

	m_strName = oldname;
  m_strImageName = oldimage;
  m_nOffset = old_offset;

  return flag;
}

//-----------------------------------------------------------------------------
// Export en GeoTIFF 16 bits
//-----------------------------------------------------------------------------
bool XGeoFDtm::ExportTiff16(std::string filename)
{
  if (!m_bValid)
    return false;

  XPath P;
  std::string xmlfile = P.PathName(filename.c_str()) + ".xml";
  std::string tifffile = P.PathName(filename.c_str()) + ".tif";

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax  + m_dGSD * 0.5, m_dGSD);
  if (!tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 16, NULL, 2))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);

  // Ecriture des donnees
  if (!StreamReady())
    return false;
  float* line_in = new float[m_nW];
  if (line_in == NULL)
    return false;
  short* line_out = new short[m_nW];
  if (line_out == NULL)
    return false;
  for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line_in, i);
    for (uint32_t j = 0; j < m_nW; j++)
      line_out[j] = XRint(line_in[j]);
    out.write((char*)line_out, m_nW * sizeof(short));
  }
  out.close();
  delete[] line_out;
  delete[] line_in;

  return true;
}

//-----------------------------------------------------------------------------
// Export au format ASC
//-----------------------------------------------------------------------------
bool XGeoFDtm::ExportAsc(std::string filename)
{
	if (!m_bValid)
		return false;

	// Ouverture du fichier de sortie
	std::ofstream out;
	out.open(filename.c_str());
	if (!out.good())
		return false;

	// Ecriture de l'entete
	if(!WriteAscHeader(&out))
		return false;

	// Ecriture des donnees
  if (!StreamReady())
    return false;
	float* line = new float[m_nW];
	if (line == NULL)
		return false;
	for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
		for (uint32_t j = 0; j < m_nW; j++) 
			out << line[j] << " ";
		out << std::endl;
	}
	out.close();
	delete[] line;
	return true;
}


//-----------------------------------------------------------------------------
// Export au format XYZ
//-----------------------------------------------------------------------------
bool XGeoFDtm::ExportXyz(std::string filename)
{
	if (!m_bValid)
		return false;

	// Ouverture du fichier de sortie
	std::ofstream out;
	out.open(filename.c_str());
	if (!out.good())
		return false;

	// Ecriture des donnees
  if (!StreamReady())
    return false;
	float* line = new float[m_nW];
	if (line == NULL)
		return false;
	for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
		for (uint32_t j = 0; j < m_nW; j++)
			if (line[j] > m_dNoData)
				out << XRint(m_Frame.Xmin + j * m_dGSD) << " " << XRint(m_Frame.Ymax - i * m_dGSD) 
						<< " " << line[j] << std::endl;
	}
	out.close();
	delete[] line;
	return true;
}

//-----------------------------------------------------------------------------
// Conversion geodesique d'un MNT au format ASC
//-----------------------------------------------------------------------------
bool XGeoFDtm::ConvertAsc(const char* file_out, XGeodConverter* L, XError* error)
{
	std::ofstream out;
	out.open(file_out);
	if (!out.good())
		return XErrorError(error,"XGeoDtm::ConvertAsc", XError::eIOOpen);

	double Xmin = Frame().Xmin;
	double Ymax = Frame().Ymax;
	double Xmax = Frame().Xmax;
	double Ymin = Frame().Ymin;
	double gsd = Resolution();

	// Emprise du MNT reprojete
	double XminP, YminP, XmaxP, YmaxP;
	double Xp[4], Yp[4];

	L->ConvertDeg(Xmin, Ymin, Xp[0], Yp[0]);
	L->ConvertDeg(Xmin, Ymax, Xp[1], Yp[1]);
	L->ConvertDeg(Xmax, Ymax, Xp[2], Yp[2]);
	L->ConvertDeg(Xmax, Ymin, Xp[3], Yp[3]);
	XminP = XmaxP = Xp[0];
	YminP = YmaxP = Yp[0];
	for (uint32_t k = 1; k < 4; k++) {
		XminP = XMin(XminP, Xp[k]);
		XmaxP = XMax(XmaxP, Xp[k]);
		YminP = XMin(YminP, Yp[k]);
		YmaxP = XMax(YmaxP, Yp[k]);
	}

	// Dimension du MNT reprojete
	XminP = XRint(XminP / gsd) * gsd;
	YmaxP = XRint(YmaxP / gsd) * gsd;
	XmaxP = XRint(XmaxP / gsd) * gsd;
	YminP = XRint(YminP / gsd) * gsd;

	uint32_t W = XRint((XmaxP - XminP)/gsd);
	uint32_t H = XRint((YmaxP - YminP)/gsd);

	out.setf(std::ios::fixed);
	out.precision(2);

	out << "ncols         " << W << std::endl;
	out << "nrows         " << H << std::endl;
	out << "xllcorner     " << XminP - gsd * 0.5 << std::endl;
	out << "yllcorner     " << YminP - gsd * 0.5 << std::endl;
	out << "cellsize      " << gsd << std::endl;
	out << "NODATA_value  " << m_dNoData << std::endl;

	// Sauvegarde des parametres de projection
	XProjCode startProj = L->StartProjection();
	XProjCode endProj = L->EndProjection();
	L->SetDefaultProjection(endProj, startProj);

	double x, y, xP, yP, z;
	for (uint32_t i = 0; i < H; i++) {
		for (uint32_t j = 0; j < W; j++) {
			xP = XminP + j * gsd;
			yP = YmaxP - i * gsd;
			L->ConvertDeg( xP, yP, x, y);
			z = Z(x, y);
			out << z << " ";		
		}
		out << std::endl;
	}

	L->SetDefaultProjection(startProj, endProj);

	return out.good();
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XGeoFDtm::ReadAttributes(std::vector<std::string>& V)
{
  char buf[256];
  V.clear();
  V.push_back("Nom");
  V.push_back(m_strName);

  V.push_back("Resolution");
  sprintf(buf,"%.2lf", m_dGSD);
  V.push_back(buf);
  V.push_back("Zmin");
  if (m_dZmin > m_dNoData)
    sprintf(buf,"%.2lf", m_dZmin);
  else
    strcpy(buf, "NOZ");
  V.push_back(buf);
  V.push_back("Zmax");
  if (m_dZmax > m_dNoData)
    sprintf(buf,"%.2lf", m_dZmax);
  else
    strcpy(buf, "NOZ");
  V.push_back(buf);
  V.push_back("NbNOZ");
  sprintf(buf,"%u", m_nNbNoZ);
  V.push_back(buf);

  V.push_back("Largeur");
  sprintf(buf,"%u", m_nW);
  V.push_back(buf);
  V.push_back("Hauteur");
  sprintf(buf,"%u", m_nH);
  V.push_back(buf);
  V.push_back("Xmin");
  sprintf(buf,"%.2lf", m_Frame.Xmin);
  V.push_back(buf);
  V.push_back("Ymin");
  sprintf(buf,"%.2lf", m_Frame.Ymin);
  V.push_back(buf);
  V.push_back("Xmax");
  sprintf(buf,"%.2lf", m_Frame.Xmax);
  V.push_back(buf);
  V.push_back("Ymax");
  sprintf(buf,"%.2lf", m_Frame.Ymax);
  V.push_back(buf);

  return true;
}

//-----------------------------------------------------------------------------
// Ecriture des informations sur l'objet
//-----------------------------------------------------------------------------
bool XGeoFDtm::WriteHtml(std::ostream* out)
{
  // Ouverture de l'image si necessaire
  *out << "<HTML> <BODY>";
  out->setf(std::ios::fixed);
  out->precision(2);
  *out << "<I>Theme</I> : " << Class()->Layer()->Name() << "<BR>";
  *out << "<I>Classe</I> : <B>" << ClassName() << "</B><BR><BR>";

  *out << "<hr style=\"width: 100%; height: 2px;\">";
  *out << "<I>Type</I> : MNT<BR>";
  *out << "<I>Nom</I> : " << m_strName << " <BR>";
  *out << "<I>Fichier</I> : " << m_strFilename << " <BR>";
  *out << "<I>Emprise</I> : (" << m_Frame.Xmin << " ; " << m_Frame.Ymin << ") - (" <<
            m_Frame.Xmax << " ; " << m_Frame.Ymax << ")<BR>";
  if ((m_dGSD < 0.1)&&(m_Frame.Width() <= 361)&&(m_Frame.Height() < 361)) // Geographique
    *out << "<I>Résolution</I> : " << m_dGSD * 111319.49 << " m<BR>";
  else
    *out << "<I>Résolution</I> : " << m_dGSD << " m<BR>";


  *out << "<hr style=\"width: 100%; height: 2px;\">";
  *out << "<I>Z min</I> : " << m_dZmin << " m<BR>";
  *out << "<I>Z max</I> : " << m_dZmax << " m<BR>";
  *out << "<I>Noeuds sans Z</I> : " << m_nNbNoZ << "<BR>";
  *out << "<I>Largeur</I> : " << m_nW << "<BR>";
  *out << "<I>Hauteur</I> : " << m_nH << "<BR>";
  *out << "<I>Offset</I> : " << m_nOffset << "<BR>";
  *out << "<I>No Data</I> : " << m_dNoData << "<BR>";

  /*
  uint32_t low, high;
  if (Volume(low, high)) {
    *out << "<I>Nb. Noeuds < 0 </I> : " << low << "<BR>";
    *out << "<I>Nb. Noeuds > 0 </I> : " << high << "<BR>";
  }*/

  *out << "</BODY> </HTML>";

  return true;
}

//-----------------------------------------------------------------------------
// Export sous forme de courbes de niveaux
//-----------------------------------------------------------------------------
bool XGeoFDtm::ExportContour(std::string filename, double equi, double resol)
{
  if (!m_bValid)
    return false;
  if (resol <= 0.)
    resol = m_dGSD * 0.1;

  int factor = XRint(m_dGSD / resol);

  // Ouverture du fichier de sortie
  std::string tifffile = filename;
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax + m_dGSD * 0.5, resol);
  if (!tiff.Write(tifffile.c_str(), m_nW * factor, m_nH * factor, 1, 8))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);

  if (!StreamReady())
    return false;
  // Ecriture des donnees
  float* lineT = new float[m_nW];
  float* lineB = new float[m_nW];
  uint8_t* pix = new uint8_t[m_nW * factor * factor];
  int* Z = new int[factor * factor];
  if ((lineB == NULL)||(lineT == NULL||(pix == NULL)||(Z == NULL))) {
    delete[] lineB;
    delete[] lineT;
    delete[] pix;
    delete[] Z;
    return false;
  }
  double N[4];
  XInterLin interpol;

  ReadLine(lineT, 0);
  for (uint32_t i = 1; i < m_nH; i++) {
    ReadLine(lineB, i);
    ::memset(pix, 255, m_nW * factor * factor);
    for (uint32_t j = 0; j < m_nW - 1; j++) {
      N[0] = lineT[j];
      N[1] = lineT[j+1];
      N[2] = lineB[j];
      N[3] = lineB[j+1];
      for (uint32_t y = 0; y < factor; y++)
        for (uint32_t x = 0; x < factor; x++) {
          Z[y * factor + x] = floor(interpol.BiCompute(N, x, y, factor, factor) / equi);
         // pix[y * m_nW * factor + x + j * factor] = Z[y * factor + x];
        }

      for (uint32_t y = 0; y < factor - 1; y++)
        for (uint32_t x = 0; x < factor - 1; x++) {
          if (Z[y * factor + x] != Z[y * factor + x + 1])
            pix[y * m_nW * factor + x + j * factor] = 0;
          if (Z[y * factor + x] != Z[(y + 1) * factor + x])
            pix[y * m_nW * factor + x + j * factor] = 0;
      }

    }

    out.write((char*)pix, m_nW * factor * factor * sizeof(uint8_t));

    float* swap = lineT;
    lineT = lineB;
    lineB = swap;
  }
  out.close();
  delete[] lineB;
  delete[] lineT;
  delete[] pix;
  delete[] Z;
  return out.good();
}

//-----------------------------------------------------------------------------
// Predicat pour l'unicite
//-----------------------------------------------------------------------------
bool predXPt2DNear(XPt2D A, XPt2D B)
{
	if (fabs(A.X - B.X) > 0.5)
		return false;
	if (fabs(A.Y - B.Y) > 0.5)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// Predicat pour les tris
//-----------------------------------------------------------------------------
bool predXPt2DSort(XPt2D A, XPt2D B)
{
	if (A.X > B.X)
		return true;
	if (A.X < B.X)
		return false;
	if (A.Y > B.Y)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Export d'une image d'inondation
//-----------------------------------------------------------------------------
int XGeoFDtm::ExportFlood(std::string filename, double Z0, std::vector<XPt2D>& P,
                          int nb_seed, std::vector<XGeoVector*> *V)
{
  if (m_dZmin > Z0)
    return false;
  // Test si les points de depart concernent ce MNT
  XFrame frame = m_Frame;
  frame.Xmin -= m_dGSD;
  frame.Xmax += m_dGSD;
  frame.Ymin -= m_dGSD;
  frame.Ymax += m_dGSD;

  int inside_pt = 0;
  for (uint32_t i = 0; i < P.size(); i++) {
    if (frame.IsIn(P[i])) {
      inside_pt++;
    }
  }
  if (inside_pt < 1)
    return 0;
  if (inside_pt <= nb_seed)
    return nb_seed;

  uint8_t* T = new uint8_t[m_nW * m_nH];
  ::memset(T, 0, m_nW * m_nH * sizeof(uint8_t));

  // Detection des noeuds sous le niveau Z0
  std::vector<int> line;
  double z;
  for (uint32_t i = 0; i < m_nH; i++) {
    bool flag = false;
    for (uint32_t j = 0; j < m_nW; j++) {
      z = Z(j, i);
      if ((z < Z0)&&(z > XGEO_NO_DATA)) {
        T[i * m_nW + j] = 200;
        flag = true;
      }
    }
    if (flag) line.push_back(i);
  }
  if (V != NULL) {
    for (uint32_t i = 0; i < V->size(); i++)
      InjectVector((*V)[i], T, 0);
  }

  // Creation des points de depart
  int u, v, nb_start_pt = 0;
  for (uint32_t i = 0; i < P.size(); i++) {
    if (!frame.IsIn(P[i]))
      continue;
    u = XRint((P[i].X - m_Frame.Xmin) / m_dGSD);
    v = XRint((m_Frame.Ymax - P[i].Y) / m_dGSD);
    if (u < 0) u = 0;
    if (v < 0) v = 0;
    if (u >= m_nW) u = m_nW - 1;
    if (v >= m_nH) v = m_nH - 1;
    if (T[v * m_nW + u] > 0) {
      T[v * m_nW + u] = 66;
      nb_start_pt++;
    }
  }
  if (nb_start_pt < 1) {
    delete[] T;
    return inside_pt;
  }

  // Propagation dans le MNT
  uint32_t nb_changed = 1000;
  uint32_t num_line;
  bool next_line = false, prev_line = false;
  while (nb_changed > 0) {
    nb_changed = 0;
    for (int i = 0; i < line.size(); i++) {
      num_line = line[i];
      next_line = false;
      prev_line = false;
      if (i < line.size() - 1) {
        if ((line[i+1] - num_line) == 1) next_line = true;
      }
      if (i > 0) {
        if (num_line - line[i-1] == 1) prev_line = true;
      }

      for (int j = 0; j < m_nW; j++) {
        if (T[num_line * m_nW + j] == 0)
          continue;
        if (T[num_line * m_nW + j] == 66) { // On inonde les noeuds voisins
          for (int k = 1; k <= j; k++) { // Inondation a gauche
            if (T[num_line * m_nW + j - k] > 66)
              { T[num_line * m_nW + j - k] = 66; nb_changed++;}
            else
              break;
          }

          for (int k = 1; k <= (m_nW - j - 1); k++) { // Inondation a droite
            if (T[num_line * m_nW + j + k] > 66)
              { T[num_line * m_nW + j + k] = 66; nb_changed++;}
            else
              break;
          }

         if (prev_line)
           if (T[(num_line-1) * m_nW + j] > 66) { T[(num_line-1) * m_nW + j] = 66; nb_changed++;}
         if (next_line)
           if (T[(num_line+1) * m_nW + j] > 66) { T[(num_line+1) * m_nW + j] = 66; nb_changed++;}

        } // endif

      } // endfor j
    } // endfor i
  } // endwhile

  // Mise a jour des points de depart
  for (uint32_t i = 0; i < m_nW; i++) {
    if (T[i] == 66) P.push_back(XPt2D(m_Frame.Xmin + i * m_dGSD, m_Frame.Ymax + m_dGSD * 0.5));
    if (T[(m_nH-1)*m_nW +i] == 66) P.push_back(XPt2D(m_Frame.Xmin + i * m_dGSD, m_Frame.Ymin - m_dGSD * 0.5));
  }
  for (uint32_t i = 0; i < m_nH; i++) {
    if (T[i * m_nH] == 66) P.push_back(XPt2D(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax - i * m_dGSD));
    if (T[i * m_nH + m_nW - 1] == 66) P.push_back(XPt2D(m_Frame.Xmax + m_dGSD * 0.5, m_Frame.Ymax - i * m_dGSD));
  }
  std::sort(P.begin(), P.end(), predXPt2DSort);
  std::vector<XPt2D>::iterator iter = std::unique(P.begin(), P.end(), predXPt2DNear);
  P.resize(iter - P.begin());

  // Ouverture du fichier de sortie
  std::string tifffile = filename;
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax + m_dGSD * 0.5, m_dGSD);
  bool tiff_flag = tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 8, T);

  // Calcul des statistiques
  uint32_t nb_0 = 0, nb_66 = 0, nb_200 = 0;
  double volume = 0;
  for (uint32_t i = 0; i < m_nH; i++) {
    for (uint32_t j = 0; j < m_nW; j++) {
      if (T[i * m_nW + j] == 0) {
        nb_0++;
        continue;
      }
      if (T[i * m_nW + j] == 200) {
        nb_200++;
        continue;
      }
      if (T[i * m_nW + j] == 66) {
        nb_66++;
        z = Z(j, i);
        volume += (Z0 - z);
        continue;
      }
    }
  }
  std::ofstream out((filename + ".info").c_str());
  out << "Nom du fichier :\t" << filename << std::endl;
  out << "Altitude d'inondation :\t" << Z0 << std::endl;
  out << "Nombre de noeuds :\t" << m_nW * m_nH << std::endl;
  out << "Noeuds non inondés (Z > " << Z0 << ") :\t" << nb_0 << std::endl;
  out << "Noeuds non inondés (Z < " << Z0 << ") :\t" << nb_200 << std::endl;
  out << "Noeuds inondés :\t" << nb_66 << std::endl;
  out << "Volume d'inondation (m3) :\t" << volume * m_dGSD * m_dGSD << std::endl;

  delete[] T;
  return inside_pt;
}
/*
//-----------------------------------------------------------------------------
// Calcul du nombre de noeuds au dessus et en dessous d'un Z0
//-----------------------------------------------------------------------------
bool XGeoFDtm::Volume(uint32_t& nbLow, uint32_t& nbHigh, float Z0)
{
  nbLow = nbHigh = 0;
  if (!m_bValid)
    return false;
  // Lecture des donnees
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return false;
  in->seekg(m_nOffset, std::ios::beg);
  float* line = new float[m_nW];
  if (line == NULL)
    return false;
  for (uint32_t i = 0; i < m_nH; i++) {
    in->read((char*)line, m_nW * sizeof(float));
    for (uint32_t j = 0; j < m_nW; j++) {
      if (line[j] > Z0)
        nbHigh++;
      if ((line[j] < Z0)&&(line[j] > m_dNoData))
        nbLow++;
    }
  }
  delete[] line;
  return true;
}
*/

//-----------------------------------------------------------------------------
// Calcul du nombre de noeuds en fonction de plages altimetriques
//-----------------------------------------------------------------------------
bool XGeoFDtm::Volume(std::vector<double>& P, std::vector<uint32_t>& N, double& zmean)
{
  N.clear();
  if (!m_bValid)
    return false;
  if (P.size() < 1)
    return false;
  uint32_t nb_val = P.size() + 1;
  uint32_t* tab = new uint32_t[nb_val];
  if (tab == NULL)
    return false;
  for (uint32_t i = 0; i < nb_val; i++) tab[i] = 0;

  // Lecture des donnees
  if (!StreamReady()) {
    delete[] tab;
    return false;
  }
  float* line = new float[m_nW];
  if (line == NULL) {
    delete[] tab;
    return false;
  }
  float z;
  zmean = 0.;
  uint32_t nb_node = 0;
  for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
    for (uint32_t j = 0; j < m_nW; j++) {
      z = line[j];
      if (z <= m_dNoData) continue;
      zmean += z; nb_node++;
      if (z >= P[P.size() - 1]) { tab[P.size()]++; continue;}
      for (uint32_t k = 0; k < P.size(); k++) {
        if (z < P[k]) { tab[k]++; break;}
      }
    }
  }
  delete[] line;
  for (uint32_t i = 0; i < nb_val; i++)
    N.push_back(tab[i]);
  delete[] tab;
  if (nb_node > 0)
    zmean /= nb_node;
  return true;
}

//-----------------------------------------------------------------------------
// Export d'une difference de MNT
//-----------------------------------------------------------------------------
bool XGeoFDtm::ExportDiff(std::string filename, XGeoFDtm* dtm)
{
  if ((!m_bValid)||(!dtm->m_bValid))
    return false;
  if (dtm->W() != W())
    return false;
  if (dtm->H() != H())
    return false;
  if (fabs(dtm->Frame().Xmin - Frame().Xmin) > m_dGSD)
    return false;
  if (fabs(dtm->Frame().Ymin - Frame().Ymin) > m_dGSD)
    return false;
  if (fabs(dtm->Resolution() - Resolution()) > m_dGSD * 0.01)
    return false;


  XPath P;
  std::string xmlfile = P.PathName(filename.c_str()) + ".xml";
  std::string tifffile = P.PathName(filename.c_str()) + ".tif";

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  if (!tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 32))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  uint32_t offset = out.tellp();

  // Ecriture des donnees
  if (!StreamReady())
    return false;
  if (!dtm->StreamReady())
    return false;
  float* line = new float[m_nW];
  if (line == NULL)
    return false;
  float* line_diff = new float[m_nW];
  if (line_diff == NULL) {
    delete[] line;
    return false;
  }
  double zmin = 1e31, zmax = -1e31;
  uint32_t nodata = 0;
  for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
    dtm->ReadLine(line_diff, i);
    for (uint32_t j = 0; j < m_nW; j++) {
      if (line_diff[j] <= dtm->m_dNoData) {
        line[j] = (float)m_dNoData;
        nodata++;
      } else {
        if (line[j] <= m_dNoData)
          line[j] = line_diff[j];
        else
          line[j] -= line_diff[j];
        zmin = XMin(zmin, (double)line[j]);
        zmax = XMax(zmax, (double)line[j]);
      }
    }
    out.write((char*)line, m_nW * sizeof(float));
  }
  out.close();
  delete[] line;
  delete[] line_diff;

  // Ecriture du fichier XML
  std::string name = P.Name(filename.c_str(), false);
  std::string oldname = m_strName;
  std::string oldimage = m_strImageName;
  m_strName = name;
  m_strImageName = name + ".tif";
  uint32_t old_offset = m_nOffset;
  m_nOffset = offset;
  double old_zmin = m_dZmin;
  m_dZmin = zmin;
  double old_zmax = m_dZmax;
  m_dZmax = zmax;
  double old_nbNoZ = m_nNbNoZ;
  m_nNbNoZ = nodata;

  std::ofstream xml;
  xml.open(xmlfile.c_str());
  bool flag = false;
  if (xml.good())
    flag = XmlWrite(&xml);

  m_strName = oldname;
  m_strImageName = oldimage;
  m_nOffset = old_offset;
  m_dZmin = old_zmin;
  m_dZmax = old_zmax;
  m_nNbNoZ = old_nbNoZ;

  return flag;
}

//-----------------------------------------------------------------------------
// Recherche des ecarts importants (DZ entre noeuds voisins)
//-----------------------------------------------------------------------------
bool XGeoFDtm::DeltaMax(double Dz, std::vector<XPt3D>& T)
{
  T.clear();
  if (!m_bValid)
    return false;

  // Lecture des donnees
  if (!StreamReady())
    return false;
  float* lineP = new float[m_nW];
  if (lineP == NULL)
    return false;
  float* lineQ = new float[m_nW];
  if (lineQ == NULL) {
    delete[] lineP;
    return false;
  }

  float z, zd, zb, diff_d, diff_b;
  XPt3D P;
  bool flag = false;
  ReadLine(lineP, 0);
  for (uint32_t i = 0; i < m_nH - 1; i++) {
    ReadLine(lineQ, i + 1);
    for (uint32_t j = 0; j < m_nW - 1; j++) {
      flag = false;
      diff_d = diff_b = 0.0;
      z = lineP[j];
      if (z <= m_dNoData) continue;
      zd = lineP[j + 1];
      zb = lineQ[j];
      if (zd > m_dNoData) {
        if (fabs(z-zd) > Dz) { flag = true; diff_d = fabs(z-zd);}
      }
      if (zb > m_dNoData) {
        if (fabs(z-zb) > Dz) { flag = true; diff_b = fabs(z-zb);}
      }
      if (flag) {
        P = XPt3D(m_Frame.Xmin + j * m_dGSD, m_Frame.Ymax - i * m_dGSD, XMax(diff_d, diff_b));
        T.push_back(P);
      }
    } // endfor j
    float* swap = lineQ;
    lineQ = lineP;
    lineP = swap;
  } // endfor i
  delete[] lineP;
  delete[] lineQ;
  return true;
}

//-----------------------------------------------------------------------------
// Recherche d'une courbe de niveau
//-----------------------------------------------------------------------------
bool XGeoFDtm::FindContourLine(double Z0, uint8_t* area)
{
  if ((Z0 < m_dZmin)||(Z0 > m_dZmax))
    return false;
  // Lecture des donnees
  if (!StreamReady())
    return false;
  float* lineP = new float[m_nW];
  if (lineP == NULL)
    return false;
  float* lineQ = new float[m_nW];
  if (lineQ == NULL) {
    delete[] lineP;
    return false;
  }

  float z, zd, zb;
  XPt3D P;
  ReadLine(lineP, 0);
  for (uint32_t i = 0; i < m_nH - 1; i++) {
    ReadLine(lineQ, i + 1);
    for (uint32_t j = 0; j < m_nW - 1; j++) {
      z = lineP[j];
      if (z <= m_dNoData) continue;
      zd = lineP[j + 1];
      if (zd <= m_dNoData) continue;
      zb = lineQ[j];
      if (zb <= m_dNoData) continue;

      if ((z <= Z0)&&(zd > Z0))
        area[i*m_nW + j] = 255;
      if ((z > Z0)&&(zd <= Z0))
        area[i*m_nW + j] = 255;
      if ((z <= Z0)&&(zb > Z0))
        area[i*m_nW + j] = 255;
      if ((z > Z0)&&(zb <= Z0))
        area[i*m_nW + j] = 255;
    } // endfor j
    float* swap = lineQ;
    lineQ = lineP;
    lineP = swap;
  } // endfor i
  delete[] lineP;
  delete[] lineQ;
  return true;

}

//-----------------------------------------------------------------------------
// Recherche des thalwegs
//-----------------------------------------------------------------------------
bool XGeoFDtm::FindThalweg(std::string filename)
{
  if (!m_bValid)
    return false;

  XPath P;
  std::string tifffile = P.PathName(filename.c_str()) + ".tif";

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_Frame.Xmin - m_dGSD * 0.5, m_Frame.Ymax + m_dGSD * 0.5, m_dGSD);
  if (!tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 16))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  uint32_t offset = out.tellp();

  // Ecriture des donnees
  if (!StreamReady())
    return false;
  float* mnt = new float[m_nW*m_nH];
  if (mnt == NULL)
    return false;
  uint16_t* thal = new uint16_t[m_nW*m_nH];
  if (thal == NULL) {
    delete[] mnt;
    return false;
  }
  uint8_t* water = new uint8_t[m_nW*m_nH];
  if (water == NULL) {
    delete[] mnt;
    delete[] thal;
    return false;
  }
  uint8_t* volume = new uint8_t[m_nW*m_nH];
  if (volume == NULL){
    delete[] mnt;
    delete[] thal;
    delete[] water;
    return false;
  }

  for (uint32_t i = 0; i < m_nH; i++)
    for (uint32_t j = 0; j < m_nW; j++) {
      thal[i*m_nW + j] = 0;
      water[i*m_nW + j] = 1;
      volume[i*m_nW + j] = 0;
    }

  ReadAll(mnt);

  int iter = 0;
  while(true) {
    iter++;
    for (uint32_t i = 1; i < m_nH - 1; i++) {
      for (uint32_t j = 1; j < m_nW - 1; j++) {

        if (water[(i - 1) * m_nW + j - 1] > 0) {  // Z11
          if (mnt[(i - 1) * m_nW + j - 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i - 1) * m_nW + j - 1];
          }
        }
        if (water[(i - 1) * m_nW + j] > 0) {  // Z12
          if (mnt[(i - 1) * m_nW + j] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i - 1) * m_nW + j];
          }
        }
        if (water[(i - 1) * m_nW + j + 1] > 0) {  // Z13
          if (mnt[(i - 1) * m_nW + j + 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i - 1) * m_nW + j + 1];
          }
        }

        if (water[i * m_nW + j - 1] > 0) {  // Z21
          if (mnt[i * m_nW + j - 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[i * m_nW + j - 1];
          }
        }
        if (water[i * m_nW + j + 1] > 0) {  // Z23
          if (mnt[i * m_nW + j + 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[i * m_nW + j + 1];
          }
        }

        if (water[(i + 1) * m_nW + j - 1] > 0) {  // Z31
          if (mnt[(i + 1) * m_nW + j - 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i + 1) * m_nW + j - 1];
          }
        }
        if (water[(i + 1) * m_nW + j] > 0) {  // Z32
          if (mnt[(i + 1) * m_nW + j] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i + 1) * m_nW + j];
          }
        }
        if (water[(i + 1) * m_nW + j + 1] > 0) {  // Z33
          if (mnt[(i + 1) * m_nW + j + 1] > mnt[i * m_nW + j]) {
            volume[i * m_nW + j] += water[(i + 1) * m_nW + j + 1];
          }
        }

        thal[i * m_nW + j] = XMin((int)thal[i * m_nW + j] + (int)volume[i * m_nW + j], 65535);

      } // endfor j
    } // endfor i

    bool no_water = true;
    for (uint32_t i = 0; i < m_nH; i++){
      for (uint32_t j = 0; j < m_nW; j++){
        if (volume[i*m_nW + j] > 0) {
          water[i*m_nW + j] = 1;
          no_water = false;
        } else
          water[i*m_nW + j] = 0;
        volume[i*m_nW + j] = 0;
      }
    }
    if (no_water)
      break;
  } // endwhile


  out.write((char*)thal, m_nW * m_nH * sizeof(uint16_t));
  out.close();
  delete[] mnt;
  delete[] thal;
  delete[] water;

  return true;
}

/*
//-----------------------------------------------------------------------------
// Recherche des thalwegs
//-----------------------------------------------------------------------------
bool XGeoFDtm::FindThalweg(std::string filename)
{
  if (!m_bValid)
    return false;

  XPath P;
  std::string xmlfile = P.PathName(filename.c_str()) + ".xml";
  std::string tifffile = P.PathName(filename.c_str()) + ".tif";

  // Ouverture du fichier de sortie
  XTiffWriter tiff;
  if (!tiff.Write(tifffile.c_str(), m_nW, m_nH, 1, 8))
    return false;
  std::ofstream out;
  out.open(tifffile.c_str(), std::ios_base::out| std::ios_base::binary| std::ios_base::app);
  if (!out.good())
    return false;
  out.seekp(0, std::ios_base::end);
  uint32_t offset = out.tellp();

  // Ecriture des donnees
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return false;
  in->seekg(m_nOffset, std::ios::beg);
  float* mnt = new float[m_nW*m_nH];
  if (mnt == NULL)
    return false;
  uint8_t* thal = new uint8_t[m_nW*m_nH];
  if (thal == NULL) {
    delete[] mnt;
    return false;
  }
  bool* flag = new bool[m_nW*m_nH];
  if (flag == NULL) {
    delete[] mnt;
    delete[] thal;
    return false;
  }

  for (uint32_t i = 0; i < m_nH; i++) {
    for (uint32_t j = 0; j < m_nW; j++) {
      thal[i*m_nW + j] = 0;
      flag[i*m_nW + j] = false;
    }
  }


  in->read((char*)mnt, m_nW * m_nH * sizeof(float));

  int u = (int)m_nW / 2, v = (int)m_nH / 2, sum, new_u, new_v;
  uint32_t nb_node = 0;
  float z11, z12, z13, z21, z22, z23, z31, z32, z33;
  float dz11, dz12, dz13, dz21, dz23, dz31, dz32, dz33;
  while(true) {
    nb_node ++;
    if (nb_node > m_nW * m_nH)
      break;
    flag[v * m_nW + u] = true;

    if ((u == 0)||(v == 0))
      z11 = m_dZmax;
    else
      z11 = mnt[(v-1)*m_nW + u - 1];

    if (v == 0)
      z12 = m_dZmax;
    else
      z12 = mnt[(v-1)*m_nW + u];

    if ((u >= (m_nW - 1))||(v == 0))
      z13 = m_dZmax;
    else
      z13 = mnt[(v-1)*m_nW + u + 1];

    if (u == 0)
      z21 = m_dZmax;
    else
      z21 = mnt[v*m_nW + u - 1];

    z22 = mnt[v*m_nW + u];

    if (u >= (m_nW - 1))
      z23 = m_dZmax;
    else
      z23 = mnt[v*m_nW + u + 1];

    if ((u == 0)||(v >= (m_nH - 1)))
      z31 = m_dZmax;
    else
      z31 = mnt[(v+1)*m_nW + u - 1];

    if (v >= (m_nH - 1))
      z32 = m_dZmax;
    else
      z32 = mnt[(v+1)*m_nW + u];

    if ((u >= (m_nW - 1))||(v >= (m_nH - 1)))
      z33 = m_dZmax;
    else
      z33 = mnt[(v+1)*m_nW + u + 1];

    dz11 = z22 - z11;
    dz12 = z22 - z12;
    dz13 = z22 - z13;
    dz21 = z22 - z21;
    dz23 = z22 - z23;
    dz31 = z22 - z31;
    dz32 = z22 - z32;
    dz33 = z22 - z33;
    new_u = 0;
    new_v = 0;

    // Z11
    if ((dz11 >= dz12) && (dz11 >= dz13) && (dz11 >= dz21) && (dz11 >= dz23) &&
        (dz11 >= dz31) && (dz11 >= dz32) && (dz11 >= dz33) && (dz11 >= 0.)) {
      new_u = -1;
      new_v = -1;
    }

    // Z12
    if ((dz12 >= dz11) && (dz12 >= dz13) && (dz12 >= dz21) && (dz12 >= dz23) &&
        (dz12 >= dz31) && (dz12 >= dz32) && (dz12 >= dz33) && (dz12 >= 0.)) {
      new_u = 0;
      new_v = -1;
    }

    // Z13
    if ((dz13 >= dz11) && (dz13 >= dz12) && (dz13 >= dz21) && (dz13 >= dz23) &&
        (dz13 >= dz31) && (dz13 >= dz32) && (dz13 >= dz33) && (dz13 >= 0.)) {
      new_u = +1;
      new_v = -1;
    }

    // Z21
    if ((dz21 >= dz11) && (dz21 >= dz12) && (dz21 >= dz13) && (dz21 >= dz23) &&
        (dz21 >= dz31) && (dz21 >= dz32) && (dz21 >= dz33) && (dz21 >= 0.)) {
      new_u = -1;
      new_v = 0;
    }

    // Z23
    if ((dz23 >= dz11) && (dz23 >= dz12) && (dz23 >= dz13) && (dz23 >= dz21) &&
        (dz23 >= dz31) && (dz23 >= dz32) && (dz23 >= dz33) && (dz23 >= 0.)) {
      new_u = +1;
      new_v = 0;
    }

    // Z31
    if ((dz31 >= dz11) && (dz31 >= dz12) && (dz31 >= dz13) && (dz31 >= dz21) &&
        (dz31 >= dz23) && (dz31 >= dz32) && (dz31 >= dz33) && (dz31 >= 0.)) {
      new_u = -1;
      new_v = +1;
    }

    // Z32
    if ((dz32 >= dz11) && (dz32 >= dz12) && (dz32 >= dz13) && (dz32 >= dz21) &&
        (dz32 >= dz23) && (dz32 >= dz31) && (dz32 >= dz33) && (dz32 >= 0.)) {
      new_u = 0;
      new_v = +1;
    }

    // Z33
    if ((dz33 >= dz11) && (dz33 >= dz12) && (dz33 >= dz13) && (dz33 >= dz21) &&
        (dz33 >= dz23) && (dz33 >= dz31) && (dz33 >= dz32) && (dz33 >= 0.)) {
      new_u = +1;
      new_v = +1;
    }

    if ((new_u != 0) && (new_v != 0)) {
      sum = thal[v*m_nW + u] + thal[(v+new_v)*m_nW + u + new_u] + 1;
      if (sum < 255)
        thal[(v+new_v)*m_nW + u + new_u] = sum;
      else
        thal[(v+new_v)*m_nW + u + new_u] = 255;
      if (flag[(v+new_v)*m_nW + u + new_u] == false) {
        v = v + new_v;
        u = u + new_u;
        continue;
      }
    }

    // Recherche d'un nouveau noeud
    bool found = false;
    for (uint32_t i = 1; i < (m_nH - 1); i++) {
      for (uint32_t j = 1; j < (m_nW - 1); j++) {
        if (flag[i * m_nW + j] == false) {
          u = j;
          v = i;
          found = true;
          break;
        }
      }
      if (found)
        break;
    }
    if (!found) // Plus de noeuds
      break;
    else
      continue;
  }

  out.write((char*)thal, m_nW * m_nH * sizeof(uint8_t));
  out.close();
  delete[] mnt;
  delete[] thal;
  delete[] flag;

  // Ecriture du fichier XML
  std::string name = P.Name(filename.c_str(), false);
  std::string oldname = m_strName;
  std::string oldimage = m_strImageName;
  m_strName = name;
  m_strImageName = name + ".tif";
  uint32_t old_offset = m_nOffset;
  m_nOffset = offset;

  std::ofstream xml;
  xml.open(xmlfile.c_str());
  if (xml.good())
    XmlWrite(&xml);

  m_strName = oldname;
  m_strImageName = oldimage;
  m_nOffset = old_offset;

  return true;
}
*/

bool XGeoFDtm::InjectVector(XGeoVector* V, uint8_t* area, uint8_t val)
{
  if (!V->Frame().Intersect(m_Frame))
    return false;
  XPt2D A, B;
  double dx, dy;
  int nb_step, u, v;
  for (uint32_t i = 0; i < V->NbPt() -  1; i++) {
    A = V->Pt(i);
    B = V->Pt(i+1);
    dx = B.X - A.X;
    dy = B.Y - A.Y;
    if (fabs(dx) > fabs(dy))
      nb_step = fabs(dx) / m_dGSD;
    else
      nb_step = fabs(dy) / m_dGSD;
    nb_step *= 2;
    for (int k = 0; k < nb_step; k++) {
      u = XRint(((A.X - m_Frame.Xmin) + k * dx / nb_step)/m_dGSD);
      v = XRint(((m_Frame.Ymax - A.Y) - k * dy / nb_step)/m_dGSD);
      if ((v >= 0)&&(u >=0)&&(v < m_nH)&&(u < m_nW))
        area[v * m_nW + u] = val;
      u++;
      if ((v >= 0)&&(u >=0)&&(v < m_nH)&&(u < m_nW))
        area[v * m_nW + u] = val;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Recherche des noeuds min / max dans un vecteur avec calcul de la moyenne
//-----------------------------------------------------------------------------
bool XGeoFDtm::FindMinMax(XGeoVector* V, XPt3D* Pmin, XPt3D* Pmax, double* zmean, uint32_t* nbNoeud)
{
  if (!m_Frame.Intersect(V->Frame()))
    return false;
  // Lecture des donnees
  if (!StreamReady())
    return false;
  float* line = new float[m_nW];
  if (line == NULL)
    return false;
  // Analyse de chaque noeud
  uint32_t nb = 0;
  XPt2D P;
  double z = 0., zcumul = 0.;
  if (!V->LoadGeom2D()) {
    delete[] line;
    return false;
  }
  for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
    for (uint32_t j = 0; j < m_nW; j++) {
      if (line[j] <= m_dNoData)
        continue;
      z = line[j];
      P.X = m_Frame.Xmin + j * m_dGSD;
      P.Y = m_Frame.Ymax - i * m_dGSD;
      if (!V->IsIn2D(P))
        continue;
      if (nb == 0) {
        *Pmin = XPt3D(P.X, P.Y, z);
        *Pmax = XPt3D(P.X, P.Y, z);
      }
      nb++;
      if (z < Pmin->Z)
        *Pmin = XPt3D(P.X, P.Y, z);
      if (z > Pmax->Z)
        *Pmax = XPt3D(P.X, P.Y, z);
      zcumul += z;
    }
  }
  *nbNoeud = nb;
  if (nb > 0)
    *zmean = zcumul / nb;
  else
    *zmean = 0;
  delete[] line;
  V->Unload();
  return true;
}

//-----------------------------------------------------------------------------
// Recherche des noeuds min / max dans un vecteur
//-----------------------------------------------------------------------------
bool XGeoFDtm::FindMinMax(XGeoVector* V, XPt3D* Pmin, XPt3D* Pmax)
{
  if (!m_Frame.Intersect(V->Frame()))
    return false;
  // Lecture des donnees
  if (!StreamReady())
    return false;
  float* line = new float[m_nW];
  if (line == NULL)
    return false;
  // Analyse de chaque noeud
  uint32_t nb = 0;
  XPt2D P;
  double z = 0.;
  if (!V->LoadGeom2D()) {
    delete[] line;
    return false;
  }
  for (uint32_t i = 0; i < m_nH; i++) {
    ReadLine(line, i);
    for (uint32_t j = 0; j < m_nW; j++) {
      if (line[j] <= m_dNoData)
        continue;
      z = line[j];
      if (nb > 0) {
        if ((z > Pmin->Z)&&(z < Pmax->Z)) // Point non interessant
          continue;
      }
      P.X = m_Frame.Xmin + j * m_dGSD;
      P.Y = m_Frame.Ymax - i * m_dGSD;
      if (!V->IsIn2D(P))  // Point non inclus dans le vecteur
        continue;
      if (nb == 0) {
        *Pmin = XPt3D(P.X, P.Y, z);
        *Pmax = XPt3D(P.X, P.Y, z);
      }
      nb++;
      if (z < Pmin->Z)
        *Pmin = XPt3D(P.X, P.Y, z);
      if (z > Pmax->Z)
        *Pmax = XPt3D(P.X, P.Y, z);
    }
  }
  delete[] line;
  V->Unload();
  if (nb < 1)
    return false;
  return true;
}
