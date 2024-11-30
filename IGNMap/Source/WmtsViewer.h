//-----------------------------------------------------------------------------
//								WmtsViewer.h
//								============
//
// Visualisation des layers d'un serveur WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/11/2024
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

#include "../../XToolAlgo/XWmts.h"

class XGeoBase;

//==============================================================================
// WmtsViewerModel : table pour montrer les objets d'une classe
//==============================================================================
class WmtsViewerModel : public juce::TableListBoxModel, public juce::ActionBroadcaster {
	friend class WmtsViewerComponent;
public:
	typedef enum { Id = 1, Title = 2, Projection = 3, TMS = 4 } Column;
	WmtsViewerModel() { ; }

	int getNumRows() override { return (int)m_Proxy.size(); }
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override { ; }
	void cellDoubleClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override;
	void sortOrderChanged(int newSortColumnId, bool isForwards) override;

private:
	std::vector<XWmtsCapabilities::LayerInfo> m_Proxy;
};

//==============================================================================
// WmtsViewerComponent : composant pour contenir le WmtsViewerModel
//==============================================================================
class WmtsViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
	public juce::TextEditor::Listener {
public:
	WmtsViewerComponent();
	void actionListenerCallback(const juce::String& message) override;
	void SetBase(XGeoBase* base) { m_Base = base; }
private:
	XGeoBase* m_Base;
	juce::TableListBox m_Table;
	WmtsViewerModel m_Model;
	juce::Label m_lblUrl;
	juce::TextEditor m_txtUrl;

	void resized() override;
	void textEditorReturnKeyPressed(juce::TextEditor&) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WmtsViewerComponent)
};

//==============================================================================
// WmtsViewer : fenetre pour contenir le WmtsViewerModel
//==============================================================================
class WmtsViewer : public juce::DocumentWindow, public juce::ActionBroadcaster, public juce::ActionListener {
public:
	WmtsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
						 juce::ActionListener* listener, XGeoBase* base);

	void closeButtonPressed() override;
	void actionListenerCallback(const juce::String& message) override;

private:
	WmtsViewerComponent m_Component;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WmtsViewer)
};

//==============================================================================
// ClassViewerMgr : gestionnaire des fenetres ClassViewer
//==============================================================================
class WmtsViewerMgr {
public:
	WmtsViewerMgr() { ; }
	virtual ~WmtsViewerMgr() { ; }

	void AddWmtsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base);
	void RemoveViewer(WmtsViewer* viewer);
	void RemoveAll();

private:
	std::list<WmtsViewer*> m_Viewer;
};

extern WmtsViewerMgr gWmtsViewerMgr;