//-----------------------------------------------------------------------------
//								XFile.h
//								=======
//
// Auteur : F.Becirspahic - MODSP
//
// 13/01/2010
//-----------------------------------------------------------------------------

#ifndef XFILE_H
#define XFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <list>

//-----------------------------------------------------------------------------
// Classe XFile
//-----------------------------------------------------------------------------
class XFile {
protected:
  std::string               m_strFilename;
  std::ifstream*            m_In;
  std::ios_base::openmode   m_Mode;

public:
  XFile();
  XFile(const char *filename, std::ios_base::openmode mode);
  virtual ~XFile();
  bool Open(const char *filename, std::ios_base::openmode mode);
  void Close();

  std::ifstream* IStream();
  bool Good();

  bool Seek(std::streampos pos);
  unsigned int Read(char* data, unsigned int maxSize);

  static void Seek(std::istream* in, std::streampos pos);

  friend class XFileManager;
};

//-----------------------------------------------------------------------------
// Classe XFileManager
//-----------------------------------------------------------------------------
class XFileManager {
protected:
  unsigned int        m_nMaxFile;
  std::list<XFile*>   m_File;
  //std::ofstream       m_Log;
  unsigned int        m_nCount;
  std::ifstream*      m_Stream;

public:
  XFileManager();
  virtual ~XFileManager();
  bool Open(XFile*);
  void Close(XFile*);
  std::ifstream* Stream();
};

#endif // XFILE_H
