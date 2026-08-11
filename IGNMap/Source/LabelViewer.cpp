//-----------------------------------------------------------------------------
//								LabelViewer.cpp
//								===============
//
// Creation de lavels
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 10/08/2026
//-----------------------------------------------------------------------------

#include "LabelViewer.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XGeoMap.h"
#include "../XTool/XGeoLabel.h"
#include "ThreadClassProcessor.h"

//==============================================================================
// LabelViewerComponent : constructeur
//==============================================================================
LabelViewerComponent::LabelViewerComponent()
{
	m_Base = nullptr;
	m_lblLayer.setText(juce::translate("Folder") + " :", juce::dontSendNotification);
	m_lblClass.setText(juce::translate("Layer") + " :", juce::dontSendNotification);
	m_lblAttribut.setText(juce::translate("Attribut") + " :", juce::dontSendNotification);
	addAndMakeVisible(m_lblLayer);
	addAndMakeVisible(m_lblClass);
	addAndMakeVisible(m_lblAttribut);

	m_cbxLayer.addListener(this);
	addAndMakeVisible(m_cbxLayer);
	m_cbxClass.addListener(this);
	addAndMakeVisible(m_cbxClass);
	m_cbxAttribut.addListener(this);
	addAndMakeVisible(m_cbxAttribut);

	m_lblLength.setText(juce::translate("Minimum Length") + " :", juce::dontSendNotification);
	m_lblArea.setText(juce::translate("Minimum Area") + " :", juce::dontSendNotification);
	m_lblImportance.setText(juce::translate("Importance") + " :", juce::dontSendNotification);
	m_lblDoublon.setText(juce::translate("Duplicate") + " :", juce::dontSendNotification);
	addAndMakeVisible(m_lblLength);
	addAndMakeVisible(m_lblArea);
	addAndMakeVisible(m_lblImportance);
	addAndMakeVisible(m_lblDoublon);

	addAndMakeVisible(m_sldMinLength);
	m_sldMinLength.setRange(0., 1000., 1.);
	m_sldMinLength.setValue(0., juce::NotificationType::dontSendNotification);
	m_sldMinLength.setSliderStyle(juce::Slider::LinearBar);
	m_sldMinLength.setTextValueSuffix(" m");
	addAndMakeVisible(m_sldMinLength);

	addAndMakeVisible(m_sldMinArea);
	m_sldMinArea.setRange(0., 1000., 1.);
	m_sldMinArea.setValue(0., juce::NotificationType::dontSendNotification);
	m_sldMinArea.setSliderStyle(juce::Slider::LinearBar);
	m_sldMinArea.setTextValueSuffix(" m2");
	addAndMakeVisible(m_sldMinArea);

	addAndMakeVisible(m_sldImportance);
	m_sldImportance.setRange(0., 10., 1.);
	m_sldImportance.setValue(5., juce::NotificationType::dontSendNotification);
	m_sldImportance.setSliderStyle(juce::Slider::LinearBar);
	addAndMakeVisible(m_sldImportance);

	addAndMakeVisible(m_sldDoublon);
	m_sldDoublon.setRange(0., 1000., 1.);
	m_sldDoublon.setValue(0., juce::NotificationType::dontSendNotification);
	m_sldDoublon.setSliderStyle(juce::Slider::LinearBar);
	m_sldDoublon.setTextValueSuffix(" m");
	addAndMakeVisible(m_sldDoublon);

	m_btnRun.setButtonText(juce::translate("Analyze"));
	addAndMakeVisible(m_btnRun);
	m_btnRun.addListener(this);
}

