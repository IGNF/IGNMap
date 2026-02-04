//-----------------------------------------------------------------------------
//								ExportImageDlg.h
//								================
//
// Dialogue d'options pour exporter les images
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 17/01/2024
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "MapThread.h"

class XGeoBase;

class ExportImageDlg : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster, private juce::Timer {
public:
	ExportImageDlg(XGeoBase* base, double xmin = 0., double ymin = 0., double xmax = 0., double ymax = 0., double gsd = 1.);
	virtual ~ExportImageDlg();
	void buttonClicked(juce::Button*) override;

private:
	XGeoBase* m_Base;
	juce::TextEditor	m_edtXmin, m_edtYmin, m_edtXmax, m_edtYmax, m_edtGsd, m_edtFilename;
	juce::Label m_lblXmin, m_lblYmin, m_lblXmax, m_lblYmax, m_lblGsd;
	juce::TextButton m_btnExport;
	juce::ImageButton m_btnView;
	juce::DrawableButton m_btnKm;

	MapThread		m_MapThread;
	juce::String m_strFilename;
	double	m_dX0 = 0., m_dY0 = 0., m_dGSD = 1.;
	int m_nW = 0, m_nH = 0, m_nNumThread = 0;
	double m_dProgress = 0.;
	juce::ProgressBar* m_progressBar;

	void ViewFrame();
	void RoundFrame();
	void Export();
	void timerCallback() override;
	void StartNextThread();
};