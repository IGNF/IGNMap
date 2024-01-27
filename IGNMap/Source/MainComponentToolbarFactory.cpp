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
}

juce::ToolbarItemComponent* MainComponentToolbarFactory::createItem(int itemId)
{
  if (m_Listener == nullptr)  // Le Listener doit etre fixe
    return nullptr;
  juce::ToolbarButton* button = nullptr;
  switch (itemId)
  {
  case Move:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Move.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Move.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(Move, juce::translate("Move"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setToggleState(true, juce::NotificationType::dontSendNotification);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Move ; Shift : select ; Ctrl : zoom"));
    break;
  }
  case Select:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Select.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Select.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(Select, juce::translate("Select"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Select one or several objects"));
    break;
  }
  case Zoom:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Zoom.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Zoom.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(Zoom, juce::translate("Zoom"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Zoom"));
    break;
  }
  case Select3D:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Select3D.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Select3D.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(Select3D, juce::translate("Select3D"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Select 3D view"));
    break;
  }
  case Gsd:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("GSD.png"));
    button = new SliderToolbarButton(Gsd, juce::translate("0."), std::move(drawable_off), nullptr);
    button->setTooltip(juce::translate("GSD of the view"));
    break;
  }

  // Outils de dessin
  case Polyline:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Polyline.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Polyline.png"));
    drawable_on->setOverlayColour(juce::Colours::darkred);
    button = new juce::ToolbarButton(Move, juce::translate("Polyline"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Polyline"));
    break;
  }
  case Polygone:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Polygone.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Polygone.png"));
    drawable_on->setOverlayColour(juce::Colours::darkred);
    button = new juce::ToolbarButton(Select, juce::translate("Polygone"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Polygone"));
    break;
  }
  case Rectangle:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Rectangle.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Rectangle.png"));
    drawable_on->setOverlayColour(juce::Colours::darkred);
    button = new juce::ToolbarButton(Select, juce::translate("Rectangle"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Rectangle"));
    break;
  }
  case Text:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Text.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Text.png"));
    drawable_on->setOverlayColour(juce::Colours::darkred);
    button = new juce::ToolbarButton(Select, juce::translate("Text"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    button->setTooltip(juce::translate("Texte"));
    break;
  }

  default: return nullptr;
  }
  button->addListener(m_Listener);
  return button;
}