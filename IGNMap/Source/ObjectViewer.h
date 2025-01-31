//-----------------------------------------------------------------------------
//								ObjectViewer.h
//								==============
//
// Visualisation des caracteristiques des objets
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/01/2025
//-----------------------------------------------------------------------------

#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include "AppUtil.h"

class XGeoObject;
class XGeoVector;
class RotationImage;

//==============================================================================
// ObjectViewerComponent : composant principal
//==============================================================================
class ObjectViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
	public juce::Button::Listener, public juce::ComboBox::Listener, public juce::Slider::Listener {
public:
	ObjectViewerComponent();
	
	void actionListenerCallback(const juce::String& message) override;
	void buttonClicked(juce::Button*) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override { ; }
	void sliderValueChanged(juce::Slider* slider) override;

	bool SetSelection(void*);
	bool SetRotationImage(RotationImage* image);
	bool UpdateRotationImage(RotationImage* image);
	bool SetGeoVector(XGeoVector* V);

private:
	XGeoObject* m_Object;

	// Interface pour les images rotation
	juce::Slider m_sldXCenter;
	juce::Slider m_sldYCenter;
	juce::Slider m_sldResolution;
	juce::Slider m_sldRotation;
	juce::Slider m_sldToneMappingPower;
	juce::Slider m_sldToneMappingSharpness;

	// Interface pour les objets vectoriels
	ColourChangeButton m_btnPen;
	ColourChangeButton m_btnFill;
	juce::Slider m_sldPenWidth;
	juce::TextButton m_btnApply;
	juce::TextButton m_btnRestore;

	void resized() override;

};

//==============================================================================
// ObjectViewer : fenetre container
//==============================================================================
class ObjectViewer : public ToolWindow {
public:
	ObjectViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Object.addActionListener(listener);
		setContentOwned(&m_Object, true);
		setResizeLimits(230, 400, 1000, 1000);
	}

	void SetTarget(const double& X, const double& Y, const double& Z) override { ; }
	void SetSelection(void* S) override { m_Object.SetSelection(S); }

private:
	ObjectViewerComponent		m_Object;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectViewer)
};


#endif // OBJECTVIEWER_H