//-----------------------------------------------------------------------------
//								XLzwCodec.h
//								===========
//
// Auteur : F.Becirspahic - MODSP
//
// 23/08/2009
//
// Classe XLzwCodec : compresseur / Decompresseur LZW
//-----------------------------------------------------------------------------

#ifndef XLZWCODEC_H
#define XLZWCODEC_H

#include "../XTool/XBase.h"

class XLzwString {
  enum { eAlloc = 1};
protected:
  uint32_t	m_nLength;
  uint32_t	m_nAlloc;
  uint8_t*		m_String;
public:
  //XLzwString() : m_nLength(0) { m_nAlloc = eAlloc; m_String = new uint8_t[m_nAlloc];}
  XLzwString() : m_nLength(0) { m_nAlloc = 0; m_String = NULL;}
  XLzwString(uint8_t c);
  XLzwString(int n, uint8_t* str);
  ~XLzwString();

  inline uint32_t length(void) const {return m_nLength;}
  inline uint8_t* string(void) const {return m_String;}

  XLzwString& operator=(const XLzwString&);
  void merge(const XLzwString& A, const XLzwString& B);
  void set(int n, uint8_t* str);
};

class XLzwCodec {
protected:
  XLzwString*	m_table;
  uint8_t*	m_lzw;		// Donnees compressees
  uint8_t*	m_out;		// Donnees decompressees
  uint8_t*	m_outpos;	// Position courante dans le buffer de sortie
  int		m_nbbit;	// Nombre de bits a lire
  int		m_bitpos;	// Position dans le buffer de lecture (en bit)
  int		m_next;		// Position du prochain code
  int		m_mask[14];		// Masque (pour les decalages vers la droite)
  uint32_t  m_size;     // Taille maximum du buffer de sortie

  void InitializeTable();
  void AddStringToTable(int, uint8_t*);
  void AddStringToTable(uint32_t oldcode, uint32_t code);
  void WriteString(int code);
  void WriteString(XLzwString* S);
  int GetNextCode();

public:
  XLzwCodec();
  virtual ~XLzwCodec();

  void SetDataIO(uint8_t* lzw, uint8_t* out, uint32_t size);
  void Decompress();
};

#endif // XLZWCODEC_H
