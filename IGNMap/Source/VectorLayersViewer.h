//-----------------------------------------------------------------------------
//								VectorLayersViewer.h
//								====================
//
// Visualisation de classes d'objets vectoriel
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/12/2021
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"

//==============================================================================
// VectorViewerModel : modele de table pour montrer les layers vectoriels
//==============================================================================
class VectorViewerModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::Slider::Listener,
	public juce::ActionBroadcaster, public juce::ActionListener {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, PenWidth = 4, PenColour = 5, FillColour = 6, Options = 7 } Column;
	VectorViewerModel();

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void sliderValueChanged(juce::Slider* slider) override;
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override { sendActionMessage(message); }

	void SetBase(XGeoBase* base) { m_Base = base; }
	XGeoClass* FindVectorClass(int index);

private:
	XGeoBase* m_Base;
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// VectorLayersViewer : fenetre pour contenir le LayerViewerComponent
//==============================================================================
class VectorLayersViewer : public juce::Component,
	public juce::ActionListener,
	public juce::ActionBroadcaster,
	public juce::DragAndDropTarget,
	public juce::DragAndDropContainer {
public:
	VectorLayersViewer();

	void SetBase(XGeoBase* base) { m_Base = base;  m_Model.SetBase(base); m_Table.updateContent(); m_Table.repaint(); }
	void Translate();
	void RenameAndViewLastClass(juce::String newName);
	void resized() override { auto b = getLocalBounds(); m_Table.setSize(b.getWidth(), b.getHeight()); }
	bool keyPressed(const juce::KeyPress& key) override;
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override;

	// Drag&Drop
	void itemDropped(const SourceDetails& details) override;
	bool isInterestedInDragSource(const SourceDetails& details) override;
	void itemDragEnter(const SourceDetails&) override { ; }
	void itemDragMove(const SourceDetails&) override { ; }
	void itemDragExit(const SourceDetails&) override { ; }

	// Algorithmes
	void ExportClass(std::vector< XGeoClass*>);

private:
	XGeoBase* m_Base;
	juce::TableListBox	m_Table;
	VectorViewerModel		m_Model;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorLayersViewer)
};
