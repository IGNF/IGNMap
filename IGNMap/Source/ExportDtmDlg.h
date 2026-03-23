//-----------------------------------------------------------------------------
//								ExportDtmDlg.h
//								==============
//
// Dialogue d'options pour exporter les fichiers LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/03/2026
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>
#include "AppUtil.h"

class XGeoBase;

//==============================================================================
// Dialogue pour l'export
//==============================================================================
class ExportDtmDlg : public juce::Component, public juce::Button::Listener, public juce::ActionBroadcaster {
public:
	ExportDtmDlg(XGeoBase* base, double xmin = 0., double ymin = 0., double xmax = 0., double ymax = 0., 
							double gsd = 0., juce::ActionListener* listener = nullptr);
	virtual ~ExportDtmDlg() { ; }
	void buttonClicked(juce::Button*) override;

private:
	XGeoBase* m_Base;
	juce::String m_strFilename;
	FrameComponent m_FrameCmp;
	juce::TextEditor m_edtFilename;
	FieldEditor m_Gsd;
	juce::TextButton m_btnExport;
	juce::ToggleButton m_btnStl, m_btnStlEdge, m_btnStlZAuto;
	FieldEditor m_StlScale, m_StlZFactor, m_StlStep, m_StlZ0;

	bool ExportStl();
	bool Export();
};