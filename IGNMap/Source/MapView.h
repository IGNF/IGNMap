//-----------------------------------------------------------------------------
//								MapView.h
//								=========
//
// Composant pour la visualisation d'objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 06/01/2023
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>

#include "MapThread.h"
#include "../../XToolVector/XAnnotation.h"

class XGeoBase;

//==============================================================================
/*
*/
class MapView : public juce::Component, private juce::Timer, public juce::ActionBroadcaster, public juce::Thread::Listener
{
public:
  MapView(juce::String name);
  ~MapView() override;
  void Clear();

  void SetFrame(XFrame F);
  void ZoomWorld();
  void ZoomLevel();
  void ZoomScale(double scale);
  void ZoomGsd(double gsd);
  void ZoomFrame(const XFrame& F, double buffer = 0.);
  void CenterView(const double& X, const double& Y, double scale = -1., bool notification = true);
  void Pixel2Ground(double& X, double& Y);
  void Ground2Pixel(double& X, double& Y);
  XFrame Pixel2Ground(const double& Xcenter, const double& Ycenter, const double& nbpix);
  void SetGeoBase(XGeoBase* base) { m_MapThread.stopThread(-1); m_GeoBase = base; resized(); }
  void StopThread() { m_MapThread.signalThreadShouldExit(); if (m_MapThread.isThreadRunning()) m_MapThread.stopThread(-1);}
  void RenderMap(bool overlay = true, bool raster = true, bool dtm = true, bool vector = true, bool las = true, bool totalUpdate = false);
  void SelectFeatures(juce::Point<int>);
  void SelectFeatures(const double& X0, const double& Y0, const double& X1, const double& Y1);
  void Update3DView(const double& X0, const double& Y0, const double& X1, const double& Y1);
  void DrawDecoration(juce::Graphics&, float deltaX = 0.f, float deltaY = 0.f);
  void DrawAnnotation(XAnnotation* annot, juce::Graphics&, float deltaX = 0.f, float deltaY = 0.f);
  void DrawAllAnnotations(juce::Graphics&, float deltaX = 0.f, float deltaY = 0.f);
  double ComputeCartoScale(double cartoscale = 0.);
  void SetTarget(const XPt3D& P, bool notify = true);
  XPt3D GetTarget() const { return m_Target; }
  void DrawTarget(juce::Graphics&, float deltaX = 0.f, float deltaY = 0.f);
  void DrawFrames(juce::Graphics&, int deltaX = 0, int deltaY = 0);
  XFrame GetFrame() const { return m_Frame; }
  XFrame GetSelectionFrame() const { return m_SelectionFrame; }
  XFrame GetViewFrame() const { return m_MapThread.Frame();}
  double GetGsd() const { return m_dScale; }
  juce::Image GetSelImage() const { return m_SelImage; }
  juce::Image GetTargetImage() const { return m_TargetImage; }
  std::vector<XAnnotation>* GetAnnot() { return &m_Annot; }
  void SetSelectionFrame(const XFrame& F) { m_SelectionFrame = F; }

  void paint(juce::Graphics&) override;
  void exitSignalSent() override { repaint(); }
  void resized() override;
  void mouseDown(const juce::MouseEvent& event) override;
  void mouseMove(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;
  void mouseDrag(const juce::MouseEvent& event) override;
  void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
  void mouseDoubleClick(const juce::MouseEvent& event) override;
  bool keyPressed(const juce::KeyPress& key) override;

  enum MouseMode { Move = 1, Select = 2, Zoom = 3, Select3D = 4, Polyline = 5, Polygone = 6, Rectangle = 7, Text = 8};
  void SetMouseMode(MouseMode mode);
  void SetModeCursor();

private:
  juce::String  m_strName;
  XFrame        m_Frame;
  double				m_dX0;
  double				m_dY0;
  double        m_dScale;
  bool					m_bDrag;
  bool          m_bZoom;
  bool          m_bSelect;
  bool          m_bDrawing;
  double        m_dX;
  double        m_dY;
  double        m_dZ;
  int           m_nMouseMode;
  juce::Point<float>  m_StartPt;
  juce::Point<float>  m_DragPt;
  juce::Image   m_Image;    // Image de la vue
  juce::Image   m_SelImage; // Partie de l'image de la vue selectionnee
  juce::Image   m_TargetImage;
  MapThread     m_MapThread;
  XGeoBase*     m_GeoBase;
  XAnnotation   m_Annotation; // Annotation en cours d'edition
  std::vector<XAnnotation>  m_Annot;  // Liste des annotations
  XPt3D         m_Target;     // Point cible
  XFrame        m_SelectionFrame;  // Rectangle de selection
  XFrame        m_3DFrame;         // Rectangle de vue 3D
  uint64_t      m_nFrameCounter;

  void timerCallback() override { repaint(); }

  void EndMouseAction();
  void AddAnnotationPoint(juce::Point<float>&);
  void CloseAnnotation();
  void SaveImage() { juce::Graphics imaG(m_Image); m_MapThread.Draw(imaG);}

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MapView)
};
