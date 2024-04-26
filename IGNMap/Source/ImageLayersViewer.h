//-----------------------------------------------------------------------------
//								ImageLayersViewer.h
//								===================
//
// Visulisation des classes d'objets images
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 16/10/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"

//==============================================================================
// LayerViewerComponent : table pour montrer les proprietes des layers
//==============================================================================
class ImageViewerModel : public juce::TableListBoxModel,
	public juce::ChangeListener,
	public juce::Slider::Listener,
	public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, Opacity = 4, FillColour = 5, Options = 6 } Column;
	ImageViewerModel();

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	juce::var getDragSourceDescription(const juce::SparseSet<int>& selectedRows) override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void sliderValueChanged(juce::Slider* slider) override;

	void SetBase(XGeoBase* base) { m_Base = base; }
	XGeoClass* FindRasterClass(int index);

private:
	XGeoBase* m_Base;
	int										m_ActiveRow;
	int										m_ActiveColumn;
};

//==============================================================================
// ImageLayersViewer : fenetre pour contenir le LayerViewerComponent
//==============================================================================
class ImageLayersViewer : public juce::Component,
	public juce::ActionListener,
	public juce::ActionBroadcaster,
	public juce::DragAndDropTarget,
	public juce::DragAndDropContainer {
public:
	ImageLayersViewer();

	void SetBase(XGeoBase* base) { m_Base = base;  m_Model.SetBase(base); m_Table.updateContent(); }
	void SetActionListener(juce::ActionListener* listener) { addActionListener(listener); m_Model.addActionListener(listener); }
	void Translate();
	void resized() override { auto b = getLocalBounds(); m_Table.setSize(b.getWidth(), b.getHeight()); }
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override;

	// Drag&Drop
	void itemDropped(const SourceDetails& details) override;
	bool isInterestedInDragSource(const SourceDetails& details) override;
	void itemDragEnter(const SourceDetails&) override { ; }
	void itemDragMove(const SourceDetails&) override { ; }
	void itemDragExit(const SourceDetails&) override { ; }

private:
	XGeoBase* m_Base;
	juce::TableListBox	m_Table;
	ImageViewerModel		m_Model;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageLayersViewer)
};