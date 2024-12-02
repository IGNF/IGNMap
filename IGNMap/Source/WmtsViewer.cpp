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
#include "../../XTool/XGeoBase.h"

WmtsViewerMgr gWmtsViewerMgr;	// Le manager de toutes les fenetres ClassViewer

//==============================================================================
// Dessin du fond
//==============================================================================
void WmtsViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected) {
		g.setColour(juce::Colours::lightblue);
		g.drawRect(g.getClipBounds());
	}
	if (m_Base != nullptr) {
		XGeoClass* C = m_Base->Class("WMTS", m_Proxy[rowNumber].Id.c_str());
		if (C != nullptr) {
			g.setColour(juce::Colours::darkgreen);
			g.fillRect(g.getClipBounds());
		}
	}
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
	switch (newSortColumnId) {
	case Column::Id:
		std::sort(m_Proxy.begin(), m_Proxy.end(), XWmtsCapabilities::predLayerInfoId);
		break;
	case Column::Projection:
		std::sort(m_Proxy.begin(), m_Proxy.end(), XWmtsCapabilities::predLayerInfoProj);
		break;
	case Column::TMS:
		std::sort(m_Proxy.begin(), m_Proxy.end(), XWmtsCapabilities::predLayerInfoTMS);
		break;
	default:;
	}
	if (!isForwards)
		std::reverse(m_Proxy.begin(), m_Proxy.end());
	juce::MouseCursor::hideWaitCursor();
	sendActionMessage("UpdateSort");
}

//==============================================================================
// Double-clic : chargement du layer
//==============================================================================
void WmtsViewerModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
	juce::String message = "LoadLayer:" + juce::String(rowNumber);
	sendActionMessage(message);
}

//==============================================================================
// Taille optimale pour les colonnes
//==============================================================================
int WmtsViewerModel::getColumnAutoSizeWidth(int columnId)
{
	size_t max_size = 0;
	switch (columnId) {
	case Column::Id:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Id.size());
		break;
	case Column::Title:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Title.size());
		break;
	case Column::Projection:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Projection.size());
		break;
	case Column::TMS:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].TMS.size());
		break;
	default:;
	}
	return XMin((int)max_size*6, 400);
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
	m_txtUrl.setName("URL");
	m_txtUrl.setText("https://data.geopf.fr/wmts", juce::dontSendNotification);
	m_txtUrl.addListener(this);

	m_Model.addActionListener(this);

	// Ajout d'une bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate("ID"), WmtsViewerModel::Id, 125);
	m_Table.getHeader().addColumn(juce::translate("Title"), WmtsViewerModel::Title, 125);
	m_Table.getHeader().addColumn(juce::translate("Projection"), WmtsViewerModel::Projection, 75);
	m_Table.getHeader().addColumn(juce::translate("TMS"), WmtsViewerModel::TMS, 75);
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
	m_txtUrl.setBounds(100, 5, XMax(400, b.getWidth() - 110), 25);
	m_Table.setBounds(5, 40, XMax(400, b.getWidth() - 10), XMax(300, b.getHeight() - 45));
	m_Table.autoSizeAllColumns();
}

//==============================================================================
// Chargement d'une URL
//==============================================================================
void WmtsViewerComponent:: textEditorReturnKeyPressed(juce::TextEditor& textEdit)
{
	if (textEdit.getName() != "URL")
		return;
	juce::MouseCursor::showWaitCursor();
	juce::String query = m_txtUrl.getText() + "?SERVICE=WMTS&VERSION=1.0.0&REQUEST=GetCapabilities";
	juce::URL url(query);
	juce::String content = url.readEntireTextStream();
	std::istringstream stream(content.toStdString());
	XParserXML parser;
	parser.Parse(&stream);
	m_Capabilities.XmlRead(&parser);

	m_Capabilities.GetLayerInfo(m_Model.m_Proxy);
	m_Table.updateContent();
	m_Table.autoSizeAllColumns();
	juce::MouseCursor::hideWaitCursor();
}

//==============================================================================
// Gestion des messages
//==============================================================================
void WmtsViewerComponent::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateSort") {
		m_Table.repaint();
		return;
	}

	juce::StringArray T;
	T.addTokens(message, ":", "");
	if ((T[0] == "LoadLayer")&&(T.size() > 1)) {
		int index = T[1].getIntValue();
		if (index >= m_Model.m_Proxy.size())
			return;
		WmtsLayerTMS* layer = new WmtsLayerTMS(m_txtUrl.getText().toStdString());
		if (m_Capabilities.SetLayerTMS(layer, m_Model.m_Proxy[index].Id, m_Model.m_Proxy[index].TMS)) {
			if (layer->FindProjection()) {
				if (GeoTools::RegisterObject(m_Base, layer, "WMTS", "WMTS", layer->Name())) {
					sendActionMessage("AddWmtsLayer");
					m_Table.repaint();
					return;
				}
			}
		}
		delete layer;
	}
}

//==============================================================================
// WmtsViewer : constructeur
//==============================================================================
WmtsViewer::WmtsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
												juce::ActionListener* listener, XGeoBase* base)
	: juce::DocumentWindow(name, backgroundColour, requiredButtons)
{
	m_Component.SetBase(base);
	m_Component.addActionListener(listener);
	setContentOwned(&m_Component, true);
	setResizable(true, true);
	setAlwaysOnTop(false);
	setSize(600, 400);
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