//==============================================================================
// AnalystViewerComponent : Redimensionnement du composant
//==============================================================================
void LabelViewerComponent::resized()
{
	auto w = getLocalBounds().getWidth();
	m_lblLayer.setBounds(5, 5, 80, 24);
	m_cbxLayer.setBounds(100, 5, w - 105, 24);
	m_lblClass.setBounds(5, 35, 80, 24);
	m_cbxClass.setBounds(100, 35, w - 105, 24);
	m_lblAttribut.setBounds(5, 65, 80, 24);
	m_cbxAttribut.setBounds(100, 65, w - 105, 24);

	m_lblLength.setBounds(5, 100, 150, 24);
	m_sldMinLength.setBounds(160, 100, w - 165, 24);
	m_lblArea.setBounds(5, 130, 150, 24);
	m_sldMinArea.setBounds(160, 130, w - 165, 24);
	m_lblImportance.setBounds(5, 160, 150, 24);
	m_sldImportance.setBounds(160, 160, w - 165, 24);
	m_lblDoublon.setBounds(5, 190, 150, 24);
	m_sldDoublon.setBounds(160, 190, w - 165, 24);

	m_btnRun.setBounds(w / 2 - 40, 230, 80, 24);

}

//==============================================================================
// LabelViewerComponent : fixe la base de donnee
//==============================================================================
void LabelViewerComponent::SetBase(XGeoBase* base)
{
	m_cbxLayer.clear(juce::NotificationType::dontSendNotification);
	m_cbxClass.clear(juce::NotificationType::dontSendNotification);
	m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
	m_Base = base;
	if (m_Base == nullptr)
		return;

	for (uint32_t i = 0; i < m_Base->NbLayer(); i++) {
		XGeoLayer* L = m_Base->Layer(i);
		if (L->Name() == "**Label**")
			continue;
		bool vector_layer = false;
		for (uint32_t j = 0; j < L->NbClass(); j++) {
			XGeoClass* C = L->Class(j);
			if (C->IsVector())
				vector_layer = true;
		}
		if (vector_layer)
			m_cbxLayer.addItem(L->Name(), i + 1);
	}
}

//==============================================================================
// LabelViewerComponent : met a jour si la base de donnee a change
//==============================================================================
void LabelViewerComponent::UpdateBase()
{
	if (m_Base == nullptr)
		return;
	SetBase(m_Base);
}

//==============================================================================
// Reponses aux combo box
//==============================================================================
void LabelViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &m_cbxLayer) {
		m_cbxClass.clear(juce::NotificationType::dontSendNotification);
		m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
		juce::String layerName = m_cbxLayer.getText();
		XGeoLayer* L = m_Base->Layer(layerName.toStdString().c_str());
		if (L == nullptr)
			return;
		for (uint32_t j = 0; j < L->NbClass(); j++) {
			XGeoClass* C = L->Class(j);
			if (C->IsVector())
				m_cbxClass.addItem(C->Name(), j + 1);
		}
	}
	if (comboBoxThatHasChanged == &m_cbxClass) {
		m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
		juce::String layerName = m_cbxLayer.getText();
		juce::String className = m_cbxClass.getText();
		XGeoClass* C = m_Base->Class(layerName.toStdString().c_str(), className.toStdString().c_str());
		if (C == nullptr)
			return;
		if (C->NbVector() < 1)
			return;
		XGeoVector* V = C->Vector((uint32_t)0);
		std::vector<std::string> Att;
		if (!V->ReadAttributes(Att))
			return;
		for (int p = 0; p < Att.size(); p += 2)
			m_cbxAttribut.addItem(Att[p], p + 1);
	}
}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void LabelViewerComponent::buttonClicked(juce::Button* button)
{
	if (button == &m_btnRun) {
		juce::String layer = m_cbxLayer.getText(), classe = m_cbxClass.getText(), attribut = m_cbxAttribut.getText();
		if (layer.isEmpty() || classe.isEmpty() || attribut.isEmpty())
			return;
		Compute();
	}

}

