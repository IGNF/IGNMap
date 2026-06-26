//-----------------------------------------------------------------------------
//								XLasFile.cpp
//								============
//
// Traitement sur les fichiers LAS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/01/2024
//-----------------------------------------------------------------------------

#include <cstring>
#include <algorithm>
#include "XLasFile.h"
#include "../XTool/XFrame.h"
#include "../XToolImage/XTiffWriter.h"

int XLasFile::m_LasNbOpenFile = 0;

//==============================================================================
// Constructeur
//==============================================================================
XLasFile::XLasFile()
{ 
  m_Reader = nullptr; 
  m_Header = nullptr; 
  m_Point = nullptr; 
  m_dXmin = m_dXmax = m_dYmin = m_dYmax = m_dZmin = m_dZmax = 0.;
  m_nNbPoint = m_nIndex = 0;
  m_bCopc = false;
  m_dWorldGsd = 0.;
}

//==============================================================================
// Ouverture d'un fichier LAS
//==============================================================================
bool XLasFile::Open(std::string filename)
{
	m_strFilename = "";
	if (laszip_create(&m_Reader))
		return false;
	laszip_BOOL compress = 0;
	if (laszip_open_reader(m_Reader, filename.c_str(), &compress)) {
		laszip_destroy(m_Reader);
    m_Reader = nullptr;
		return false;
	}
	if (laszip_get_header_pointer(m_Reader, &m_Header)) {
    Close();
		return false;
	}
	if (laszip_get_point_pointer(m_Reader, &m_Point)) {
    Close();
		return false;
	}

	m_strFilename = filename;
  m_LasNbOpenFile++;
  m_bCopc = IsCopc();
	return true;
}

//==============================================================================
// Reouverture d'un fichier LAS
//==============================================================================
bool XLasFile::ReOpen()
{
  if (m_strFilename.empty())  // Le fichier n'a jamais ete ouvert
    return false;
  if (m_Reader != nullptr)    // Le fichier est deja ouvert
    return true;
  return Open(m_strFilename);
}

//==============================================================================
// Fermerture d'un fichier LAS
//==============================================================================
bool XLasFile::Close()
{
	if (m_Reader != nullptr) {
		laszip_close_reader(m_Reader);
		laszip_destroy(m_Reader);
	}
  m_Reader = nullptr;
  m_Header = nullptr;
  m_Point = nullptr;
  m_LasNbOpenFile--;
	return true;
}

//==============================================================================
// Fermerture eventuel d'un fichier LAS
//==============================================================================
bool XLasFile::CloseIfNeeded(int maxLasFile)
{
  if (m_LasNbOpenFile < maxLasFile)
    return true;
  return Close();
}

//==============================================================================
// Emprise a traiter
//==============================================================================
bool XLasFile::SetWorld(const XFrame& F, const double& zmin, const double& zmax, const double& gsd)
{
  if ((m_Header == nullptr)||(m_Reader == nullptr)||(m_Point == nullptr))
    return false;
  m_WorldFrame = F;
  m_dWorldGsd = gsd;
  m_dXmin = (F.Xmin - m_Header->x_offset) / m_Header->x_scale_factor;
  m_dXmax = (F.Xmax - m_Header->x_offset) / m_Header->x_scale_factor;
  m_dYmin = (F.Ymin - m_Header->y_offset) / m_Header->y_scale_factor;
  m_dYmax = (F.Ymax - m_Header->y_offset) / m_Header->y_scale_factor;
  m_dZmin = (zmin - m_Header->z_offset) / m_Header->z_scale_factor;
  m_dZmax = (zmax - m_Header->z_offset) / m_Header->z_scale_factor;

  m_nNbPoint = NbLasPoints();
  m_nIndex = 0;
  if (m_bCopc) {
    m_CopcReader.m_nActiveEntry = 0;
    m_CopcReader.m_bStarted = false;
  } else
    laszip_seek_point(m_Reader, 0);
  return true;
}

