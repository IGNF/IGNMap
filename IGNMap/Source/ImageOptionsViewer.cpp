//-----------------------------------------------------------------------------
//								ImageOptionsViewer.cpp
//								======================
//
// Visulisation des options d'une image
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/09/2023
//-----------------------------------------------------------------------------

#include "ImageOptionsViewer.h"
#include "GeoBase.h"
#include "../../XTool/XGeoBase.h"

//==============================================================================
// ImageOptionsViewer : constructeur
//==============================================================================
ImageOptionsViewer::ImageOptionsViewer()
{
  m_Image = nullptr;

  addAndMakeVisible(m_lblImageName);
  m_lblImageName.setText(juce::translate("Name :"), juce::dontSendNotification);

  addAndMakeVisible(m_txtImageName);
  m_txtImageName.setReadOnly(true);

  addAndMakeVisible(m_txtMetadata);
  m_txtMetadata.setReadOnly(true);
  m_txtMetadata.setMultiLine(true);

  addAndMakeVisible(m_lblRGBChannels);
  m_lblRGBChannels.setText(juce::translate("RGB channels :"), juce::dontSendNotification);
  addAndMakeVisible(m_cbxRChannel);
  addAndMakeVisible(m_cbxGChannel);
  addAndMakeVisible(m_cbxBChannel);
  m_cbxRChannel.addListener(this);
  m_cbxGChannel.addListener(this);
  m_cbxBChannel.addListener(this);

  addAndMakeVisible(m_btnPalette);
  m_btnPalette.addListener(this);
  m_btnPalette.setButtonText(juce::translate("Palette"));
  addAndMakeVisible(m_txtPalette);
  m_txtPalette.setMultiLine(true);

  m_tblPixels.setModel(&m_PixModel);
  m_tblPixels.setTooltip(juce::translate(" Double-click for copying"));
  for (int i = 1; i <= (int)(m_PixModel.WinSize * 2 + 1); i++)
    m_tblPixels.getHeader().addColumn(juce::String(i - (int)m_PixModel.WinSize - 1), i, 20, 30, -1, 
      juce::TableHeaderComponent::ColumnPropertyFlags::notSortable| juce::TableHeaderComponent::ColumnPropertyFlags::visible);
  m_tblPixels.getHeader().setStretchToFitActive(true);
  m_tblPixels.autoSizeAllColumns();
  //m_tblPixels.setHeaderHeight(0);
  addAndMakeVisible(m_tblPixels);

  addAndMakeVisible(m_sldLine);
  m_sldLine.setSliderStyle(juce::Slider::LinearBar);
  m_sldLine.setNumDecimalPlacesToDisplay(0);
  m_sldLine.setTextValueSuffix(" line");
  m_sldLine.addListener(this);

  addAndMakeVisible(m_sldColumn);
  m_sldColumn.setSliderStyle(juce::Slider::LinearBar);
  m_sldColumn.setNumDecimalPlacesToDisplay(0);
  m_sldColumn.setTextValueSuffix(" col");
  m_sldColumn.addListener(this);
}

//==============================================================================
// Redimensionnement du composant
//==============================================================================
void ImageOptionsViewer::resized()
{
  juce::Grid grid;

  grid.rowGap = juce::Grid::Px(20);
  grid.columnGap = juce::Grid::Px(10);

  using Track = juce::Grid::TrackInfo;

  grid.templateRows = { Track(juce::Grid::Px(25)) };
  grid.templateColumns = { Track(juce::Grid::Fr(1)), Track(juce::Grid::Fr(1)), Track(juce::Grid::Fr(1)), 
                           Track(juce::Grid::Fr(1)), Track(juce::Grid::Fr(1)), Track(juce::Grid::Fr(1)) };

  grid.autoColumns = Track(juce::Grid::Fr(1));
  grid.autoRows = Track(juce::Grid::Fr(1));
  grid.autoFlow = juce::Grid::AutoFlow::row;

  grid.items.addArray({ juce::GridItem(m_lblImageName), juce::GridItem(m_txtImageName).withArea(1, 2, 2, 7),
                        juce::GridItem(m_txtMetadata).withArea(2, 1, 4, 7),
                        juce::GridItem(m_lblRGBChannels).withArea(4, 1, 5, 3), juce::GridItem(m_cbxRChannel).withArea(5, 1),
                        juce::GridItem(m_cbxGChannel).withArea(5, 2), juce::GridItem(m_cbxBChannel).withArea(5, 3),
                        juce::GridItem(m_btnPalette).withArea(4, 4, 5, 7), juce::GridItem(m_txtPalette).withArea(5, 4, 8, 7),
                        juce::GridItem(m_tblPixels).withArea(8, 1, 13, 7),
                        juce::GridItem(m_sldColumn).withArea(13, 1, 14, 4),
                        juce::GridItem(m_sldLine).withArea(13, 4, 14, 7)
    });
  juce::Rectangle<int> R = getLocalBounds();
  R.reduce(5, 5);
  grid.performLayout(R);
}

