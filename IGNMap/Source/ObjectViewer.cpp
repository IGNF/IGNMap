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

#include "ObjectViewer.h"
#include "AffineImage.h"
#include "GeoBase.h"

//==============================================================================
// ObjectViewerComponent : constructeur
//==============================================================================
ObjectViewerComponent::ObjectViewerComponent()
{ 
	m_Object = nullptr; 

	// Interface pour les RotationImage
	m_sldXCenter.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldXCenter.setRange(0., 100., 1.);
	m_sldXCenter.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldXCenter.setTextValueSuffix(juce::translate(" : X"));
	addAndMakeVisible(m_sldXCenter);
	m_sldXCenter.addListener(this);

	m_sldYCenter.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldYCenter.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldYCenter.setTextValueSuffix(juce::translate(" : Y"));
	addAndMakeVisible(m_sldYCenter);
	m_sldYCenter.addListener(this);

	m_sldResolution.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldResolution.setRange(0., 100., 1.);
	m_sldResolution.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldResolution.setTextValueSuffix(juce::String(" : ") + juce::translate("GSD"));
	addAndMakeVisible(m_sldResolution);
	m_sldResolution.addListener(this);

	m_sldRotation.setSliderStyle(juce::Slider::Rotary);
	m_sldRotation.setRotaryParameters(0., (float)(2. * XPI), false);
	m_sldRotation.setRange(0., 360., 1.);
	m_sldRotation.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 25);
	addAndMakeVisible(m_sldRotation);
	m_sldRotation.addListener(this);

	m_sldToneMappingPower.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldToneMappingPower.setRange(0., 100., 1.);
	m_sldToneMappingPower.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldToneMappingPower.setTextValueSuffix(juce::String(" : ") + juce::translate("Luminosity"));
	addAndMakeVisible(m_sldToneMappingPower);
	m_sldToneMappingPower.addListener(this);

	m_sldToneMappingSharpness.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldToneMappingSharpness.setRange(0., 100., 1.);
	m_sldToneMappingSharpness.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldToneMappingSharpness.setTextValueSuffix(juce::String(" : ") + juce::translate("Sharpness"));
	addAndMakeVisible(m_sldToneMappingSharpness);
	m_sldToneMappingSharpness.addListener(this);

	// Interface pour les objets vectoriels
	m_btnPen.setButtonText(juce::translate("Pen"));
	m_btnFill.setButtonText(juce::translate("Fill"));
	addAndMakeVisible(m_btnPen);
	addAndMakeVisible(m_btnFill);
	m_btnPen.addListener(this);
	m_btnFill.addListener(this);

	m_sldPenWidth.setRange(0., 20., 1.);
	m_sldPenWidth.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	addAndMakeVisible(m_sldPenWidth);
	m_sldPenWidth.addListener(this);

	m_btnApply.setButtonText(juce::translate("Apply"));
	m_btnRestore.setButtonText(juce::translate("Restore"));
	m_btnApply.addListener(this);
	m_btnRestore.addListener(this);
	addAndMakeVisible(m_btnApply);
	addAndMakeVisible(m_btnRestore);

	// Interface pour les images Internet
	m_sldZoomCorrection.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldZoomCorrection.setRange(-3., 3., 1.);
	m_sldZoomCorrection.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldZoomCorrection.setTextValueSuffix(juce::String(" : ") + juce::translate("Zoom correction"));
	addAndMakeVisible(m_sldZoomCorrection);
	m_sldZoomCorrection.addListener(this);

	setSize(230, 300);
	SetSelection(nullptr);
}

//==============================================================================
// ObjectViewerComponent : Redimensionnement du composant
//==============================================================================
void ObjectViewerComponent::resized()
{
	auto b = getLocalBounds();
	// Interface pour les RotationImage
	m_sldXCenter.setBounds(10, 10, 100, 30);
	m_sldYCenter.setBounds(120, 10, 100, 30);
	m_sldResolution.setBounds(65, 50, 100, 30);
	m_sldRotation.setBounds(55, 90, 120, 120);
	m_sldToneMappingPower.setBounds(10, 220, 100, 40);
	m_sldToneMappingSharpness.setBounds(120, 220, 100, 40);

	// Interface pour les objets vectoriels
	m_btnPen.setBounds(10, 10, 100, 30);
	m_btnFill.setBounds(120, 10, 100, 30);
	m_sldPenWidth.setBounds(65, 50, 100, 30);
	m_btnRestore.setBounds(10, 100, 100, 30);
	m_btnApply.setBounds(120, 100, 100, 30);

	// Interface pour les images Internet
	m_sldZoomCorrection.setBounds(10, 10, 200, 40);
}

//==============================================================================
// ObjectViewerComponent : Reponse aux actions
//==============================================================================
void ObjectViewerComponent::actionListenerCallback(const juce::String& /*message*/)
{
	
}

//==============================================================================
// ObjectViewerComponent :modification des sliders
//==============================================================================
void ObjectViewerComponent::sliderValueChanged(juce::Slider* /*slider*/)
{
	if (m_Object == nullptr)
		return;
	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr) {
		UpdateRotationImage(image);
		image->SetDirty();
		sendActionMessage("UpdateRaster");
	}

	GeoInternetImage* internet = dynamic_cast<GeoInternetImage*>(m_Object);
	if (internet != nullptr) {
		UpdateInternetImage(internet);
		internet->SetDirty();
		sendActionMessage("UpdateRaster");
	}

}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void ObjectViewerComponent::buttonClicked(juce::Button* button)
{
	if (m_Object == nullptr)
		return;
	XGeoVector* V = dynamic_cast<XGeoVector*>(m_Object);
	if (V != nullptr) {
		if (button == &m_btnRestore) {
			V->Repres(nullptr);
			SetGeoVector(V);
		}
		if (button == &m_btnApply) {
			XGeoRepres* R = new XGeoRepres;
			R->Deletable(true);
			juce::Colour color = m_btnPen.findColour(juce::TextButton::buttonColourId);
			R->Color(color.getARGB());
			color = m_btnFill.findColour(juce::TextButton::buttonColourId);
			R->FillColor(color.getARGB());
			R->Size((uint8_t)m_sldPenWidth.getValue());
			V->Repres(R);
		}
		sendActionMessage("UpdateVector");
		return;
	}
}


