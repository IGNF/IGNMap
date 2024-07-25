//-----------------------------------------------------------------------------
//								XDBase.cpp
//								==========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 17/03/2003
//-----------------------------------------------------------------------------

#include <cstring>
#include <algorithm>
#include "XDBase.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XDBaseFileHeader::XDBaseFileHeader(uint32_t nbRecord, uint32_t recordSize, uint32_t nb_field)
{
	m_cVersion = 0x03;
	m_Date[0] = 99;										// Date YYMMDD
	m_Date[1] = 12;
	m_Date[2] = 31;
	m_nNbRecord = nbRecord;						// Nombre de records dans le fichier
	m_nRecordSize = recordSize;				// Taille d'un record en octets
	m_nHeaderSize = nb_field * 32 + 32 + 1;		// Taille de l'entete en octets

	m_cTransaction = 0;
	m_cEncryption = 0;
	::memset(m_MultiUser, 0, 12);
	m_cIndex = 0;
	m_cLanguage = 0x03;
}

//-----------------------------------------------------------------------------
// Lecture dans un fichier
//-----------------------------------------------------------------------------
bool XDBaseFileHeader::Read(std::ifstream* in, XError* error)
{
	char buf[256];

	in->read((char*)&m_cVersion, 1);
	switch(m_cVersion) {
		case 0x03 : // DBF
		case 0x04 :	// DBF
		case 0x05 :	// DBF
		case 0x43 :	// DBV
		case 0xB3 :	// DBV et DBT
		case 0x83 :	// DBT
		case 0x8B :	// DBT
		case 0x8E :	// DBT
		case 0xF5 :	// FMP
			break;
		default:
			return XErrorError(error, "XDBaseFileHeader::Read", XError::eBadFormat);
	}
	in->read((char*)&m_Date, 3);
	in->read((char*)&m_nNbRecord, 4);
	in->read((char*)&m_nHeaderSize, 2);
	in->read((char*)&m_nRecordSize, 2);

	in->read(buf, 2);
	in->read((char*)&m_cTransaction, 1);
	in->read((char*)&m_cEncryption, 1);
	in->read(buf, 12);
	in->read((char*)&m_cIndex, 1);
	in->read((char*)&m_cLanguage, 1);
	in->read(buf, 2);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier
//-----------------------------------------------------------------------------
bool XDBaseFileHeader::Write(std::ofstream* out, XError* error)
{
	out->write((const char*)&m_cVersion, 1);
	out->write((const char*)&m_Date, 3);
	out->write((char*)&m_nNbRecord, 4);
	out->write((char*)&m_nHeaderSize, 2);
	out->write((char*)&m_nRecordSize, 2);
	char buf[256];
	::memset(buf, 0, 256);
	out->write(buf, 20);
	return true;
}

//-----------------------------------------------------------------------------
// Constructeur de XDBaseFieldDescriptor
//-----------------------------------------------------------------------------
XDBaseFieldDescriptor::XDBaseFieldDescriptor(const char* name, char t, uint8_t size, uint8_t dec)
{
	m_strName = name;
	m_nLength = size;
	m_nDecCount = dec;
	Char2Type(t);
}

//-----------------------------------------------------------------------------
// Passage type sous forme de char vers type enumere
//-----------------------------------------------------------------------------
bool XDBaseFieldDescriptor::Char2Type(char t)
{
	switch(t) {
		case 'C' : m_Type = String; return true;
		case 'D' : m_Type = Date; m_nLength = 8; return true;
		case 'F' : m_Type = NumericF; return true;
		case 'N' : m_Type = NumericN; return true;
		case 'L' : m_Type = Logical; m_nLength = 1; return true;
		case 'M' : m_Type = Memo; m_nLength = 10; return true;
		case 'V' : m_Type = Variable; m_nLength = 10; return true;
		case 'P' : m_Type = Picture; m_nLength = 10; return true;
		case 'B' : m_Type = Binary; m_nLength = 10; return true;
		case 'G' : m_Type = General; m_nLength = 10; return true;
		case '2' : m_Type = Short; m_nLength = 2; return true;
		case '4' : m_Type = Long; m_nLength = 4; return true;
		case '8' : m_Type = Double; m_nLength = 8; return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Lecture d'un descripteur de champs
//-----------------------------------------------------------------------------
bool XDBaseFieldDescriptor::Read(std::ifstream* in, XError* error)
{
	char buf[256];

	in->read(buf, 11);
	m_strName = buf;
	in->read(buf, 1);
	Char2Type(buf[0]);
	in->read(buf, 4);
	in->read(buf, 1);
	m_nLength = ((uint8_t*)buf)[0];
	in->read(buf, 1);
	m_nDecCount = ((uint8_t*)buf)[0];
	in->read(buf, 14);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un descripteur de champs
//-----------------------------------------------------------------------------
bool XDBaseFieldDescriptor::Write(std::ofstream* out, XError* error)
{
	char buf[256];

	::memset(buf, 0, 256);
	::strncpy(buf, m_strName.c_str(), 10);
	buf[10] = 0;
	out->write(buf, 11);
	char type;
	switch (m_Type) {
		case String : type = 'C'; break;
		case Date : type = 'D'; break;
		case NumericN : type = 'N'; break;
		case NumericF : type = 'F'; break;
		case Logical : type = 'L'; break;
		case Memo : type = 'M'; break;
		case Variable : type = 'V'; break;
		case Picture : type = 'P'; break;
		case Binary : type = 'B'; break;
		case General : type = 'G'; break;
		case Short : type = '2'; break;
		case Long : type = '4'; break;
		case Double : type = '8'; break;
	}
	out->write(&type, 1);

	::memset(buf, 0, 256);
	out->write(buf, 4);
	char l = (char)m_nLength;
	out->write(&l, 1);
	l = (char)m_nDecCount;
	out->write(&l, 1);
	out->write(buf, 14);

	return out->good();
}

//-----------------------------------------------------------------------------
// Lecture d'un fichier DBF
//-----------------------------------------------------------------------------
bool XDBaseFile::ReadHeader(const char* filename, XError* error)
{
	m_strFilename = filename;
  //m_In.open(filename, std::ios_base::in| std::ios_base::binary);
  m_In.Open(filename, std::ios_base::in| std::ios_base::binary);
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return XErrorError(error, "XDBaseFile::Read", XError::eIOOpen);
  if (!in->good())
		return XErrorError(error, "XDBaseFile::Read", XError::eIOOpen);

  if (!m_Header.Read(in, error))
		return XErrorError(error, "XDBaseFile::Read", XError::eBadFormat);

	uint32_t nb_field = (m_Header.HeaderSize() - 32) / 32;
	m_FieldDesc.clear();
	for (uint32_t i = 0; i < nb_field; i++) {
		XDBaseFieldDescriptor field("Indefini");
    field.Read(in, error);
		m_FieldDesc.push_back(field);
	}
  in->seekg(m_Header.HeaderSize());

  return in->good();
}

//-----------------------------------------------------------------------------
// Lecture d'un enregistrement
//-----------------------------------------------------------------------------
bool XDBaseFile::ReadRecord(uint32_t num, std::vector<std::string>& V, XError* error)
{
	V.clear();
  std::ifstream* in = m_In.IStream();
  if (in == NULL)
    return  false;
  uint32_t pos = (uint32_t)m_Header.HeaderSize() + num * (uint32_t)m_Header.RecordSize() + 1L;
  if (pos >= 2147483647L){ // Fichier > 2GB
    in->seekg(2147483647L);
    in->seekg(pos - 2147483647L, std::ios_base::cur);
  } else
    in->seekg(pos);

	char buf[1024];
	short s;
	int n;
	double x;
	for (uint32_t i = 0; i < m_FieldDesc.size(); i++) {
		XDBaseFieldDescriptor desc = m_FieldDesc[i];
    in->read(buf, desc.Length());
		switch(desc.Type()) {
		case XDBaseFieldDescriptor::Short :
			memcpy((void*)&s, (void*)buf, sizeof(short));
			snprintf(buf, 1024, "%d", s);
			break;
		case XDBaseFieldDescriptor::Long :
			memcpy((void*)&n, (void*)buf, sizeof(int));
			snprintf(buf, 1024, "%d", n);
			break;
		case XDBaseFieldDescriptor::Double :
			memcpy((void*)&x, (void*)buf, sizeof(double));
			snprintf(buf, 1024, "%lf", x);
			break;
		case XDBaseFieldDescriptor::NumericN :
		case XDBaseFieldDescriptor::NumericF :
			buf[desc.Length()] = '\0';
			uint32_t cmpt;
			for (cmpt = 0; cmpt < strlen(buf); cmpt++)
				if (buf[cmpt] != ' ')
					break;
			::memmove(buf, &buf[cmpt], strlen(buf) + 1 - cmpt);
			break;
		default:
			buf[desc.Length()] = '\0';
			for (uint32_t j = desc.Length() - 1; j > 0; j--)
				if (buf[j] == ' ')
					buf[j] = '\0';
				else
					break;
		}
		V.push_back(desc.Name());
		V.push_back((std::string)buf);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Recuperation de la description d'un champ
//-----------------------------------------------------------------------------
bool XDBaseFile::GetFieldDesc(uint32_t index, std::string& name, XDBaseFieldDescriptor::eType& type,
															uint16_t& length, uint8_t& dec)
{
	if (index >= m_FieldDesc.size())
		return false;
	name = m_FieldDesc[index].Name();
	type = m_FieldDesc[index].Type();
	length = m_FieldDesc[index].Length();
	dec = m_FieldDesc[index].DecCount();
	return true;
}

//-----------------------------------------------------------------------------
// Recherche de l'existence d'un champ
//-----------------------------------------------------------------------------
int XDBaseFile::FindField(std::string fieldname, bool no_case)
{
	if (!no_case) {
		for (uint32_t i = 0; i < m_FieldDesc.size(); i++)
			if (fieldname == m_FieldDesc[i].Name())
				return i;
	} else {
		std::string name;
		for (uint32_t i = 0; i < m_FieldDesc.size(); i++) {
			name = m_FieldDesc[i].Name();
			std::transform(name.begin(), name.end(), name.begin(), tolower);
			if (fieldname == name)
				return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Ajout d'un champ
//-----------------------------------------------------------------------------
bool XDBaseFile::AddField(const char* name, char type, uint8_t size, uint8_t dec)
{
	XDBaseFieldDescriptor desc(name, type, size, dec);
	m_FieldDesc.push_back(desc);
	return true;
}

//-----------------------------------------------------------------------------
// Fixe le nombre d'enregistrements
//-----------------------------------------------------------------------------
bool XDBaseFile::SetNbRecord(uint32_t nb)
{
	uint32_t size = 1;
	for (uint32_t i = 0; i < m_FieldDesc.size(); i++)
		size += m_FieldDesc[i].Length();
	m_Header = XDBaseFileHeader(nb, size, m_FieldDesc.size());
	return true;
}

//-----------------------------------------------------------------------------
// Ecriture de l'entete du fichier DBase
//-----------------------------------------------------------------------------
bool XDBaseFile::WriteHeader(const char* filename, XError* error)
{
	m_strFilename = filename;
	m_Out.open(filename, std::ios_base::out| std::ios_base::binary);
	m_Header.Write(&m_Out, error);
	for (uint32_t i = 0; i <  m_FieldDesc.size(); i++)
		m_FieldDesc[i].Write(&m_Out);
	char terminator = 0x0D;
	m_Out.write(&terminator, 1);
	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Ecriture d'un enregistrement
//-----------------------------------------------------------------------------
bool XDBaseFile::WriteRecord(std::vector<std::string>& V, XError* error)
{
	char flag = ' ';
	char buf[256];
	short s;
	int n;
	double x;

	m_Out.write(&flag, 1);
	for (uint32_t i = 0; i <  m_FieldDesc.size(); i++) {
		::memset(buf, ' ', 256);
		::strncpy(buf, V[2*i+1].c_str(), V[2*i+1].size());
		switch(m_FieldDesc[i].Type()) {
		case XDBaseFieldDescriptor::Short :
			sscanf(buf,"%hd", &s);
			m_Out.write((char*)&s, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::Long :
			sscanf(buf,"%d", &n);
			m_Out.write((char*)&n, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::Double :
			sscanf(buf,"%lf", &x);
			m_Out.write((char*)&x, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::String :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::NumericN :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::NumericF :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
    default: ;  // Date, Logical, Memo, Variable, Picture, Binary, General
		}
	}

	return m_Out.good();
}

//-----------------------------------------------------------------------------
// Mise a jour d'un enregistrement
//-----------------------------------------------------------------------------
bool XDBaseFile::UpdateRecord(uint32_t num, std::vector<std::string>& V, XError* error)
{
	char buf[256];
	short s;
	int n;
	double x;
  /*
	if (!m_In.good())
		return false;
	m_In.close();
  */
  std::ifstream* in = m_In.IStream();
  if (in == NULL) return false;
  if (!in->good()) return false;
  m_In.Close();
	m_Out.open(m_strFilename.c_str(), std::ios_base::in |std::ios_base::out | std::ios_base::binary);
	if (!m_Out.good())
		return XErrorError(error, "XDBaseFile::UpdateRecord", XError::eIOOpen);
	m_Out.seekp(m_Header.HeaderSize() + num * m_Header.RecordSize() + 1);
	if (!m_Out.good())
		return false;

	for (uint32_t i = 0; i <  m_FieldDesc.size(); i++) {
		::memset(buf, ' ', 256);
		::strncpy(buf, V[2*i+1].c_str(), V[2*i+1].size());
		switch(m_FieldDesc[i].Type()) {
		case XDBaseFieldDescriptor::Short :
			sscanf(buf,"%hd", &s);
			m_Out.write((char*)&s, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::Long :
			sscanf(buf,"%d", &n);
			m_Out.write((char*)&n, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::Double :
			sscanf(buf,"%lf", &x);
			m_Out.write((char*)&x, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::String :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::NumericN :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
		case XDBaseFieldDescriptor::NumericF :
			m_Out.write(buf, m_FieldDesc[i].Length());
			break;
    default: ;  // Date, Logical, Memo, Variable, Picture, Binary, General
		}
	}

	m_Out.close();
  m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
  return m_In.Good();
}

//-----------------------------------------------------------------------------
// Ajout d'un enregistrement
//-----------------------------------------------------------------------------
bool XDBaseFile::AddRecord(std::vector<std::string>& V, XError* error)
{
  /*
	if (!m_In.good())
		return false;
	m_In.close();
  */
  std::ifstream* in = m_In.IStream();
  if (in == NULL) return false;
  if (!in->good()) return false;
  m_In.Close();
  m_Out.open(m_strFilename.c_str(), std::ios_base::in |std::ios_base::out | std::ios_base::binary);
	if (!m_Out.good())
		return XErrorError(error, "XDBaseFile::AddRecord", XError::eIOOpen);

	// Mise a jour dun nombre d'enregistrement
	uint32_t nb_record = NbRecord();
	SetNbRecord(nb_record + 1);
	m_Header.Write(&m_Out, error);

	m_Out.seekp(m_Header.HeaderSize() + nb_record * m_Header.RecordSize());
	if (!m_Out.good())
		return false;

	WriteRecord(V, error);

	m_Out.close();
  m_In.Open(m_strFilename.c_str(), std::ios_base::in| std::ios_base::binary);
  return m_In.Good();
}
