//-----------------------------------------------------------------------------
//								XPath.cpp
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 05/07/2001
//-----------------------------------------------------------------------------

#include "XPath.h"
#include <vector>

// Separateur par defaut
#if defined(WIN32) || defined(_WIN64)
char XPath::gDefaultSep = '\\';
#else 
char XPath::gDefaultSep = '/';
#endif

//-----------------------------------------------------------------------------
// Renvoie le nom du repertoire parent
//-----------------------------------------------------------------------------
std::string XPath::Path(const char* filename)
{
	std::string P = filename;
	return P.substr(0, P.rfind(m_Sep));
}

//-----------------------------------------------------------------------------
// Renvoie le nom du fichier ou du repertoire (sans le chemin)
//-----------------------------------------------------------------------------
std::string XPath::Name(const char* filename)
{
	std::string P = filename;
	return P.substr(P.rfind(m_Sep)+1);
}

//-----------------------------------------------------------------------------
// Renvoie le nom du fichier (sans le chemin) avec ou sans l'extension
//-----------------------------------------------------------------------------
std::string XPath::Name(const char* filename, bool extension)
{
	if (extension)
		return Name(filename);
	std::string P = Name(filename);
	return P.substr(0, P.rfind('.'));
}

//-----------------------------------------------------------------------------
// Renvoie l'extension du fichier
//-----------------------------------------------------------------------------
std::string XPath::Ext(const char* filename)
{
	std::string P = filename;
	if (P.rfind('.') != std::string::npos)
		return P.substr(P.rfind('.'));
	return "";
}

//-----------------------------------------------------------------------------
// Renvoie le chemin et le nom (sans l'extension)
//-----------------------------------------------------------------------------
std::string XPath::PathName(const char* filename)
{
	std::string P = filename;
	return P.substr(0, P.rfind('.'));
}

//-----------------------------------------------------------------------------
// Renvoie le chemin complet (repertoire + nom)
//-----------------------------------------------------------------------------
std::string XPath::FullName(const char* folder, const char* filename)
{
	std::string F = folder, P = filename;
	return F + m_Sep + P;
}

//-----------------------------------------------------------------------------
// Renvoie un chemin relatif
//-----------------------------------------------------------------------------
std::string XPath::Relative(const char* root, const char* path)
{
	std::string R = root;
	std::string P = path;
	std::string Rel;

	if (P == R)
		return (std::string)"." + m_Sep;

	// Cas simple
	// root = C:\toto
	// path = C:\toto\titi
	// => .\titi
	size_t pos = P.find(R);
	if (pos == 0) {
		if (P[R.length()] == m_Sep) {
			Rel = "." + P.substr(R.length());
			return Rel;
		}
	}

	// Cas difficile
	// root = C:\toto\titi
	// path = C:\toto\tata\tutu
	// => ..\tata\tutu

	std::string root1 = R.substr(0, R.find(m_Sep));
	std::string root2 = P.substr(0, P.find(m_Sep));
	if (root1.compare(root2) != 0)	// Pas le meme disque
		return P;

	std::vector<std::string> T, U;
	std::string tmp = R;
	while(true) {
		std::string s = tmp.substr(0, tmp.find(m_Sep));
		if (s.size() < 1)
			break;
		T.push_back(s);
		if (tmp.find(m_Sep) == std::string::npos)
			break;
		tmp = tmp.substr(tmp.find(m_Sep)+1);
		if (tmp.size() < 1)
			break;
	}
	tmp = P;
	while(true) {
		std::string s = tmp.substr(0, tmp.find(m_Sep));
		if (s.size() < 1)
			break;
		U.push_back(s);
		if (tmp.find(m_Sep) == std::string::npos)
			break;
		tmp = tmp.substr(tmp.find(m_Sep)+1);
		if (tmp.size() < 1)
			break;
	}

	for (uint32_t i = 0; i < T.size(); i++) {
		if (i < U.size())
			if (T[i].compare(U[i]) == 0)
				continue;
    uint32_t j;
    for (j = i; j < T.size(); j++)
			Rel = Rel + ".." + m_Sep;
		for (j = i; j < U.size() - 1; j++)
			Rel = Rel + U[j] + m_Sep;
		Rel += U[U.size() - 1];
		break;
	}

	return Rel;
}

//-----------------------------------------------------------------------------
// Renvoie un chemin absolue
//-----------------------------------------------------------------------------
std::string XPath::Absolute(const char* root, const char* path)
{
	std::string R = root;
	std::string P = path;
	std::string Abs;

	if (P.length() < 2)
		return P;
	// Cas ./toto
	if ((P[0] == '.')&&(P[1] == m_Sep)) {
		Abs = root + P.substr(1);
		return Abs;
	}

	// Cas ../toto
	if ((P[0] == '.')&&(P[1] == '.')&&(P[2] == m_Sep)) {
		std::string subpath = P.substr(P.find(m_Sep) + 1);
		std::string subroot = R.substr(0, R.rfind(m_Sep));
		if ((subpath[0] == '.')&&(subpath[1] == '.')&&(subpath[2] == m_Sep))
			return Absolute(subroot.c_str(), subpath.c_str());
		Abs = subroot + m_Sep + subpath;
		return Abs;
	}

	return P;
}

//-----------------------------------------------------------------------------
// Traduit un nom de fichier a la norme Windows
// Les caracteres : \ / : * ? " < > | sont interdits
//-----------------------------------------------------------------------------
std::string XPath::ConvertWindows(const char* filename)
{
	std::string P = filename;
	for (uint32_t i = 0; i < P.length(); i++)
		if ((P[i] == '\\') || (P[i] == '/') || (P[i] == ':') || (P[i] == '*') ||
				(P[i] == '?') || (P[i] == '"') || (P[i] == '<') || (P[i] == '>') ||
				(P[i] == '|'))
				P[i] = '_';

	return P;
}
