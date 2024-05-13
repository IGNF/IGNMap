//-----------------------------------------------------------------------------
//								XTime.h
//								=======
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 24/11/2000
//-----------------------------------------------------------------------------

#ifndef _XTIME_H
#define _XTIME_H

#include <string>

namespace XTime {
	double HMSDecToS(double HMSDec);
	double SToHMSDec(double SDec);

	uint16_t DMYToD(uint32_t DMY); 
	uint16_t DMYToM(uint32_t DMY); 
	uint16_t DMYToY(uint32_t DMY); 

	uint16_t DMYYToD(uint32_t DMYY); 
	uint16_t DMYYToM(uint32_t DMYY); 
	uint16_t DMYYToY(uint32_t DMYY); 

	uint16_t MYToM(uint32_t MY); 
	uint16_t MYToY(uint32_t MY); 

	int DMYDiff(uint32_t DMY1, uint32_t DMY2); 
	int DMYYDiff(uint32_t DMYY1, uint32_t DMYY2); 
	double HMSDiff(double HMSDec1, double HMSDec2);
	double HMSAdd(double HMSDec1, double HMSDec2);

	std::string HMSDecToString(double HMSDec, uint16_t nbDec = 3);
	std::string HMSToString(uint32_t HMS);
	std::string DMYToString(uint32_t DMY, char sep='/');
	std::string DMYYToString(uint32_t DMYY, char sep='/');
	std::string MYToString(uint32_t DMY, char sep='/');

	uint32_t DMYToDMYY(uint32_t DMY, uint16_t maxY, uint16_t root = 1900);
}

#endif //_XTIME_H