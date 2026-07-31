//-----------------------------------------------------------------------------
//								AnalystViewer.h
//								===============
//
// Visualisation des analyses vectorielles
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 31/07/2026
//-----------------------------------------------------------------------------

#include "AnalystViewer.h"
#include "../XTool/XGeoBase.h"

//==============================================================================
// AnalystViewerComponent : constructeur
//==============================================================================
AnalystViewerComponent::AnalystViewerComponent()
{
	m_Base = nullptr;
	m_cbxLayer.addListener(this);
	addAndMakeVisible(m_cbxLayer);
	m_cbxClass.addListener(this);
	addAndMakeVisible(m_cbxClass);
	m_cbxAttribut.addListener(this);
	addAndMakeVisible(m_cbxAttribut);

}

//==============================================================================
// AnalystViewerComponent : fixe la base de donnee
//==============================================================================
void AnalystViewerComponent::SetBase(XGeoBase* base)
{
	m_Base = base;
	m_cbxLayer.clear(juce::NotificationType::dontSendNotification);
	m_cbxClass.clear(juce::NotificationType::dontSendNotification);
	m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);

	for (uint32_t i = 0; i < m_Base->NbLayer(); i++) {
		XGeoLayer* L = m_Base->Layer(i);
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
// Reponses aux combo box
//==============================================================================
void AnalystViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
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
		for (int p = 0; p < Att.size(); p+=2)
			m_cbxAttribut.addItem(Att[p], p+1);
	}


}

//==============================================================================
// SentinelViewerComponent : Redimensionnement du composant
//==============================================================================
void AnalystViewerComponent::resized()
{
	m_cbxLayer.setBounds(5, 5, 200, 25);
	m_cbxClass.setBounds(15, 30, 200, 25);
	m_cbxAttribut.setBounds(25, 55, 200, 25);
}