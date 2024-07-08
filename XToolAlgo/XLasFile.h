//-----------------------------------------------------------------------------
//								XLasFile.h
//								==========
//
// Traitement sur les fichiers LAS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/01/2024
//-----------------------------------------------------------------------------

#ifndef XLASFILE_H
#define XLASFILE_H

#include <string>
#include <vector>
#include "../XTool/XBase.h"
#include "../XTool/XFrame.h"
#include "../LASzip/dll/laszip_api.h"

class CopcReader {
public:
	struct CopcInfo {
		double center_x; // Actual (unscaled) X coordinate of center of octree
		double center_y; // Actual (unscaled) Y coordinate of center of octree
		double center_z; // Actual (unscaled) Z coordinate of center of octree
		double halfsize; // Perpendicular distance from the center to any side of the root node.
		double spacing; // Space between points at the root node. This value is halved at each octree level
		uint64_t root_hier_offset; // File offset to the first hierarchy page
		uint64_t root_hier_size; // Size of the first hierarchy page in bytes
		double gpstime_minimum; // Minimum of GPSTime
		double gpstime_maximum; // Maximum of GPSTime
		uint64_t reserved[11]; // Must be 0
	};
	struct VoxelKey {
		int32_t level; // A value < 0 indicates an invalid VoxelKey
		int32_t x;
		int32_t y;
		int32_t z;
	};
	struct Entry {
		VoxelKey key;
		// Absolute offset to the data chunk if the pointCount > 0.
		// Absolute offset to a child hierarchy page if the pointCount is -1.
		// 0 if the pointCount is 0.
		uint64_t offset;
		// Size of the data chunk in bytes (compressed size) if the pointCount > 0.
		// Size of the hierarchy page if the pointCount is -1.
		// 0 if the pointCount is 0.
		int32_t byteSize;
		// If > 0, represents the number of points in the data chunk.
		// If -1, indicates the information for this octree node is found in another hierarchy page.
		// If 0, no point data exists for this key, though may exist for child entries.
		int32_t pointCount;
	};

	CopcReader() { m_dSpacing = m_dHalfSize = 0.; m_nActiveEntry = 0; m_nIndex = m_nIndexMax = 0; m_bStarted = false; m_dXmin = m_dYmin = 0.; }
	bool SetInfo(laszip_U8* data, std::string filename);
	bool ReadSubPages(std::ifstream* in, Entry* entries, int nb_entries);
	int MaxLevel() { if (m_Entries.size() == 0) return -1; return m_Entries[m_Entries.size() - 1].key.level; }

	std::vector<Entry> m_Entries;
	double m_dSpacing;
	double m_dHalfSize;
	double m_dXmin, m_dYmin;
	int m_nActiveEntry;
	int32_t m_nIndex, m_nIndexMax;
	bool m_bStarted;
};

class XLasFile {
public:
	XLasFile();
	~XLasFile() { Close(); }

	typedef enum { ZMinimum = 1, ZAverage = 2, ZMaximum = 3 } AlgoDtm;

	bool Open(std::string filename);
	bool ReOpen();
	bool Close();
	bool CloseIfNeeded(int maxLasFile = 10);
	bool IsNewClassification() { if (m_Header == nullptr) return false; if (m_Header->version_minor < 4) return false; return true; }
	laszip_POINTER GetReader() { return m_Reader; }
	laszip_header* GetHeader() { return m_Header; }
	laszip_point* GetPoint() { return m_Point; }
	uint64_t NbLasPoints() {
		if (m_Header == nullptr) return 0;
		return (m_Header->number_of_point_records ? m_Header->number_of_point_records : m_Header->extended_number_of_point_records);
	}

	bool SetWorld(const XFrame& F, const double& zmin, const double& zmax, const double& gsd = 0.);
	bool GetNextPoint(double* X, double* Y, double* Z);

	bool ComputeDtm(std::string file_out, double gsd, AlgoDtm algo = ZMinimum, bool classif_visibility[256] = nullptr, XError* error = nullptr);
	bool StatLas(std::string file_out, std::ofstream* mif = nullptr, std::ofstream* mid = nullptr);

protected:
	std::string m_strFilename;
	laszip_POINTER m_Reader;
	laszip_header* m_Header;
	laszip_point* m_Point;

	double m_dXmin, m_dXmax, m_dYmin, m_dYmax, m_dZmin, m_dZmax;
	uint64_t m_nNbPoint, m_nIndex;
	XFrame m_WorldFrame;
	double m_dWorldGsd;
	CopcReader m_CopcReader;
	bool m_bCopc;

	static int m_LasNbOpenFile;		// Nombre de fichiers LAS ouverts

	bool IsCopc();
};



#endif //XLASFILE_H