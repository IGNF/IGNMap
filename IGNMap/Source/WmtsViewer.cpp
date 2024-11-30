//-----------------------------------------------------------------------------
//								WmtsViewer.cpp
//								==============
//
// Visualisation des layers d'un serveur WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/11/2024
//-----------------------------------------------------------------------------

#include "WmtsViewer.h"
#include "WmtsLayer.h"
#include "GeoBase.h"

WmtsViewerMgr gWmtsViewerMgr;	// Le manager de toutes les fenetres ClassViewer

//==============================================================================
// Dessin du fond
//==============================================================================
void WmtsViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void WmtsViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (rowNumber >= m_Proxy.size())
		return;

	std::vector<std::string> Att;
	juce::Image icone;
	switch (columnId) {
	case Column::Id:
		g.drawText(juce::String(m_Proxy[rowNumber].Id), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Title:
		g.drawText(juce::String(m_Proxy[rowNumber].Title), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Projection:
		g.drawText(juce::String(m_Proxy[rowNumber].Projection), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::TMS:
		g.drawText(juce::String(m_Proxy[rowNumber].TMS), 0, 0, width, height, juce::Justification::centred);
		break;
	default:;
	}
}

//==============================================================================
// Tri des colonnes
//==============================================================================
void WmtsViewerModel::sortOrderChanged(int newSortColumnId, bool isForwards)
{
	juce::MouseCursor::showWaitCursor();

	juce::MouseCursor::hideWaitCursor();
	sendActionMessage("UpdateSort");
}

//==============================================================================
// Double-clic : chargement du layer
//==============================================================================
void WmtsViewerModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
	juce::String message = "LoadLayer" + juce::String(rowNumber);
	sendActionMessage(message);
}

//==============================================================================
// WmtsViewerComponent : constructeur
//==============================================================================
WmtsViewerComponent::WmtsViewerComponent()
{
	m_Base = nullptr;
	setWantsKeyboardFocus(true);

	addAndMakeVisible(m_lblUrl);
	m_lblUrl.setText(juce::translate("Server URL :"), juce::dontSendNotification);
	addAndMakeVisible(m_txtUrl);
	m_txtUrl.setText("https://data.geopf.fr/wmts", juce::dontSendNotification);
	m_txtUrl.addListener(this);

	m_Model.addActionListener(this);

	// Ajout d'une bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate("ID"), WmtsViewerModel::Id, 100);
	m_Table.getHeader().addColumn(juce::translate("Title"), WmtsViewerModel::Title, 100);
	m_Table.getHeader().addColumn(juce::translate("Projection"), WmtsViewerModel::Projection, 100);
	m_Table.getHeader().addColumn(juce::translate("TMS"), WmtsViewerModel::TMS, 100);
	m_Table.setSize(600, 200);

	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// Redimensionnement du composant
//==============================================================================
void WmtsViewerComponent::resized()
{
	auto b = getLocalBounds();
	m_lblUrl.setBounds(5, 5, 90, 25);
	m_txtUrl.setBounds(100, 5, XMax(300, b.getWidth() - 110), 25);
	m_Table.setBounds(5, 40, XMax(300, b.getWidth() - 10), XMax(300, b.getHeight() - 45));
}

//==============================================================================
// Chargement d'une URL
//==============================================================================
void WmtsViewerComponent:: textEditorReturnKeyPressed(juce::TextEditor& textEdit)
{
	juce::MouseCursor::showWaitCursor();
	juce::String query = m_txtUrl.getText() + "?SERVICE=WMTS&VERSION=1.0.0&REQUEST=GetCapabilities";
	juce::URL url(query);
	juce::String content = url.readEntireTextStream();
	std::istringstream stream(content.toStdString());
	XParserXML parser;
	parser.Parse(&stream);
	XWmtsCapabilities cap;
	cap.XmlRead(&parser);

	std::vector<XWmtsCapabilities::LayerInfo> L;
	cap.GetLayerInfo(m_Model.m_Proxy);
	m_Table.updateContent();
	juce::MouseCursor::hideWaitCursor();
}

//==============================================================================
// Gestion des messages
//==============================================================================
void WmtsViewerComponent::actionListenerCallback(const juce::String& message)
{

}

//==============================================================================
// WmtsViewer : constructeur
//==============================================================================
WmtsViewer::WmtsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
												juce::ActionListener* listener, XGeoBase* base)
	: juce::DocumentWindow(name, backgroundColour, requiredButtons)
{
	m_Component.SetBase(base);
	setContentOwned(&m_Component, true);
	setResizable(true, true);
	setAlwaysOnTop(false);
	setSize(400, 400);
}

//==============================================================================
// WmtsViewer : fermeture de la fenetre
//==============================================================================
void WmtsViewer::closeButtonPressed()
{
	gWmtsViewerMgr.RemoveViewer(this);
	delete this;
}

//==============================================================================
// Gestion des actions
//==============================================================================
void WmtsViewer::actionListenerCallback(const juce::String& message)
{
	/*
	if (message == "UpdateSort") {
		m_Table.repaint();
		return;
	}

	// Objets selectionnees
	juce::SparseSet< int > S = m_Table.getSelectedRows();
	for (int i = 0; i < S.size(); i++) {
		
	}
	*/
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Ajout d'un Viewer au gestionnaire de Viewer
//==============================================================================
void WmtsViewerMgr::AddWmtsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base)
{
	WmtsViewer* viewer = new WmtsViewer(name, juce::Colours::grey, juce::DocumentWindow::allButtons, listener, base);
	viewer->setVisible(true);
	m_Viewer.push_back(viewer);
}

//==============================================================================
// Retrait d'un Viewer du gestionnaire de Viewer
//==============================================================================
void WmtsViewerMgr::RemoveViewer(WmtsViewer* viewer)
{
	m_Viewer.remove(viewer);
}

//==============================================================================
// Retrait de tous les Viewers
//==============================================================================
void WmtsViewerMgr::RemoveAll()
{
	std::list<WmtsViewer*>::iterator iter;
	for (iter = m_Viewer.begin(); iter != m_Viewer.end(); iter++) {
		delete (*iter);
	}
	m_Viewer.clear();
}