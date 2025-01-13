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

#include "AppUtil.h"
#include "GeoBase.h"
#include <functional>

class XGeoBase;
class XGeoClass;

//==============================================================================
// SentinelImportTask : import des images Sentinel
//==============================================================================
class SentinelImportTask : public juce::ThreadWithProgressWindow {
public:
	SentinelImportTask() : ThreadWithProgressWindow("busy...", true, true) { ; }
	void SetImportParam(XGeoBase* base, juce::String foldername, bool resol10, bool resol20, bool resol60)
	{
		m_Base = base; m_strFoldername = foldername; m_b10m = resol10; m_b20m = resol20; m_b60m = resol60;
	}
	void SetActiveParam(int activeResol, XSentinelScene::ViewMode mode)
	{
		m_nActiveResolution = activeResol;
		m_ViewMode = mode;
	}
	void run();
	std::string Projection() const { return m_strProjection; }
private:
	XGeoBase* m_Base = nullptr;
	juce::String m_strFoldername;
	bool m_b10m = false;
	bool m_b20m = false;
	bool m_b60m = false;
	int m_nActiveResolution = 10;
	XSentinelScene::ViewMode m_ViewMode = XSentinelScene::ViewMode::RGB;
	std::string m_strProjection;

	void ImportResol(GeoSentinelImage* scene, juce::File* folder, int resol);
};

//==============================================================================
// SentinelAnalyzeTask : analyse des images Sentinel
//==============================================================================
class SentinelAnalyzeTask : public juce::ThreadWithProgressWindow {
public:
	struct Result {
		GeoSentinelImage* Scene = nullptr;
		double Index = -1.;
		juce::Time Time;
		int State = 0;
	};

	SentinelAnalyzeTask() : ThreadWithProgressWindow("busy...", true, true) { ; }
	void run();

	XGeoMap* m_Map = nullptr;
	XPt2D m_P;
	std::vector<Result> m_Result;
	static bool predDate(const Result& a, const Result& b) { return (b.Scene->Date() > a.Scene->Date()); }
};

//==============================================================================
// SentinelAnalyzeDraw : dessin d'une analyse
//==============================================================================
class SentinelAnalyzeDraw: public juce::Component, public juce::ActionBroadcaster, public juce::SettableTooltipClient {
public:
	void paint(juce::Graphics& g) override;
	void mouseDoubleClick(const juce::MouseEvent& event) override;
	void SetDate(juce::int64 time) { m_ShowDate = time; juce::Time T(time); setTooltip(T.toString(true, false)); }

	std::vector<SentinelAnalyzeTask::Result>* m_Result = nullptr;
	juce::int64 m_ShowDate = 0;
};

//==============================================================================
// SentinelSceneModel : modele pour contenir les scenes Sentinel
//==============================================================================
class SentinelSceneModel : public juce::TableListBoxModel, public juce::ActionBroadcaster, public juce::ComboBox::Listener {
public:
	typedef enum { Visibility = 1, Selectable = 2, Name = 3, NbScene = 4, Date = 5 } Column;
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
// SentinelExtractModel : modele pour contenir des extraits Sentinel
//==============================================================================
class SentinelExtractModel : public juce::TableListBoxModel, public juce::ActionBroadcaster {
public:
	typedef enum { Visibility = 1, Selectable = 2, Date = 3, Image = 4} Column;
	SentinelExtractModel() : juce::TableListBoxModel() { m_Result = nullptr; }
	~SentinelExtractModel() { ; }

	void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	void paintRowBackground(juce::Graphics&, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/) override;
	int getNumRows() override { return 1; }
	void cellClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;

	int NbScene() { return (int)m_Result->size(); }
	std::vector<SentinelAnalyzeTask::Result>* m_Result;
};

//==============================================================================
// SentinelViewerComponent : composant principal
//==============================================================================
class SentinelViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
																public juce::Button::Listener, public juce::ComboBox::Listener, public juce::Slider::Listener {
public:
	SentinelViewerComponent();
	void SetBase(XGeoBase* base) { m_Base = base; m_mdlScene.SetBase(base); }

	void actionListenerCallback(const juce::String& message) override;
	void buttonClicked(juce::Button*) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	void sliderValueChanged(juce::Slider* slider) override;

	void SetTarget(const double& X, const double& Y, const double& /*Z*/);

private:
	XGeoBase* m_Base;
	double m_dX0, m_dY0;
	juce::GroupComponent m_grpImport;
	juce::TextButton m_btnImport;
	juce::ToggleButton m_btn10m;
	juce::ToggleButton m_btn20m;
	juce::ToggleButton m_btn60m;
	juce::ComboBox m_cbxResol;
	juce::ComboBox m_cbxMode;
	juce::Label m_lblParam;
	juce::Slider m_sldParam;
	juce::TableListBox m_tblScene;
	SentinelSceneModel m_mdlScene;
	juce::TextButton m_btnAnalyze;
	juce::TableListBox m_tblExtract;
	SentinelExtractModel m_mdlExtract;
	std::vector<SentinelAnalyzeTask::Result> m_AnalyzeResult;
	SentinelAnalyzeDraw m_DrawAnalyze;

	void resized() override;

	void ImportScenes();
	void EnableViewMode(int resol = 0);
	void SetProjection(std::string projection);
	void Analyze();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SentinelViewerComponent)
};

//==============================================================================
// SentinelViewer : fenetre container
//==============================================================================
class SentinelViewer : public ToolWindow {
public:
	SentinelViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Sentinel.SetBase(base);
		m_Sentinel.addActionListener(listener);
		setContentOwned(&m_Sentinel, true);
	}

	void SetTarget(const double& X, const double& Y, const double& Z) override { m_Sentinel.SetTarget(X, Y, Z); }

private:
	SentinelViewerComponent		m_Sentinel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SentinelViewer)
};

#endif // SENTINELVIEWER_H