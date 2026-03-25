//-----------------------------------------------------------------------------
//								ExportDtmDlg.cpp
//								================
//
// Dialogue d'options pour exporter les fichiers LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/03/2026
//-----------------------------------------------------------------------------

#include "ExportDtmDlg.h"
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoLayer.h"
#include "../../XToolAlgo/XStlWriter.h"
#include "../../XToolImage/XTiffWriter.h"
#include "../../XToolGeod/XGeoPref.h"
#include "GeoBase.h"

//==============================================================================
// Constructeur
//==============================================================================
ExportDtmDlg::ExportDtmDlg(XGeoBase* base, double xmin, double ymin, double xmax,double ymax, 
                            double gsd, juce::ActionListener* listener)
                          : m_FrameCmp(xmin, ymin, xmax, ymax)
{
  m_Base = base;
  if (listener != nullptr)
    addActionListener(listener);

  addAndMakeVisible(m_FrameCmp);
  m_FrameCmp.setBounds(0, 0, 400, 200);

  m_Gsd.SetEditor(juce::translate("GSD :"), juce::String(gsd, 2), FieldEditor::Double, 50, 80);
  addAndMakeVisible(m_Gsd);
  m_Gsd.setBounds(100, 130, 130, 24);

  addAndMakeVisible(m_btnStl);
  m_btnStl.setButtonText(juce::translate("STL Format"));
  m_btnStl.setBounds(160, 160, 120, 24);
  m_btnStl.addListener(this);

  m_StlScale.SetEditor(juce::translate("Planimetric scale :"), "10000", FieldEditor::Uint, 130, 50);
  addChildComponent(m_StlScale);
  m_StlScale.setBounds(10, 190, 190, 24);

  m_StlZFactor.SetEditor(juce::translate("Z factor :"), "1", FieldEditor::Uint, 130, 50);
  addChildComponent(m_StlZFactor);
  m_StlZFactor.setBounds(200, 190, 190, 24);

  m_StlStep.SetEditor(juce::translate("Step (mm) :"), "1", FieldEditor::Uint, 130, 50);
  addChildComponent(m_StlStep);
  m_StlStep.setBounds(10, 220, 190, 24);

  m_StlZ0.SetEditor(juce::translate("Zmin :"), "0", FieldEditor::Double, 130, 50);
  addChildComponent(m_StlZ0);
  m_StlZ0.setBounds(200, 220, 190, 24);

  addChildComponent(m_btnStlEdge);
  m_btnStlEdge.setButtonText(juce::translate("View borders"));
  m_btnStlEdge.setBounds(10, 250, 120, 24);
  m_btnStlEdge.setToggleState(true, juce::NotificationType::dontSendNotification);
  addChildComponent(m_btnStlZAuto);
  m_btnStlZAuto.setButtonText(juce::translate("Automatic Zmin"));
  m_btnStlZAuto.setBounds(200, 250, 120, 24);
  m_btnStlZAuto.setToggleState(true, juce::NotificationType::dontSendNotification);
  m_btnStlZAuto.addListener(this);

  addAndMakeVisible(m_btnExport);
  m_btnExport.setButtonText(juce::translate("Export"));
  m_btnExport.setBounds(160, 280, 80, 30);
  m_btnExport.addListener(this);
}

//==============================================================================
// Validation de l'export
//==============================================================================
void ExportDtmDlg::buttonClicked(juce::Button* button)
{
  if (button == &m_btnStl) {
    bool flag = m_btnStl.getToggleState();
    m_Gsd.setVisible(!flag);
    m_StlScale.setVisible(flag);
    m_StlZFactor.setVisible(flag);
    m_StlStep.setVisible(flag);
    m_StlZ0.setVisible(!m_btnStlZAuto.getToggleState());
    m_btnStlEdge.setVisible(flag);
    m_btnStlZAuto.setVisible(flag);
    return;
  }
  if (button == &m_btnStlZAuto) {
    m_StlZ0.setVisible(!m_btnStlZAuto.getToggleState());
    return;
  }
  if (button == &m_btnExport) {
    if (m_btnStl.getToggleState())
      m_strFilename = AppUtil::SaveFile("ExportStlFile", juce::translate("File to save"), "*.stl");
    else
      m_strFilename = AppUtil::SaveFile("ExportDtmFile", juce::translate("File to save"), "*.tif");
    if (m_strFilename.isEmpty())
      return;
    bool flag;
    if (m_btnStl.getToggleState())
      flag = ExportStl();
    else
      flag = Export();
    if (flag) {
      juce::Component* parent = getParentComponent();
      if (parent != nullptr)
        delete parent;
    }
    return;
  }
}

