//-----------------------------------------------------------------------------
//								MainComponentToolbarFactory.cpp
//								===============================
//
// Toolbar Factory pour le composant principal de l'application
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 28/12/2023
//-----------------------------------------------------------------------------

#include "MainComponentToolbarFactory.h"
#include "Utilities.h"

void MainComponentToolbarFactory::getAllToolbarItemIds(juce::Array<int>& ids)
{
  // This returns the complete list of all item IDs that are allowed to
  // go in our toolbar. Any items you might want to add must be listed here. The
  // order in which they are listed will be used by the toolbar customisation panel.

  ids.add(Move);
  ids.add(Select);
  ids.add(Zoom);
  ids.add(Select3D);
  ids.add(Gsd);
  ids.add(Polyline);
  ids.add(Polygone);
  ids.add(Rectangle);
  ids.add(Text);
  ids.add(Search);

  // If you're going to use separators, then they must also be added explicitly
  // to the list.
  ids.add(separatorBarId);
  ids.add(spacerId);
  ids.add(flexibleSpacerId);
}

void MainComponentToolbarFactory::getDefaultItemSet(juce::Array<int>& ids)
{
  // This returns an ordered list of the set of items that make up a
  // toolbar's default set. Not all items need to be on this list, and
  // items can appear multiple times (e.g. the separators used here).
  ids.add(spacerId);
  ids.add(Move);
  ids.add(Select);
  ids.add(Zoom);
  ids.add(Select3D);
  ids.add(separatorBarId);
  ids.add(Polyline);
  ids.add(Polygone);
  ids.add(Rectangle);
  ids.add(Text);
  ids.add(spacerId);
  ids.add(separatorBarId);
  ids.add(Gsd);
  ids.add(Search);
}

juce::ToolbarItemComponent* MainComponentToolbarFactory::createItem(int itemId)
{
  if (m_Listener == nullptr)  // Le Listener doit etre fixe
    return nullptr;
  juce::ToolbarButton* button = nullptr;
  switch (itemId) {
  case Move:
    m_btnMove = CreateButton(Move, juce::translate("Move"), juce::translate("Move ; Shift : select ; Ctrl : zoom"), 1, true, BinaryData::Move_png, BinaryData::Move_pngSize);
    return m_btnMove;
  case Select:
    m_btnSelect = CreateButton(Select, juce::translate("Select"), juce::translate("Select one or several objects"), 1, false, BinaryData::Select_png, BinaryData::Select_pngSize);
    return m_btnSelect;
  case Zoom:
    m_btnZoom = CreateButton(Zoom, juce::translate("Zoom"), juce::translate("Zoom"), 1, false, BinaryData::Zoom_png, BinaryData::Zoom_pngSize);
    return m_btnZoom;
  case Select3D:
    m_btnSelect3D = CreateButton(Select3D, juce::translate("Select3D"), juce::translate("Select 3D view"), 1, false, BinaryData::Select3D_png, BinaryData::Select3D_pngSize);
    return m_btnSelect3D;
 
  // Slider de GSD
  case Gsd:
  {
    auto image = juce::ImageCache::getFromMemory(BinaryData::GSD_png, BinaryData::GSD_pngSize);
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(image);
    m_btnGsd = new SliderToolbarButton(Gsd, juce::translate("0."), std::move(drawable_off), nullptr);
    m_btnGsd->setTooltip(juce::translate("GSD of the view"));
    m_btnGsd->addListener(m_Listener);
    return m_btnGsd;
  }

  // Recherche textuelle
  case Search:
  {
    auto image = juce::ImageCache::getFromMemory(BinaryData::Search_png, BinaryData::Search_pngSize);
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(image);
    m_btnSearch = new TextToolbarButton(Search, juce::translate("0."), std::move(drawable_off), nullptr);
    m_btnSearch->setTooltip(juce::translate("Search"));
    m_btnSearch->addListener(m_Listener);
    return m_btnSearch;
  }

  // Outils de dessin
  case Polyline:
    m_btnPolyline = CreateButton(Polyline, juce::translate("Polyline"), juce::translate("Polyline"), 1, false, BinaryData::Polyline_png, BinaryData::Polyline_pngSize);
    return m_btnPolyline;
  case Polygone:
    m_btnPolygon = CreateButton(Polygone, juce::translate("Polygone"), juce::translate("Polygone"), 1, false, BinaryData::Polygone_png, BinaryData::Polygone_pngSize);
    return m_btnPolygon;
  case Rectangle:
    m_btnRectangle = CreateButton(Rectangle, juce::translate("Rectangle"), juce::translate("Rectangle"), 1, false, BinaryData::Rectangle_png, BinaryData::Rectangle_pngSize);
    return m_btnRectangle;
  case Text:
    m_btnText = CreateButton(Text, juce::translate("Text"), juce::translate("Text"), 1, false, BinaryData::Text_png, BinaryData::Text_pngSize);
    return m_btnText;

  default: return nullptr;
  }
 
  return nullptr;
}

//==============================================================================
// Creation d'un bouton de la barre d'outils
//==============================================================================
juce::ToolbarButton* MainComponentToolbarFactory::CreateButton(int id, juce::String label, juce::String tooltip, int groupId,
                                                              bool toggle, const void* imageData, int dataSize)
{
  auto image = juce::ImageCache::getFromMemory(imageData, dataSize);
  auto drawable_off = std::make_unique<juce::DrawableImage>();
  //drawable_off->setImage(getImageFromAssets("Move.png"));
  drawable_off->setImage(image);
  auto drawable_on = std::make_unique<juce::DrawableImage>();
  //drawable_on->setImage(getImageFromAssets("Move.png"));
  drawable_on->setImage(image);
  drawable_on->setOverlayColour(juce::Colours::yellow);
  juce::ToolbarButton* button = new juce::ToolbarButton(id, label, std::move(drawable_off), std::move(drawable_on));
  button->setClickingTogglesState(true);
  if (toggle)
    button->setToggleState(true, juce::NotificationType::dontSendNotification);
  button->setRadioGroupId(groupId, juce::NotificationType::dontSendNotification);
  button->setTooltip(tooltip);
  button->addListener(m_Listener);
  return button;
}

//==============================================================================
// Traduction
//==============================================================================
void MainComponentToolbarFactory::Translate()
{
  m_btnMove->setTooltip(juce::translate("Move ; Shift : select ; Ctrl : zoom"));
  m_btnSelect->setTooltip(juce::translate("Select one or several objects"));
  m_btnZoom->setTooltip(juce::translate("Zoom"));
  m_btnSelect3D->setTooltip(juce::translate("Select 3D view"));
  m_btnPolyline->setTooltip(juce::translate("Polyline"));
  m_btnPolygon->setTooltip(juce::translate("Polygone"));
  m_btnRectangle->setTooltip(juce::translate("Rectangle"));
  m_btnText->setTooltip(juce::translate("Text"));
  m_btnGsd->setTooltip(juce::translate("GSD of the view"));
  m_btnSearch->setTooltip(juce::translate("Search"));
}