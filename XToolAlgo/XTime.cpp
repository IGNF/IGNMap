//-----------------------------------------------------------------------------
//								XTime.cpp
//								=========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 24/11/2000
//-----------------------------------------------------------------------------

#include "XTime.h"
#include <cmath>
#include <cstring>
#include "../XTool/XBase.h"

//-----------------------------------------------------------------------------
//Conversion de HeureMinuteSeconde.Décimales en Secondes.Décimales
//-----------------------------------------------------------------------------
double XTime::HMSDecToS(double HMSDec)
{
	int heure = floor(HMSDec * 0.0001);
	int min   = floor((HMSDec-heure*10000.)*0.01); 
	double sec = HMSDec - heure*10000 - min*100;
	return (heure*3600 + min*60 + sec);
}

//-------------------------------------------------------------------------
// Conversion de Secondes.Décimales vers HeureMinuteSeconde.Décimales
//-------------------------------------------------------------------------
double XTime::SToHMSDec(double SDec)
{
	double sign = 1.;
	if (SDec < 0.0)
		sign = -1.;

	double heure = SDec/3600.;
	double valheure, valmin;

	double reste = modf(fabs(heure),&valheure);
	reste = modf(reste*60.,&valmin);
	
  return sign * (valheure*10000. + valmin*100. + reste*60.);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYY -> DD (par ex : 120401 -> 12)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYToD(uint32_t DMY)
{
	double x = (double)DMY * 0.0001;
	return (uint16_t)floor(x);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYY -> MM (par ex : 120401 -> 04)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYToM(uint32_t DMY)
{
	double x = (double)DMY * 0.01;
	x = floor(x) * 0.01;
	return (uint16_t)XRint((x - floor(x)) * 100.);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYY -> YY (par ex : 120401 -> 01)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYToY(uint32_t DMY)
{
	double x = (double)DMY * 0.01;
	return (uint16_t)XRint((x - floor(x)) * 100.);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYYYY -> DD (par ex : 12042001 -> 12)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYYToD(uint32_t DMYY)
{
	double x = (double)DMYY * 0.000001;
	return (uint16_t)floor(x);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYYYY -> MM (par ex : 12042001 -> 04)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYYToM(uint32_t DMYY)
{
	double x = (double)DMYY * 0.0001;
	x = floor(x) * 0.01;
	return (uint16_t)XRint((x - floor(x)) * 100.);
}

//-----------------------------------------------------------------------------
// Extraction d'une date DDMMYYYY -> YYYY (par ex : 12042001 -> 2001)
//-----------------------------------------------------------------------------
uint16_t XTime::DMYYToY(uint32_t DMYY)
{
	double x = (double)DMYY * 0.0001;
	return (uint16_t)XRint((x - floor(x)) * 10000.);
}

//-----------------------------------------------------------------------------
// Extraction d'une date MMYY -> MM (par ex : 0401 -> 04)
//-----------------------------------------------------------------------------
uint16_t XTime::MYToM(uint32_t MY)
{
	double x = (double)MY * 0.01;
	return (uint16_t)floor(x);
}

//-----------------------------------------------------------------------------
// Extraction d'une date MMYY -> DD (par ex : 0401 -> 01)
//-----------------------------------------------------------------------------
uint16_t XTime::MYToY(uint32_t MY)
{
	uint16_t x = MYToM(MY);
	return (MY - x * 100);
}

//-----------------------------------------------------------------------------
// Ecart en jours entre deux dates DDMMYY
//-----------------------------------------------------------------------------
int XTime::DMYDiff(uint32_t DMY1, uint32_t DMY2)
{
	int d1 = DMYToD(DMY1);
	int d2 = DMYToD(DMY2);

	int m1 = DMYToM(DMY1);
	int m2 = DMYToM(DMY2);

	int y1 = DMYToY(DMY1);
	int y2 = DMYToY(DMY2);

	return XRint((d1 - d2) + (double)(m1 - m2) * (365. / 12.) + (y1 - y2) * 365);
}

//-----------------------------------------------------------------------------
// Ecart en jours entre deux dates DDMMYYYY
//-----------------------------------------------------------------------------
int XTime::DMYYDiff(uint32_t DMYY1, uint32_t DMYY2)
{
	int d1 = DMYYToD(DMYY1);
	int d2 = DMYYToD(DMYY2);

	int m1 = DMYYToM(DMYY1);
	int m2 = DMYYToM(DMYY2);

	int y1 = DMYYToY(DMYY1);
	int y2 = DMYYToY(DMYY2);

	return XRint((d1 - d2) + (double)(m1 - m2) * (365. / 12.) + (y1 - y2) * 365);
}

//-----------------------------------------------------------------------------
// Ecart horaire entre deux temps HMS
//-----------------------------------------------------------------------------
double XTime::HMSDiff(double HMSDec1, double HMSDec2)
{
	double s1 = HMSDecToS(HMSDec1);
	double s2 = HMSDecToS(HMSDec2);
	double t = s1 - s2;
	uint32_t h = 0, m = 0;
	h = (uint32_t)floor(fabs(t) / 3600.);	
	m = (uint32_t)floor((fabs(t) - h * 3600.) / 60.);
	double s = fabs(t) - h * 3600. - m * 60.;
	double hms = h * 10000 + m * 100 + s;
	if ( t < 0.)
		hms *= -1.;
	return hms;
}

//-----------------------------------------------------------------------------
// Somme horaire entre deux temps HMS
//-----------------------------------------------------------------------------
double XTime::HMSAdd(double HMSDec1, double HMSDec2)
{
	double s1 = HMSDecToS(HMSDec1);
	double s2 = HMSDecToS(HMSDec2);
	double t = s1 + s2;
	uint32_t h = 0, m = 0;
	h = (uint32_t)floor(fabs(t) / 3600.);	
	m = (uint32_t)floor((fabs(t) - h * 3600.) / 60.);
	double s = fabs(t) - h * 3600. - m * 60.;
	double hms = h * 10000 + m * 100 + s;
	if ( t < 0.)
		hms *= -1.;
	return hms;
}

//-----------------------------------------------------------------------------
// Chaine explicite pour une heure HMSDec (par ex : 122305.3 -> 12h23m05.3s)
//-----------------------------------------------------------------------------
std::string XTime::HMSDecToString(double HMSDec, uint16_t nbDec)
{
	if (HMSDec < 0.)
		return (std::string)"Heure inconnue";
	int heure = floor(HMSDec * 0.0001);
	int min   = floor((HMSDec-heure*10000.)*0.01); 
	double sec = HMSDec - heure*10000 - min*100;

	char chaine[80], format[80];
	sprintf(format, "%%02dh %%02dm %%.%dlf", (int)nbDec);
	sprintf(chaine, format, heure, min, sec);
	return (std::string)chaine;
}

//-----------------------------------------------------------------------------
// Chaine explicite pour une heure HMSDec (par ex : 122305 -> 12h23m05)
//-----------------------------------------------------------------------------
std::string XTime::HMSToString(uint32_t HMS)
{
	int heure = floor((double)HMS * 0.0001);
	int min   = floor(((double)HMS-heure*10000.)*0.01); 
	int sec = HMS - heure*10000 - min*100;

	char chaine[80];
	if (sec != 0)
		sprintf(chaine,"%02dh %02dm %02d", heure, min, sec);
	else
		sprintf(chaine,"%02dh %02d", heure, min);
	return (std::string)chaine;
}

//-----------------------------------------------------------------------------
// Chaine explicite pour une date DMY (par ex : 120401 -> 12/04/2001)
//-----------------------------------------------------------------------------
std::string XTime::DMYToString(uint32_t DMY, char sep)
{
	char chaine[80];
	if (DMY == 0) {
		strcpy(chaine, "Date inconnue");
		return (std::string)chaine;
	}

	if ((DMY > 1300) && (DMY < 10000)) {	// On ne connait que l'annee
		sprintf(chaine,"%d", DMY);
		return (std::string)chaine;
	}
	if (DMY < 1300) 	// On connait MMYY
		return MYToString(DMY, sep);		

	int year = 2000;
	if (DMYToY(DMY) > 20)
		year = 1900;
	sprintf(chaine,"%02d%c%02d%c%d", DMYToD(DMY), sep, DMYToM(DMY), sep, year + DMYToY(DMY));
	return (std::string)chaine;
}

//-----------------------------------------------------------------------------
// Chaine explicite pour une date DMYY (par ex : 12042001 -> 12/04/2001)
//-----------------------------------------------------------------------------
std::string XTime::DMYYToString(uint32_t DMYY, char sep)
{
	char chaine[80];
	if (DMYY == 0) {
		strcpy(chaine, "Date inconnue");
		return (std::string)chaine;
	}

	if (DMYY < 10000) {	// On ne connait que l'annee
		sprintf(chaine,"%d", DMYY);
		return (std::string)chaine;
	}

	uint16_t d = DMYYToD(DMYY);
	char dstr[80];
	if ((d > 31)||(d < 1))
		strcpy(dstr,"??");
	else
		sprintf(dstr,"%02d", d);

	uint32_t m = DMYYToM(DMYY);
	char mstr[80];
	if ((m > 12)||(m < 1))
		strcpy(mstr,"??");
	else
		sprintf(mstr,"%02d", m);

	sprintf(chaine,"%s%c%s%c%d", dstr, sep, mstr, sep, DMYYToY(DMYY));
	return (std::string)chaine;
}

//-----------------------------------------------------------------------------
// Chaine explicite pour une date MY (par ex : 0401 -> 04/2001)
//-----------------------------------------------------------------------------
std::string XTime::MYToString(uint32_t MY, char sep)
{
	char chaine[80];
	if (MY == 0) {
		strcpy(chaine, "Date inconnue");
		return (std::string)chaine;
	}
	if (MY >= 10000) 
		return DMYToString(MY, sep);

	if (MY > 1300) {	// On ne connait que l'annee
		sprintf(chaine,"%d", MY);
		return (std::string)chaine;
	}

	int year = 2000;
	if (MYToY(MY) > 20)
		year = 1900;
	sprintf(chaine,"%02d%c%d", MYToM(MY), sep, year + MYToY(MY));
	return (std::string)chaine;
}

//-----------------------------------------------------------------------------
// Conversion d'une date DMY vers une date DMYY (par ex : 120401 ->12042001)
// * maxY represente l'annee de changement de siecle
//-----------------------------------------------------------------------------
uint32_t XTime::DMYToDMYY(uint32_t DMY, uint16_t maxY, uint16_t root)
{
	uint32_t day = DMYToD(DMY);
	uint32_t month = DMYToM(DMY);
	uint32_t year = DMYToY(DMY);
	if (year > maxY)
		year += root;
	else
		year += root + 100;
	return (day * 1000000 + month * 10000 + year);
}
