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
  MainComponentToolbarFactory() { m_Listener = nullptr; }
  void SetListener(juce::Button::Listener* listener) { m_Listener = listener; }

  juce::Button::Listener* m_Listener;

  // ID unique des outils de la toolbar
  enum MainComponentToolbarItemIds
  {
    Move = 1,
    Select = 2,
    Zoom = 3,
    Select3D = 4
  };

  void getAllToolbarItemIds(juce::Array<int>& ids) override;
  void getDefaultItemSet(juce::Array<int>& ids) override;
  juce::ToolbarItemComponent* createItem(int itemId) override;
};