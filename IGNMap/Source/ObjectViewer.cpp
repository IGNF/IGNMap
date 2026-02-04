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
#include "MvtLayer.h"
#include "../XToolImage/XTiffWriter.h"

//==============================================================================
// ObjectViewerComponent : constructeur
//==============================================================================
ObjectViewerComponent::ObjectViewerComponent()
{ 
	m_Object = nullptr;
	m_Repres = nullptr;

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

	m_sldFrameExport.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldFrameExport.setRange(0., 100., 1.);
	m_sldFrameExport.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	m_sldFrameExport.setTextValueSuffix(juce::String(" : ") + juce::translate("Useful Frame"));
	addAndMakeVisible(m_sldFrameExport);
	m_sldFrameExport.addListener(this);

	addAndMakeVisible(m_lblFrameSize);

	m_btnExportUsefulFrame.setButtonText(juce::translate("Export"));
	addAndMakeVisible(m_btnExportUsefulFrame);
	m_btnExportUsefulFrame.addListener(this);

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

	m_sldFontSize.setRange(6., 72., 1.);
	m_sldFontSize.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
	addAndMakeVisible(m_sldFontSize);
	m_sldFontSize.addListener(this);

	juce::StringArray fonts = juce::Font::findAllTypefaceNames();
	m_cbxFont.addItem(" ", 1);
	m_cbxFont.addItemList(fonts, 2);
	addAndMakeVisible(m_cbxFont);
	m_cbxFont.addListener(this);

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

	addAndMakeVisible(m_cbxStyle);
	m_cbxStyle.addListener(this);

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
	m_sldFrameExport.setBounds(10, 270, 100, 40);
	m_lblFrameSize.setBounds(120, 275, 100, 30);
	m_btnExportUsefulFrame.setBounds(65, 320, 100, 30);

	// Interface pour les objets vectoriels
	m_btnPen.setBounds(10, 10, 100, 30);
	m_btnFill.setBounds(120, 10, 100, 30);
	m_sldPenWidth.setBounds(65, 50, 100, 30);
	m_cbxFont.setBounds(10, 100, 200, 40);
	m_sldFontSize.setBounds(65, 150, 100, 30);
	m_btnRestore.setBounds(10, 200, 100, 30);
	m_btnApply.setBounds(120, 200, 100, 30);

	// Interface pour les images Internet
	m_sldZoomCorrection.setBounds(10, 10, 200, 40);
	m_cbxStyle.setBounds(10, 100, 200, 40);
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
void ObjectViewerComponent::sliderValueChanged(juce::Slider* slider)
{
	if (m_Object == nullptr)
		return;
	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr) {
		if (slider == &m_sldFrameExport) {
			UpdateFrameExport(image);
		}
		else {
			UpdateRotationImage(image);
			image->SetDirty();
			sendActionMessage("UpdateRaster");
		}
		return;
	}

	GeoInternetImage* internet = dynamic_cast<GeoInternetImage*>(m_Object);
	if (internet != nullptr) {
		UpdateInternetImage(internet);
		internet->SetDirty();
		sendActionMessage("UpdateRaster");
		return;
	}

}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void ObjectViewerComponent::buttonClicked(juce::Button* button)
{
	if (m_Object == nullptr)
		return;
	if (button == &m_btnExportUsefulFrame) {
		ExportUsefulFrame();
		return;
	}
	XGeoVector* V = dynamic_cast<XGeoVector*>(m_Object);
	if (V != nullptr) {
		if (button == &m_btnRestore) {
			V->Repres(nullptr);
			SetGeoRepres(V->Repres());
		}
		if (button == &m_btnApply) {
			XGeoRepres* R = new XGeoRepres;
			R->Deletable(true);
			juce::Colour color = m_btnPen.findColour(juce::TextButton::buttonColourId);
			R->Color(color.getARGB());
			color = m_btnFill.findColour(juce::TextButton::buttonColourId);
			R->FillColor(color.getARGB());
			R->Size((uint8_t)m_sldPenWidth.getValue());
			R->FontSize((uint8_t)m_sldFontSize.getValue());
			R->Font(m_cbxFont.getText().toStdString().c_str());
			V->Repres(R);
		}
		sendActionMessage("UpdateVector");
		return;
	}
	XGeoClass* C = dynamic_cast<XGeoClass*>(m_Object);
	if (C != nullptr) {
		if (button == &m_btnRestore) {
			*(C->Repres()) = *m_Repres;
			SetGeoRepres(C->Repres());
		}
		if (button == &m_btnApply) {
			XGeoRepres* R = C->Repres();
			juce::Colour color = m_btnPen.findColour(juce::TextButton::buttonColourId);
			R->Color(color.getARGB());
			color = m_btnFill.findColour(juce::TextButton::buttonColourId);
			R->FillColor(color.getARGB());
			R->Size((uint8_t)m_sldPenWidth.getValue());
			R->FontSize((uint8_t)m_sldFontSize.getValue());
			R->Font(m_cbxFont.getText().toStdString().c_str());
		}
		sendActionMessage("UpdateVector");
		return;
	}
}

