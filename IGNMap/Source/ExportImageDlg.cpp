//-----------------------------------------------------------------------------
//								ExportImageDlg.h
//								================
//
// Dialogue d'options pour exporter les images
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 17/01/2024
//-----------------------------------------------------------------------------

#include "ExportImageDlg.h"
#include "AppUtil.h"
#include "../../XToolImage/XTiffWriter.h"

//==============================================================================
// Constructeur
//==============================================================================
ExportImageDlg::ExportImageDlg(XGeoBase* base, double xmin, double ymin, double xmax, 
                                               double ymax, double gsd, juce::ActionListener* listener) :
                m_MapThread("MapThread"), m_FrameCmp(xmin, ymin, xmax, ymax)
{
  m_Base = base;
  if (listener != nullptr)
    addActionListener(listener);

  addAndMakeVisible(m_FrameCmp);
  m_FrameCmp.setBounds(0, 0, 400, 200);
  
  addAndMakeVisible(m_lblGsd);
  m_lblGsd.setBounds(100, 130, 50, 24);
  m_lblGsd.setText(juce::translate("GSD : "), juce::NotificationType::dontSendNotification);
  addAndMakeVisible(m_edtGsd);
  m_edtGsd.setBounds(160, 130, 80, 24);
  m_edtGsd.setText(juce::String(gsd, 2));

  addAndMakeVisible(m_btnExport);
  m_btnExport.setButtonText(juce::translate("Export"));
  m_btnExport.setBounds(160, 170, 80, 30);
  m_btnExport.addListener(this);

  m_progressBar = new juce::ProgressBar(m_dProgress);
  addAndMakeVisible(m_progressBar);
  m_progressBar->setBounds(100, 210, 200, 30);

  addAndMakeVisible(m_edtFilename);
  m_edtFilename.setBounds(10, 260, 380, 24);
  m_edtFilename.setText("");
  m_edtFilename.setReadOnly(true);

  getWantsKeyboardFocus();
}

//==============================================================================
// Destructeur
//==============================================================================
ExportImageDlg::~ExportImageDlg()
{
  delete m_progressBar;
  AppUtil::DownloadNbTry = 10;
  stopTimer();
  m_MapThread.stopThread(5000);
}

//==============================================================================
// Clic des boutons
//==============================================================================
void ExportImageDlg::buttonClicked(juce::Button* button)
{
  if (m_Base == nullptr)
    return;
  if (button == &m_btnExport)
    Export();
}

//==============================================================================
// Validation de l'export
//==============================================================================
void ExportImageDlg::Export()
{
  if (m_btnExport.getButtonText() == juce::translate("Cancel")) { // Annulation
    stopTimer();
    m_MapThread.signalThreadShouldExit();
    if (m_MapThread.isThreadRunning())
      m_MapThread.stopThread(-1);
    m_dProgress = 0.;
    m_btnExport.setButtonText(juce::translate("Export"));
    juce::File file(m_strFilename);
    file.deleteFile();
    AppUtil::DownloadNbTry = 10;
    return;
  }
  double xmin = 0., ymin = 0., xmax = 0., ymax = 0.;
  m_FrameCmp.GetFrame(xmin, ymin, xmax, ymax);
  double gsd = m_edtGsd.getText().getDoubleValue();
  m_nW = (int)((xmax - xmin) / gsd);
  m_nH = (int)((ymax - ymin) / gsd);
  m_dX0 = xmin;
  m_dY0 = ymax;
  m_dGSD = gsd;
  if ((m_nW <= 0) || (m_nH <= 0) || (m_dGSD < 0.1))
    return;
  m_dProgress = 0.;

  m_btnExport.setButtonText(juce::translate("Cancel"));
  sendActionMessage("SetSelectionFrame:" + juce::String(xmin) + ":" + juce::String(xmax) + ":" +
    juce::String(ymin) + ":" + juce::String(ymax));

  // Creation du fichier TIFF
  m_strFilename = AppUtil::SaveFile("ExportImageFile", juce::translate("File to save"), "*.tif");
  if (m_strFilename.isEmpty())
    return;
  m_edtFilename.setText(m_strFilename, false);
  XTiffWriter tiff;
  tiff.SetGeoTiff(m_dX0, m_dY0, m_dGSD);
  tiff.Write(m_strFilename.toStdString().c_str(), m_nW, m_nH, 3, 8);

  m_MapThread.signalThreadShouldExit();
  if (m_MapThread.isThreadRunning()) {
    if (!m_MapThread.stopThread(-1))
      return;
  }
  m_nNumThread = 0;

  startTimerHz(10);
  StartNextThread();
}

//==============================================================================
// Lancement d'un thread de dessin
//==============================================================================
void ExportImageDlg::StartNextThread()
{
  int nbLine = 1000;
  AppUtil::DownloadNbTry = 100;
  if (m_nNumThread * nbLine >= m_nH) {
    m_dProgress = 1.;
    stopTimer();
    AppUtil::DownloadNbTry = 10;
    m_btnExport.setButtonText(juce::translate("Export"));
    juce::File file(m_strFilename);
    file.revealToUser();
    juce::Component* parent = getParentComponent();
    if (parent != nullptr)
      delete parent;
    return;
  }
 
  double x0 = m_dX0;
  double y0 = m_dY0 - nbLine * m_dGSD * m_nNumThread;
  int32_t w = m_nW;
  int32_t h = XMin(nbLine, m_nH - m_nNumThread * nbLine);
  m_dProgress = (((double)m_nNumThread * nbLine / (double)m_nH));

  m_MapThread.SetGeoBase(m_Base);
  m_MapThread.SetWorld(x0, y0, m_dGSD, w, h, true);
  m_MapThread.SetUpdate(true, true, true, true, true);
  m_MapThread.startThread();
  m_nNumThread++;
}

//==============================================================================
// Callback du Timer
//==============================================================================
void ExportImageDlg::timerCallback()
{
  if (m_MapThread.isThreadRunning())
    return;
 	// Sur Mac, on n'a que des images ARGB
  juce::Image image(juce::Image::PixelFormat::ARGB, m_MapThread.ImageWidth(), m_MapThread.ImageHeight(), true);
  { // Scope pour Direct2D
    juce::Graphics g(image);
    m_MapThread.Draw(g, 0, 0, false);
  }

  juce::Image::BitmapData bitmap(image, juce::Image::BitmapData::readOnly);

  std::ofstream file;
  file.open(m_strFilename.toStdString().c_str(), std::ios::out | std::ios::binary | std::ios::app);
  file.seekp(0, std::ios_base::end);
  for (int i = 0; i < (int)m_MapThread.ImageHeight(); i++) {
    uint8_t* line = bitmap.getLinePointer(i);
    XBaseImage::SwitchARGB2BGR(line, m_MapThread.ImageWidth());
    file.write((char*)line, m_MapThread.ImageWidth() * 3);
   }
  file.close();

  StartNextThread();
}
