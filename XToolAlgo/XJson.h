//-----------------------------------------------------------------------------
//								XJson.h
//								=======
//
// Traitement sur les fichiers JSON
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 09/11/2026
//-----------------------------------------------------------------------------

#ifndef XJSON_H
#define XJSON_H

#include <string>
#include <vector>
#include <sstream>

namespace XJson {
	std::string ReadStringToken(std::istream* in);
	bool ReadKeyValue(std::istream* in, std::vector<std::string>& keys, std::vector<std::string>& values);
	bool ReadKeyValue(std::string source, std::vector<std::string>& keys, std::vector<std::string>& values);

	std::string FindValue(std::vector<std::string> find_keys, const std::string& source);
	int ReadArrayAsString(const std::string& source, std::vector<std::string>& T);
	int ReadArrayAsDouble(const std::string& source, std::vector<double>& T);
}

#endif //XJSON_H