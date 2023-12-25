//-----------------------------------------------------------------------------
//								XLzwCodec.cpp
//								=============
//
// Auteur : F.Becirspahic - MODSP
//
// 23/08/2009
//-----------------------------------------------------------------------------

#include <cstring>
#include "XLzwCodec.h"

// Class XLzwString
XLzwString::XLzwString(uint8_t c)
{
  m_nLength = 1;
  m_nAlloc = eAlloc;
  m_String = new uint8_t[m_nAlloc];
  m_String[0] = c;
}

XLzwString::XLzwString(int n, uint8_t* str)
{
  m_nLength = n;
  m_nAlloc = n;
  m_String = new uint8_t[m_nAlloc];
  ::memcpy((void*)m_String, (void*)str, m_nLength);
}

XLzwString::~XLzwString()
{
  if (m_String != NULL)
    delete[] m_String;
  m_nLength = 0;
}

XLzwString& XLzwString::operator=(const XLzwString& S)
{
  if (m_String != NULL)
    delete[] m_String;
  m_nLength = S.m_nLength;
  m_nAlloc = m_nLength;
  m_String = new uint8_t[m_nAlloc];
  ::memcpy((void*)m_String, (void*)S.m_String, m_nLength);
  return *this;
}

void XLzwString::merge(const XLzwString& A, const XLzwString& B)
{
  if (m_String == NULL) {
    m_nAlloc = (A.m_nLength + 1);
    m_String = new uint8_t[m_nAlloc];
  } else {
    if (m_nAlloc < (A.m_nLength + 1)) {
      delete[] m_String;
      m_nAlloc = (A.m_nLength + 1);
      m_String = new uint8_t[m_nAlloc];
    }
  }

  m_nLength = A.m_nLength + 1;
  if (A.m_nLength > 0)
    ::memcpy((void*)m_String, (void*)A.m_String, m_nLength - 1);
  if (B.m_nLength > 0)
    m_String[m_nLength - 1] = B.m_String[0];
}

void XLzwString::set(int n, uint8_t* str)
{
  if (m_String == NULL) {
    m_nAlloc = n;
    m_String = new uint8_t[m_nAlloc];
  } else {
    if (m_nAlloc < n) {
      delete[] m_String;
      m_nAlloc = n;
      m_String = new uint8_t[m_nAlloc];
    }
  }

  m_nLength = n;
  ::memcpy((void*)m_String, (void*)str, n);
}

// Classe CLzwCodec
XLzwCodec::XLzwCodec()
{
   m_table = new XLzwString[4096];
  for(int i = 0; i < 256; i++)
    //m_table[i] = XLzwString((uint8_t)i);
    m_table[i].set(1, (uint8_t*)&i);
  m_nbbit = 9;	// Le premier code a 9 bits
  m_next = 258;	// 256 et 257 sont reserves pour le ClearCode et le EoiCode
  m_bitpos = 0;
  m_mask[0] = 1 << 31;
  for (int size = 1; size < 14; size++)
    m_mask[size] = (~((1 << 31)>>(31-size)));
}

XLzwCodec::~XLzwCodec()
{
  delete[] m_table;
  m_nbbit = 9;
  m_next = 258;
  m_bitpos = 0;
  m_lzw = NULL;
  m_out = NULL;
  m_outpos = NULL;
}

void XLzwCodec::SetDataIO(uint8_t* lzw, uint8_t* out, uint32_t size)
{
  m_lzw = lzw;
  m_out = out;
  m_outpos = out;
  m_nbbit = 9;	// Le premier code a 9 bits
  m_next = 258;	// 256 et 257 sont reserves pour le ClearCode et le EoiCode
  m_bitpos = 0;
  m_size = size;
}

void XLzwCodec::InitializeTable()
{
  m_nbbit = 9;
  m_next = 258;
}

void XLzwCodec::AddStringToTable(int n, uint8_t* chaine)
{
  //m_table[m_next] = XLzwString(n, chaine);
  m_table[m_next].set(n, chaine);
  m_next++;
  if (m_next == 511)
    m_nbbit = 10;
  if (m_next == 1023)
    m_nbbit = 11;
  if (m_next == 2047)
    m_nbbit = 12;
}

void XLzwCodec::AddStringToTable(uint32_t oldcode, uint32_t code)
{
  m_table[m_next].merge(m_table[oldcode], m_table[code]);
  m_next++;
  if (m_next == 511)
    m_nbbit = 10;
  if (m_next == 1023)
    m_nbbit = 11;
  if (m_next == 2047)
    m_nbbit = 12;
}

int XLzwCodec::GetNextCode()
{
  static int n, offset;
  offset = m_nbbit + m_bitpos;

  n = (*m_lzw << 24 ) + (*(m_lzw+1) << 16 ) + (*(m_lzw+2) << 8 );

  m_lzw += (offset / 8);
  m_bitpos = offset % 8;

  return ((n >> (32 - offset)) & m_mask[m_nbbit]);
}

void XLzwCodec::WriteString(int code)
{
  ::memcpy((void*)m_outpos, (void*)m_table[code].string(), m_table[code].length());
  m_outpos += m_table[code].length();
}

void XLzwCodec::WriteString(XLzwString* S)
{
  ::memcpy((void*)m_outpos, (void*)S->string(), S->length());
  m_outpos += S->length();
}

// Decompression
void XLzwCodec::Decompress()
{
  uint32_t code, oldcode = 0, ClearCode = 256, EoiCode = 257;

  while ((code = GetNextCode()) != EoiCode){
    if ((m_outpos - m_out) >= m_size)
      break;  // Normalement, cela ne devrait jamais arriver ...
    if (code == ClearCode){
      InitializeTable();
      code = GetNextCode();
      if (code == EoiCode)
        break;
      WriteString(code);
      oldcode = code;
    } else {
      if (code < (uint32_t)m_next){
        WriteString(&m_table[code]);
        AddStringToTable(oldcode, code);
        oldcode = code;
      } else {
        if (oldcode < (uint32_t)m_next) {
          AddStringToTable(oldcode, oldcode);
          WriteString(&m_table[m_next-1]);
          oldcode = code;
        } else
          break; // Normalement, cela ne devrait pas arriver
      }
    }
  } //endwhile
}
