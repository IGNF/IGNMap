//-----------------------------------------------------------------------------
//								AnnotViewer.h
//								=============
//
// Visualisation des annotations
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 05/08/2024
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>
#include "../../XToolVector/XAnnotation.h"


//==============================================================================
// EditableTextCustomComponent : texte editable pour les tables
//==============================================================================
class EditableTextCustomAnnotation final : public juce::Label {
public:
	EditableTextCustomAnnotation(juce::TableListBox* tb, std::vector<XAnnotation>* A, int rowNumber)
	{
		m_Table = tb;
		m_Annot = A;
		m_nRowNumber = rowNumber;
		setEditable(false, true, false);
		if (m_nRowNumber < m_Annot->size())
			setText((*m_Annot)[m_nRowNumber].Text(), juce::dontSendNotification);
	}

	void mouseDown(const juce::MouseEvent& event) override
	{
		// single click on the label should simply select the row
		if (m_Table != nullptr)
			m_Table->selectRowsBasedOnModifierKeys(m_nRowNumber, event.mods, false);
		juce::Label::mouseDown(event);
	}

	void 	mouseDrag(const juce::MouseEvent& event) override
	{
		juce::SparseSet<int> set;
		set.addRange(juce::Range<int>(m_nRowNumber, m_nRowNumber+1));
		juce::String message(m_nRowNumber);
		m_Table->startDragAndDrop(event, set, message, false);
	}

	void textWasEdited() override
	{
		if (m_nRowNumber < m_Annot->size())
			(*m_Annot)[m_nRowNumber].Text(getText().toStdString());
	}

	void paint(juce::Graphics& g) override
	{
		auto& lf = getLookAndFeel();
		if (!dynamic_cast<juce::LookAndFeel_V4*> (&lf))
			lf.setColour(textColourId, juce::Colours::black);
		Label::paint(g);
	}

private:
	juce::TableListBox* m_Table = nullptr;
	std::vector<XAnnotation>* m_Annot = nullptr;
	int		m_nRowNumber;
};

//==============================================================================
// AnnotViewerModel : modele de table pour montrer les annotations
//==============================================================================
class AnnotViewerModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::Slider::Listener,
	public juce::ActionBroadcaster, public juce::ActionListener {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, PenWidth = 4, PenColour = 5, FillColour = 6, Options = 7 } Column;
	AnnotViewerModel();

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;
	juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void sliderValueChanged(juce::Slider* slider) override;
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override { sendActionMessage(message); }

	void SetAnnot(std::vector<XAnnotation>* T) { m_Annot = T; }
	void SetTable(juce::TableListBox* tb) { m_Table = tb; }

private:
	juce::TableListBox*		m_Table;
	std::vector<XAnnotation>* m_Annot;
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// AnnotViewer : fenetre pour contenir le AnnotViewerModel
//==============================================================================
class AnnotViewer : public juce::Component,
	public juce::ActionListener,
	public juce::ActionBroadcaster,
	public juce::DragAndDropTarget,
	public juce::DragAndDropContainer {
public:
	AnnotViewer();

	void SetAnnot(std::vector<XAnnotation>* T) { m_Annot = T;  m_Model.SetAnnot(T); m_Table.updateContent(); m_Table.repaint(); }
	void Update() { m_Table.updateContent(); m_Table.repaint(); m_bDirty = true; }
	void Translate();
	void resized() override;
	bool keyPressed(const juce::KeyPress& key) override;
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override;

	// Drag&Drop
	void itemDropped(const SourceDetails& details) override;
	bool isInterestedInDragSource(const SourceDetails& details) override;
	void itemDragEnter(const SourceDetails&) override { ; }
	void itemDragMove(const SourceDetails&) override { ; }
	void itemDragExit(const SourceDetails&) override { ; }

	// Import / Export des annotations
	void ImportAnnotation();
	void ExportAnnotation();

private:
	std::vector<XAnnotation>* m_Annot;
	bool		m_bDirty;		// Indique que les mises à jours n'ont pas ete sauvees
	juce::TableListBox	m_Table;
	AnnotViewerModel		m_Model;
	juce::TextButton		m_btnImport;
	juce::TextButton		m_btnExport;
	juce::TextButton		m_btnClear;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnnotViewer)
};
