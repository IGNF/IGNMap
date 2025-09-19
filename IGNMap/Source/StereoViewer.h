//-----------------------------------------------------------------------------
//								SetereoViewer.h
//								===============
//
// Visualisation en stereoscopie par ananglyphes
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 03/09/2025
//-----------------------------------------------------------------------------

#ifndef STEREOVIEWER_H
#define STEREOVIEWER_H

#include <JuceHeader.h>
#include "AppUtil.h"
#include "GeoBase.h"
#include "GeoSearch.h"
#include "../../XTool/XPt2D.h"
#include "../../XTool/XFrame.h"
#include "../../XToolAlgo/XStereoModel.h"
#include "../../XToolAlgo/XOrthoModel.h"
#include "../../XToolImage/XMemRaster.h"


class StereoView : public juce::Component
{
public:
  StereoView();

  void Clear();

  bool OpenImage(std::string filename, bool left, int rot = 1);
  bool OpenOrientation(std::string filename);
  bool OpenCamera(std::string filename);
  bool OpenPseudoOrientation(std::string filename);
  bool OpenProject(std::string filename);
  bool SaveProject(std::string filename);
  void SetPseudoOrientation(const XPt3D& SL, const XPt3D& SR, const XPt3D& Ori, const double& gsd);

  void resized() override;
  void paint(juce::Graphics&) override;
  bool keyPressed(const juce::KeyPress& key) override;
  void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
  void mouseMove(const juce::MouseEvent& event) override;
  void mouseDown(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;
  void mouseDrag(const juce::MouseEvent& event) override;

  static bool DrawAnaglyph(juce::Image& left, juce::Image& right, juce::Image& stereo, bool redLeft = true);

private:
  enum ViewMode { Anaglyph = 0, Split = 1, Left = 2, Right = 3 };
  enum BallonnetShape { Dot = 0, BigDot, Cross, BigCross, XCross, BigXCross };
  enum OrientationType { NoOrientation = 0, StereoModel, OrthoModel };
#if JUCE_MAC || JUCE_IOS
  enum { indexR = 0, indexG = 1, indexB = 2 };
#else
  enum { indexR = 2, indexG = 1, indexB = 0 };
#endif


  XMemRaster m_ImageL;
  XMemRaster m_ImageR;

  XStereoModel    m_Model;
  XOrthoModel     m_Ortho;
  std::string     m_strOrientationFile;
  std::string     m_strCameraFile;
  OrientationType m_OrientationType; // Indique que l'on a une vraie orientation

  juce::Image     m_Image;
  bool            m_bDirty;

  XPt2D     m_BalL;   // Ballonnet image gauche
  XPt2D     m_BalR;   // Ballonnet image droite
  XPt3D     m_Bal;    // Ballonnet en coordonnees terrain
  double    m_dPara;   // Parallaxe du ballonnet
  double    m_dGSD;   // Resolution approchee des images

  bool  m_bRedLeft; // Rouge a gauche
  int   m_ViewMode;
  bool  m_bStereoOnly; // Indique si en bord de zone hors stereo, on affiche ou pas
  bool  m_Restit;

  BallonnetShape  m_BalShape;
  int m_nBalColor;
  juce::Colour m_BalColorL;
  juce::Colour m_BalColorR;
  juce::Image m_BalImage;
  juce::Point<int> m_BalPos;

  int m_ViewX;  // Origine du ViewPort
  int m_ViewY;
  int m_nFactor;  // Facteur d'affichage
  int m_nGamma;   // Gamma d'affichage
  GeoSearch m_GeoSearch;

  bool					m_bDrag;
  juce::Point<float>  m_StartPt;
  juce::Point<float>  m_DragPt;

  void UpdateImage();
  bool DrawStereo(juce::Graphics&);
  bool DrawSplit(juce::Graphics&);
  bool DrawMono(juce::Graphics&, bool left);
  void DrawDecoration(juce::Graphics&);
  void Update();

  void Component2Viewport(int& x, int& y);
  void Viewport2Component(int& x, int& y);
  void Component2Image(bool left, int& x, int& y);
  void Image2Component(bool left, int& x, int& y);
  bool Ground2Image(XPt3D P, XPt2D& uL, XPt2D& uR);
  double Image2Ground(XPt2D uL, XPt2D uR, XPt3D& PL, XPt3D& PR);

  void GoToPix(int x, int y);
  void SetBallonnet(double x, double y, double z);
  void SetBallonnet() { SetBallonnet(m_Bal.X, m_Bal.Y, m_Bal.Z); }
  void DrawBallonnet(juce::Graphics&, int u, int v, juce::Colour& color);
  void SetApproximatePosition(double overlap = 0.6);
  void SetCenterPosition();
  void SetOrthoPosition();
  void SetZBallonnet();
  void Correlation();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoView)
};

//==============================================================================
// StereoViewer : fenetre container
//==============================================================================
class StereoViewer : public ToolWindow {
public:
  StereoViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
    juce::ActionListener* listener, XGeoBase* base)
    : ToolWindow(name, backgroundColour, requiredButtons)
  {
    setResizable(true, true);
    setAlwaysOnTop(false);
    //m_Stereo.SetBase(base);
    //m_Stereo.addActionListener(listener);
    setContentOwned(&m_Stereo, true);
    setResizeLimits(400, 450, 10000, 10000);
  }

  void SetTarget(const double& X, const double& Y, const double& Z) override { /*m_Stereo.SetTarget(X, Y, Z);*/ }
  void SetSelection(void*) override { ; }
  bool OpenImage(std::string filename, bool left, int rot = 1) { return m_Stereo.OpenImage(filename, left, rot); }
  void SetPseudoOrientation(XPt3D SL, XPt3D SR, XPt3D Ori, double gsd) { m_Stereo.SetPseudoOrientation(SL, SR, Ori, gsd); }

private:
  StereoView		m_Stereo;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoViewer)
};


#endif //STEREOVIEWER_H