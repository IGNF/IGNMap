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
    auto slider = std::make_unique<juce::Slider>();
    slider->setRange(Min, Max, Interval);
    slider->setSkewFactorFromMidPoint(MidPoint);
    slider->setValue(Value);
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
    slider->setSize(400, 50);
    slider->setChangeNotificationOnlyOnRelease(true);
    slider->addListener(this);
    juce::CallOutBox& box = juce::CallOutBox::launchAsynchronously(std::move(slider), getScreenBounds(), nullptr);
  }

  void sliderValueChanged(juce::Slider* slider) 
  {
    double val = slider->getValue();
    if (fabs(Value - val) < 0.1)
      return;
    Value = val;
    juce::CallOutBox* box = dynamic_cast<juce::CallOutBox *>(slider->getParentComponent());
    if (box != nullptr)
      box->dismiss();
    setButtonText(juce::String(Value));
    juce::ToolbarButton::triggerClick();
  }

  double Min, Max, Interval, Value, MidPoint;
  bool Init;
  
};

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
    Ellipse = 203,
    Text = 204
  };

  void getAllToolbarItemIds(juce::Array<int>& ids) override;
  void getDefaultItemSet(juce::Array<int>& ids) override;
  juce::ToolbarItemComponent* createItem(int itemId) override;
};