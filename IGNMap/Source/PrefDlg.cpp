//-----------------------------------------------------------------------------
//								PrefDlg.cpp
//								===========
//
// Dialogue de preferences
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/04/2024
//-----------------------------------------------------------------------------

#include "PrefDlg.h"
#include "../../XToolGeod/XGeoProjection.h"
#include "../../XToolGeod/XGeoPref.h"

PrefDlg::PrefDlg()
{
	addAndMakeVisible(m_cbxRegion);
	m_cbxRegion.addItem(juce::translate("World"), 1);
	m_cbxRegion.addItem(juce::translate("Europe"), 2);
	m_cbxRegion.addItem(juce::translate("France"), 3);
	m_cbxRegion.addItem(juce::translate("Antilles"), 4);
	m_cbxRegion.addItem(juce::translate("French Guiana"), 5);
	m_cbxRegion.addItem(juce::translate("Reunion"), 6);
	m_cbxRegion.addItem(juce::translate("Mayotte"), 7);
	m_cbxRegion.addItem(juce::translate("Saint-Pierre-et-Miquelon"), 8);
	m_cbxRegion.addItem(juce::translate("New Caledonia"), 9);
	m_cbxRegion.addListener(this);

	addAndMakeVisible(m_cbxProjection);
	XGeoPref pref;
	juce::String region = juce::translate(XGeoProjection::Region(pref.Projection()));
	juce::String projection = XGeoProjection::ProjectionName(pref.Projection());
	for (int i = 0; i < m_cbxRegion.getNumItems(); i++)
		if (m_cbxRegion.getItemText(i) == region)
			m_cbxRegion.setSelectedItemIndex(i, juce::NotificationType::dontSendNotification);
	SetProjectionComboBox();
	for (int i = 0; i < m_cbxProjection.getNumItems(); i++)
		if (m_cbxProjection.getItemText(i) == projection)
			m_cbxProjection.setSelectedItemIndex(i, juce::NotificationType::dontSendNotification);

	addAndMakeVisible(m_lblRegion);
	addAndMakeVisible(m_lblProjection);

	m_lblRegion.setText(juce::translate("Region : "), juce::NotificationType::dontSendNotification);
	m_lblProjection.setText(juce::translate("Projection : "), juce::NotificationType::dontSendNotification);

	m_lblRegion.setBounds(10, 30, 70, 24);
	m_cbxRegion.setBounds(90, 30, 200, 24);
	m_lblProjection.setBounds(10, 60, 70, 24);
	m_cbxProjection.setBounds(90, 60, 200, 24);

	addAndMakeVisible(m_btnApply);
	m_btnApply.setButtonText(juce::translate("Apply"));
	m_btnApply.setBounds(160, 170, 80, 30);
	m_btnApply.addListener(this);
}

//-----------------------------------------------------------------------------
// Fixe la combo box des projections en fonction de la region
//-----------------------------------------------------------------------------
void PrefDlg::SetProjectionComboBox()
{
	int proj;
	m_cbxProjection.clear(juce::NotificationType::dontSendNotification);
	switch (m_cbxRegion.getSelectedId()) {
	case 1 : // World
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::WebMercator), XGeoProjection::WebMercator);
		break;
	case 2 : // Europe
		for (proj = XGeoProjection::ETRS89LCC; proj <= XGeoProjection::ETRS89TM32; proj++)
			m_cbxProjection.addItem(XGeoProjection::ProjectionName((XGeoProjection::XProjCode)proj), proj);
		break;
	case 3: // France
		for (proj = XGeoProjection::Lambert93; proj <= XGeoProjection::LambertCC50; proj++)
			m_cbxProjection.addItem(XGeoProjection::ProjectionName((XGeoProjection::XProjCode)proj), proj);
		break;
	case 4: // Antilles
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::RGAF09), XGeoProjection::RGAF09);
		break;
	case 5: // Guyane
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::RGFG95), XGeoProjection::RGFG95);
		break;
	case 6: // Reunion
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::RGR92), XGeoProjection::RGR92);
		break;
	case 7: // Mayotte
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::RGM04), XGeoProjection::RGM04);
		break;
	case 8: // StPierre
		m_cbxProjection.addItem(XGeoProjection::ProjectionName(XGeoProjection::RGSPM06), XGeoProjection::RGSPM06);
		break;
	case 9: // Nouvelle-Caledonie
		for (proj = XGeoProjection::NC_RGNC91_Lambert; proj <= XGeoProjection::NC_RGNC91_UTM59; proj++)
			m_cbxProjection.addItem(XGeoProjection::ProjectionName((XGeoProjection::XProjCode)proj), proj);
		break;
	default: return;
	}
	m_cbxProjection.setSelectedItemIndex(0);
}

//-----------------------------------------------------------------------------
// Changement de region
//-----------------------------------------------------------------------------
void PrefDlg::comboBoxChanged(juce::ComboBox* box)
{
	if (box == &m_cbxRegion)
		SetProjectionComboBox();
}

//-----------------------------------------------------------------------------
// Validation de la projection
//-----------------------------------------------------------------------------
void PrefDlg::buttonClicked(juce::Button* button)
{
	if (button == &m_btnApply) {
		int choice = m_cbxProjection.getSelectedId();
		XGeoPref pref;
		pref.Projection((XGeoProjection::XProjCode)choice);
		pref.ProjecView((XGeoProjection::XProjCode)choice);
		juce::Component* parent = getParentComponent();
		if (parent != nullptr)
			delete parent;
		return;
	}
}

