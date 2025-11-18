//-----------------------------------------------------------------------------
//								Utilities.h
//								===========
//
// Fonctions utilitaires JUCE : inspirees de Assets/DemoUtilities.h des exemples JUCE
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 06/01/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

//==============================================================================
inline juce::Colour getRandomColour(float brightness) noexcept
{
  return juce::Colour::fromHSV(juce::Random::getSystemRandom().nextFloat(), 0.5f, brightness, 1.0f);
}

inline juce::Colour getRandomBrightColour() noexcept { return getRandomColour(0.8f); }
inline juce::Colour getRandomDarkColour() noexcept { return getRandomColour(0.3f); }
inline juce::Colour getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour uiColour,
                                            juce::Colour fallback = juce::Colour(0xff4d4d4d)) noexcept
{
  if (auto* v4 = dynamic_cast<juce::LookAndFeel_V4*> (&juce::LookAndFeel::getDefaultLookAndFeel()))
    return v4->getCurrentColourScheme().getUIColour(uiColour);
  return fallback;
}

inline std::unique_ptr<juce::InputStream> createAssetInputStream(const char* resourcePath)
{
  auto assetsDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
    .getParentDirectory().getChildFile("Images");
  auto resourceFile = assetsDir.getChildFile(resourcePath);
  jassert(resourceFile.existsAsFile());
  return resourceFile.createInputStream();
}

//==============================================================================
inline juce::Image getImageFromAssets(const char* assetName)
{
  auto hashCode = (juce::String(assetName) + "@ignmap_assets").hashCode64();
  auto img = juce::ImageCache::getFromHashCode(hashCode);

  if (img.isNull())
  {
    std::unique_ptr<juce::InputStream> juceIconStream(createAssetInputStream(assetName));
    if (juceIconStream == nullptr)
      return {};
    img = juce::ImageFileFormat::loadFrom(*juceIconStream);
    juce::ImageCache::addImageToCache(img, hashCode);
  }

  return img;
}

//==============================================================================
// Affichage d'un contenu JSON
//==============================================================================
class JsonTreeItem final : public juce::TreeViewItem
{
public:
  JsonTreeItem(juce::String text, juce::var value) : m_Text(text), m_Json(value)
  {
  }

  bool mightContainSubItems() override
  {
    if (auto* obj = m_Json.getDynamicObject())
      return obj->getProperties().size() > 0;
    if (!m_Json.isArray())
      return false;
    if (m_Json.size() == 0)
      return false;
    return true;
  }

  void paintItem(juce::Graphics& g, int width, int height) override
  {
    // if this item is selected, fill it with a background colour..
    if (isSelected())
      g.fillAll(juce::Colours::blue.withAlpha(0.3f));

    g.setColour(juce::Colours::black);
    g.setFont((float)height * 0.7f);

    g.drawText(m_Text, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
  }

  void itemOpennessChanged(bool isNowOpen) override
  {
    if (isNowOpen)
    {
      // if we've not already done so, we'll now add the tree's sub-items. You could
      // also choose to delete the existing ones and refresh them if that's more suitable
      // in your app.
      if (getNumSubItems() == 0)
      {
        // create and add sub-items to this node of the tree, corresponding to
        // the type of object this var represents

        if (m_Json.isArray())
        {
          for (int i = 0; i < m_Json.size(); ++i) {
            auto& child = m_Json[i];
            jassert(!child.isVoid());
            juce::String text = juce::String(i);
            if ((child.getDynamicObject() == nullptr)&&(!child.isArray()))
              text = text + " : " + child.toString();
            addSubItem(new JsonTreeItem(text, child));
          }
        }
        else if (auto* obj = m_Json.getDynamicObject())
        {
          auto& props = obj->getProperties();
          for (int i = 0; i < props.size(); ++i) {
            auto id = props.getName(i);
            auto child = props[id];
            auto val = props.getValueAt(i);
            juce::String text = id.toString();
            if ((!val.isArray())&&(!val.isObject()))
              text = text + " : " + val.toString();
            jassert(!child.isVoid());
            if (val.isArray())
              if (val.size() == 0)
                text += " : []";
            addSubItem(new JsonTreeItem(text, child));
          }
        }
      }
    }
    else
    {
      // in this case, we'll leave any sub-items in the tree when the node gets closed,
      // though you could choose to delete them if that's more appropriate for
      // your application.
    }
  }

private:
  juce::String m_Text;
  juce::var m_Json;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JsonTreeItem)
};