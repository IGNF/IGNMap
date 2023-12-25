//-----------------------------------------------------------------------------
//								XDBase.h
//								========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 17/03/2003
//-----------------------------------------------------------------------------

#ifndef _XDBASE_H
#define _XDBASE_H

#include "../XTool/XBase.h"
#include "../XTool/XFile.h"
#include <vector>

//-----------------------------------------------------------------------------
// Entete d'un fichier DBase
//-----------------------------------------------------------------------------
class XDBaseFileHeader {
protected:
	uint8_t		m_cVersion;
	uint8_t		m_Date[3];			// Date YYMMDD
	uint32_t	m_nNbRecord;		// Nombre de records dans le fichier
	uint16_t	m_nHeaderSize;	// Taille de l'entete en octets
	uint16_t	m_nRecordSize;	// Taille d'un record en octets
	uint8_t		m_cTransaction;
	uint8_t		m_cEncryption;
	uint8_t		m_MultiUser[12];
	uint8_t		m_cIndex;
	uint8_t		m_cLanguage;

public:
	XDBaseFileHeader(uint32_t nbRecord = 0, uint32_t recordSize = 0,  uint32_t nb_field = 0);
	virtual ~XDBaseFileHeader() {;}

	inline uint32_t	NbRecord() const { return m_nNbRecord;}
	inline uint16_t	HeaderSize() const { return m_nHeaderSize;}
	inline uint16_t	RecordSize() const { return m_nRecordSize;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);
};

//-----------------------------------------------------------------------------
// Descripteur de champs DBase
//-----------------------------------------------------------------------------
class XDBaseFieldDescriptor {
public :
	enum eType { String, Date, NumericF, NumericN, Logical, Memo, Variable, Picture, Binary, General,
							 Short, Long, Double};
protected:
	std::string		m_strName;
	eType					m_Type;
	uint8_t					m_nLength;
	uint8_t					m_nDecCount;

	bool Char2Type(char t);

public:
	XDBaseFieldDescriptor(const char* name, char t = 'C', uint8_t size = 254, uint8_t dec = 0);
	virtual ~XDBaseFieldDescriptor() {;}

	bool Read(std::ifstream* in, XError* error = NULL);
	bool Write(std::ofstream* out, XError* error = NULL);

	inline uint16_t Length() const { return (uint16_t)m_nLength;}
	inline uint16_t DecCount() const { return (uint16_t)m_nDecCount;}
	inline std::string Name() const { return m_strName;}
	inline eType Type() const { return m_Type;}
};

//-----------------------------------------------------------------------------
// Fichier DBase
//-----------------------------------------------------------------------------
class XDBaseFile {
protected:
	std::string	m_strFilename;
	XDBaseFileHeader	m_Header;
	std::vector<XDBaseFieldDescriptor>	m_FieldDesc;
  //std::ifstream			m_In;
  XFile             m_In;
  std::ofstream			m_Out;

public:
	XDBaseFile() { m_strFilename = "<sans nom>";}
	virtual ~XDBaseFile() {;}

	bool ReadHeader(const char* filename, XError* error = NULL);
	bool ReadRecord(uint32_t num, std::vector<std::string>& V, XError* error = NULL);

	uint32_t NbField() const { return m_FieldDesc.size();}
	uint32_t NbRecord() const { return m_Header.NbRecord();}
	bool GetFieldDesc(uint32_t index, std::string& name, XDBaseFieldDescriptor::eType& type, uint16_t& length, uint8_t& dec);

	int FindField(std::string fieldname, bool no_case = false);

	bool AddField(const char* name, char type, uint8_t size, uint8_t dec = 0);
	bool SetNbRecord(uint32_t nb);

	bool WriteHeader(const char* filename, XError* error = NULL);
	bool WriteRecord(std::vector<std::string>& V, XError* error = NULL);
	bool UpdateRecord(uint32_t num, std::vector<std::string>& V, XError* error = NULL);
	bool AddRecord(std::vector<std::string>& V, XError* error = NULL);

};

#endif //_XDBASE_H