//==============================================================================
// Recuperation du prochain point de l'emprise a traiter
//==============================================================================
bool XLasFile::GetNextPoint(double* X, double* Y, double* Z)
{
  if (!m_bCopc) {
    do {
      laszip_read_point(m_Reader);
      m_nIndex++;
      if ((m_nIndex >= m_nNbPoint)) break;
      if (m_Point->X <= m_dXmin) continue;
      if (m_Point->X >= m_dXmax) continue;
      if (m_Point->Y <= m_dYmin) continue;
      if (m_Point->Y >= m_dYmax) continue;
      if (m_Point->Z < m_dZmin) continue;
      if (m_Point->Z > m_dZmax) continue;
      *X = m_Point->X * m_Header->x_scale_factor + m_Header->x_offset;
      *Y = m_Point->Y * m_Header->y_scale_factor + m_Header->y_offset;
      *Z = m_Point->Z * m_Header->z_scale_factor + m_Header->z_offset;
      return true;
    } while (m_nIndex < m_nNbPoint);
    laszip_seek_point(m_Reader, 0);
    return false;
  }

  if (!m_CopcReader.m_bStarted) {
    while (m_CopcReader.m_nActiveEntry < m_CopcReader.m_Entries.size()) {
      CopcReader::Entry entry = m_CopcReader.m_Entries[m_CopcReader.m_nActiveEntry];
      if (m_CopcReader.m_dSpacing / pow(2, entry.key.level) < m_dWorldGsd) {
        m_CopcReader.m_bStarted = false;
        m_CopcReader.m_nActiveEntry++;
        return false; 
      }
      double cell_size = (m_CopcReader.m_dHalfSize * 2.) / pow(2, entry.key.level);
      XFrame F;
      F.Xmin = m_CopcReader.m_dXmin + entry.key.x * cell_size;
      F.Xmax = m_CopcReader.m_dXmin + (entry.key.x + 1) * cell_size;
      F.Ymin = m_CopcReader.m_dYmin + entry.key.y * cell_size;
      F.Ymax = m_CopcReader.m_dYmin + (entry.key.y + 1) * cell_size;
      if (!F.Intersect(m_WorldFrame)) {
        m_CopcReader.m_nActiveEntry++;
        continue;
      }
      m_CopcReader.m_bStarted = true;
      m_CopcReader.m_nIndex = 0;
      m_CopcReader.m_nIndexMax = entry.pointCount;
      laszip_seek_point(m_Reader, entry.offset);
      break;
    }
    if (!m_CopcReader.m_bStarted)
      return false;
  }
  while (m_CopcReader.m_nIndex < m_CopcReader.m_nIndexMax) {
    laszip_read_point(m_Reader);
    m_CopcReader.m_nIndex++;
    if (m_Point->X <= m_dXmin) continue;
    if (m_Point->X >= m_dXmax) continue;
    if (m_Point->Y <= m_dYmin) continue;
    if (m_Point->Y >= m_dYmax) continue;
    if (m_Point->Z < m_dZmin) continue;
    if (m_Point->Z > m_dZmax) continue;
    *X = m_Point->X * m_Header->x_scale_factor + m_Header->x_offset;
    *Y = m_Point->Y * m_Header->y_scale_factor + m_Header->y_offset;
    *Z = m_Point->Z * m_Header->z_scale_factor + m_Header->z_offset;
    return true;
  }
  m_CopcReader.m_bStarted = false;
  m_CopcReader.m_nActiveEntry++;
  if (m_CopcReader.m_nActiveEntry >= m_CopcReader.m_Entries.size())
    return false;
  return GetNextPoint(X, Y, Z);
}

