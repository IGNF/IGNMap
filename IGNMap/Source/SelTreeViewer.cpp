//-----------------------------------------------------------------------------
//								SelTreeViewer.cpp
//								=================
//
// Visualisation d'une selection d'objets sous forme d'arbre
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 14/03/2022
//-----------------------------------------------------------------------------

#include <JuceHeader.h>
#include "SelTreeViewer.h"

#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoVector.h"

//==============================================================================
// Ouverture / fermeture d'un SelTreeItem
//==============================================================================
void SelTreeItem::itemOpennessChanged(bool isNowOpen)
{
  if (m_Base == nullptr)
    return;
  if (isNowOpen) {
    if (m_Feature == nullptr) { // Racine
      for (uint32_t i = 0; i < m_Base->NbSelection(); i++) {
        SelTreeItem* item = new SelTreeItem(m_Base, m_Base->Selection(i));
        addSubItem(item);
      }
      return;
    }
    else {  // Feature
      std::vector<std::string> Att;
      m_Feature->ReadAttributes(Att);
      for (int i = 0; i < Att.size() / 2; i++) {
        SelTreeItem* item = new SelTreeItem(Att[2*i], Att[2 * i + 1]);
        addSubItem(item);
      }
      return;
    }
  }
  else
    clearSubItems();
}

//==============================================================================
// Dessin d'un SelTreeItem
//==============================================================================
void SelTreeItem::paintItem(juce::Graphics& g, int width, int height)
{
  g.setFont(15.0f);
  if ((m_Feature != nullptr) && (m_Base != nullptr)) {
    g.setColour(juce::Colours::silver);
    if (isSelected())
      g.fillAll(juce::Colours::teal);
    g.drawText(juce::String(m_Feature->ClassName()), 4, 0, width - 4, height, juce::Justification::centredLeft, true);
    return;
  }
  juce::String text = m_AttName;
  g.setColour(juce::Colours::coral);
  //g.drawText(text, 4, 0, width / 4 - 4, height, juce::Justification::centredLeft, true);
  g.drawText(text, 4, 0, m_Margin, height, juce::Justification::centredLeft, true);
  text = m_AttValue;
  g.setColour(juce::Colours::lightyellow);
  //g.drawText(text, 4 + width / 4, 0, 3 * width / 4 - 4, height, juce::Justification::centredLeft, true);
  g.drawText(text, 4 + m_Margin, 0, width - m_Margin - 4, height, juce::Justification::centredLeft, true);
}

//==============================================================================
// Clic simple
//==============================================================================
void SelTreeItem::itemClicked(const juce::MouseEvent&)
{
  if ((m_Feature != nullptr) && (m_Base != nullptr)) {
    SelTreeViewer* viewer = static_cast<SelTreeViewer*>(getOwnerView());
    //viewer->sendActionMessage("SelectFeature:" + juce::String(m_Feature.Id()) + ":" + juce::String(m_Feature.IdLayer()));
    XFrame F = m_Feature->Frame();
    viewer->sendActionMessage("DrawEnvelope:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
      juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
  }
}

//==============================================================================
// Double-clic : deplacement sur l'objet
//==============================================================================
void SelTreeItem::itemDoubleClicked(const juce::MouseEvent&)
{
  if ((m_Feature != nullptr) && (m_Base != nullptr)) {
    SelTreeViewer* viewer = static_cast<SelTreeViewer*>(getOwnerView());
    XFrame F = m_Feature->Frame();
    viewer->sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
        juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
    return;
  }
  juce::SystemClipboard::copyTextToClipboard(m_AttValue);
}

//==============================================================================
// Largeur de l'item
//==============================================================================
int SelTreeItem::getItemWidth() const
{
  if ((m_Feature != nullptr) && (m_Base != nullptr))
    return TreeViewItem::getItemWidth();
  return juce::Font(15.0f).getStringWidth(m_AttName + m_AttValue) + m_Margin;
}

//==============================================================================
// Infobulle de l'item
//==============================================================================
juce::String SelTreeItem::getTooltip()
{
  if ((m_Feature != nullptr) && (m_Base != nullptr))
    return juce::translate("Double-click for viewing");
  return juce::translate("Double-click for copying");
}

//==============================================================================
// Constructeur
//==============================================================================
SelTreeViewer::SelTreeViewer()
{
  m_rootItem.reset(new SelTreeItem);
  setRootItem(m_rootItem.get());
}

//==============================================================================
// Fixe la GeoBase
//==============================================================================
void SelTreeViewer::SetBase(XGeoBase* base)
{
  m_rootItem.get()->SetBase(base);
  repaint();
  setRootItemVisible(false);
  if (getNumRowsInTree() == 1) {
    juce::TreeViewItem* item = getItemOnRow(0);
    if (item != nullptr)
      item->setOpen(true);
  }
}