//==============================================================================
// SetImage : fixe l'image sur laquelle travailler
//==============================================================================
void ImageOptionsViewer::SetImage(GeoFileImage* image)
{
  if (image == nullptr) {
    m_Image = nullptr;
    m_txtImageName.setText("");
    m_txtMetadata.setText("");
    m_txtPalette.setText("");
    return;
  }
  m_Image = image;
  m_txtImageName.setText(image->Filename());
  m_txtMetadata.setText(image->GetMetadata());

  uint8_t r, g, b;
  image->GetRGBChannel(r, g, b);
  m_cbxRChannel.clear(juce::NotificationType::dontSendNotification);
  m_cbxGChannel.clear(juce::NotificationType::dontSendNotification);
  m_cbxBChannel.clear(juce::NotificationType::dontSendNotification);

  for (int i = 0; i < image->NbSample(); i++) {
    m_cbxRChannel.addItem(juce::String(i), i+1);  // Les ID ne peuvent pas etre egaux à 0 !!!
    m_cbxGChannel.addItem(juce::String(i), i+1);
    m_cbxBChannel.addItem(juce::String(i), i+1);
  }
  m_cbxRChannel.setSelectedId(r+1, juce::NotificationType::dontSendNotification);
  m_cbxGChannel.setSelectedId(g+1, juce::NotificationType::dontSendNotification);
  m_cbxBChannel.setSelectedId(b+1, juce::NotificationType::dontSendNotification);

  m_sldLine.setRange(0, m_Image->XFileImage::Height() - 1, 1.);
  m_sldColumn.setRange(0, m_Image->XFileImage::Width() - 1, 1.);
}

//==============================================================================
// SetGeoBase : fixe la base de donnees sur laquelle travailler
//==============================================================================
void ImageOptionsViewer::SetGeoBase(XGeoBase* base)
{
  if (base == nullptr) {
    SetImage(nullptr);
    return;
  }
  GeoFileImage* image = nullptr;
  for (int i = 0; i < base->NbSelection(); i++) {
    XGeoVector* V = base->Selection(i);
    if (V->TypeVector() == XGeoVector::Raster) {
      image = dynamic_cast<GeoFileImage*>(V);
      if (image != nullptr)
        break;
    }
  }
  SetImage(image);
}

//==============================================================================
// Modification des valeurs des ComboBox
//==============================================================================
void ImageOptionsViewer::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
  if (m_Image == nullptr)
    return;
  if ((comboBoxThatHasChanged == &m_cbxRChannel)||(comboBoxThatHasChanged == &m_cbxGChannel)|| 
      (comboBoxThatHasChanged == &m_cbxBChannel)) {
    int r = m_cbxRChannel.getSelectedId() - 1;
    int g = m_cbxGChannel.getSelectedId() - 1;
    int b = m_cbxBChannel.getSelectedId() - 1;
    m_Image->SetRGBChannel((uint8_t)r, (uint8_t)g, (uint8_t)b);
    sendActionMessage("UpdateRaster");
  }
}

//==============================================================================
// Modification des valeurs des Sliders
//==============================================================================
void ImageOptionsViewer::sliderValueChanged(juce::Slider* slider)
{
  if (m_Image == nullptr)
    return;
  if ((slider == &m_sldLine) || (slider == &m_sldColumn)) {
    SetPixPos((int)m_sldColumn.getValue(), (int)m_sldLine.getValue());
    return;
  }
}

//==============================================================================
// Clic sur un bouton
//==============================================================================
void ImageOptionsViewer::buttonClicked(juce::Button* button)
{
  if (button == dynamic_cast<juce::Button*>(&m_btnPalette)) {
    bool state = m_btnPalette.getToggleState(); // true pour bouton ON
    m_cbxRChannel.setEnabled(!state); 
    m_cbxGChannel.setEnabled(!state); 
    m_cbxBChannel.setEnabled(!state);
    if (m_Image == nullptr)
      return;
    if (state) {
      juce::String val = m_txtPalette.getText();
      juce::StringArray array = juce::StringArray::fromLines(val);
      uint8_t* palette = new uint8_t[m_Image->NbSample() * 3];
      memset(palette, 0, m_Image->NbSample() * 3);
      for (int i = 0; i < array.size(); i++) {
        juce::String line = array[i];
        juce::StringArray row = juce::StringArray::fromTokens(line, ",;", "");
        if (row.size() != 4) continue;
        int index = row[0].getIntValue();
        if (index < m_Image->NbSample()) {
          palette[3 * index] = (uint8_t)row[1].getIntValue();
          palette[3 * index + 1] = (uint8_t)row[2].getIntValue();
          palette[3 * index + 2] = (uint8_t)row[3].getIntValue();
        }
      }
      m_Image->SetPalette(palette);
    }
    else {
      m_Image->SetPalette(nullptr);
    }
    sendActionMessage("UpdateRaster");
  }
}