//-----------------------------------------------------------------------------
// Calcul d'un MNT/MNS a parti d'un fichier LAS
//-----------------------------------------------------------------------------
bool XLasFile::ComputeDtm(std::string file_out, double gsd, AlgoDtm algo, bool classif_visibility[256], bool fill_hole, uint16_t epsg, XError* error)
{
	if (!ReOpen())	// Le fichier LAS n'a pas ete ouvert
		return false;

	XFrame F = XFrame(m_Header->min_x, m_Header->min_y, m_Header->max_x, m_Header->max_y);
	F.Xmin = gsd * floor(F.Xmin / gsd);
	F.Ymin = gsd * floor(F.Ymin / gsd);
	F.Xmax = gsd * ceil(F.Xmax / gsd);
	F.Ymax = gsd * ceil(F.Ymax / gsd);

	uint32_t W = (uint32_t)ceil(F.Width() / gsd);
	uint32_t H = (uint32_t)ceil(F.Height() / gsd);

	// Allocation du tableau MNT
	float* area = new float[W * H];
	if (area == nullptr)
		return XErrorError(error, "TLasFile::ComputeDtm", XError::eAllocation);
	::memset(area, 0, W * H * sizeof(float));
	uint16_t* count = new uint16_t[W * H];
	if (count == nullptr) {
		delete[] area;
		return XErrorError(error, "TLasFile::ComputeDtm", XError::eAllocation);
	}
	::memset(count, 0, W * H * sizeof(uint16_t));
  InlineStat* Stat = nullptr;
  if ((algo == StdDev)||(algo == Height)) {
    Stat = new InlineStat[W * H];
    if (Stat == nullptr) {
      delete[] area; delete[] count;
      return XErrorError(error, "TLasFile::ComputeDtm", XError::eAllocation);
    }
  }

	double X = 0., Y = 0., Z = 0.;
	int u = 0, v = 0;
	laszip_seek_point(m_Reader, 0);
	for (uint64_t i = 0; i < NbLasPoints(); i++) {  // Debut de l'analyse des points LAS
		laszip_read_point(m_Reader);
		if (classif_visibility != nullptr)
			if (!classif_visibility[m_Point->classification])
				continue;
		
		X = m_Point->X * m_Header->x_scale_factor + m_Header->x_offset;
		Y = m_Point->Y * m_Header->y_scale_factor + m_Header->y_offset;
		Z = m_Point->Z * m_Header->z_scale_factor + m_Header->z_offset;

		u = (int)XRint((X - F.Xmin) / gsd);
		v = (int)XRint((F.Ymax - Y) / gsd);
		if (((uint32_t)u >= W) || ((uint32_t)v >= H))	// Theoriquement cela ne devrait pas arrive si l'entete du LAS est correcte
			continue;
		if ((u < 0) || (v < 0))
			continue;

		if (count[v * W + u] == 0) {	// Premier Z dans la cellule
			area[v * W + u] = (float)Z;
			count[v * W + u] += 1;
      if (Stat != nullptr) Stat[v * W + u].AddValue(Z);
			continue;
		}

		switch (algo) {
			case ZAverage: area[v * W + u] += (float)Z;
				break;
			case ZMinimum: if (area[v * W + u] > Z) area[v * W + u] = (float)Z;
				break;
			case ZMaximum: if (area[v * W + u] < Z) area[v * W + u] = (float)Z;
				break;
      case StdDev: Stat[v * W + u].AddValue(Z);
        break;
      case Height: Stat[v * W + u].AddValue(Z);
        break;
      default:;
			}
		count[v * W + u] += 1;
	} // Fin de l'analyse des points LAS

  if (algo != Density) {
    uint32_t countZ = 0;
    for (uint32_t i = 0; i < W * H; i++) {
      if (count[i] > 0) {
        countZ++;
        if (algo == ZAverage)
          area[i] = area[i] / count[i];
        if (algo == StdDev)
          area[i] = (float)Stat[i].StandardDeviation();
        if (algo == Height)
          area[i] = (float)(Stat[i].Max() - Stat[i].Min());
      }
      else
        area[i] = -9999;
    }

    if ((fill_hole) && (countZ < W * H)) {  // Bouchage des trous en NOZ
      FillHole(area, count, W, H);
    }
  }
  else {
    for (uint32_t i = 0; i < W * H; i++)
      area[i] = (float)count[i];
  }

	XTiffWriter writer;
	writer.SetGeoTiff(F.Xmin, F.Ymax, gsd, epsg);
	writer.Write(file_out.c_str(), W, H, 1, 32, (uint8_t*)area, 3);

	delete[] area;
	delete[] count;
  if (Stat != nullptr) delete[] Stat;
  CloseIfNeeded();

	return true;
}

