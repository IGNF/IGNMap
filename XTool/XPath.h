//-----------------------------------------------------------------------------
//								XPath.h
//								=======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 05/07/2001
//-----------------------------------------------------------------------------

#ifndef _XPATH_H
#define _XPATH_H

#include "XBase.h"
#include <string>

class XPath  {
protected:
	char				m_Sep;
public:
  static char gDefaultSep;
  XPath() { m_Sep = gDefaultSep; }
  XPath(char sep) { m_Sep = sep;}

	std::string Path(const char* filename);
	std::string Name(const char* filename);
	std::string Name(const char* filename, bool extension);
	std::string Ext(const char* filename);
	std::string PathName(const char* filename);
	std::string FullName(const char* folder, const char* filename);

	std::string Relative(const char* root, const char* path);
	std::string Absolute(const char* root, const char* path);

	static std::string ConvertWindows(const char* filename);
	static void SetDefaultSep(char c) { gDefaultSep = c; }
};

#endif //_XPATH_H
