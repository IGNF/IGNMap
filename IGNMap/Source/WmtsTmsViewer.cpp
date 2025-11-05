//-----------------------------------------------------------------------------
//								WmtsTmsViewer.cpp
//								=================
//
// Visualisation des layers d'un serveur WMTS ou TMS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/11/2024
//-----------------------------------------------------------------------------

#include "WmtsTmsViewer.h"
#include "WmtsLayer.h"
#include "TmsLayer.h"
#include "MvtLayer.h"
#include "GeoBase.h"
#include "../../XTool/XGeoBase.h"

WmtsTmsViewerMgr gWmtsTmsViewerMgr;	// Le manager de toutes les fenetres ClassViewer

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
	m_Table.setTooltip(juce::translate("Double-click for loading the layer"));
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
	m_Model.m_Proxy.clear();
	m_Capabilities.Clear();
	juce::MouseCursor::showWaitCursor();
	juce::URL tmpUrl(m_txtUrl.getText());
	// On essaye de corriger l'URL si necessaire
	if (tmpUrl.getScheme().isEmpty())
		tmpUrl = juce::URL("https://" + m_txtUrl.getText());
	juce::String query = tmpUrl.getScheme() + "://" + tmpUrl.getDomain() + "/" + tmpUrl.getSubPath();
	juce::URL url(query);
	// On regarde si on pointe directement sur un fichier XML
	juce::String filename = tmpUrl.getFileName();
	if (filename.endsWithIgnoreCase(".xml")) {
		url = tmpUrl;
		m_strLastUrl = tmpUrl.getParentURL().toString(false);
	} 
	else {	// Cas normal de requete du GetCapabilities
		url = url.withParameter("SERVICE", "WMTS");
		url = url.withParameter("VERSION", "1.0.0");
		url = url.withParameter("REQUEST", "GetCapabilities");
		m_strLastUrl = url.toString(false);
	}
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
		WmtsLayerTMS* layer = new WmtsLayerTMS(m_strLastUrl.toStdString());
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
// WmtsTmsViewer : fermeture de la fenetre
//==============================================================================
void WmtsTmsViewer::closeButtonPressed()
{
	gWmtsTmsViewerMgr.RemoveViewer(this);
	delete this;
}

//==============================================================================
// Ajout d'un Viewer WMTS au gestionnaire de Viewer
//==============================================================================
void WmtsTmsViewerMgr::AddWmtsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base)
{
	WmtsViewer* viewer = new WmtsViewer(name, juce::Colours::grey, juce::DocumentWindow::allButtons, listener, base);
	viewer->setVisible(true);
	m_Viewer.push_back(viewer);
}

//==============================================================================
// Ajout d'un Viewer TMS au gestionnaire de Viewer
//==============================================================================
void WmtsTmsViewerMgr::AddTmsViewer(const juce::String& name, juce::ActionListener* listener, XGeoBase* base)
{
	TmsViewer* viewer = new TmsViewer(name, juce::Colours::grey, juce::DocumentWindow::allButtons, listener, base);
	viewer->setVisible(true);
	m_Viewer.push_back(viewer);
}

//==============================================================================
// Retrait de tous les Viewers
//==============================================================================
void WmtsTmsViewerMgr::RemoveAll()
{
	std::list<WmtsTmsViewer*>::iterator iter;
	for (iter = m_Viewer.begin(); iter != m_Viewer.end(); iter++) {
		delete (*iter);
	}
	m_Viewer.clear();
}

