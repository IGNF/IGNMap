//-----------------------------------------------------------------------------
//								XXml.cpp
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 21/08/00
//-----------------------------------------------------------------------------

#include "XXml.h"

//-----------------------------------------------------------------------------
// Ecriture de l'entete d'un fichier XML
//-----------------------------------------------------------------------------
bool XXml::WriteXMLHeader(std::ostream* out, std::string xslFile)
{
	*out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << std::endl;
	*out << "<?xml-stylesheet type=\"text/xsl\" href=\"" << xslFile<< "\"?>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Conversion d'une chaine en chaine compatible avec XML
//-----------------------------------------------------------------------------
std::string XXml::OemToXml(std::string s)
{
	std::string result = "";
	char newStr[10];
	for (uint32_t i = 0; i < s.length(); i++) {
		if (((uint8_t)s[i] < 160) && (s[i] != '<') && (s[i] != '>'))
			result += s[i];
		else {
			snprintf(newStr, 10, "&#%d;", (uint8_t)s[i]);
			result += newStr;
		}
	}
	return result;
}

//-----------------------------------------------------------------------------
// Conversion d'une chaine XML en chaine compatible normale
//-----------------------------------------------------------------------------
std::string XXml::XmlToOem(std::string s)
{
	std::string result = "", numstr;
	int num;
	for (uint32_t i = 0; i < s.length(); i++) {
		if ((s[i] != '&')||(i >= s.length() - 3))
			result += s[i];
		else {
			if (s[i+1] != '#')
				continue;
			numstr = s.substr(i+2);
			int pos = numstr.find(';');
			if (pos < 0)
				continue;
			numstr = numstr.substr(0, pos);
			
			sscanf(numstr.c_str(), "%d", &num);
			result += (char)num;
			i += (pos + 2);
		}
	}
	return result;
}

//-----------------------------------------------------------------------------
// Creation d'un chemin relatif a partir d'un chemin absolu
//-----------------------------------------------------------------------------
std::string XXml::AbsToRelPath(std::string root, std::string path)
{
	std::string result;
	// Premier cas : le root est inclus dans le path
	if (path.find(root) == 0) {
		result = path.substr(root.size());
		return result;
	}
	return path;
}
