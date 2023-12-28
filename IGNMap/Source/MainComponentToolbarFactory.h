//-----------------------------------------------------------------------------
//								MainComponentToolbarFactory.h
//								=============================
//
// Toolbar Factory pour le composant principal de l'application
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 28/12/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

class MainComponentToolbarFactory final : public juce::ToolbarItemFactory
{
public:
  MainComponentToolbarFactory() {}

  // ID unique des outils de la toolbar
  enum MainComponentToolbarItemIds
  {
    doc_new = 1,
    doc_open = 2,
    doc_save = 3,
    doc_saveAs = 4,
    edit_copy = 5,
    edit_cut = 6,
    edit_paste = 7,
    move = 8,
    select = 9,
    zoom = 10
  };

  void getAllToolbarItemIds(juce::Array<int>& ids) override;
  void getDefaultItemSet(juce::Array<int>& ids) override;
  juce::ToolbarItemComponent* createItem(int itemId) override;
};