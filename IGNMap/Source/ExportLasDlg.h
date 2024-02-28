//-----------------------------------------------------------------------------
//								ExportLasDlg.h
//								==============
//
// Dialogue d'options pour exporter les fichiers LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/02/2024
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>
#include "GeoBase.h"

class XGeoBase;

//==============================================================================
// Thread pour l'export
//==============================================================================
class LasExportThread : public juce::Thread {
public:
	LasExportThread(const juce::String& threadName, size_t threadStackSize = 0) : juce::Thread(threadName, threadStackSize) { m_GeoBase = nullptr; }
	virtual ~LasExportThread() { ; }

	void SetExportFrame(const XFrame& F) { m_Frame = F; }
	void SetGeoBase(XGeoBase* base) { m_GeoBase = base; }
	void SetFilename(juce::String name) { m_Filename = name; }
	void SetCompression(bool compression) { m_Compression = compression; }

	virtual void 	run() override;

private:
	XGeoBase*			m_GeoBase;
	XFrame        m_Frame;		// Cadre pour l'export
	juce::String	m_Filename;
	bool					m_Compression = false;
	laszip_POINTER	m_Writer = nullptr;
	laszip_header*	m_Header = nullptr;
	laszip_point*		m_Point = nullptr;
	laszip_I64			m_Count = 0;

	void ExportLas(GeoLAS* las);
};

//==============================================================================
// Dialogue pour l'export
//==============================================================================
class ExportLasDlg : public juce::Component, public juce::Button::Listener, private juce::Timer {
public:
	ExportLasDlg(XGeoBase* base, double xmin = 0., double ymin = 0., double xmax = 0., double ymax = 0.);
	virtual ~ExportLasDlg();
	void buttonClicked(juce::Button*) override;
	
private:
	XGeoBase* m_Base;
	juce::TextEditor	m_edtXmin, m_edtYmin, m_edtXmax, m_edtYmax, m_edtFilename;
	juce::Label m_lblXmin, m_lblYmin, m_lblXmax, m_lblYmax;
	juce::TextButton m_btnExport;
	juce::ToggleButton m_btnLaz;

	LasExportThread	m_ExportThread;
	juce::String m_strFilename;
	XFrame m_Frame;
	double m_dProgress = 0.;
	juce::ProgressBar* m_progressBar;
	
	void timerCallback() override;
};