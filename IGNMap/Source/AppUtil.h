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
// ToolWindow : classe mere pour les fenetres container outils
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
	virtual bool NeedTargetImage() { return false; };
	virtual void SetTargetImage(const juce::Image& /*image*/) { ; }
	virtual void SetSelection(void*) = 0;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToolWindow)
};

//==============================================================================
// ColourChangeButton : bouton permettant de choisir une couleur
//==============================================================================
class ColourChangeButton : public juce::TextButton, public juce::ChangeListener {
public:
  ColourChangeButton() : TextButton("Click to change colour...")
  {
    setSize(10, 24);
    changeWidthToFitText();
  }

	void clicked() override;

  void changeListenerCallback(juce::ChangeBroadcaster* source) override
  {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			juce::Colour color = cs->getCurrentColour();
			setColour(juce::TextButton::buttonColourId, color);
			setColour(juce::TextButton::textColourOffId, color.contrasting());
		}
  }

	juce::Colour GetColour() const { return findColour(juce::TextButton::buttonColourId); }
	void SetColour(juce::Colour color) { 
		setColour(juce::TextButton::buttonColourId, color); 
		setColour(juce::TextButton::textColourOffId, color.contrasting());
	}
};