//==============================================================================
// Fixe la position terrain (clic dans la MapView)
//==============================================================================
void ImageOptionsViewer::SetGroundPos(const double& X, const double& Y)
{
  if (m_Image == nullptr)
    return;
  if (getParentHeight() < 100)  // On suppose que le panneau est ferme
    return;
  double xmin = 0., ymax = 0., gsd = 0.;
  if (!m_Image->GetGeoref(&xmin, &ymax, &gsd))
    return;
  SetPixPos(XRint((X - xmin) / gsd), XRint((ymax - Y) / gsd));
}

//==============================================================================
// Fixe la position pixel dans l'image selectionnee
//==============================================================================
void ImageOptionsViewer::SetPixPos(const int& X, const int& Y)
{
  if (m_Image == nullptr)
    return;
  if ((X < 0) || (Y < 0) || (X >= m_Image->XFileImage::Width()) || (Y >= m_Image->XFileImage::Height())) {
    m_PixModel.ClearPixels();  // Hors image
    m_tblPixels.updateContent();
    m_tblPixels.repaint();
    return;
  }

  if (X <= (int)m_PixModel.WinSize)
    m_PixModel.PixX = m_PixModel.WinSize + 1;
  else
    m_PixModel.PixX = (uint32_t)X;
  if (m_PixModel.PixX >= (m_Image->XFileImage::Width() - m_PixModel.WinSize))
    m_PixModel.PixX = m_Image->XFileImage::Width() - m_PixModel.WinSize -1;

  if (Y <= (int)m_PixModel.WinSize)
    m_PixModel.PixY = m_PixModel.WinSize + 1;
  else
    m_PixModel.PixY = (uint32_t)Y;
  if (m_PixModel.PixY >= (m_Image->XFileImage::Height() - m_PixModel.WinSize))
    m_PixModel.PixY = m_Image->XFileImage::Height() - m_PixModel.WinSize - 1;

  m_PixModel.NbBits = m_Image->NbBits();
  if (!m_PixModel.AllocPixels(m_Image->NbSample()))
    return;

  if (m_Image->GetRawPixel(m_PixModel.PixX, m_PixModel.PixY, m_PixModel.WinSize, m_PixModel.PixValue, &m_PixModel.NbSample)) {
    m_Image->GetRGBChannel(m_PixModel.R_channel, m_PixModel.G_channel, m_PixModel.B_channel);
    m_tblPixels.updateContent();
    m_tblPixels.repaint();
  }

  m_sldColumn.setValue(m_PixModel.PixX, juce::NotificationType::dontSendNotification);
  m_sldLine.setValue(m_PixModel.PixY, juce::NotificationType::dontSendNotification);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void PixelValuesModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
  if (PixValue == nullptr) {
    g.drawText("no data", 0, 0, width, height, juce::Justification::centred);
    return;
  }
  double factor = 1.;
  if (NbBits == 16)
    factor = 256;
  uint8_t red = PixValue[rowNumber * (2 * WinSize + 1) * NbSample + (columnId - 1) * NbSample + R_channel] / factor;
  uint8_t green = PixValue[rowNumber * (2 * WinSize + 1) * NbSample + (columnId - 1) * NbSample + G_channel] / factor;
  uint8_t blue = PixValue[rowNumber * (2 * WinSize + 1) * NbSample + (columnId - 1) * NbSample + B_channel] / factor;
  if (NbSample == 1)
    blue = green = red;

  juce::Colour color(red, green, blue);
  g.setColour(color);
  g.fillRect(0, 0, width, height);
  g.setColour(color.contrasting());
  g.drawText(GetText(rowNumber, columnId), 0, 0, width, height, juce::Justification::centred);
}

//==============================================================================
// Double-clic sur une cellule
//==============================================================================
void PixelValuesModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&)
{
  juce::SystemClipboard::copyTextToClipboard(GetText(rowNumber, columnId));
}

//==============================================================================
// Alloue le tableau pour les valeurs de pixels
//==============================================================================
bool PixelValuesModel::AllocPixels(uint32_t nbSample)
{
  if ((nbSample == NbSample) && (PixValue != nullptr)) // Tableau deja alloue et suffisant
    return true;
  if (PixValue != nullptr) 
    delete[] PixValue;

  NbSample = nbSample;
  PixValue = new double[NbSample * (2 * WinSize + 1) * (2 * WinSize + 1)];
  if (PixValue == nullptr)  // Probleme d'allocation
    return false;
  memset(PixValue, 0, NbSample * (2 * WinSize + 1) * (2 * WinSize + 1) * sizeof(double));
  return true;
}

//==============================================================================
// Retourne le texte pour une cellule
//==============================================================================
juce::String PixelValuesModel::GetText(int rowNumber, int columnId)
{
  if (PixValue == nullptr)
    return "no data";
  juce::String text;
  for (uint32_t i = 0; i < NbSample; i++)
    text = text + juce::String(PixValue[rowNumber * (2 * WinSize + 1) * NbSample + (columnId - 1) * NbSample + i]) + ";";
  return text;
}