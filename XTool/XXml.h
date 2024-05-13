//-----------------------------------------------------------------------------
//								XXml.h
//								======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 21/08/00
//
// L'espace des noms XXml definit des fonctions utilitaires pour la gestion des
// fichiers XML
//-----------------------------------------------------------------------------

#ifndef _XXML_H
#define _XXML_H

#include <iostream>
#include <string>
#include "XBase.h"

namespace XXml {
	bool WriteXMLHeader(std::ostream* out, std::string xslFile);
	std::string OemToXml(std::string s);
	std::string XmlToOem(std::string s);
	std::string AbsToRelPath(std::string root, std::string path);
}

#endif //_XXML_H
