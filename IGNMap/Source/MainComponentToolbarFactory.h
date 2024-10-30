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

//-----------------------------------------------------------------------------
// Slider pour le choix du GSD
//-----------------------------------------------------------------------------
class SliderToolbarButton : public juce::ToolbarButton, public juce::Slider::Listener {
public:
  SliderToolbarButton(int itemId, const juce::String& labelText, std::unique_ptr<juce::Drawable> normalImage,
    std::unique_ptr<juce::Drawable> toggledOnImage) : ToolbarButton(itemId, labelText, std::move(normalImage), std::move(toggledOnImage)) {
    Min = 0.1;
    Max = 1000.;
    MidPoint = 25.;
    Interval = 0.1;
    Value = 50.;
    Init = false;
  }

  void clicked() override
  {
    if (Init) {
      Init = false;
      return;
    }
    auto slider = std::make_unique<juce::Slider>();
    slider->setRange(Min, Max, Interval);
    slider->setSkewFactorFromMidPoint(MidPoint);
    slider->setValue(Value);
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
    slider->setSize(400, 50);
    slider->setChangeNotificationOnlyOnRelease(true);
    slider->addListener(this);
    juce::CallOutBox::launchAsynchronously(std::move(slider), getScreenBounds(), nullptr);
  }

  void sliderValueChanged(juce::Slider* slider) override
  {
    double val = slider->getValue();
    if (fabs(Value - val) < 0.1)
      return;
    Value = val;
    juce::CallOutBox* box = dynamic_cast<juce::CallOutBox *>(slider->getParentComponent());
    if (box != nullptr)
      box->dismiss();
    setButtonText(juce::String(Value));
    Init = true;
    juce::ToolbarButton::triggerClick();
  }

  double Min, Max, Interval, Value, MidPoint;
  bool Init;
};

//-----------------------------------------------------------------------------
// Barre de recherche
//-----------------------------------------------------------------------------
class TextToolbarButton : public juce::ToolbarButton, public juce::TextEditor::Listener {
public:
  TextToolbarButton(int itemId, const juce::String& labelText, std::unique_ptr<juce::Drawable> normalImage,
    std::unique_ptr<juce::Drawable> toggledOnImage) : ToolbarButton(itemId, labelText, std::move(normalImage), std::move(toggledOnImage)) {
    Init = false;
  }

  void clicked() override
  {
    if (Init) {
      Init = false;
      return;
    }
    auto textEditor = std::make_unique<juce::TextEditor>();
    textEditor->setReturnKeyStartsNewLine(false);
    textEditor->setSize(400, 50);
    textEditor->addListener(this);
    juce::CallOutBox::launchAsynchronously(std::move(textEditor), getScreenBounds(), nullptr);
  }

  void textEditorReturnKeyPressed(juce::TextEditor& textEditor) override
  {
    juce::CallOutBox* box = dynamic_cast<juce::CallOutBox*>(textEditor.getParentComponent());
    if (box != nullptr)
      box->dismiss();
    setButtonText(textEditor.getText());
    Init = true;
    juce::ToolbarButton::triggerClick();
  }

  bool Init;
};

//-----------------------------------------------------------------------------
// Toolbar
//-----------------------------------------------------------------------------
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
    Select3D = 4,
    Gsd = 100,
    Polyline = 200,
    Polygone = 201,
    Rectangle = 202,
    Text = 203,
    Ellipse = 204,
    Search = 300
  };

  void getAllToolbarItemIds(juce::Array<int>& ids) override;
  void getDefaultItemSet(juce::Array<int>& ids) override;
  juce::ToolbarItemComponent* createItem(int itemId) override;
  void Translate();

private:
  juce::ToolbarButton* m_btnMove = nullptr;
  juce::ToolbarButton* m_btnSelect = nullptr;
  juce::ToolbarButton* m_btnZoom = nullptr;
  juce::ToolbarButton* m_btnSelect3D = nullptr;
  juce::ToolbarButton* m_btnPolyline = nullptr;
  juce::ToolbarButton* m_btnPolygon = nullptr;
  juce::ToolbarButton* m_btnRectangle = nullptr;
  juce::ToolbarButton* m_btnText = nullptr;
  juce::ToolbarButton* m_btnGsd = nullptr;
  juce::ToolbarButton* m_btnSearch = nullptr;

  juce::ToolbarButton* CreateButton(int id, juce::String label, juce::String tooltip, int groupId, bool toggle,
                                    const void* imageData, int dataSize);
};