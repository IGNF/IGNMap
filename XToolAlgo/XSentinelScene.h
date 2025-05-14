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
	enum ViewMode { AOT = 0, B01 = 1, B02 = 2, B03 = 3, B04 = 4, B05 = 5, B06 = 6, B07 = 7, B8A = 8, B09 = 9,
									B11 = 11, B12 = 12, SCL = 13, TCI = 14, WVP = 15, RGB, IRC, NDVI, NDWI, SWIR, URBAN };

	XSentinelScene();
	virtual ~XSentinelScene();

	virtual uint32_t Width() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->Width(); return 0; }
	virtual uint32_t Height() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->Height(); return 0; }
	virtual bool GetGeoref(double* xmin, double* ymax, double* gsd) {
		XFileImage* image = GetActiveImage(); if (image == nullptr) return false; return image->GetGeoref(xmin, ymax, gsd);
	}
	virtual uint16_t NbBits() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->NbBits(); return 0; }
	virtual uint16_t NbSample();
	virtual std::string Format() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->Format(); return ""; }

	virtual int NbByte();
	virtual std::string GetMetadata() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->GetMetadata(); return ""; }
	virtual std::string GetXmlMetadata() { XFileImage* image = GetActiveImage(); if (image != nullptr) return image->GetXmlMetadata(); return ""; }

	virtual bool GetArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area);
	virtual bool GetZoomArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor);

	virtual bool GetRawPixel(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample);
	virtual bool GetRawArea(uint32_t x, uint32_t y, uint32_t w, uint32_t h, float* pix, uint32_t* nb_sample, uint32_t factor = 1);

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
	std::string Projection() const { return m_strProjection; }
	float CutNDVI() const { return m_CutNDVI; }
	float CutNDWI() const { return m_CutNDWI; }
	void CutNDVI(float cut) { if ((cut <= 1.f) && (cut >= -1.f))  m_CutNDVI = cut; }
	void CutNDWI(float cut) { if ((cut <= 1.f) && (cut >= -1.f))  m_CutNDWI = cut; }
	double GetIndex(double X, double Y, ViewMode indexMode);

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
	float m_CutNDVI;
	float m_CutNDWI;
	uint16_t m_Offset;	// Les images Sentinel ont en general un offset de 1000 : la valeur minimale est 1000

	XFileImage* GetActiveImage();
	bool AnalyzeFilename(std::string filename, std::string& proj, std::string& name, std::string& date, 
												std::string& id, std::string& channel, std::string& resol);
	int IndexChannel(std::string channel);

	bool BuildRGBImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
										XFileImage* imageR, XFileImage* imageG, XFileImage* imageB);
	bool BuildIndexImage(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t* area, uint32_t factor,
										XFileImage* imageA, XFileImage* imageB);
	bool BuildRawPixelRGB(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample,
										XFileImage* imageR, XFileImage* imageG, XFileImage* imageB);
	bool BuildRawPixelIndex(uint32_t x, uint32_t y, uint32_t win, double* pix, uint32_t* nb_sample,
													XFileImage* imageA, XFileImage* imageB);

};


#endif // ! XSENTINELSCENE_H