//-----------------------------------------------------------------------------
// Remplissage des trous d'un MNT
//-----------------------------------------------------------------------------
bool XLasFile::FillHole(float* area, uint16_t* count, const uint32_t& W, const uint32_t& H)
{
  uint32_t win = W / 2, hin = H / 2;
  if (win < 1) win = 1;
  if (hin < 1) hin = 1;

  float* new_area = new float[win * hin];
  uint16_t* new_count = new uint16_t[win * hin];
  for (uint32_t i = 0; i < hin; i++) {
    for (uint32_t j = 0; j < win; j++) {
      new_area[i * win + j] = 0;
      new_count[i * win + j] = 0;
      if (count[2 * i * W + 2 * j] > 0) {
        new_area[i * win + j] += area[2 * i * W + 2 * j];
        new_count[i * win + j]++;
      }
      if (count[2 * i * W + 2 * j + 1] > 0) {
        new_area[i * win + j] += area[2 * i * W + 2 * j + 1];
        new_count[i * win + j]++;
      }
      if (count[(2 * i + 1) * W + 2 * j] > 0) {
        new_area[i * win + j] += area[(2 * i + 1) * W + 2 * j];
        new_count[i * win + j]++;
      }
      if (count[(2 * i + 1) * W + 2 * j + 1] > 0) {
        new_area[i * win + j] += area[(2 * i + 1) * W + 2 * j + 1];
        new_count[i * win + j]++;
      }
    }
  }
  uint32_t nbNoZ = 0;
  for (uint32_t i = 0; i < win * hin; i++) {
    if (new_count[i] > 0)
      new_area[i] /= new_count[i];
    else
      nbNoZ++;
  }
  if (nbNoZ > 0)
    FillHole(new_area, new_count, win, hin);
  for (uint32_t i = 0; i < hin; i++) {
    for (uint32_t j = 0; j < win; j++) {
      if (count[2 * i * W + 2 * j] == 0)
        area[2 * i * W + 2 * j] = new_area[i * win + j];
       
      if (count[2 * i * W + 2 * j + 1] == 0) 
        area[2 * i * W + 2 * j + 1] = new_area[i * win + j];
        
      if (count[(2 * i + 1) * W + 2 * j] == 0)
        area[(2 * i + 1) * W + 2 * j] = new_area[i * win + j];
        
      if (count[(2 * i + 1) * W + 2 * j + 1] == 0) 
        area[(2 * i + 1) * W + 2 * j + 1] = new_area[i * win + j];
    }
  }
  
  delete[] new_count;
  delete[] new_area;
  return true;
}

