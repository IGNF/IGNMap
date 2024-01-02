//-----------------------------------------------------------------------------
//								DtmLayersViewer.h
//								=================
//
// Visualisation des classes d'objets MNT
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 21/01/2022
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"

//==============================================================================
// DtmViewerModel : modele pour montrer les proprietes des MNT
//==============================================================================
class DtmViewerModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, Zmin = 4, Zmax = 5, Options = 6 } Column;
	DtmViewerModel();

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void SetBase(XGeoBase* base) { m_Base = base; }
	XGeoClass* FindDtmClass(int index);

private:
	XGeoBase* m_Base;
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// DtmRangeModel : modele pour montrer les plages d'altitudes
//==============================================================================
class DtmRangeModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::Slider::Listener,
	public juce::ActionBroadcaster {
public:
	typedef enum { Altitude = 1, Colour = 2 } Column;
	DtmRangeModel() { m_ActiveRow = m_ActiveColumn = -1; }

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void sliderValueChanged(juce::Slider* slider) override;

private:
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// DtmLayersViewer : fenetre pour voir les caracteristiques des MNT
//==============================================================================
class DtmLayersViewer : public juce::Component,
	public juce::ActionListener,
	public juce::ActionBroadcaster,
	public juce::ComboBox::Listener,
	public juce::Slider::Listener,
	public juce::DragAndDropTarget,
	public juce::DragAndDropContainer {
public:
	DtmLayersViewer();

	void SetBase(XGeoBase* base) { m_Base = base;  m_ModelDtm.SetBase(base); m_TableDtm.updateContent(); }
	void SetActionListener(juce::ActionListener* listener) 
		{ addActionListener(listener); m_ModelDtm.addActionListener(listener); m_ModelRange.addActionListener(listener); }
	void UpdateColumnName();
	void resized() override;
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	void sliderValueChanged(juce::Slider* slider) override;

	// Drag&Drop
	void itemDropped(const SourceDetails& details) override;
	bool isInterestedInDragSource(const SourceDetails& details) override;
	void itemDragEnter(const SourceDetails&) override { ; }
	void itemDragMove(const SourceDetails&) override { ; }
	void itemDragExit(const SourceDetails&) override { ; }

private:
	XGeoBase* m_Base;
	juce::TableListBox	m_TableDtm;
	DtmViewerModel			m_ModelDtm;
	juce::TableListBox	m_TableRange;
	DtmRangeModel				m_ModelRange;
	juce::ComboBox			m_Mode;
	juce::Slider				m_IsoStep;
	juce::Slider				m_Azimuth;
	juce::Slider				m_Zenith;
	juce::Slider				m_Opacity;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DtmLayersViewer)
};