//-----------------------------------------------------------------------------
//								LasLayersViewer.h
//								=================
//
// Visualisation des classes d'objets LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 27/10/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"

//==============================================================================
// LasViewerModel : modele pour montrer les proprietes des LAS
//==============================================================================
class LasViewerModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, NbElem = 4, Zmin = 5, Zmax = 6, Options = 7 } Column;
	LasViewerModel() { m_Base = nullptr; m_ActiveRow = m_ActiveColumn = -1;}

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
	juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void SetBase(XGeoBase* base) { m_Base = base; }
	XGeoClass* FindLasClass(int index);

private:
	XGeoBase* m_Base;
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// ClassifModel : modele pour montrer les classifications des LAS
//==============================================================================
class ClassifModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Number = 3, Name = 4, Colour = 5 } Column;
	ClassifModel() { m_ActiveRow = m_ActiveColumn = -1; }

	int getNumRows() override { return 256; }
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// LasLayersViewer : fenetre pour voir les caracteristiques des MNT
//==============================================================================
class LasLayersViewer : public juce::Component,
	public juce::ActionListener,
	public juce::ActionBroadcaster,
	public juce::ComboBox::Listener,
	public juce::Slider::Listener,
	public juce::DragAndDropTarget,
	public juce::DragAndDropContainer {
public:
	LasLayersViewer();

	void SetBase(XGeoBase* base);
	void SetActionListener(juce::ActionListener* listener)
	{
		addActionListener(listener);
		m_ModelLas.addActionListener(listener); 
	}
	void UpdateColumnName();
	void UpdateAltiColors();
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

	// Algorithmes
	void ComputeDtm(std::vector< XGeoClass*>);
	void ComputeStat(std::vector<XGeoClass*>);

private:
	XGeoBase* m_Base;
	juce::TableListBox	m_TableLas;
	LasViewerModel			m_ModelLas;
	juce::ComboBox			m_Mode;
	juce::Slider				m_Opacity;
	juce::Slider				m_MaxGsd;
	juce::Slider				m_sldZRange;
	juce::Label					m_lblZRange;
	juce::DrawableRectangle m_drwZRect;
	juce::TableListBox	m_TableClassif;
	ClassifModel				m_ModelClassif;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LasLayersViewer)
};