//-----------------------------------------------------------------------------
// Calcul des statistiques
//-----------------------------------------------------------------------------
bool XLasFile::StatLas(std::string file_out, std::ofstream* mif, std::ofstream* mid)
{
  if (!ReOpen())	// Le fichier LAS n'a pas ete ouvert
    return false;

  std::ofstream out;
  out.open(file_out);
  if (!out.good())
    return false;
  out.setf(std::ios::fixed);
  out.precision(3);
  out << "Statistiques fichier : " << m_strFilename << std::endl;
  out << "===================================================================" << std::endl;
  out << std::endl << "Donnees d'entete : " << std::endl;
  out << "Software : " << m_Header->generating_software << std::endl;
  out << "SystemID : " << m_Header->system_identifier << std::endl;
  out << "Global encoding : " << m_Header->global_encoding << std::endl;
  out << "Version : " << (int)m_Header->version_major << "." << (int)m_Header->version_minor << std::endl;
  out << "Format des points : " << (int)m_Header->point_data_format << std::endl;
  out << "Date : " << m_Header->file_creation_day << "/" << m_Header->file_creation_year << std::endl;
  out << "XMin entete : " << m_Header->min_x << std::endl;
  out << "XMax entete : " << m_Header->max_x << std::endl;
  out << "YMin entete : " << m_Header->min_y << std::endl;
  out << "YMax entete : " << m_Header->max_y << std::endl;
  out << "ZMin entete : " << m_Header->min_z << std::endl;
  out << "ZMax entete : " << m_Header->max_z << std::endl;

  laszip_seek_point(m_Reader, 0);
  laszip_I64 count = 0;
  double X, Y, Z, zmin = 0., zmax = 0.;
  laszip_F64 gps_time_min = 0., gps_time_max = 0., gps_time;
  laszip_U8 classification;
  unsigned int TClassif[256];
  memset(TClassif, 0, 256 * sizeof(unsigned int));
  double Xmin = 0., Xmax = 0., Ymin = 0., Ymax = 0.;
  for (uint64_t i = 0; i < NbLasPoints(); i++) {
    laszip_read_point(m_Reader);
    X = m_Point->X * m_Header->x_scale_factor + m_Header->x_offset;
    Y = m_Point->Y * m_Header->y_scale_factor + m_Header->y_offset;
    Z = m_Point->Z * m_Header->z_scale_factor + m_Header->z_offset;
    classification = m_Point->classification;
    if (m_Header->version_minor >= 4)
      classification = m_Point->extended_classification;
    gps_time = m_Point->gps_time;

    if (count == 0) { // Debut d'un nouveau lot
      Xmin = Xmax = X;
      Ymin = Ymax = Y;
      zmin = zmax = Z;
      gps_time_min = gps_time_max = gps_time;
      TClassif[classification]++;
      count++;
      continue;
    }

    Xmin = XMin(Xmin, X);
    Xmax = XMax(Xmax, X);
    Ymin = XMin(Ymin, Y);
    Ymax = XMax(Ymax, Y);
    zmin = XMin(zmin, Z);
    zmax = XMax(zmax, Z);
    gps_time_min = XMin(gps_time_min, gps_time);
    gps_time_max = XMax(gps_time_max, gps_time);
    TClassif[classification]++;
    count++;
  }

  out << std::endl;
  out << "Lecture du fichier : " << m_strFilename << std::endl;
  out << "===================================================================" << std::endl;
  out << "Nombre de points dans le fichier : " << NbLasPoints() << std::endl;
  out << "Nombre de points lus: " << count << std::endl;

  out << "===================================================================" << std::endl;
  out << "Xmin = " << Xmin << std::endl;
  out << "Xmax = " << Xmax << std::endl;
  out << "Ymin = " << Ymin << std::endl;
  out << "Ymax = " << Ymax << std::endl;
  out << "Zmin = " << zmin << std::endl;
  out << "Zmax = " << zmax << std::endl;
  out << "GPS Time Min = " << gps_time_min << std::endl;
  out << "GPS Time Max = " << gps_time_max << std::endl;

  out << "===================================================================" << std::endl;
  out << "Classifications : " << std::endl;
  out << "0 : Created, Never Classified : " << TClassif[0] << std::endl;
  out << "1 : Unclassified : " << TClassif[1] << std::endl;
  out << "2 : Ground : " << TClassif[2] << std::endl;
  out << "3 : Low Vegetation : " << TClassif[3] << std::endl;
  out << "4 : Medium Vegetation : " << TClassif[4] << std::endl;
  out << "5 : High Vegetation : " << TClassif[5] << std::endl;
  out << "6 : Building : " << TClassif[6] << std::endl;
  out << "7 : Low Point (Noise) : " << TClassif[7] << std::endl;
  out << "8 : Reserved : " << TClassif[8] << std::endl;
  out << "9 : Water : " << TClassif[9] << std::endl;
  out << "10 : Rail : " << TClassif[10] << std::endl;
  out << "11 : Road Surface : " << TClassif[11] << std::endl;
  out << "12 : " << TClassif[12] << std::endl;
  out << "13 : Wire - Guard (Shield) : " << TClassif[13] << std::endl;
  out << "14 : Wire - Conductor (Phase) : " << TClassif[14] << std::endl;
  out << "15 : Transmission Tower : " << TClassif[15] << std::endl;
  out << "16 : Wire-Structure Connector : " << TClassif[16] << std::endl;
  out << "17 : Bridge Deck : " << TClassif[17] << std::endl;
  out << "18 : High Noise : " << TClassif[18] << std::endl;
  out << "19 : Overhead Structure : " << TClassif[19] << std::endl;
  out << "20 : Ignored Ground : " << TClassif[20] << std::endl;
  out << "21 : Snow : " << TClassif[21] << std::endl;
  out << "22 : Temporal Exclusion : " << TClassif[22] << std::endl;

  out << "===================================================================" << std::endl;
  out << "Classifications etendues : " << std::endl;
  for (int i = 23; i < 256; i++) {
    if (TClassif[i] > 0)
      out << i << " : " << TClassif[i] << std::endl;
  }

  if ((mif == nullptr) || (mid == nullptr))
    return true;

  *mif << "REGION 1" << std::endl;
  *mif << "5" << std::endl;
  *mif << Xmin << " " << Ymin << std::endl;
  *mif << Xmin << " " << Ymax << std::endl;
  *mif << Xmax << " " << Ymax << std::endl;
  *mif << Xmax << " " << Ymin << std::endl;
  *mif << Xmin << " " << Ymin << std::endl;

  unsigned int classif_autre = TClassif[7] + TClassif[8];
  for (unsigned int i = 10; i < 17; i++)
    classif_autre += TClassif[i];
  for (unsigned int i = 18; i < 64; i++)
    classif_autre += TClassif[i];
  for (unsigned int i = 68; i < 256; i++)
    classif_autre += TClassif[i];
  std::string name = m_strFilename.substr(m_strFilename.rfind('\\') + 1);

  *mid << name << "\t" << zmin << "\t" << zmax << "\t"
    << TClassif[0] << "\t" << TClassif[1] << "\t" << TClassif[2] << "\t"
    << TClassif[3] << "\t" << TClassif[4] << "\t" << TClassif[5] << "\t"
    << TClassif[6] << "\t" << TClassif[9] << "\t" << TClassif[17] << "\t"
    << TClassif[64] << "\t" << TClassif[65] << "\t" << TClassif[66] << "\t"
    << TClassif[67] << "\t" << classif_autre
    << std::endl;

  CloseIfNeeded();
  return true;
}

