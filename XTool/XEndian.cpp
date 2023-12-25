//-----------------------------------------------------------------------------
//								XEndian.cpp
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 07/03/2003
//-----------------------------------------------------------------------------

#include <cstring>
#include "XEndian.h"

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XEndian::XEndian()
{
	short w = 0x0001;
	char *b = (char*) &w;
	if (b[0])
		m_bByteOrder = true;
	else
		m_bByteOrder = false;
}

//-----------------------------------------------------------------------------
// Lecture d'une donnee
//-----------------------------------------------------------------------------
bool XEndian::Read(std::istream* in, bool byteorder, void* data, int size)
{
	if (byteorder != m_bByteOrder) {
		in->read((char*)m_Buf, size);
		Swap(m_Buf, size);
	::memcpy(data, m_Buf, size);
	} else
		in->read((char*)data, size);

	return in->good();
}

//-----------------------------------------------------------------------------
// Ecriture d'une donnee
//-----------------------------------------------------------------------------
bool XEndian::Write(std::ostream* out, bool byteorder, void* data, int size)
{
	::memcpy(m_Buf, data, size);
	if (byteorder != m_bByteOrder)
		Swap(m_Buf, size);
	out->write((char*)m_Buf, size);
	return out->good();
}

//-----------------------------------------------------------------------------
// Lecture d'un tableau de donnees
//-----------------------------------------------------------------------------
bool XEndian::ReadArray(std::istream* in, bool byteorder, void* data, int size, uint32_t nb_elt)
{
	if (byteorder == m_bByteOrder) {
		in->read((char*)data, size * nb_elt);	
		return in->good();
	}
	uint8_t *ptr = (uint8_t*)data;
	for (uint32_t i = 0; i < nb_elt; i++) {
		Read(in, byteorder, ptr, size);
		ptr += size;
	}
	return in->good();
}

//-----------------------------------------------------------------------------
// Swap du buffer de lecture/ecriture
//-----------------------------------------------------------------------------
void XEndian::Swap(uint8_t* buf, int size)
{
	XAssert(size < 64);
	uint8_t* ptr = buf;
	uint8_t* swp = m_Swap + size - 1;
	for(int i = 0; i < size; i++) {
		*swp = *ptr;
		swp--; ptr++;
	}
	::memcpy(buf, m_Swap, size);	
}
