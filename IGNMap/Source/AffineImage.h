//-----------------------------------------------------------------------------
//								AffineImage.h
//								=============
//
// Gestion des images georeferencees avec une affinite (par exemple cliche aerien)
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/05/2024
//-----------------------------------------------------------------------------
#pragma once

#include <JuceHeader.h>
#include <string>
#include "GeoBase.h"
#include "../../XTool/XFrame.h"
#include "../../XTool/XTransfo.h"
#include "../../XToolAlgo/XToneMapper.h"

class XTransfoRotation : public XTransfo {
protected:
  XPt2D       m_C;    // Coordonnees terrain du centre de l'image
  XPt2D       m_O;    // Centre de rotation de l'image en coordoonnes image 
  double      m_dGsd; // Resolution terrain de l'image
  uint32_t    m_U0, m_V0;
  uint32_t    m_Factor;
  double      cosT, sinT;
  XFrame      m_Ff;

public:
  XTransfoRotation(double angle, XPt2D C, double gsd, XPt2D O, uint32_t U0, uint32_t V0, uint32_t factor)
  {
    cosT = cos(angle); sinT = sin(angle);
    m_C = C; m_dGsd = gsd;  
    m_U0 = U0; m_V0 = V0; m_Factor = factor;
    m_O = O;
  }
  void SetEndFrame(const XFrame& f) { m_Ff = f; }		// Cadre de l'image d'arrivee
  void Direct(double x, double y, double* u, double* v) override;
  void Dimension(int w, int h, int* wout, int* hout) override;
};

class RotationImage : public GeoInternetImage {
protected:
  XFileImage  m_Image;
  XPt2D       m_C;    // Coordonnees terrain du centre de l'image
  XPt2D       m_O;    // Centre de rotation de l'image en coordoonnes image
  double      m_dGsd; // Resolution terrain de l'image
  double      m_dRot; // Rotation en radians
  uint32_t    m_nW;   // Largeur de l'image
  uint32_t    m_nH;   // Hauteur de l'image
  XToneMapper::ToneMappingInt* m_ToneMapper;

  void Ground2Image(double x, double y, double* u, double* v);
  void Image2Ground(double u, double v, double* x, double* y);

public:
  RotationImage() { m_dGsd = 1.; m_dRot = 0.; m_nW = 0; m_nH = 0; m_ToneMapper = nullptr; }
  virtual ~RotationImage() { if (m_ToneMapper != nullptr) delete m_ToneMapper; }

  virtual	bool ReadAttributes(std::vector<std::string>& V);
  inline virtual double Resolution() const { return m_dGsd; }
  inline double Rotation() const { return m_dRot; }
  virtual XPt2D Centroide() { return m_C; }
  bool SetPosition(double X, double Y, double gsd) { m_C = XPt2D(X, Y); m_dGsd = gsd; return ComputeFrame();}
  bool SetImageCenter(double X, double Y) { m_O = XPt2D(X, Y);  return ComputeFrame(); }
  bool SetRotation(double rot) { m_dRot = rot; return ComputeFrame(); }
  bool AddRotation(double rot) { m_dRot += rot; return ComputeFrame(); }
  bool ComputeFrame();

  bool AnalyzeImage(std::string path);
  uint32_t GetImageW() const { return m_nW; }
  uint32_t GetImageH() const { return m_nH; }
  virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
  void AddToneMapper() { if (m_ToneMapper == nullptr) m_ToneMapper = new XToneMapper::ToneMappingInt; }
  void RemoveToneMapper() {
    if (m_ToneMapper != nullptr) { delete m_ToneMapper; m_ToneMapper = nullptr; }
  }
  void SetToneMapperPower(int power) {
    bool flag = true;
    if (power == 0) flag = false;
    if (m_ToneMapper != nullptr) {
      m_ToneMapper->set_enabled(0, flag);
      m_ToneMapper->set_power(0, (float)power);
    }
  }
  int GetToneMapperPower() { if (m_ToneMapper != nullptr) return (int)m_ToneMapper->get_power(0); return 0; }
  void SetToneMapperSharpness(int sharp) { 
    if (m_ToneMapper != nullptr) {
      bool flag = true;
      if (sharp == 0) flag = false;
      m_ToneMapper->set_unsharp_mask_enabled(flag);
      m_ToneMapper->set_unsharp_mask_power((float)sharp);
    }
  }
  int GetToneMapperSharpness() { if (m_ToneMapper != nullptr) return (int)m_ToneMapper->get_unsharp_mask_power(); return 0; }
};