//-----------------------------------------------------------------------------
//								ObjectViewer.h
//								==============
//
// Visualisation des caracteristiques des objets
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/01/2025
//-----------------------------------------------------------------------------

#ifndef OBJECTVIEWER_H
#define OBJECTVIEWER_H

#include "AppUtil.h"
#include "../XTool/XPt3D.h"
#include "../XTool/XGeoRepres.h"

class XGeoObject;
class XGeoVector;
class XGeoRepres;
class RotationImage;
class GeoInternetImage;

//==============================================================================
// ObjectViewerComponent : composant principal
//==============================================================================
class ObjectViewerComponent : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster,
	public juce::Button::Listener, public juce::ComboBox::Listener, public juce::Slider::Listener {
public:
	ObjectViewerComponent();
	virtual ~ObjectViewerComponent() { if (m_Repres != nullptr) delete m_Repres; }
	
	void actionListenerCallback(const juce::String& message) override;
	void buttonClicked(juce::Button*) override;
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	void sliderValueChanged(juce::Slider* slider) override;

	bool SetSelection(XGeoObject*);
	bool SetRotationImage(RotationImage* image);
	bool UpdateRotationImage(RotationImage* image);
	void ComputeFrameExport(RotationImage* image, std::vector<XPt2D>& T);
	bool UpdateFrameExport(RotationImage* image);
	void ExportUsefulFrame();
	bool SetGeoRepres(XGeoRepres* R);
	bool SetInternetImage(GeoInternetImage* internet);
	bool UpdateInternetImage(GeoInternetImage* internet);

	void SetTarget(const double& X, const double& Y, const double& Z) { m_Target = XPt3D(X, Y, Z); }

private:
	XGeoObject* m_Object;
	XGeoRepres* m_Repres;
	XPt3D m_Target;

	// Interface pour les images rotation
	juce::Slider m_sldXCenter;
	juce::Slider m_sldYCenter;
	juce::Slider m_sldResolution;
	juce::Slider m_sldRotation;
	juce::Slider m_sldToneMappingPower;
	juce::Slider m_sldToneMappingSharpness;
	juce::Slider m_sldFrameExport;
	juce::TextButton m_btnExportUsefulFrame;
	juce::Label m_lblFrameSize;

	// Interface pour les objets vectoriels
	ColourChangeButton m_btnPen;
	ColourChangeButton m_btnFill;
	juce::Slider m_sldPenWidth;
	juce::TextButton m_btnApply;
	juce::TextButton m_btnRestore;
	juce::ComboBox m_cbxFont;
	juce::Slider m_sldFontSize;

	// Interface pour les images internet
	juce::Slider m_sldZoomCorrection;
	juce::ComboBox m_cbxStyle;

	void resized() override;
};

//==============================================================================
// ObjectViewer : fenetre container
//==============================================================================
class ObjectViewer : public ToolWindow {
public:
	ObjectViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		m_Object.addActionListener(listener);
		setContentOwned(&m_Object, true);
		setResizeLimits(230, 400, 1000, 1000);
	}

	void SetTarget(const double& X, const double& Y, const double& Z) override { m_Object.SetTarget(X, Y, Z); }
	void SetSelection(void* S) override { m_Object.SetSelection((XGeoObject*)S); }

private:
	ObjectViewerComponent		m_Object;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectViewer)
};


#endif // OBJECTVIEWER_H