//==============================================================================
// Reponses aux combo box
//==============================================================================
void ObjectViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (m_Object == nullptr)
		return;
	MvtLayer* V = dynamic_cast<MvtLayer*>(m_Object);
	if ((V != nullptr)&&(comboBoxThatHasChanged == &m_cbxStyle)){
		V->LoadStyle(m_cbxStyle.getText());
		V->SetDirty();
		sendActionMessage("UpdateRaster");
		return;
	}
}

//==============================================================================
// ObjectViewerComponent : fixe le point cible
//==============================================================================
void ObjectViewerComponent::SetTarget(const double& X, const double& Y, const double& Z)
{
	m_Target = XPt3D(X, Y, Z); 
	if (m_Object == nullptr)
		return;
	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr)
		UpdateFrameExport(image);
}

//==============================================================================
// ObjectViewerComponent : fixe la selection
//==============================================================================
bool ObjectViewerComponent::SetSelection(XGeoObject* S)
{
	m_sldXCenter.setVisible(false);
	m_sldYCenter.setVisible(false);
	m_sldResolution.setVisible(false);
	m_sldRotation.setVisible(false);
	m_sldToneMappingPower.setVisible(false);
	m_sldToneMappingSharpness.setVisible(false);
	m_sldFrameExport.setVisible(false);
	m_btnExportUsefulFrame.setVisible(false);
	m_lblFrameSize.setVisible(false);
	m_btnPen.setVisible(false);
	m_btnFill.setVisible(false);
	m_sldPenWidth.setVisible(false);
	m_cbxFont.setVisible(false);
	m_sldFontSize.setVisible(false);
	m_btnApply.setVisible(false);
	m_btnRestore.setVisible(false);
	m_sldZoomCorrection.setVisible(false);
	m_cbxStyle.setVisible(false);
	if (m_Repres != nullptr)
		delete m_Repres;
	m_Repres = nullptr;

	m_Object = (XGeoObject*)S;
	if (m_Object == nullptr)
		return true;

	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image != nullptr)
		return SetRotationImage(image);
	GeoInternetImage* internet = dynamic_cast<GeoInternetImage*>(m_Object);
	if (internet != nullptr)
		return SetInternetImage(internet);
	XGeoRepres* R = nullptr;
	XGeoVector* V = dynamic_cast<XGeoVector*>(m_Object);
	if (V != nullptr)
		R = V->Repres();
	XGeoClass* C = dynamic_cast<XGeoClass*>(m_Object);
	if (C != nullptr)
		R = C->Repres();
	if (R != nullptr) {
		m_Repres = new XGeoRepres;
		*m_Repres = *R;
		SetGeoRepres(R);
	}
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
	m_sldFrameExport.setVisible(true);
	m_btnExportUsefulFrame.setVisible(true);
	m_lblFrameSize.setVisible(true);
	
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
	m_sldFrameExport.setValue(0, juce::NotificationType::dontSendNotification);;
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
bool ObjectViewerComponent::SetGeoRepres(XGeoRepres* R)
{
	if (R == nullptr)
		return false;

	m_btnPen.setVisible(true);
	m_btnFill.setVisible(true);
	m_sldPenWidth.setVisible(true);
	m_cbxFont.setVisible(true);
	m_sldFontSize.setVisible(true);
	m_btnApply.setVisible(true);
	m_btnRestore.setVisible(true);

	m_btnPen.setColour(juce::TextButton::buttonColourId, juce::Colour(R->Color()));
	m_btnFill.setColour(juce::TextButton::buttonColourId, juce::Colour(R->FillColor()));
	m_sldPenWidth.setValue(R->Size(), juce::NotificationType::dontSendNotification);
	m_cbxFont.setText(R->Font(), juce::NotificationType::dontSendNotification);
	m_sldFontSize.setValue(R->FontSize(), juce::NotificationType::dontSendNotification);
	return true;
}

