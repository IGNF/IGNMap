//-----------------------------------------------------------------------------
//								SentinelViewer.h
//								================
//
// Visualisation des scenes SENTINEL
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 22/12/2024
//-----------------------------------------------------------------------------

#ifndef SENTINELVIEWER_H
#define SENTINELVIEWER_H

#include <JuceHeader.h>

class XGeoBase;
class GeoSentinelImage;

//==============================================================================
// SentinelViewerComponent : composant principal
//==============================================================================
class SentinelViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
																public juce::Button::Listener {
public:
	SentinelViewerComponent();
	void SetBase(XGeoBase* base) { m_Base = base; }

	void actionListenerCallback(const juce::String& message) override { ; }
	void buttonClicked(juce::Button*) override;

private:
	XGeoBase* m_Base;
	juce::TextButton m_btnImport;

	void resized() override;

	void ImportScenes();
	void ImportResol(GeoSentinelImage* scene, juce::File* folder);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SentinelViewerComponent)
};

//==============================================================================
// SentinelViewer : fenetre container
//==============================================================================
class SentinelViewer : public juce::DocumentWindow {
public:
	SentinelViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: juce::DocumentWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Sentinel.SetBase(base);
		m_Sentinel.addActionListener(listener);
		setContentComponent(&m_Sentinel);
	}

	void closeButtonPressed() override { ; }

private:
	SentinelViewerComponent		m_Sentinel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SentinelViewer)
};

#endif // SENTINELVIEWER_H