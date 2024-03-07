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
#include "../XTool/XBase.h"
#include "../LASzip/src/laszip_api.h"

class XLasFile {
public:
	XLasFile() { m_Reader = nullptr; m_Header = nullptr; m_Point = nullptr; }
	~XLasFile() { Close(); }

	typedef enum { ZMinimum = 1, ZAverage = 2, ZMaximum = 3 } AlgoDtm;

	bool Open(std::string filename);
	bool ReOpen();
	bool Close();
	bool CloseIfNeeded(int maxLasFile = 10);
	laszip_POINTER GetReader() { return m_Reader; }
	laszip_header* GetHeader() { return m_Header; }
	laszip_point* GetPoint() { return m_Point; }
	laszip_I64 NbLasPoints() {
		if (m_Header == nullptr) return 0;
		return (m_Header->number_of_point_records ? m_Header->number_of_point_records : m_Header->extended_number_of_point_records);
	}

	bool ComputeDtm(std::string file_out, double gsd, AlgoDtm algo = ZMinimum, bool classif_visibility[256] = nullptr, XError* error = nullptr);
	bool StatLas(std::string file_out, std::ofstream* mif = nullptr, std::ofstream* mid = nullptr);

protected:
	std::string m_strFilename;
	laszip_POINTER m_Reader;
	laszip_header* m_Header;
	laszip_point* m_Point;

	static int m_LasNbOpenFile;		// Nombre de fichiers LAS ouverts
};

#endif //XLASFILE_H