//*----------------------------------------------------------------------------
//								XParserXML.cpp
//								==============
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 08/08/00
//*----------------------------------------------------------------------------

#include "XParserXML.h"

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XNodeXML::~XNodeXML()
{
	DeleteNodes();
}

//-----------------------------------------------------------------------------
// Destruction des noeuds fils
//-----------------------------------------------------------------------------
void XNodeXML::DeleteNodes()
{
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++)
		delete *iter;
	m_Tree.clear();
}

//-----------------------------------------------------------------------------
// Operateur de copie
//-----------------------------------------------------------------------------
XNodeXML& XNodeXML::operator=(const XNodeXML& N)
{
	if (this != &N){
		m_strName = N.m_strName;
		m_strValue = N.m_strValue;
		std::list<XNodeXML*>::const_iterator iter;
		// Effacement de l'ancien arbre
		DeleteNodes();
		// Copie du nouvel arbre
		for (iter = N.m_Tree.begin(); iter != N.m_Tree.end(); iter++){
			XNodeXML* newNode = new XNodeXML;
			*newNode = *(*iter);
			m_Tree.push_back(newNode);		
		}
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Renvoie un noeud fils
//-----------------------------------------------------------------------------
XNodeXML* XNodeXML::GetNode(std::string nodename, uint32_t num)
{
	if (nodename[0] != '/')
		return nullptr;
	std::string::size_type pos = nodename.find('/', 1);
	std::string topnode = nodename.substr(1, pos - 1);
	std::string subnode = nodename.substr(topnode.length() + 1);
	if (topnode != m_strName)
		return nullptr;
	if (pos == std::string::npos)// Fin d'un arbre
		return this;

	// Recherche dans les noeuds fils
	int nb_find = -1;
	std::list<XNodeXML*>::iterator iter;
	XNodeXML* nodeXML;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++){
		nodeXML = (*iter)->GetNode(subnode);
		if (nodeXML != nullptr) {
			nb_find++;
			if (nb_find == num)
				return nodeXML;
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// Lecture d'un token dans un fichier XML
//-----------------------------------------------------------------------------
std::string XNodeXML::GetToken(std::istream* in)
{
	// On retire les blancs en debut de chaine
	char c = 0;
	while(!in->eof()) {
		*in >> c;
		if ((c != 0x09)&&(c != 0x0A)&&(c != 0x0D)&&(c != 0x20))	// Espace, tabulation, ...
			break;
	}
	if (in->eof())
		return "";
	std::string token;
	token += c;
	// On lit le token
	while(!in->eof()) {
		c = in->get();
		if ((c == 0x09)||(c == 0x0A)||(c == 0x0D) || (c == 0x20)) {	// Espace, tabulation, ...
			if (token[token.size() - 1] != ' ')
				token += ' ';
			continue;
		}
		//	break;
		if (c == '<') {	// Debut d'un tag XML
			in->putback(c);
			break;
		}
		token += c;
		if (c == '>')	// Fin d'un tag XML
			break;
	}
	if (token[token.size() - 1] == ' ')
		token.erase(token.size() - 1);
	return token;
}

//-----------------------------------------------------------------------------
// Lecture des attributs
//-----------------------------------------------------------------------------
bool XNodeXML::FindAttributes()
{
	std::string::size_type pos = m_strName.find(' ');
	if (pos == std::string::npos)
		return true;

	std::string token = m_strName, att, val;
	m_strName = token.substr(0, pos);

	std::string::size_type next;
	do {
		next = token.find('=', pos);
		if (next == std::string::npos)
			return true;
		att = token.substr(pos+1, next-pos-1);
		pos = token.find('"', next) + 1;
		next = token.find('"', pos);
		if (next == std::string::npos)
			return false;
		val = token.substr(pos, next-pos);
		XNodeXML* newNode = new XNodeXML(att, val);
		m_Tree.push_back(newNode);
		pos = next + 1;
	} while(true);

	return true;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier
//-----------------------------------------------------------------------------
bool XNodeXML::Read(std::istream* in, bool root)
{
	std::string token = GetToken(in);
	if ((token == "")&&(in->eof()))
		return true;
	// Detection des entetes XML <?xml ...?>
	if (token.compare(0, 5, "<?xml") == 0) {
		while(token.compare(token.length() - 2, 2, "?>") != 0)
			*in >> token;
		token.erase();
		Read(in, root);
	}
	// Detection des CDATA <![CDATA[ ... ]]>
	if (token.compare(0, 9, "<![CDATA[") == 0) {
    while( (in->good()) && (token.compare(token.length() - 3, 3, "]]>") != 0)) {
			token += in->get();
//			if (token.compare(token.length() - 3, 3, "]]>") == 0)
//				break;
		}
		token.erase();
		return Read(in, root);	// Return sinon probleme pour les commentaires inclus
	}
	// Detection des commentaires <!-- -->
	if (token.compare(0, 4, "<!--") == 0) {
		while(token.compare(token.length() - 3, 3, "-->") != 0)
			token += in->get();
		token.erase();
		return Read(in, root);	// Return sinon probleme pour les commentaires inclus
	}

	// Detection des commentaires <! ...>
	if (token.compare(0, 2, "<!") == 0) {
		while(token.compare(token.length() - 1, 1, ">") != 0)
			*in >> token;
		token.erase();
		return Read(in, root);	// Return sinon probleme pour les commentaires inclus
	}
	if (token.compare("]>") == 0) // Fin d'un commentaire imbrique ?
		Read(in, root);

	// Detection d'un mot cle
	if((token[0] == '<')&&(token[token.length() - 1] == '>')) {
		if ((m_strName == "")&&(token[1] == '/'))	// Probleme : fin de mot cle avant 
			return false;														// que le debut n'ait ete trouve
		if (m_strName == "")	// Debut d'un arbre
			m_strName = token.substr(1, token.length() - 2);
		FindAttributes();
		if ((m_strName != "")&&(token == ("</"+m_strName+">")))	// Fin d'un arbre
			return true;
		if (!root)
			if (token[token.length() - 2] == '/') { // Cas des noeuds vides
				XNodeXML* newNode = new XNodeXML(token.substr(1, token.length() - 3));
				newNode->FindAttributes();
				m_Tree.push_back(newNode);
			} else {
				XNodeXML* newNode = new XNodeXML(token.substr(1, token.length() - 2));
				if (newNode->Read(in))
					m_Tree.push_back(newNode);
				else
					delete newNode;
			}
	} else
		if (token != "")
			if (m_strValue == "")
				m_strValue = token;
			else
				m_strValue += (" " + token);
	if ((m_strValue != "")&&(m_strName == ""))	// Probleme : pas de mot cle
		return false;
	return Read(in);
}

//-----------------------------------------------------------------------------
// Recherche d'un noeud
//-----------------------------------------------------------------------------
bool XNodeXML::FindNode(std::string nodename, uint32_t num)
{
	if (nodename[0] != '/')
		return false;
	std::string::size_type pos = nodename.find('/', 1);
	std::string topnode = nodename.substr(1, pos - 1);
	std::string subnode = nodename.substr(topnode.length() + 1);
	if (topnode != m_strName)
		return false;
	if (pos == std::string::npos)  // Fin d'un arbre
		return true;

	// Recherche dans les noeuds fils
	int nb_find = -1;
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++){
		if ((*iter)->FindNode(subnode)) {
			nb_find++;
			if (nb_find == num)
				return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Lecture d'un noeud
//-----------------------------------------------------------------------------
std::string XNodeXML::ReadNode(std::string nodename, uint32_t num)
{
	std::string result = "";
	if (nodename[0] != '/')
		return result;
	std::string::size_type pos = nodename.find('/', 1);
	std::string topnode = nodename.substr(1, pos - 1);
	std::string subnode = nodename.substr(topnode.length() + 1);
	if (topnode != m_strName)
		return result;
	if (pos == std::string::npos)  // Fin d'un arbre
		return m_strValue;

	// Recherche dans les noeuds fils
	int nb_find = -1;
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++){
		result = (*iter)->ReadNode(subnode);
		if (result != "") {
			nb_find++;
			if (nb_find == num)
				return result;
		}
	}
	return "";
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de noeuds
//-----------------------------------------------------------------------------
uint32_t XNodeXML::ReadArrayNode(std::string nodename, std::vector<std::string>* V)
{
	if (nodename[0] != '/')
		return 0;
	std::string::size_type pos = nodename.find('/', 1);
	std::string topnode = nodename.substr(1, pos - 1);
	std::string subnode = nodename.substr(topnode.length() + 1);
	if (topnode != m_strName)
		return 0;
	if (pos == std::string::npos) { // Fin d'un arbre
		V->push_back(m_strValue);
		return V->size();
	}

	// Recherche dans les noeuds fils
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++)
		(*iter)->ReadArrayNode(subnode, V);

	return V->size();
}

//-----------------------------------------------------------------------------
// Lecture de tous les noeuds
//-----------------------------------------------------------------------------
uint32_t XNodeXML::ReadAllNodes(std::string nodename, std::vector<XNodeXML*>* T)
{
	if (nodename[0] != '/')
		return 0;
	std::string::size_type pos = nodename.find('/', 1);
	std::string topnode = nodename.substr(1, pos - 1);
	std::string subnode = nodename.substr(topnode.length() + 1);
	if (topnode != m_strName)
		return 0;
	if (pos == std::string::npos) { // Fin d'un arbre
		T->push_back(this);
		return T->size();
	}

	// Recherche dans les noeuds fils
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++)
		(*iter)->ReadAllNodes(subnode, T);

	return T->size();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XNodeXML::Write(std::ostream* out, bool root)
{
	if (root) {
		*out << "<" << m_strName << ">";
		if (m_strValue != "")
			*out << " " << m_strValue << " ";
		else
			*out << std::endl;
	}
	// Ecriture des noeuds fils
	std::list<XNodeXML*>::iterator iter;
	for (iter = m_Tree.begin(); iter != m_Tree.end(); iter++)
		(*iter)->Write(out);
	if (root)
		*out << "</" << m_strName << ">" << std::endl;
	return true;
}

//-----------------------------------------------------------------------------
// Analyse d'un fichier XML
//-----------------------------------------------------------------------------
bool XParserXML::Parse(std::string filename)
{
	if (filename == "")
		return false;
	std::ifstream in;
	in.open(filename.c_str());
	if (!in.is_open())
		return false;
	m_Root.DeleteNodes();	// On detruit l'arborescence existante
	m_strFile = filename;
	// On regarde s'il y a un Byte-order mark
	char c;
	do {
		in >> c;
		if (!in.good()) return false;
	} while (c != '<');
	in.putback(c);
	return m_Root.Read(&in, true);
}

//-----------------------------------------------------------------------------
// Analyse d'un flux XML
//-----------------------------------------------------------------------------
bool XParserXML::Parse(std::istream* stream)
{
	m_Root.DeleteNodes();	// On detruit l'arborescence existante
	m_strFile = "";
  return m_Root.Read(stream, true);
}

//-----------------------------------------------------------------------------
// Recherche d'un noeud
//-----------------------------------------------------------------------------
bool XParserXML::FindNode(std::string nodename, uint32_t num)
{
	if (m_Sub != NULL)
		return m_Sub->FindNode(nodename, num);
	return m_Root.FindNode(nodename, num);
}

//-----------------------------------------------------------------------------
// Lecture d'un noeud
//-----------------------------------------------------------------------------
std::string XParserXML::ReadNode(std::string nodename, uint32_t num)
{
	if (m_Sub != NULL)
		return m_Sub->ReadNode(nodename, num);
	return m_Root.ReadNode(nodename, num);
}

//-----------------------------------------------------------------------------
// Lecture d'un noeud avec conversion du type
//-----------------------------------------------------------------------------
int XParserXML::ReadNodeAsInt(std::string nodename, uint32_t num)
{
	if (!FindNode(nodename, num))
		return 0;
	std::string result = ReadNode(nodename, num);
	int n;
	(void)sscanf(result.c_str(), "%d", &n);
	return n;
}

uint32_t XParserXML::ReadNodeAsUInt32(std::string nodename, uint32_t num)
{
	if (!FindNode(nodename, num))
		return 0;
	std::string result = ReadNode(nodename, num);
	uint32_t n;
	(void)sscanf(result.c_str(), "%u", &n);
	return n;
}

uint32_t XParserXML::ReadNodeAsHexUInt32(std::string nodename, uint32_t num)
{
	if (!FindNode(nodename, num))
		return 0;
	std::string result = ReadNode(nodename, num);
	uint32_t n;
	(void)sscanf(result.c_str(), "%x", &n);
	return n;
}

double XParserXML::ReadNodeAsDouble(std::string nodename, uint32_t num)
{
	if (!FindNode(nodename, num))
		return 0.;
	std::string result = ReadNode(nodename, num);
	double x;
	(void)sscanf(result.c_str(), "%lf", &x);
	return x;
}

bool XParserXML::ReadNodeAsBool(std::string nodename, uint32_t num)
{
	int n = ReadNodeAsInt(nodename, num);
	if (n)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de noeuds
//-----------------------------------------------------------------------------
uint32_t XParserXML::ReadArrayNode(std::string nodename, std::vector<std::string>* V)
{
	if (m_Sub != NULL)
		return m_Sub->ReadArrayNode(nodename, V);
	return m_Root.ReadArrayNode(nodename, V);
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de noeuds avec conversion de type
//-----------------------------------------------------------------------------
uint32_t XParserXML::ReadArrayNodeAsInt(std::string nodename, std::vector<int>* V)
{
	std::vector<std::string> T;
	ReadArrayNode(nodename, &T);
	int x;
	for (uint32_t i = 0; i < T.size(); i++) {
		(void)sscanf(T[i].c_str(), "%d", &x);
		V->push_back(x);
	}
	return V->size();
}

uint32_t XParserXML::ReadArrayNodeAsUInt32(std::string nodename, std::vector<uint32_t>* V)
{
	std::vector<std::string> T;
	ReadArrayNode(nodename, &T);
	uint32_t x;
	for (uint32_t i = 0; i < T.size(); i++) {
		(void)sscanf(T[i].c_str(), "%u", &x);
		V->push_back(x);
	}
	return V->size();
}

uint32_t XParserXML::ReadArrayNodeAsDouble(std::string nodename, std::vector<double>* V)
{
	std::vector<std::string> T;
	ReadArrayNode(nodename, &T);
	double x;
	for (uint32_t i = 0; i < T.size(); i++) {
		(void)sscanf(T[i].c_str(), "%lf", &x);
		V->push_back(x);
	}
	return V->size();
}

//-----------------------------------------------------------------------------
// Recherche d'un parser fils
//-----------------------------------------------------------------------------
XParserXML XParserXML::FindSubParser(std::string nodename, uint32_t num)
{
	XParserXML P;
	P.m_strFile = Filename();
	if (m_Sub != NULL)
		P.m_Sub = m_Sub->GetNode(nodename, num);
	else
		P.m_Sub = m_Root.GetNode(nodename, num);
	return P;
}

//-----------------------------------------------------------------------------
// Recherche de tous les parsers fils
//-----------------------------------------------------------------------------
uint32_t XParserXML::FindAllSubParsers(std::string nodename, std::vector<XParserXML>* T)
{
	std::vector<XNodeXML*> Node;
	if (m_Sub != NULL)
		m_Sub->ReadAllNodes(nodename, &Node);
	else
		m_Root.ReadAllNodes(nodename, &Node);
	for (uint32_t i = 0; i < Node.size(); i++) {
		XParserXML P;
		P.m_strFile = Filename();
		P.m_Sub = Node[i];
		T->push_back(P);
	}

	return T->size();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XParserXML::Write(std::ostream* out, std::string xsl_file)
{	
	*out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << std::endl;
	*out << "<?xml-stylesheet type=\"text/xsl\" href=\"" << xsl_file << "\"?>" << std::endl;
	if (m_Sub != NULL)
		return m_Sub->Write(out);
	return m_Root.Write(out);
}

bool XParserXML::Write(std::string filename, std::string xsl_file)
{	
	std::ofstream out;
	out.open(filename.c_str());
	if (!out.is_open())
		return false;
	return Write(&out, xsl_file);
}