//==============================================================================
// SetInternetImage : choix d'une selection GeoInternetImage
//==============================================================================
bool ObjectViewerComponent::SetInternetImage(GeoInternetImage* internet)
{
	m_sldZoomCorrection.setVisible(true);
	m_sldZoomCorrection.setValue(internet->GetZoomCorrection());
	MvtLayer* mvt = dynamic_cast<MvtLayer*>(internet);
	if (mvt != nullptr) {
		m_cbxStyle.clear(juce::NotificationType::dontSendNotification);
		m_cbxStyle.setVisible(true);
		juce::StringArray A;
		mvt->GetStyleFiles(A);
		m_cbxStyle.addItemList(A, 1);
	}

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

//==============================================================================
// ComputeFrameExport : calcul du cadre pour l'export
//==============================================================================
void ObjectViewerComponent::ComputeFrameExport(RotationImage* image, std::vector<XPt2D>& T)
{
	XPt3D C = m_Target;
	double rot = image->Rotation(); rot = 0.;
	double ratio = m_sldFrameExport.getValue() * 0.005;
	double W = image->GetImageW(), H = image->GetImageH();
	W = H = XMin(W, H);
	double gsd = image->Resolution();
	
	T.clear();
	double X = ratio * W * gsd * cos(rot) - ratio * H * gsd * sin(rot);
	double Y = ratio * W * gsd * sin(rot) + ratio * H * gsd * cos(rot);
	T.push_back(XPt2D(C.X + X, C.Y + Y));
	X = ratio * W * gsd * cos(rot) + ratio * H * gsd * sin(rot);
	Y = ratio * W * gsd * sin(rot) - ratio * H * gsd * cos(rot);
	T.push_back(XPt2D(C.X + X, C.Y + Y));
	X = -ratio * W * gsd * cos(rot) + ratio * H * gsd * sin(rot);
	Y = -ratio * W * gsd * sin(rot) - ratio * H * gsd * cos(rot);
	T.push_back(XPt2D(C.X + X, C.Y + Y));
	X = -ratio * W * gsd * cos(rot) - ratio * H * gsd * sin(rot);
	Y = -ratio * W * gsd * sin(rot) + ratio * H * gsd * cos(rot);
	T.push_back(XPt2D(C.X + X, C.Y + Y));
}

//==============================================================================
// UpdateFrameExport : mise a jour du cadre pour l'export
//==============================================================================
bool ObjectViewerComponent::UpdateFrameExport(RotationImage* image)
{
	juce::String message = "UpdateTargetPoly:";
	std::vector<XPt2D> T;
	ComputeFrameExport(image, T);
	message += (juce::String(T[0].X, 2) + ":" + juce::String(T[0].Y, 2) + ":0.0:");
	message += (juce::String(T[1].X, 2) + ":" + juce::String(T[1].Y, 2) + ":0.0:");
	message += (juce::String(T[2].X, 2) + ":" + juce::String(T[2].Y, 2) + ":0.0:");
	message += (juce::String(T[3].X, 2) + ":" + juce::String(T[3].Y, 2) + ":0.0");
	sendActionMessage(message);
	double gsd = image->Resolution();
	m_lblFrameSize.setText(juce::String(XRint(fabs(T[2].X - T[0].X)/gsd)) + ";" + juce::String(XRint(fabs(T[1].Y - T[0].Y)/gsd)), 
												 juce::NotificationType::dontSendNotification);
	return true;
}

//==============================================================================
// ExportUsefulFrame : export du cadre pour l'export
//==============================================================================
void ObjectViewerComponent::ExportUsefulFrame()
{
	RotationImage* image = dynamic_cast<RotationImage*>(m_Object);
	if (image == nullptr)
		return;
	std::vector<XPt2D> T;
	ComputeFrameExport(image, T);
	XFrame F;
	for (size_t i = 0; i < T.size(); i++)
		F += T[i];
	juce::String filename = AppUtil::SaveFile("ExportUsefulFrame", juce::translate("File to save"), "*.png");
	if (filename.isEmpty())
		return;
	double gsd = image->Resolution();
	juce::MouseCursor::showWaitCursor();
	juce::Image export_ima = image->GetAreaImage(F, gsd);
	juce::File file(filename);
	if (file.existsAsFile())
		file.deleteFile();
	juce::FileOutputStream outputFileStream(file);
	juce::PNGImageFormat png;
	png.writeImageToStream(export_ima, outputFileStream);
	juce::MouseCursor::hideWaitCursor();
	file.revealToUser();
	// Creation du fichier TFW
	juce::File tfw = file.withFileExtension("tfw");
	tfw.replaceWithText(juce::String(gsd, 2) + "\n0.\n0.\n0.\n" + juce::String(F.Xmin + gsd * 0.5, 2) +
																							"\n" + juce::String(F.Ymax - gsd * 0.5, 2));
}