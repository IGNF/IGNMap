//-----------------------------------------------------------------------------
//								WmtsTmsViewer.h
//								===============
//
// Visualisation des layers d'un serveur WMTS ou TMS
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
// WmtsTmsViewer : classe mere pour WmtsViewer et TmsViewer
//==============================================================================
class WmtsTmsViewer : public juce::DocumentWindow {
public:
	WmtsTmsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* , XGeoBase* )
		: juce::DocumentWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
	}

	void closeButtonPressed() override;

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WmtsTmsViewer)
};

//==============================================================================
// WmtsTmsViewerMgr : gestionnaire des fenetres WmtsViewer et TmsViewer
//==============================================================================
class WmtsTmsViewerMgr {
public:
	WmtsTmsViewerMgr() { ; }
	virtual ~WmtsTmsViewerMgr() { ; }

	void AddWmtsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base);
	void AddTmsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base);
	void RemoveViewer(WmtsTmsViewer* viewer) { m_Viewer.remove(viewer); }
	void RemoveAll();

private:
	std::list<WmtsTmsViewer*> m_Viewer;
};

extern WmtsTmsViewerMgr gWmtsTmsViewerMgr;	// Gestionnaire de fenetres

//==============================================================================
// WmtsViewerModel : table pour montrer les objets d'une classe
//==============================================================================
class WmtsViewerModel : public juce::TableListBoxModel, public juce::ActionBroadcaster {
	friend class WmtsViewerComponent;
public:
	typedef enum { Id = 1, Title = 2, Projection = 3, TMS = 4 } Column;
	WmtsViewerModel() { m_Base = nullptr; }

	int getNumRows() override { return (int)m_Proxy.size(); }
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override { ; }
	void cellDoubleClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override;
	void sortOrderChanged(int newSortColumnId, bool isForwards) override;
	int getColumnAutoSizeWidth(int columnId) override;

private:
	XGeoBase* m_Base;
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
	void SetBase(XGeoBase* base) { m_Base = base; m_Model.m_Base = base; }
private:
	XGeoBase* m_Base;
	juce::TableListBox m_Table;
	WmtsViewerModel m_Model;
	juce::Label m_lblUrl;
	juce::TextEditor m_txtUrl;
	juce::String m_strLastUrl;
	XWmtsCapabilities m_Capabilities;

	void resized() override;
	void textEditorReturnKeyPressed(juce::TextEditor&) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WmtsViewerComponent)
};

//==============================================================================
// WmtsViewer : fenetre pour contenir le WmtsViewerModel
//==============================================================================
class WmtsViewer : public WmtsTmsViewer {
public:
	WmtsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
						 juce::ActionListener* listener, XGeoBase* base)
					: WmtsTmsViewer(name, backgroundColour, requiredButtons, listener, base)
	{
		m_Component.SetBase(base);
		m_Component.addActionListener(listener);
		setContentOwned(&m_Component, true);
		setSize(600, 400);
	}

private:
	WmtsViewerComponent m_Component;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WmtsViewer)
};

//==============================================================================
// WmtsViewerModel : table pour montrer les objets d'une classe
//==============================================================================
class TmsViewerModel : public juce::TableListBoxModel, public juce::ActionBroadcaster {
	friend class TmsViewerComponent;
public:
	typedef enum { Id = 1, Title = 2, Projection = 3, Format = 4 } Column;
	TmsViewerModel() { m_Base = nullptr; }

	struct TmsInfo {
		juce::String Href;
		juce::String Id;
		juce::String Title;
		juce::String Projection;
		juce::String Format;
	};
	static bool predTmsInfoId(const TmsInfo& A, const TmsInfo& B) {return (A.Id > B.Id);}
	static bool predTmsInfoTitle(const TmsInfo& A, const TmsInfo& B) { return (A.Title > B.Title); }
	static bool predTmsInfoProjection(const TmsInfo& A, const TmsInfo& B) { return (A.Projection > B.Projection); }
	static bool predTmsInfoFormat(const TmsInfo& A, const TmsInfo& B) { return (A.Format > B.Format); }

	int getNumRows() override { return (int)m_Proxy.size(); }
	void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
	void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override;
	void cellClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override { ; }
	void cellDoubleClicked(int /*rowNumber*/, int /*columnId*/, const juce::MouseEvent&) override;
	void sortOrderChanged(int newSortColumnId, bool isForwards) override;
	int getColumnAutoSizeWidth(int columnId) override;

private:
	XGeoBase* m_Base;
	std::vector<TmsInfo> m_Proxy;
};

//==============================================================================
// WmtsViewerComponent : composant pour contenir le WmtsViewerModel
//==============================================================================
class TmsViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
	public juce::TextEditor::Listener {
public:
	TmsViewerComponent();
	void actionListenerCallback(const juce::String& message) override;
	void SetBase(XGeoBase* base) { m_Base = base; m_Model.m_Base = base; }
private:
	XGeoBase* m_Base;
	juce::TableListBox m_Table;
	TmsViewerModel m_Model;
	juce::Label m_lblUrl;
	juce::TextEditor m_txtUrl;
	juce::String m_strLastUrl;

	void resized() override;
	void textEditorReturnKeyPressed(juce::TextEditor&) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TmsViewerComponent)
};

//==============================================================================
// WmtsViewer : fenetre pour contenir le WmtsViewerModel
//==============================================================================
class TmsViewer : public WmtsTmsViewer {
public:
	TmsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: WmtsTmsViewer(name, backgroundColour, requiredButtons, listener, base)
	{
		m_Component.SetBase(base);
		m_Component.addActionListener(listener);
		setContentOwned(&m_Component, true);
		setSize(600, 400);
	}

private:
	TmsViewerComponent m_Component;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TmsViewer)
};

