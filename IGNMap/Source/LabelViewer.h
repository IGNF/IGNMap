//-----------------------------------------------------------------------------
//								LabelViewer.h
//								=============
//
// Creation de lavels
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 10/08/2026
//-----------------------------------------------------------------------------

#ifndef LABELVIEWER_H
#define LABELVIEWER_H

#include "AppUtil.h"
#include "GeoBase.h"

//==============================================================================
// AnalystViewerComponent : composant principal
//==============================================================================
class LabelViewerComponent : public juce::Component, public juce::ComboBox::Listener,
	public juce::Button::Listener, public juce::ActionBroadcaster {
public:
	LabelViewerComponent();
	void SetBase(XGeoBase* base);
	void UpdateBase();

	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	virtual void buttonClicked(juce::Button*) override;

private:
	XGeoBase* m_Base;

	juce::Label m_lblLayer, m_lblClass, m_lblAttribut;
	juce::ComboBox m_cbxLayer;
	juce::ComboBox m_cbxClass;
	juce::ComboBox m_cbxAttribut;
	juce::Label m_lblLength, m_lblArea, m_lblImportance, m_lblDoublon;
	juce::Slider m_sldMinLength;
	juce::Slider m_sldMinArea;
	juce::Slider m_sldImportance;
	juce::Slider m_sldDoublon;
	juce::TextButton m_btnRun;

	void resized() override;

	void Compute();
	void AddLabel(XGeoClass* C, XGeoVector* V, const double& minDist);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelViewerComponent)
};

//==============================================================================
// AnalystViewer : fenetre container
//==============================================================================
class LabelViewer : public ToolWindow {
public:
	LabelViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Label.SetBase(base);
		m_Label.addActionListener(listener);
		setContentOwned(&m_Label, true);
		setResizeLimits(400, 290, 10000, 10000);
	}

	void SetTarget(const double&, const double&, const double&) override { ; }
	void SetSelection(void*) override { ; }
	virtual void SetMessage(const juce::String& message) override;

private:
	LabelViewerComponent		m_Label;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelViewer)
};

#endif // LABELVIEWER_H