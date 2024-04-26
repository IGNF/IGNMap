//-----------------------------------------------------------------------------
//								PrefDlg.h
//								=========
//
// Dialogue de preferences
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/04/2024
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>

class PrefDlg : public juce::Component, public juce::Button::Listener, public juce::ComboBox::Listener {
public:
	PrefDlg();

	void buttonClicked(juce::Button*) override;
	void comboBoxChanged(juce::ComboBox*) override;

private:
	juce::Label m_lblRegion;
	juce::Label m_lblProjection;
	juce::ComboBox m_cbxRegion;
	juce::ComboBox m_cbxProjection;
	juce::TextButton m_btnApply;

	void SetProjectionComboBox();
};