//==============================================================================
// Export STL
//==============================================================================
bool ExportDtmDlg::ExportStl()
{
  if (m_Base == nullptr)
    return false;
  XGeoLayer* layer = m_Base->Layer("DTM");
  if (layer == nullptr)
    return false;

  XFrame F;
  m_FrameCmp.GetFrame(F.Xmin, F.Ymin, F.Xmax, F.Ymax);;
  if (F.IsEmpty())
    return false;
  double scale = m_StlScale.DoubleValue();
  double exag = m_StlZFactor.DoubleValue();
  double pas = m_StlStep.DoubleValue() * 0.001;  // en m
  bool bords = m_btnStlEdge.getToggleState();
  bool zmin_auto = m_btnStlZAuto.getToggleState();
  double zmin_val = m_StlZ0.DoubleValue();

  double W = F.Width() / scale;
  uint32_t nbW = W / pas;
  double H = F.Height() / scale;
  uint32_t nbH = H / pas;
  double resol = pas * scale;

  float* T = new(std::nothrow) float[nbW * nbH];
  if (T == nullptr) {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, juce::translate("STL Export"),
      juce::translate("Unable to allocate memory ! The DTM is too big !"), "OK");
    return false;
  }

  juce::MouseCursor::showWaitCursor();
  GeoTools::ComputeZGrid(m_Base, T, nbW, nbH, &F);

  float z, zmin = 9999, zmax = T[0];
  float* ptr = T;
  for (uint32_t i = 0; i < nbH; i++)
    for (uint32_t j = 0; j < nbW; j++) {
      z = *ptr;
      ptr++;
      if (z < 0) continue;
      zmin = XMin(zmin, z);
      zmax = XMax(zmax, z);
    }

  if ((!zmin_auto) && (zmin_val > zmin)) {
    juce::MouseCursor::hideWaitCursor();
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, juce::translate("STL Export"), 
                juce::translate("The Zmin field is higher than the Zmin of the DTM !"), "OK");
    return false;
  }

  std::ofstream out;
  std::string stloutfile = m_strFilename.toStdString().c_str();
  out.open(stloutfile.c_str(), std::ios_base::out | std::ios_base::binary);

  XStlWriter stl;
  if (zmin_auto)
    stl.SetOptions(scale, exag, pas, zmin - (zmax - zmin) * 0.1);
  else
    stl.SetOptions(scale, exag, pas, zmin_val);
  stl.WriteStlFile(&out, T, nbW, nbH, bords);

  delete[] T;
  out.close();
  juce::MouseCursor::hideWaitCursor();

  juce::File file(m_strFilename);
  file.revealToUser();
  return true;
}

//==============================================================================
// Export TIFF 32 bits
//==============================================================================
bool ExportDtmDlg::Export()
{
  if (m_Base == nullptr)
    return false;
  XGeoLayer* layer = m_Base->Layer("DTM");
  if (layer == nullptr)
    return false;

  XFrame F;
  m_FrameCmp.GetFrame(F.Xmin, F.Ymin, F.Xmax, F.Ymax);;
  if (F.IsEmpty())
    F = layer->Frame();
  double gsd = m_Gsd.DoubleValue();

  uint32_t W = (uint32_t)XRint(F.Width() / gsd);
  uint32_t H = (uint32_t)XRint(F.Height() / gsd);

  float* T = new(std::nothrow) float[W * H];
  if (T == nullptr) {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, juce::translate("DTM Export"),
      juce::translate("Unable to allocate memory ! The DTM is too big !"), "OK");
    return false;
  }

  juce::MouseCursor::showWaitCursor();
  GeoTools::ComputeZGrid(m_Base, T, W, H, &F);

  std::string outfile = m_strFilename.toStdString().c_str();
  XGeoPref pref;
  XTiffWriter tiff;
  tiff.SetGeoTiff(F.Xmin, F.Ymax, gsd, XGeoProjection::EPSGCode(pref.Projection()));
  tiff.Write(outfile.c_str(), W, H, 1, 32, (uint8_t*)T);

  delete[] T;
  juce::MouseCursor::hideWaitCursor();

  juce::File file(m_strFilename);
  file.revealToUser();
  return true;
}
