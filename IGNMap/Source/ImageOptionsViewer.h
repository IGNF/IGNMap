//-----------------------------------------------------------------------------
//								ImageOptionsViewer.h
//								====================
//
// Visulisation des options d'une image
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/09/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XBase.h"
class GeoFileImage;
class XGeoBase;

//==============================================================================
// PixelValuesModel : fenetre pour voir les options d'une image
//==============================================================================
class PixelValuesModel : public juce::TableListBoxModel {
public:
	PixelValuesModel() : juce::TableListBoxModel() { PixX = PixY = 0; WinSize = 3; NbSample = 0; PixValue = nullptr;}
	~PixelValuesModel() { ClearPixels(); }

	virtual void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual void paintRowBackground(juce::Graphics&, int /*rowNumber*/, int /*width*/, int /*height*/, bool /*rowIsSelected*/) override { ; }
	virtual int getNumRows() override { return 2 * WinSize + 1; }
	virtual void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent&) override;
	virtual juce::String getCellTooltip(int rowNumber, int columnId) override { return GetText(rowNumber, columnId); }

	bool AllocPixels(uint32_t nbSample);
	void ClearPixels() { if (PixValue != nullptr) { delete[] PixValue; PixValue = nullptr; } }
	juce::String GetText(int rowNumber, int columnId);

	uint32_t PixX;
	uint32_t PixY;
	uint32_t WinSize;
	uint32_t NbSample;
	double* PixValue;
	uint8_t R_channel = 0, G_channel = 1, B_channel = 2;
	uint8_t NbBits = 8;
};

//==============================================================================
// ImageOptionsViewer : fenetre pour voir les options d'une image
//==============================================================================
class ImageOptionsViewer : public juce::Component, 
	public juce::ComboBox::Listener,
	public juce::Slider::Listener,
	public juce::Button::Listener,
	public juce::ActionBroadcaster {
public:
	ImageOptionsViewer();

	void SetImage(GeoFileImage* image);
	void SetGeoBase(XGeoBase* base);
	void SetGroundPos(const double& X, const double& Y);
	void SetPixPos(const int& X, const int& Y);

	virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	virtual void sliderValueChanged(juce::Slider* slider) override;
	virtual void buttonClicked(juce::Button*) override;

private:
	juce::Label m_lblImageName;
	juce::TextEditor m_txtImageName;
	juce::TextEditor m_txtMetadata;
	juce::Label m_lblRGBChannels;
	juce::ComboBox m_cbxRChannel;
	juce::ComboBox m_cbxGChannel;
	juce::ComboBox m_cbxBChannel;
	juce::TableListBox m_tblPixels;
	PixelValuesModel m_PixModel;
	juce::Slider m_sldLine;
	juce::Slider m_sldColumn;
	juce::ToggleButton m_btnPalette;
	juce::TextEditor m_txtPalette;

	GeoFileImage* m_Image;

	void resized() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImageOptionsViewer)
};