//==============================================================================
// ObjectViewerComponent : fixe la selection
//==============================================================================
bool ObjectViewerComponent::SetSelection(void* S)
{
	m_sldXCenter.setVisible(false);
	m_sldYCenter.setVisible(false);
	m_sldResolution.setVisible(false);
	m_sldRotation.setVisible(false);
	m_sldToneMappingPower.setVisible(false);
	m_sldToneMappingSharpness.setVisible(false);
	m_btnPen.setVisible(false);
	m_btnFill.setVisible(false);
	m_sldPenWidth.setVisible(false);
	m_btnApply.setVisible(false);
	m_btnRestore.setVisible(false);
	m_sldZoomCorrection.setVisible(false);

	m_Object = (XGeoObject*)S;
	if (m_Object == nullptr)
		return true;

	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr)
		return SetRotationImage(image);
	GeoInternetImage* internet = dynamic_cast<GeoInternetImage*>(m_Object);
	if (internet != nullptr)
		return SetInternetImage(internet);
	XGeoVector* V = dynamic_cast<XGeoVector*>(m_Object);
	if (V != nullptr)
		return SetGeoVector(V);
	return false;
}

//==============================================================================
// ObjectViewerComponent : choix d'une selection RotationImage
//==============================================================================
bool ObjectViewerComponent::SetRotationImage(RotationImage* image)
{
	m_sldXCenter.setVisible(true);
	m_sldYCenter.setVisible(true);
	m_sldResolution.setVisible(true);
	m_sldRotation.setVisible(true);
	m_sldToneMappingPower.setVisible(true);
	m_sldToneMappingSharpness.setVisible(true);
	
	XPt2D S = image->Centroide();
	m_sldXCenter.setRange(S.X - 200., S.X + 200., 5.);
	m_sldYCenter.setRange(S.Y - 200., S.Y + 200., 5.);
	m_sldXCenter.setValue(S.X, juce::NotificationType::dontSendNotification);
	m_sldYCenter.setValue(S.Y, juce::NotificationType::dontSendNotification);
	double gsd = image->Resolution();
	m_sldResolution.setRange(gsd - gsd * 0.5, gsd + gsd * 0.5, gsd * 0.02);
	m_sldResolution.setValue(gsd, juce::NotificationType::dontSendNotification);
	int rot = XRint(360. - image->Rotation() * 180. / XPI) % 360;
	m_sldRotation.setValue(rot, juce::NotificationType::dontSendNotification);
	m_sldToneMappingPower.setValue(image->GetToneMapperPower(), juce::NotificationType::dontSendNotification);
	m_sldToneMappingSharpness.setValue(image->GetToneMapperSharpness(), juce::NotificationType::dontSendNotification);
	return true;
}

//==============================================================================
// UpdateRotationImage : mise a jour d'une RotationImage
//==============================================================================
bool ObjectViewerComponent::UpdateRotationImage(RotationImage* image)
{
	double X = m_sldXCenter.getValue();
	double Y = m_sldYCenter.getValue();
	double gsd = m_sldResolution.getValue();
	double rot = m_sldRotation.getValue();
	image->SetPosition(X, Y, gsd);
	image->SetRotation(-rot * XPI / 180.);
	double power = m_sldToneMappingPower.getValue();
	double sharp = m_sldToneMappingSharpness.getValue();
	if ((power < 1) && (sharp < 1)) {
		image->RemoveToneMapper();
		return true;
	}
	image->AddToneMapper();
	image->SetToneMapperPower((int)power);
	image->SetToneMapperSharpness((int)sharp);
	return true;
}

//==============================================================================
// SetGeoVector : choix d'une selection XGeoVector
//==============================================================================
bool ObjectViewerComponent::SetGeoVector(XGeoVector* V)
{
	m_btnPen.setVisible(true);
	m_btnFill.setVisible(true);
	m_sldPenWidth.setVisible(true);
	m_btnApply.setVisible(true);
	m_btnRestore.setVisible(true);
	XGeoRepres* R = V->Repres();
	if (R == nullptr)
		return false;
	m_btnPen.setColour(juce::TextButton::buttonColourId, juce::Colour(R->Color()));
	m_btnFill.setColour(juce::TextButton::buttonColourId, juce::Colour(R->FillColor()));
	m_sldPenWidth.setValue(R->Size(), juce::NotificationType::dontSendNotification);
	return true;
}

//==============================================================================
// SetInternetImage : choix d'une selection GeoInternetImage
//==============================================================================
bool ObjectViewerComponent::SetInternetImage(GeoInternetImage* internet)
{
	m_sldZoomCorrection.setVisible(true);
	m_sldZoomCorrection.setValue(internet->GetZoomCorrection());
	return true;
}

//==============================================================================
// UpdateInternetImage : mise a jour d'une GeoInternetImage
//==============================================================================
bool ObjectViewerComponent::UpdateInternetImage(GeoInternetImage* internet)
{
	double X = m_sldZoomCorrection.getValue();
	internet->SetZoomCorrection((int)X);
	return true;
}