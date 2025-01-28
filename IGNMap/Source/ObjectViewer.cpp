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


//==============================================================================
// ObjectViewerComponent : constructeur
//==============================================================================
ObjectViewerComponent::ObjectViewerComponent()
{ 
	m_Object = nullptr; 

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
	m_sldResolution.setTextValueSuffix(juce::translate(" : GSD"));
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
	m_sldToneMappingPower.setTextValueSuffix(juce::translate(" : Luminosity"));
	addAndMakeVisible(m_sldToneMappingPower);
	m_sldToneMappingPower.addListener(this);

	m_sldToneMappingSharpness.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldToneMappingSharpness.setRange(0., 100., 1.);
	m_sldToneMappingSharpness.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldToneMappingSharpness.setTextValueSuffix(juce::translate(" : Sharpness"));
	addAndMakeVisible(m_sldToneMappingSharpness);
	m_sldToneMappingSharpness.addListener(this);

	setSize(400, 450);
	SetSelection(nullptr);
}

//==============================================================================
// ObjectViewerComponent : Redimensionnement du composant
//==============================================================================
void ObjectViewerComponent::resized()
{
	auto b = getLocalBounds();
	m_sldXCenter.setBounds(10, 10, 100, 30);
	m_sldYCenter.setBounds(120, 10, 100, 30);
	m_sldResolution.setBounds(65, 50, 100, 30);
	m_sldRotation.setBounds(55, 90, 120, 120);
	m_sldToneMappingPower.setBounds(10, 220, 100, 40);
	m_sldToneMappingSharpness.setBounds(120, 220, 100, 40);
}

//==============================================================================
// ObjectViewerComponent : Reponse aux actions
//==============================================================================
void ObjectViewerComponent::actionListenerCallback(const juce::String& message)
{
	
}

//==============================================================================
// ObjectViewerComponent :modification des sliders
//==============================================================================
void ObjectViewerComponent::sliderValueChanged(juce::Slider* slider)
{
	if (m_Object == nullptr)
		return;
	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr) {
		UpdateRotationImage(image);
		image->SetDirty();
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

}


//==============================================================================
// ObjectViewerComponent : fixe la selection
//==============================================================================
bool ObjectViewerComponent::SetSelection(void* S)
{
	m_Object = (XGeoObject*)S;
	if (m_Object == nullptr) {
		m_sldXCenter.setVisible(false);
		m_sldYCenter.setVisible(false);
		m_sldResolution.setVisible(false);
		m_sldRotation.setVisible(false);
		m_sldToneMappingPower.setVisible(false);
		m_sldToneMappingSharpness.setVisible(false);
		return true;
	}

	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr)
		return SetRotationImage(image);
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
// ObjectViewerComponent : mise a jour d'une RotationImage
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