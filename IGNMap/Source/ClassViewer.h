//-----------------------------------------------------------------------------
//								ClassViewer.h
//								=============
//
// Visualisation des des objets d'une classe
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/05/2024
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

class XGeoClass;
class XGeoVector;

//==============================================================================
// ClassViewerModel : table pour montrer les objets d'une classe
//==============================================================================
class ClassViewerModel : public juce::TableListBoxModel, public juce::ActionBroadcaster {
public:
	typedef enum { Attribut = 1, Visibility = 1000, Selectable = 1001 } Column;
	ClassViewerModel() { m_Class = nullptr; }

	int getNumRows() override;
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

	void SetClass(XGeoClass* C) { m_Class = C; }
	XGeoVector* FindVector(int index);

private:
	XGeoClass* m_Class;
};

//==============================================================================
// ClassViewer : fenetre pour contenir le ClassViewerModel
//==============================================================================
class ClassViewer : public juce::DocumentWindow, public juce::ActionBroadcaster, public juce::ActionListener {
public:
	ClassViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons, 
							XGeoClass* C, juce::ActionListener* listener = nullptr);

	void closeButtonPressed() override { delete this; }
	// Gestion des actions
	void actionListenerCallback(const juce::String& message) override;

private:
	juce::TableListBox m_Table;
	ClassViewerModel m_Model;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassViewer)
};
