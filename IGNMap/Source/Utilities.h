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