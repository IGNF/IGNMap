//-----------------------------------------------------------------------------
//								XSentinelScene.h
//								================
//
// Acces aux images Sentinel 2
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 18/12/2024
//-----------------------------------------------------------------------------

#ifndef  XSENTINELSCENE_H
#define XSENTINELSCENE_H

#include <vector>
#include "../XToolImage/XFileImage.h"

class XSentinelScene : public XFileImage {
public:
	enum ViewMode { RGB = 1, IRC, NDVI, NDWI, SWIR, URBAN };

	XSentinelScene();
	virtual ~XSentinelScene();

	virtual uint32_t Width() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->Width(); return 0; }
	virtual uint32_t Height() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->Height(); return 0; }
	virtual bool GetGeoref(double* xmin, double* ymax, double* gsd) {
		XFileImage* image = GetActiveImage(); if (image == nullptr) return false; return image->GetGeoref(xmin, ymax, gsd);
	}

	virtual int NbByte() { return 3; }
	virtual std::string GetMetadata() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->GetMetadata(); return ""; }
	virtual std::string GetXmlMetadata() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->GetXmlMetadata(); return ""; }

	virtual bool GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	virtual bool GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

	bool ImportImage(std::string path, std::string filename);
	void SetViewMode(ViewMode mode, bool adjustResol = false);
	ViewMode GetViewMode() const { return m_ViewMode; }
	bool CheckViewMode(XFileImage* &imaA, XFileImage*& imaB, XFileImage*& imaC) const;
	int NbImages() const;
	void SetActiveResolution(int resol) { 
		if (resol == 10) m_nResol = 10; if (resol == 20) m_nResol = 20;  if (resol == 60) m_nResol = 60;}
	double GetActiveResolution() const { return m_nResol; }
	bool SentinelAttributes(std::vector<std::string>& V);
	std::string Date() const { return m_strDate; }

protected:
	XFileImage* m_Ima10m[16];	// Images a 10m
	XFileImage* m_Ima20m[16];	// Images a 20m
	XFileImage* m_Ima60m[16];	// Images a 60m
	// On stocke les images dans l'ordre des canaux avec :
	// 0 : AOT, 1 : B01, 2 : B02 ... 13 : SCL, 14 : TCI, 15 : WVP
	std::string	m_strProjection;	// T30
	std::string m_strName;				// TYQ
	std::string m_strDate;				// 20220306
	std::string m_strId;					// T104819

	int m_nResol;	// Resolution active : 10, 20 ou 60m;
	ViewMode m_ViewMode;

	XFileImage* GetActiveImage();
	bool AnalyzeFilename(std::string filename, std::string& proj, std::string& name, std::string& date, 
												std::string& id, std::string& channel, std::string& resol);
	int IndexChannel(std::string channel);

	bool BuildRGBImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
										XFileImage* imageR, XFileImage* imageG, XFileImage* imageB);
	bool BuildIndexImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
										XFileImage* imageA, XFileImage* imageB);

};


#endif // ! XSENTINELSCENE_H
