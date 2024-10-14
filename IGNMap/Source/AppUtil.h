//-----------------------------------------------------------------------------
//								AppUtil.h
//								=========
//
// Fonctions utilitaires
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/01/2024
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>

namespace AppUtil {
	juce::String OpenFolder(juce::String optionName = "", juce::String mes = "");
	juce::String OpenFile(juce::String optionName = "", juce::String mes = "", juce::String filter = "");
	juce::String SaveFile(juce::String optionName = "", juce::String mes = "", juce::String filter = "");

	juce::String GetAppOption(juce::String name);
	void SaveAppOption(juce::String name, juce::String value);

	std::string GetStringFilename(juce::String filename);
	void SaveComponent(juce::Component* component);
}