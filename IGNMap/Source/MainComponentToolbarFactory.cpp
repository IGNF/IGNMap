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
  ids.add(Move);
  ids.add(Select);
  ids.add(Zoom);
  ids.add(Select3D);
  ids.add(spacerId);
  ids.add(separatorBarId);
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
    break;
  }

  default: return nullptr;
  }
  button->addListener(m_Listener);
  return button;
}