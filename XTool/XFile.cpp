//-----------------------------------------------------------------------------
//								XFile.cpp
//								=========
//
// Auteur : F.Becirspahic - MODSP
//
// 13/01/2010
//-----------------------------------------------------------------------------

#include "XFile.h"

XFileManager gFileManager;

//-----------------------------------------------------------------------------
// Deplacement dans un fichier -> executable 32 bits sous Windows
//-----------------------------------------------------------------------------
void XFile::Seek(std::istream* in, std::streampos pos)
{
#ifndef IGNMAP_WIN32
  in->seekg(pos);
  return;
#endif // IGNMAP_WIN32

  std::streampos max_offset = 0x7FFFFFFF; // Limite 2GB - 1
  if (pos <= max_offset) {
    in->seekg(pos);
    return;
  }
  // Fichier de taille > 2GB
  in->seekg(max_offset, std::ios_base::beg);
  std::streampos curpos = pos - max_offset;
#ifndef IGNMAP_WIN32
  while (curpos > max_offset) {
    in->seekg(max_offset, std::ios_base::cur);
    curpos -= max_offset;
  }
  in->seekg(curpos, std::ios_base::cur);
  return;
#else // Bug de MinGW dans la methode seekg pour les gros fichiers
  /*
  if (curpos < max_offset) {
    in->seekg(curpos, std::ios_base::cur);
    return;
  }
  if (curpos > max_offset) {
    in->seekg(max_offset, std::ios_base::cur);
    curpos -= max_offset;
  }
  std::streampos bufsize = 1024*1024*1024;
  while (curpos > bufsize) {
    in->ignore(bufsize);
    curpos -= bufsize;
  }
  in->ignore(curpos);
  */
  while (curpos > max_offset) {
    //in->seekg(max_offset, std::ios_base::cur);
    in->rdbuf()->pubseekoff(max_offset, std::ios_base::cur, std::ios_base::in);
    curpos -= max_offset;
  }
  //in->seekg(curpos, std::ios_base::cur);
  in->rdbuf()->pubseekoff(curpos, std::ios_base::cur, std::ios_base::in);

#endif
}

//-----------------------------------------------------------------------------
// Classe XFile
//-----------------------------------------------------------------------------
XFile::XFile()
{
  m_In = NULL;
}

XFile::XFile(const char *filename, std::ios_base::openmode mode)
{
  m_strFilename = filename;
  m_Mode = mode;
  m_In = NULL;
}

XFile::~XFile()
{
  Close();
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier
//-----------------------------------------------------------------------------
bool XFile::Open(const char *filename, std::ios_base::openmode mode)
{
  m_strFilename = filename;
  m_Mode = mode;
  return gFileManager.Open(this);
}

//-----------------------------------------------------------------------------
// Fermeture d'un fichier
//-----------------------------------------------------------------------------
void XFile::Close()
{
  gFileManager.Close(this);
}

//-----------------------------------------------------------------------------
// Indique si le stream est operationnel
//-----------------------------------------------------------------------------
bool XFile::Good()
{
  if (m_In == NULL)
    return false;
  return m_In->good();
}

//-----------------------------------------------------------------------------
// Acces au stream
//-----------------------------------------------------------------------------
std::ifstream* XFile::IStream()
{
  if (m_In == NULL) {
    if (gFileManager.Open(this))
      return m_In;
    else
      return NULL;
  }
  if (m_In->good())
    return m_In;
  if (gFileManager.Open(this))
    return m_In;
  return NULL;
}

//-----------------------------------------------------------------------------
// Acces au stream
//-----------------------------------------------------------------------------
bool XFile::Seek(std::streampos pos)
{ 
  if (IStream() == NULL)
    return false;
  Seek(m_In, pos);
  return m_In->good();
}

//-----------------------------------------------------------------------------
// Acces au stream
//-----------------------------------------------------------------------------
unsigned int XFile::Read(char* data, unsigned int maxSize)
{ 
  if (IStream() == NULL)
    return 0;
  m_In->read(data, maxSize);
  return (unsigned int)m_In->gcount();
}

//-----------------------------------------------------------------------------
// Classe XFileManager
//-----------------------------------------------------------------------------
XFileManager::XFileManager()
{
  m_nMaxFile = 128;
  //m_Log.open("D:\\log.txt");
  m_nCount = 0;
  m_Stream = new std::ifstream[m_nMaxFile];
}

XFileManager::~XFileManager()
{
  delete[] m_Stream;
}

//-----------------------------------------------------------------------------
// Ouverture d'un fichier
//-----------------------------------------------------------------------------
bool XFileManager::Open(XFile* file)
{
  //m_Log << "Open : " << file->m_strFilename << std::endl;
  if (file->m_In != NULL) {
    file->m_In->close();
  } else
    file->m_In = Stream();

  if (file->m_In == NULL)
    return false;

  file->m_In->open(file->m_strFilename.c_str(), file->m_Mode);
  m_File.push_front(file);
  m_nCount++;
  //m_Log << "* " << m_nCount << " " << m_File.size() << std::endl;
  return file->m_In->good();
}

//-----------------------------------------------------------------------------
// Renvoi un stream libre
//-----------------------------------------------------------------------------
std::ifstream* XFileManager::Stream()
{
  if (m_File.size() >= m_nMaxFile) {
    bool flag = true;
    do {
      if (m_File.size() < 1) break;
      XFile* oldFile = m_File.back();
      //m_Log << "Close : " << oldFile->m_strFilename << std::endl;
      if (oldFile->m_In != NULL) {
        oldFile->m_In->close();
        flag = false;
      }
      oldFile->m_In = NULL;
      m_File.pop_back();
    } while (flag);
  }

  for (unsigned int i = 0; i < m_nMaxFile; i++)
    if (!m_Stream[i].is_open())
      return &(m_Stream[i]);

  //m_Log << "Bad ! " << std::endl;
  return NULL;
}

//-----------------------------------------------------------------------------
// Fermeture d'un fichier
//-----------------------------------------------------------------------------
void XFileManager::Close(XFile* file)
{
  //m_Log << "--- Close " << m_nCount << " " << m_File.size() << " " << file->m_strFilename << std::endl;
  if (file->m_In != NULL) {
    file->m_In->close();
    file->m_In = NULL;
  }
  m_File.remove(file);
}