//-----------------------------------------------------------------------------
// Gestion du COPC
//-----------------------------------------------------------------------------
bool XLasFile::IsCopc()
{
  if (m_Header->number_of_extended_variable_length_records < 1)
    return false;
  if (m_Header->vlrs[0].record_id != 1)
    return false;
  if (m_CopcReader.MaxLevel() >= 0) // Le CopcReader a deja ete lu
    return true;
  return m_CopcReader.SetInfo(m_Header->vlrs[0].data, m_strFilename);
}

//-----------------------------------------------------------------------------
// Predicats pour le tri des Entry
//-----------------------------------------------------------------------------
bool PredEntriesOffset(CopcReader::Entry A, CopcReader::Entry B) { if (A.offset < B.offset) return true; return false; }
bool PredEntriesDepth(CopcReader::Entry A, CopcReader::Entry B)
{
  if (A.key.level < B.key.level) return true;
  if (A.key.level > B.key.level) return false;
  if (A.key.x < B.key.x) return true;
  if (A.key.x > B.key.x) return false;
  if (A.key.y < B.key.y) return true;
  if (A.key.y > B.key.y) return false;
  if (A.key.z < B.key.z) return true;
  if (A.key.z > B.key.z) return false;
  return false;
}

//-----------------------------------------------------------------------------
// Lecteure des informations COPC
//-----------------------------------------------------------------------------
bool CopcReader::SetInfo(laszip_U8* data, std::string filename)
{
  CopcInfo* info = (CopcInfo*)data;
  int nb_entries = (int)(info->root_hier_size / 32);
  if (nb_entries < 1)
    return false;
  m_dSpacing = info->spacing;
  m_dHalfSize = info->halfsize;
  m_dXmin = info->center_x - info->halfsize;
  m_dYmin = info->center_y - info->halfsize;
  m_dGpsTimeMin = info->gpstime_minimum;
  m_dGpsTimeMax = info->gpstime_maximum;
  Entry* entries = new Entry[nb_entries];
  std::ifstream in;
  in.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!in.good())
    return false;
  in.seekg(info->root_hier_offset);
  in.read((char*)entries, info->root_hier_size);

  ReadSubPages(&in, entries, nb_entries);
  delete[] entries;
  // Passage des offsets fichier en numero d'ordre pour les points LAS
  std::sort(m_Entries.begin(), m_Entries.end(), PredEntriesOffset);
  uint64_t count = 0;
  for (int i = 0; i < m_Entries.size(); i++) {
    m_Entries[i].offset = i + count;
    count += m_Entries[i].pointCount;
  }
  // Passage en ordre de profondeur
  std::sort(m_Entries.begin(), m_Entries.end(), PredEntriesDepth);

  return true;
}

