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

#include "XLasFile.h"
#include "../XTool/XFrame.h"
#include "../XToolImage/XTiffWriter.h"

//==============================================================================
// Ouverture d'un fichier LAS
//==============================================================================
bool XLasFile::Open(std::string filename)
{
	m_strFilename = "";
	if (laszip_create(&m_Reader))
		return false;
	laszip_BOOL compress;
	if (laszip_open_reader(m_Reader, filename.c_str(), &compress)) {
		laszip_destroy(m_Reader);
		return false;
	}
	if (laszip_get_header_pointer(m_Reader, &m_Header)) {
		laszip_destroy(m_Reader);
		return false;
	}
	if (laszip_get_point_pointer(m_Reader, &m_Point)) {
		laszip_destroy(m_Reader);
		return false;
	}

	m_strFilename = filename;

	return true;
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
	m_strFilename = "";
	return true;
}

//-----------------------------------------------------------------------------
// Calcul d'un MNT/MNS a parti d'un fichier LAS
//-----------------------------------------------------------------------------
bool XLasFile::ComputeDtm(std::string file_out, double gsd, AlgoDtm algo, XError* error)
{
	if (m_strFilename.empty())	// Le fichier LAS n'a pas ete ouvert
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

	double X = 0., Y = 0., Z = 0.;
	int u = 0, v = 0;
	laszip_seek_point(m_Reader, 0);
	for (laszip_I64 i = 0; i < NbLasPoints(); i++) {
		laszip_read_point(m_Reader);
		
		X = m_Point->X * m_Header->x_scale_factor + m_Header->x_offset;
		Y = m_Point->Y * m_Header->y_scale_factor + m_Header->y_offset;
		Z = m_Point->Z * m_Header->z_scale_factor + m_Header->z_offset;

		u = (int)XRint((X - F.Xmin) / gsd);
		v = (int)XRint((F.Ymax - Y) / gsd);
		if ((u >= W) || (v >= H))	// Theoriquement cela ne devrait pas arrive si l'entete du LAS est correcte
			continue;
		if ((u < 0) || (v < 0))
			continue;

		if (count[v * W + u] == 0) {	// Premier Z dans la cellule
			area[v * W + u] = Z;
			count[v * W + u] += 1;
			continue;
		}

		switch (algo) {
			case ZAverage: area[v * W + u] += Z;
				break;
			case ZMinimum: if (area[v * W + u] > Z) area[v * W + u] = Z;
				break;
			case ZMaximum: if (area[v * W + u] < Z) area[v * W + u] = Z;
				break;
			}
		count[v * W + u] += 1;
	}

	for (uint32_t i = 0; i < W * H; i++) {
		if (count[i] > 0) {
			if (algo == ZAverage)
				area[i] = area[i] / count[i];
		}
		else
			area[i] = -9999;
	}

	XTiffWriter writer;
	writer.SetGeoTiff(F.Xmin, F.Ymax, gsd);
	writer.Write(file_out.c_str(), W, H, 1, 32, (uint8_t*)area, 3);

	delete[] area;
	delete[] count;

	return true;
}