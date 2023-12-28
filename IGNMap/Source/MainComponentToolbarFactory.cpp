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
  switch (itemId)
  {
  case doc_new:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("New.png"));
    return new juce::ToolbarButton(move, juce::translate("New"), std::move(drawable), {});
  }
  case doc_open:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Open.png"));
    return new juce::ToolbarButton(move, juce::translate("Open"), std::move(drawable), {});
  }
  case doc_save:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Save.png"));
    return new juce::ToolbarButton(move, juce::translate("Save"), std::move(drawable), {});
  }
  case edit_copy:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Copy.png"));
    return new juce::ToolbarButton(move, juce::translate("Copy"), std::move(drawable), {});
  }
  case edit_cut:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Cut.png"));
    return new juce::ToolbarButton(move, juce::translate("Cut"), std::move(drawable), {});
  }
  case edit_paste:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Paste.png"));
    return new juce::ToolbarButton(move, juce::translate("Paste"), std::move(drawable), {});
  }
  case move:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Move.png"));
    return new juce::ToolbarButton(move, juce::translate("Move"), std::move(drawable), {});
  }
  case select:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Select.png"));
    return new juce::ToolbarButton(move, juce::translate("Select"), std::move(drawable), {});
  }
  case zoom:
  {
    auto drawable = std::make_unique<juce::DrawableImage>();
    drawable->setImage(getImageFromAssets("Zoom.png"));
    return new juce::ToolbarButton(move, juce::translate("Zoom"), std::move(drawable), {});
  }

  default:                break;
  }

  return nullptr;
}