//-----------------------------------------------------------------------------
// Lecture des Hierarchy pages
//-----------------------------------------------------------------------------
bool CopcReader::ReadSubPages(std::ifstream* in, Entry* entries, int nb_entries)
{
  for (int i = 0; i < nb_entries; i++) {
    if (entries[i].pointCount > 0) {
      m_Entries.push_back(entries[i]);
      continue;
    }
    if (entries[i].pointCount == 0)
      continue;
    int nb_NewEntries = entries[i].byteSize / 32;
    Entry* newEntries = new Entry[nb_NewEntries];
    in->seekg(entries[i].offset);
    in->read((char*)newEntries, entries[i].byteSize);
    ReadSubPages(in, newEntries, nb_NewEntries);
    delete[] newEntries;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Export d'un tableau de points avec une classification unique
//-----------------------------------------------------------------------------
bool XLasFile::Export(std::string file_out, const std::vector<XPt3D> T, uint8_t classif, int projCode, bool compression,
                      uint8_t red, uint8_t green, uint8_t blue)
{
  laszip_POINTER	las_Writer = nullptr;
  laszip_header* las_Header = nullptr;
  laszip_point* las_Point = nullptr;
  laszip_I64			count = 0;

  if (laszip_create(&las_Writer))  // Impossible de creer le Writer
    return false;
  if (laszip_get_header_pointer(las_Writer, &las_Header))  // Impossible de recuperer l'entete
    return false;

  // Donnees d'entete
  las_Header->file_source_ID = 0;
  las_Header->global_encoding = (1 << 0) | (1 << 4);     // see LAS specification for details
  las_Header->version_major = 1;
  las_Header->version_minor = 4;
  strncpy(las_Header->system_identifier, "Export LAS IGNMap", 32);
  strncpy(las_Header->generating_software, "IGNMap v3", 32);
  las_Header->file_creation_day = (laszip_U16)1;
  las_Header->file_creation_year = (laszip_U16)2026;
  las_Header->header_size = 375;
  las_Header->offset_to_point_data = 375;
  las_Header->point_data_format = 7;
  las_Header->point_data_record_length = 30;
  las_Header->number_of_point_records = 0;           // legacy 32-bit counters should be zero for new point types > 5
  for (int i = 0; i < 5; i++) {
    las_Header->number_of_points_by_return[i] = 0;   // legacy 32-bit counters should be zero for new point types > 5
  }
  las_Header->extended_number_of_point_records = 0;  // a-priori unknown number of points
  for (int i = 0; i < 15; i++) {
    las_Header->extended_number_of_points_by_return[i] = 0;
  }
  las_Header->max_x = 0.0;                           // a-priori unknown bounding box
  las_Header->min_x = 0.0;
  las_Header->max_y = 0.0;
  las_Header->min_y = 0.0;
  las_Header->max_z = 0.0;
  las_Header->min_z = 0.0;

  // Ajout de la projection
  laszip_geokey_struct key_entries[5];

  // projected coordinates
  key_entries[0].key_id = 1024; // GTModelTypeGeoKey
  key_entries[0].tiff_tag_location = 0;
  key_entries[0].count = 1;
  key_entries[0].value_offset = 1; // ModelTypeProjected

  // projection
  key_entries[1].key_id = 3072; // ProjectedCSTypeGeoKey
  key_entries[1].tiff_tag_location = 0;
  key_entries[1].count = 1;
  key_entries[1].value_offset = projCode; // Code EPSG

  // horizontal units
  key_entries[2].key_id = 3076; // ProjLinearUnitsGeoKey
  key_entries[2].tiff_tag_location = 0;
  key_entries[2].count = 1;
  key_entries[2].value_offset = 9001; // meters

  // vertical units
  key_entries[3].key_id = 4099; // VerticalUnitsGeoKey
  key_entries[3].tiff_tag_location = 0;
  key_entries[3].count = 1;
  key_entries[3].value_offset = 9001; // meters

  // vertical datum
  key_entries[4].key_id = 4096; // VerticalCSTypeGeoKey
  key_entries[4].tiff_tag_location = 0;
  key_entries[4].count = 1;
  key_entries[4].value_offset = 5720; // IGN69

  // add the geokeys (create or replace the appropriate VLR)
  if (laszip_set_geokeys(las_Writer, 5, key_entries))
    return false;

  laszip_BOOL compress = 0;
  if (compression) compress = 1;

  laszip_preserve_generating_software(las_Writer, 1);
  if (laszip_open_writer(las_Writer, file_out.c_str(), compress)) // Ouverture du Writer
    return false;

  // Pointeur pour ecrire les points
  if (laszip_get_point_pointer(las_Writer, &las_Point))
    return false;

  // Ecriture des points
  laszip_F64 coordinates[3];
  for (size_t i = 0; i < T.size(); i++) {
    if (T[i].isNull())
      continue;
    coordinates[0] = T[i].X;
    coordinates[1] = T[i].Y;
    coordinates[2] = T[i].Z;
    if (laszip_set_coordinates(las_Writer, coordinates))
      return false;

    las_Point->intensity = 0;
    las_Point->extended_return_number = 1;
    las_Point->extended_number_of_returns = 1;
    las_Point->classification = classif;                // it must be set because it "fits" in 5 bits
    las_Point->extended_classification = classif;
    las_Point->extended_scan_angle = 0;
    las_Point->extended_scanner_channel = 0;
    las_Point->extended_classification_flags = 1; // overflag flag is set
    las_Point->gps_time = 0;
    las_Point->synthetic_flag = 1;
    las_Point->rgb[0] = red * 256;
    las_Point->rgb[1] = green * 256;
    las_Point->rgb[2] = blue * 256;

    if (laszip_write_point(las_Writer))
      return false;
    if (laszip_update_inventory(las_Writer))
      return false;
    count++;
  }

  // Fermeture du Writer
  if (laszip_get_point_count(las_Writer, &count))
    return false;

  if (laszip_close_writer(las_Writer))
    return false;

  if (laszip_destroy(las_Writer))
    return false;

  return true;
}
