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
    ;
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

  double Min = 0.1, Max = 1000., Interval = 0.1, Value = 50., MidPoint = 25.;
  bool Init = false;
};

//-----------------------------------------------------------------------------
// Combo Box
//-----------------------------------------------------------------------------
/*
class LayerToolbarItem final : public juce::ToolbarItemComponent
{
public:
  LayerToolbarItem(const int toolbarItemId)
    : juce::ToolbarItemComponent(toolbarItemId, "Custom Toolbar Item", false)
  {
    m_cbxLayer.setColour(juce::ComboBox::buttonColourId, juce::Colours::blue);
    m_cbxLayer.setColour(juce::ComboBox::textColourId, juce::Colours::green);

    addAndMakeVisible(m_cbxLayer);
    juce::PopupMenu* menu = m_cbxLayer.getRootMenu();

    auto image = juce::ImageCache::getFromMemory(BinaryData::Polyline_png, BinaryData::Polyline_pngSize);
    menu->addColouredItem(1, "Vide", juce::Colours::bisque, true, false, image);

    auto image2 = juce::ImageCache::getFromMemory(BinaryData::Move_png, BinaryData::Move_pngSize);
    menu->addColouredItem(2, "Ortho", juce::Colours::white, true, false, image2);

    auto image3 = juce::ImageCache::getFromMemory(BinaryData::IGNMapv3_png, BinaryData::IGNMapv3_pngSize);
    menu->addColouredItem(3, "Carto", juce::Colours::bisque, true, false, image3);
    
    //m_cbxLayer.setSelectedId(1);

    auto icon = juce::ImageCache::getFromMemory(BinaryData::IGNMapv3_png, BinaryData::IGNMapv3_pngSize);
    m_drwIcon.setImage(icon);
    addAndMakeVisible(m_drwIcon);

  }

  bool getToolbarItemSizes(int toolbarDepth, bool isVertical,
    int& preferredSize, int& minSize, int& maxSize) override
  {
    preferredSize = toolbarDepth;
    minSize = toolbarDepth;
    maxSize = toolbarDepth;
    return true;
  }

  void paintButtonArea(juce::Graphics& g, int w, int h, bool isMouseOver, bool isMouseDown) override
  {
  }

  void contentAreaChanged(const juce::Rectangle<int>& newArea) override
  {
   // m_cbxLayer.setSize(newArea.getWidth() - 2,
   //   juce::jmin(newArea.getHeight() - 2, 22));
    m_cbxLayer.setSize(newArea.getWidth() - 2, 40);
    m_cbxLayer.setCentrePosition(newArea.getCentreX(), newArea.getCentreY());
    m_drwIcon.setSize(newArea.getWidth(), newArea.getHeight());
    m_drwIcon.setCentrePosition(newArea.getCentreX(), newArea.getCentreY());
  }

private:
  juce::ComboBox m_cbxLayer{ "Layers" };
  juce::ImageComponent m_drwIcon;
  
};
*/

class LayerButton : public juce::ToolbarButton {
public:
  LayerButton(int itemId, const juce::String& labelText, std::unique_ptr<juce::Drawable> normalImage,
    std::unique_ptr<juce::Drawable> toggledOnImage) : ToolbarButton(itemId, labelText, std::move(normalImage), std::move(toggledOnImage)) {
    ;
  }

  void clicked() override
  {
    if (Init) {
      Init = false;
      return;
    }
    juce::PopupMenu menu;
    auto image = juce::ImageCache::getFromMemory(BinaryData::Layer_png, BinaryData::Layer_pngSize);
    menu.addColouredItem(1, juce::translate("Empty"), juce::Colours::bisque, true, false, image);
    auto image2 = juce::ImageCache::getFromMemory(BinaryData::Ortho_png, BinaryData::Ortho_pngSize);
    menu.addColouredItem(2, juce::translate("Ortho"), juce::Colours::white, true, false, image2);
    auto image3 = juce::ImageCache::getFromMemory(BinaryData::Carto_png, BinaryData::Carto_pngSize);
    menu.addColouredItem(3, juce::translate("Carto"), juce::Colours::bisque, true, false, image3);
    auto image4 = juce::ImageCache::getFromMemory(BinaryData::OrthoCarto_png, BinaryData::OrthoCarto_pngSize);
    menu.addColouredItem(4, juce::translate("Ortho+Carto"), juce::Colours::bisque, true, false, image4);

    std::function< void(int) > ChangeLayer = [=](int result) { // Change la couche
      if (result == 0) return;
      if (result == 1) setButtonText(juce::String("Empty"));
      if (result == 2) setButtonText(juce::String("Ortho"));
      if (result == 3) setButtonText(juce::String("Carto"));
      if (result == 4) setButtonText(juce::String("Ortho+Carto"));
      Init = true;
      juce::ToolbarButton::triggerClick();
     };

    menu.showMenuAsync(juce::PopupMenu::Options().withStandardItemHeight(40), ChangeLayer);
      
  }
  bool Init = false;
};

//-----------------------------------------------------------------------------
// Bouton pour l'echelle
//-----------------------------------------------------------------------------
class ScaleButton : public juce::ToolbarButton {
public:
  ScaleButton(int itemId, const juce::String& labelText, std::unique_ptr<juce::Drawable> normalImage,
    std::unique_ptr<juce::Drawable> toggledOnImage) : ToolbarButton(itemId, labelText, std::move(normalImage), std::move(toggledOnImage)) {
    ;
  }

  void clicked() override
  {
    if (Init) {
      Init = false;
      return;
    }
    juce::PopupMenu menu;
    menu.addItem(1, "1 : 1000");
    menu.addItem(2, "1 : 5000");
    menu.addItem(3, "1 : 10 000");
    menu.addItem(4, "1 : 25 000");
    menu.addItem(5, "1 : 50 000");
    menu.addItem(6, "1 : 100 000");
    menu.addItem(7, "1 : 250 000");
    menu.addItem(8, "1 : 500 000");
    menu.addItem(9, "1 : 1 000 000");
    menu.addItem(9, juce::translate("Total"));

    std::function< void(int) > ChangeScale = [=](int result) { // Change l'echelle
      if (result == 0) return;
      if (result == 1) setButtonText(juce::String("1000"));
      if (result == 2) setButtonText(juce::String("5000"));
      if (result == 3) setButtonText(juce::String("10000"));
      if (result == 4) setButtonText(juce::String("25000"));
      if (result == 5) setButtonText(juce::String("50000"));
      if (result == 6) setButtonText(juce::String("100000"));
      if (result == 7) setButtonText(juce::String("250000"));
      if (result == 8) setButtonText(juce::String("500000"));
      if (result == 9) setButtonText(juce::String("1000000"));
      if (result == 9) setButtonText(juce::String("Total"));
      Init = true;
      juce::ToolbarButton::triggerClick();
      };

    menu.showMenuAsync(juce::PopupMenu::Options(), ChangeScale);
  }
  bool Init = false;
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
    Scale = 101,
    Search = 102,
    Layer = 103,
    Polyline = 200,
    Polygone = 201,
    Rectangle = 202,
    Text = 203,
    Ellipse = 204
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
  juce::ToolbarButton* m_btnScale = nullptr;
  juce::ToolbarButton* m_btnSearch = nullptr;
  juce::ToolbarButton* m_btnLayer = nullptr;

  juce::ToolbarButton* CreateButton(int id, juce::String label, juce::String tooltip, int groupId, bool toggle,
                                    const void* imageData, int dataSize, juce::Colour offColor, juce::Colour onColor) const;
};