//==============================================================================
// Calcul des labels
//==============================================================================
void LabelViewerComponent::Compute()
{
	std::string layer = m_cbxLayer.getText().toStdString();
	std::string classe = m_cbxClass.getText().toStdString();
	std::string attribut = m_cbxAttribut.getText().toStdString();

	if (attribut.size() < 1)
		return;
	XGeoClass* C = m_Base->Class(layer.c_str(), classe.c_str());
	if (C == nullptr)
		return;

	XGeoClass* label_classe = m_Base->AddClass("**Label**", (classe + "_" + attribut).c_str());
	label_classe->Repres()->Name(label_classe->Name().c_str());
	label_classe->Repres()->ZOrder(100000);
	label_classe->Repres()->Color(juce::Colours::black.getARGB());
	label_classe->Repres()->FillColor(juce::Colours::lightcoral.getARGB());

	m_Base->SortClass();

	class MyTask : public ThreadClassProcessor {
	public:
		double m_dAreaMin = 0., m_dLengthMin = 0., m_dDistFiltrage = 0.;
		std::string m_strAttribut;
		uint16_t m_nImportance = 0;
		XGeoClass* m_LabelClass = nullptr;

		MyTask() : ThreadClassProcessor(juce::translate("Compute Labels ..."), true)
		{
		}

		void AddLabel(XGeoClass* C, XGeoVector* V) const
		{
			// Filtrage
			if (m_dDistFiltrage > 0.) {
				double d2 = m_dDistFiltrage * m_dDistFiltrage;
				for (uint32_t i = 0; i < C->NbVector(); i++) {
					XGeoVector* P = C->Vector(i);
					if (dist2(P->Frame().Center(), V->Frame().Center()) > d2)
						continue;
					if (P->Name().compare(V->Name()) != 0)
						continue;
					delete V;
					return;
				}
			}
			V->Class(C);
			C->Vector(V);
		}

		virtual bool Process(XGeoVector* V)
		{
			if (!V->Visible())
				return false;

			if (m_dAreaMin > 0.)
				if (V->IsClosed())
					if (V->Area() < m_dAreaMin)
						return false;

			if (m_dLengthMin > 0.)
				if (V->Length() < m_dLengthMin)
					return false;

			std::string att = V->FindAttribute(m_strAttribut);
			XGeoMap* map = V->Map();
			if (map != NULL)
				if (map->UTF8()) {
					juce::String text = juce::String::fromUTF8(att.c_str());
					att = text.toStdString();
				}

			// Cas des polygones
			if (V->IsClosed()) {
				double dist_min = XMin(V->Frame().Width(), V->Frame().Height());
				XPt2D P = V->Centroide2D(dist_min * 0.4);
				if (P != XPt2D(XGEO_NO_DATA, XGEO_NO_DATA)) {
					XGeoLabel* label = new XGeoLabel(P);
					label->Name(att.c_str());
					label->Rotation(0);
					label->Importance(m_nImportance);
					AddLabel(m_LabelClass, label);
				}
			}

			// Cas des lineaires
			if (!V->IsClosed()) {
				uint16_t rot = 0;
				XGeoLabel* label = new XGeoLabel(V->LabelPoint(&rot));
				label->Name(att.c_str());
				label->Rotation(rot);
				label->Importance(m_nImportance);
				AddLabel(m_LabelClass, label);
			}
			return true;
		}
	};

	MyTask M;
	M.m_dAreaMin = m_sldMinArea.getValue();
	M.m_dDistFiltrage = m_sldDoublon.getValue();
	M.m_dLengthMin = m_sldMinLength.getValue();
	M.m_nImportance = (uint16_t)m_sldImportance.getValue();
	M.m_strAttribut = attribut;
	M.m_T.push_back(C);
	M.m_LabelClass = label_classe;
	if (M.m_dDistFiltrage > 0.) C->QuickSort();
	M.runThread();

	sendActionMessage("UpdateVectorClass");
}

//==============================================================================
// Ajout d'un label a une classe
//==============================================================================
void LabelViewerComponent::AddLabel(XGeoClass* C, XGeoVector* V, const double& minDist)
{
	// Filtrage
	if (minDist > 0.) {
		double d2 = minDist * minDist;
		for (uint32_t i = 0; i < C->NbVector(); i++) {
			XGeoVector* P = C->Vector(i);
			if (dist2(P->Frame().Center(), V->Frame().Center()) > d2)
				continue;
			if (P->Name().compare(V->Name()) != 0)
				continue;
			delete V;
			return;
		}
	}

	V->Class(C);
	C->Vector(V);
}

//==============================================================================
// AnalystViewer : transmission des messages
//==============================================================================
void LabelViewer::SetMessage(const juce::String& message)
{
	if (message == "UpdateVector")
		m_Label.UpdateBase();
}

