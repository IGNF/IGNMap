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

	static int DownloadNbTry = 20;
	juce::String DownloadFile(const juce::String& request, const juce::String& filename, int nb_try = 0, int timeout = 10);
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

//==============================================================================
// FieldEditor : composant pour remplir un champ
//==============================================================================
class FieldEditor : public juce::Component {
public:
	enum FieldType { String = 0, Double = 1, Int = 2, Uint = 3};
	FieldEditor() { ; }
	void SetEditor(juce::String label, juce::String value, FieldType type, int labelW, int editorW);

	juce::String StringValue() { return m_Editor.getText(); }
	double DoubleValue() { return m_Editor.getText().getDoubleValue(); }
	int IntValue() { return m_Editor.getText().getIntValue(); }
	uint32_t UintValue() { return (uint32_t)m_Editor.getText().getIntValue(); }
	void SetReadOnly(bool shouldBeReadOnly) { m_Editor.setReadOnly(shouldBeReadOnly); }
	void SetValue(juce::String text) { m_Editor.setText(text); }

private:
	juce::Label m_Label;
	juce::TextEditor	m_Editor;
};

//==============================================================================
// FrameComponent : composant pour visualiser et modifier un cadre
//==============================================================================
class FrameComponent : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster {
public:
	FrameComponent(double xmin = 0., double ymin = 0., double xmax = 0., double ymax = 0.);
	virtual ~FrameComponent() { ; }
	void buttonClicked(juce::Button*) override;
	void GetFrame(double& xmin, double& ymin, double& xmax, double& ymax);

private:
	juce::TextEditor	m_edtXmin, m_edtYmin, m_edtXmax, m_edtYmax;
	juce::Label m_lblXmin, m_lblYmin, m_lblXmax, m_lblYmax;
	juce::ImageButton m_btnView;
	juce::DrawableButton m_btnKm;

	void ViewFrame();
	void RoundFrame();
};