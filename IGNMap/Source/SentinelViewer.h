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
class XGeoClass;
class GeoSentinelImage;

//==============================================================================
// SentinelSceneModel : modele pour contenir les scenes Sentinel
//==============================================================================
class SentinelSceneModel : public juce::TableListBoxModel, public juce::ActionBroadcaster, public juce::ComboBox::Listener {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, Date = 4 } Column;
	SentinelSceneModel() : juce::TableListBoxModel() { m_Base = nullptr; m_ActiveRow = m_ActiveColumn = -1; }
	~SentinelSceneModel() { ; }
	void SetBase(XGeoBase* base) { m_Base = base; }
	XGeoClass* FindScene(int number, juce::String& date);

	void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	void paintRowBackground(juce::Graphics&, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/) override;
	int getNumRows() override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
	XGeoBase* m_Base;
	int				m_ActiveRow;
	int				m_ActiveColumn;
};

//==============================================================================
// SentinelViewerComponent : composant principal
//==============================================================================
class SentinelViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
																public juce::Button::Listener, public juce::ComboBox::Listener {
public:
	SentinelViewerComponent();
	void SetBase(XGeoBase* base) { m_Base = base; m_mdlScene.SetBase(base); }

	void actionListenerCallback(const juce::String& message) override;
	void buttonClicked(juce::Button*) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
	XGeoBase* m_Base;
	juce::GroupComponent m_grpImport;
	juce::TextButton m_btnImport;
	juce::ToggleButton m_btn10m;
	juce::ToggleButton m_btn20m;
	juce::ToggleButton m_btn60m;
	juce::ComboBox m_cbxResol;
	juce::ComboBox m_cbxMode;
	juce::TableListBox m_tblScene;
	SentinelSceneModel m_mdlScene;

	void resized() override;

	void ImportScenes();
	void ImportResol(GeoSentinelImage* scene, juce::File* folder, int resol);

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
		setContentOwned(&m_Sentinel, true);
		//setContentComponent(&m_Sentinel);
	}

	void closeButtonPressed() override { setVisible(false); }

private:
	SentinelViewerComponent		m_Sentinel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SentinelViewer)
};

#endif // SENTINELVIEWER_H