//==============================================================================
// Dessin du fond
//==============================================================================
void TmsViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected) {
		g.setColour(juce::Colours::lightblue);
		g.drawRect(g.getClipBounds());
	}
	if (m_Base != nullptr) {
		XGeoClass* C = m_Base->Class("TMS", m_Proxy[rowNumber].Id.toStdString().c_str());
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
void TmsViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (rowNumber >= m_Proxy.size())
		return;

	std::vector<std::string> Att;
	juce::Image icone;
	switch (columnId) {
	case Column::Id:
		g.drawText(m_Proxy[rowNumber].Id, 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Title:
		g.drawText(m_Proxy[rowNumber].Title, 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Projection:
		g.drawText(m_Proxy[rowNumber].Projection, 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Format:
		g.drawText(m_Proxy[rowNumber].Format, 0, 0, width, height, juce::Justification::centred);
		break;
	default:;
	}
}

//==============================================================================
// Tri des colonnes
//==============================================================================
void TmsViewerModel::sortOrderChanged(int newSortColumnId, bool isForwards)
{
	juce::MouseCursor::showWaitCursor();
	switch (newSortColumnId) {
	case Column::Id:
		std::sort(m_Proxy.begin(), m_Proxy.end(), predTmsInfoId);
		break;
	case Column::Title:
		std::sort(m_Proxy.begin(), m_Proxy.end(), predTmsInfoTitle);
		break;
	case Column::Projection:
		std::sort(m_Proxy.begin(), m_Proxy.end(), predTmsInfoProjection);
		break;
	case Column::Format:
		std::sort(m_Proxy.begin(), m_Proxy.end(), predTmsInfoFormat);
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
void TmsViewerModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
	juce::String message = "LoadLayer:" + juce::String(rowNumber);
	sendActionMessage(message);
}

//==============================================================================
// Taille optimale pour les colonnes
//==============================================================================
int TmsViewerModel::getColumnAutoSizeWidth(int columnId)
{
	int max_size = 0;
	switch (columnId) {
	case Column::Id:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Id.length());
		break;
	case Column::Title:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Title.length());
		break;
	case Column::Projection:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Projection.length());
		break;
	case Column::Format:
		for (int i = 0; i < m_Proxy.size(); i++) max_size = XMax(max_size, m_Proxy[i].Format.length());
		break;
	default:;
	}
	return XMin((int)max_size * 6, 400);
}

//==============================================================================
// WmtsViewerComponent : constructeur
//==============================================================================
TmsViewerComponent::TmsViewerComponent()
{
	m_Base = nullptr;
	setWantsKeyboardFocus(true);

	addAndMakeVisible(m_lblUrl);
	m_lblUrl.setText(juce::translate("Server URL :"), juce::dontSendNotification);
	addAndMakeVisible(m_txtUrl);
	m_txtUrl.setName("URL");
	m_txtUrl.setText("https://data.geopf.fr/tms/1.0.0/", juce::dontSendNotification);
	m_txtUrl.addListener(this);

	m_Model.addActionListener(this);

	// Ajout d'une bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate("ID"), TmsViewerModel::Id, 125);
	m_Table.getHeader().addColumn(juce::translate("Title"), TmsViewerModel::Title, 125);
	m_Table.getHeader().addColumn(juce::translate("Projection"), TmsViewerModel::Projection, 75);
	m_Table.getHeader().addColumn(juce::translate("Format"), TmsViewerModel::Format, 75);
	m_Table.setSize(600, 200);

	m_Table.setModel(&m_Model);
	m_Table.setTooltip(juce::translate("Double-click for loading the layer"));
	addAndMakeVisible(m_Table);
}

//==============================================================================
// Redimensionnement du composant
//==============================================================================
void TmsViewerComponent::resized()
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
void TmsViewerComponent::textEditorReturnKeyPressed(juce::TextEditor& textEdit)
{
	if (textEdit.getName() != "URL")
		return;
	m_Model.m_Proxy.clear();
	juce::MouseCursor::showWaitCursor();
	juce::URL tmpUrl(m_txtUrl.getText());
	// On essaye de corriger l'URL si necessaire
	if (tmpUrl.getScheme().isEmpty())
		tmpUrl = juce::URL("https://" + m_txtUrl.getText());
	juce::String query = tmpUrl.getScheme() + "://" + tmpUrl.getDomain() + "/" + tmpUrl.getSubPath();
	juce::URL url(query);

	// Lecture du XML
	std::unique_ptr<juce::XmlElement> root = url.readEntireXmlStream();
	if (root.get()->getTagName() != "TileMapService")
		return;
	juce::XmlElement* element = root.get()->getChildByName("TileMaps");
	if (element == nullptr) return;
	for (int i = 0; i < element->getNumChildElements(); i++) {
		juce::XmlElement* tileMap = element->getChildElement(i);
		if (tileMap->getTagName() != "TileMap")
			continue;
		TmsViewerModel::TmsInfo info;
		info.Href = tileMap->getStringAttribute("href");
		info.Format = tileMap->getStringAttribute("extension");
		info.Projection = tileMap->getStringAttribute("srs");
		if (info.Projection == "OSGEO:41001") info.Projection = "EPSG:3857";
		info.Title = tileMap->getStringAttribute("title");
		info.Id = info.Href.fromLastOccurrenceOf("/", false, true);
		m_Model.m_Proxy.push_back(info);
	}

	m_Table.updateContent();
	m_Table.autoSizeAllColumns();
	juce::MouseCursor::hideWaitCursor();
}

//==============================================================================
// Gestion des messages
//==============================================================================
void TmsViewerComponent::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateSort") {
		m_Table.repaint();
		return;
	}

	juce::StringArray T;
	T.addTokens(message, ":", "");
	if ((T[0] == "LoadLayer") && (T.size() > 1)) {
		int index = T[1].getIntValue();
		if (index >= m_Model.m_Proxy.size())
			return;
		if ((m_Model.m_Proxy[index].Format == "pbf")|| (m_Model.m_Proxy[index].Format == "mvt")) {
			MvtLayer* mvt = new MvtLayer();
			if (mvt->ReadServer(m_Model.m_Proxy[index].Href)) {
				if (GeoTools::RegisterObject(m_Base, mvt, "MVT", "MVT", m_Model.m_Proxy[index].Id.toStdString())) {
					sendActionMessage("AddWmtsLayer");
					m_Table.repaint();
					return;
				}
			}
			delete mvt;
		}
		else {
			TmsLayer* layer = new TmsLayer();
			if (layer->ReadServer(m_Model.m_Proxy[index].Href)) {
				if (GeoTools::RegisterObject(m_Base, layer, "TMS", "TMS", m_Model.m_Proxy[index].Id.toStdString())) {
					sendActionMessage("AddWmtsLayer");
					m_Table.repaint();
					return;
				}
			}
			delete layer;
		}
	}
}
