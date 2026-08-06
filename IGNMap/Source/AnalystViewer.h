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
#include "../XToolAlgo/XGeoAnalyst.h"

//==============================================================================
// AnalystViewerModel : modele de table pour montrer les resultats d'analyse
//==============================================================================
class AnalystViewerModel : public juce::TableListBoxModel, public juce::ChangeListener, public juce::Slider::Listener,
													 public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, PenWidth = 4, PenColour = 5, FillColour = 6, Options = 7 } Column;

	int getNumRows() override { if (m_Analyse != nullptr) return m_Analyse->NbRepres(); return 0; }
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void sortOrderChanged(int newSortColumnId, bool /*isForwards*/) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void sliderValueChanged(juce::Slider* slider) override;

	void SetAnalyse(XGeoAnalyst* A) { m_Analyse = A; }

protected:
	XGeoAnalyst* m_Analyse = nullptr;
	int	m_ActiveRow = -1;
	int	m_ActiveColumn = -1;
};

//==============================================================================
// AnalystViewerComponent : composant principal
//==============================================================================
class AnalystViewerComponent : public juce::Component, public juce::ComboBox::Listener,
															 public juce::Button::Listener, public juce::ActionBroadcaster, public juce::ActionListener {
public:
	AnalystViewerComponent();
	virtual ~AnalystViewerComponent();
	void SetBase(XGeoBase* base);
	void UpdateBase();

	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	virtual void buttonClicked(juce::Button*) override;
	void actionListenerCallback(const juce::String& message) override;

private:
	XGeoBase* m_Base;
	std::vector<XGeoAnalyst*> m_Analyse;
	juce::TableListBox	m_Table;
	AnalystViewerModel	m_Model;

	juce::Label m_lblLayer, m_lblClass, m_lblAttribut, m_lblDistribution;
	juce::ComboBox m_cbxAnalyse;
	juce::ComboBox m_cbxLayer;
	juce::ComboBox m_cbxClass;
	juce::ComboBox m_cbxAttribut;
	juce::ComboBox m_cbxDistribution;
	juce::Slider m_sldPlage;
	ColourChangeButton m_btnFirstColour;
	ColourChangeButton m_btnLastColour;
	juce::TextButton m_btnRun;
	juce::TextButton m_btnDelete;

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
		m_Analyst.addActionListener(listener);
		setContentOwned(&m_Analyst, true);
		setResizeLimits(400, 450, 10000, 10000);
	}

	void SetTarget(const double& , const double& , const double& ) override { ; }
	void SetSelection(void*) override { ; }
	virtual void SetMessage(const juce::String& message) override;

private:
	AnalystViewerComponent		m_Analyst;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalystViewer)
};

#endif // ANALYSTVIEWER_H