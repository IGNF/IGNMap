//-----------------------------------------------------------------------------
//								AnalystViewer.h
//								===============
//
// Visualisation des analyses vectorielles
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 31/07/2026
//-----------------------------------------------------------------------------

#ifndef ANALYSTVIEWER_H
#define ANALYSTVIEWER_H

#include "AppUtil.h"
#include "GeoBase.h"

//==============================================================================
// AnalystViewerComponent : composant principal
//==============================================================================
class AnalystViewerComponent : public juce::Component, public juce::ComboBox::Listener {
public:
	AnalystViewerComponent();
	void SetBase(XGeoBase* base);

	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
	XGeoBase* m_Base;

	juce::ComboBox m_cbxLayer;
	juce::ComboBox m_cbxClass;
	juce::ComboBox m_cbxAttribut;

	void resized() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalystViewerComponent)
};

//==============================================================================
// AnalystViewer : fenetre container
//==============================================================================
class AnalystViewer : public ToolWindow {
public:
	AnalystViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Analyst.SetBase(base);
		//m_Analyst.addActionListener(listener);
		setContentOwned(&m_Analyst, true);
		setResizeLimits(400, 450, 10000, 10000);
	}

	void SetTarget(const double& , const double& , const double& ) override { ; }
	void SetSelection(void*) override { ; }

private:
	AnalystViewerComponent		m_Analyst;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalystViewer)
};

#endif // ANALYSTVIEWER_H