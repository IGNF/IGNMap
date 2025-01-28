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
	void SaveTableComponent(juce::TableListBox* table);
}

//==============================================================================
// ToolWindow : class merer pour les fenetres container outils
//==============================================================================
class ToolWindow : public juce::DocumentWindow {
public:
	ToolWindow(const juce::String& name, juce::Colour backgroundColour, int requiredButtons)
		: juce::DocumentWindow(name, backgroundColour, requiredButtons)
	{
		;
	}

	void closeButtonPressed() override { setVisible(false); }
	virtual void SetTarget(const double& /*X*/, const double& /*Y*/, const double& /*Z*/) = 0;
	virtual void SetSelection(void*) = 0;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolWindow)
};