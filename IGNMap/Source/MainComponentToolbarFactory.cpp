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

  ids.add(doc_new);
  ids.add(doc_open);
  ids.add(doc_save);
  ids.add(edit_copy);
  ids.add(edit_cut);
  ids.add(edit_paste);
  ids.add(move);
  ids.add(select);
  ids.add(zoom);

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
  ids.add(doc_new);
  ids.add(doc_open);
  ids.add(doc_save);
  ids.add(spacerId);
  ids.add(separatorBarId);
  ids.add(edit_copy);
  ids.add(edit_cut);
  ids.add(edit_paste);
  ids.add(separatorBarId);
  ids.add(move);
  ids.add(select);
  ids.add(zoom);
}

juce::ToolbarItemComponent* MainComponentToolbarFactory::createItem(int itemId)
{
  if (m_Listener == nullptr)  // Le Listener doit etre fixe
    return nullptr;
  juce::ToolbarButton* button = nullptr;
  switch (itemId)
  {
  case doc_new:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("New.png"));
    button = new juce::ToolbarButton(doc_new, juce::translate("New"), std::move(drawable), {});
    break;
  }
  case doc_open:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Open.png"));
    button = new juce::ToolbarButton(doc_open, juce::translate("Open"), std::move(drawable), {});
    break;
  }
  case doc_save:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Save.png"));
    button = new juce::ToolbarButton(doc_save, juce::translate("Save"), std::move(drawable), {});
    break;
  }
  case edit_copy:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Copy.png"));
    button =  new juce::ToolbarButton(edit_copy, juce::translate("Copy"), std::move(drawable), {});
    break;
  }
  case edit_cut:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Cut.png"));
    button = new juce::ToolbarButton(edit_cut, juce::translate("Cut"), std::move(drawable), {});
    break;
  }
  case edit_paste:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Paste.png"));
    button = new juce::ToolbarButton(edit_paste, juce::translate("Paste"), std::move(drawable), {});
    break;
  }
  case move:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Move.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Move.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(move, juce::translate("Move"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    break;
  }
  case select:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Select.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Select.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(select, juce::translate("Select"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    break;
  }
  case zoom:
  {
    auto drawable_off = std::make_unique<juce::DrawableImage>();
    drawable_off->setImage(getImageFromAssets("Zoom.png"));
    auto drawable_on = std::make_unique<juce::DrawableImage>();
    drawable_on->setImage(getImageFromAssets("Zoom.png"));
    drawable_on->setOverlayColour(juce::Colours::darkcyan);
    button = new juce::ToolbarButton(zoom, juce::translate("Zoom"), std::move(drawable_off), std::move(drawable_on));
    button->setClickingTogglesState(true);
    button->setRadioGroupId(1, juce::NotificationType::dontSendNotification);
    break;
  }

  default: return nullptr;
  }
  button->addListener(m_Listener);
  return button;
}