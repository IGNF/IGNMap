//-----------------------------------------------------------------------------
//								ImageLayersViewer.cpp
//								=====================
//
// Visulisation des classes d'objets images
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 16/10/2023
//-----------------------------------------------------------------------------

#include "ImageLayersViewer.h"
#include "ClassViewer.h"
#include "Utilities.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"

//==============================================================================
// LayerViewerComponent : constructeur
//==============================================================================
ImageViewerModel::ImageViewerModel()
{
	m_Base = nullptr;
	m_ActiveRow = m_ActiveColumn = -1;
}

//==============================================================================
// FindDtmClass : renvoie la ieme classe DTM de la base ou nullptr sinon
//==============================================================================
XGeoClass* ImageViewerModel::FindRasterClass(int index)
{
	if (m_Base == nullptr)
		return nullptr;
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsRaster()) {
			count++;
			if (count == index)
				return C;
		}
	}
	return nullptr;
}

//==============================================================================
// Nombre de lignes de la table
//==============================================================================
int ImageViewerModel::getNumRows()
{
	if (m_Base == nullptr)
		return 0;
	int count = 0;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsRaster())
			count++;
	}
	return count;
}

//==============================================================================
// Dessin du fond
//==============================================================================
void ImageViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void ImageViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	XGeoClass* geoLayer = FindRasterClass(rowNumber);
	if (geoLayer == nullptr)
		return;
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (geoLayer->Visible())
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (geoLayer->Selectable())
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:// Name
		g.drawText(juce::String(geoLayer->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Opacity:// Opacite
		g.drawText(juce::String(100 - geoLayer->Repres()->Transparency()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::FillColour:// brush
		g.setColour(juce::Colour(geoLayer->Repres()->FillColor()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::Options:// Options
		icone = juce::ImageCache::getFromMemory(BinaryData::Options_png, BinaryData::Options_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	}
}

//==============================================================================
// Clic dans une cellule
//==============================================================================
void ImageViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* geoLayer = FindRasterClass(rowNumber);
	if (geoLayer == nullptr)
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		sendActionMessage("UpdateImageVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateImageSelectability");
		return;
	}

	// Choix d'une couleur
	if (columnId == Column::FillColour) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("background");
		if (columnId == Column::FillColour)
			colourSelector->setCurrentColour(juce::Colour(geoLayer->Repres()->FillColor()));
		colourSelector->addChangeListener(this);
		colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
		colourSelector->setSize(400, 300);

		juce::CallOutBox::launchAsynchronously(std::move(colourSelector), bounds, nullptr);
		return;
	}
	// Choix d'une epaisseur
	if (columnId == Column::Opacity) {
		auto widthSelector = std::make_unique<juce::Slider>();
		widthSelector->setRange(0., 100., 1.);
		widthSelector->setValue(100. - geoLayer->Repres()->Transparency());
		widthSelector->setSliderStyle(juce::Slider::LinearHorizontal);
		widthSelector->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
		widthSelector->setSize(200, 50);
		widthSelector->setChangeNotificationOnlyOnRelease(true);
		widthSelector->addListener(this);
		juce::CallOutBox::launchAsynchronously(std::move(widthSelector), bounds, nullptr);
		return;
	}
	// Options
	if (columnId == Column::Options) { // Creation d'un popup menu

		std::function< void() > LayerCenter = [=]() {	// Position au centre du cadre
			XPt2D P = geoLayer->Frame().Center();
			sendActionMessage("CenterFrame:" + juce::String(P.X, 2) + ":" + juce::String(P.Y, 2)); };
		std::function< void() > LayerFrame = [=]() { // Zoom pour voir l'ensemble du cadre
			XFrame F = geoLayer->Frame();
			sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
				juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2)); };
		std::function< void() > LayerGsd = [=]() { // Zoom a la resolution native de la couche
			XGeoVector* V = geoLayer->Vector((uint32_t)0);
			if (V != nullptr)
				sendActionMessage("ZoomGsd:" + juce::String(V->Resolution(), 2)); };
		std::function< void() > LayerRemove = [=]() { // Retire la couche
			sendActionMessage("RemoveImageClass"); };
		std::function< void() > ViewObjects = [=]() { // Visualisation des objets de la classe
			gClassViewerMgr.AddClassViewer(geoLayer->Name(), geoLayer, this);
			};

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addItem(juce::translate("Layer GSD"), LayerGsd);
		menu.addItem(juce::translate("View Objects"), ViewObjects);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void ImageViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
{
	XGeoClass* geoLayer = FindRasterClass(rowNumber);
	if (geoLayer == nullptr)
		return;

	// Nom du layer
	if (columnId == Column::Name) {
		XFrame F = geoLayer->Frame();
		sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
			juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
		return;
	}

}

//==============================================================================
// Drag&Drop des lignes pour changer l'ordre des layers
//==============================================================================
juce::var ImageViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// ImageViewerModel : changeListenerCallback
//==============================================================================
void ImageViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	XGeoClass* geoLayer = FindRasterClass(m_ActiveRow);
	if (geoLayer == nullptr)
		return;

	// Choix d'une couleur
	if (m_ActiveColumn == Column::FillColour) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			uint32_t color = cs->getCurrentColour().getARGB();
			if (m_ActiveColumn == Column::FillColour)
				geoLayer->Repres()->FillColor(color);
			sendActionMessage("UpdateFillOpacity");
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void ImageViewerModel::sliderValueChanged(juce::Slider* slider)
{
	XGeoClass* geoLayer = FindRasterClass(m_ActiveRow);
	if (geoLayer == nullptr)
		return;

	// Choix d'une opacite
	if (m_ActiveColumn == Column::Opacity) {
		if (geoLayer->Repres()->Transparency() != (100 - (int)slider->getValue())) {
			geoLayer->Repres()->Transparency((uint8_t)(100 - (int)slider->getValue()));
			sendActionMessage("UpdateFillOpacity");
		}
	}
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
ImageLayersViewer::ImageLayersViewer()
{
	m_Base = nullptr;
	setName("Layers");
	m_Model.addActionListener(this);
	// Bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), ImageViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), ImageViewerModel::Column::Selectable, 25);
	m_Table.getHeader().addColumn(juce::translate("Name"), ImageViewerModel::Column::Name, 200);
	m_Table.getHeader().addColumn(juce::translate("Opacity"), ImageViewerModel::Column::Opacity, 50);
	m_Table.getHeader().addColumn(juce::translate("Background"), ImageViewerModel::Column::FillColour, 50);
	m_Table.getHeader().addColumn(juce::translate(" "), ImageViewerModel::Column::Options, 25);
	m_Table.setSize(377, 200);
	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void ImageLayersViewer::Translate()
{
	m_Table.getHeader().setColumnName(ImageViewerModel::Column::Name, juce::translate("Name"));
	m_Table.getHeader().setColumnName(ImageViewerModel::Column::Opacity, juce::translate("Opacity"));
	m_Table.getHeader().setColumnName(ImageViewerModel::Column::FillColour, juce::translate("Background"));
}

//==============================================================================
// Gestion des actions
//==============================================================================
void ImageLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateFillOpacity") {
		repaint();
		return;
	}
	if (message == "NewWindow") {
		m_Table.updateContent();
		m_Table.repaint();
		return;
	}
	if (message == "UpdateClass") {
		sendActionMessage("UpdateRaster");
		return;
	}
	
	// Classes selectionnees
	std::vector<XGeoClass*> T;
	if (m_Base != nullptr) {
		juce::SparseSet< int > S = m_Table.getSelectedRows();
		int count = -1;
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsRaster()) {
				count++;
				if (S.contains(count))
					T.push_back(C);
			}
		}
	}

	if (message == "UpdateImageVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_Table.repaint();
		sendActionMessage("UpdateRaster");
		return;
	}
	if (message == "UpdateImageSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_Table.repaint();
		return;
	}
	if (message == "RemoveImageClass") {
		m_Base->ClearSelection();
		for (int i = 0; i < T.size(); i++)
			m_Base->RemoveClass(T[i]->Layer()->Name().c_str(), T[i]->Name().c_str());
		m_Table.deselectAllRows();
		m_Table.repaint();
		sendActionMessage("UpdateSelectFeatures");
		sendActionMessage("UpdateRaster");
		return;
	}
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Drag&Drop
//==============================================================================
void ImageLayersViewer::itemDropped(const SourceDetails& details)
{
	juce::String message = details.description.toString();
	juce::StringArray T;
	T.addTokens(message, ":", "");
	if (T.size() < 1)
		return;
	int item;
	item = T[0].getIntValue();
	int row = m_Table.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	int index = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (!C->IsRaster())
			continue;
		index++;
		if (index == item)
			C->Repres()->ZOrder(row * 10 + 5);
		else
			C->Repres()->ZOrder(index * 10);
	}
	m_Base->SortClass();
	m_Model.sendActionMessage("UpdateRaster");
}

bool ImageLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_Table)
		return false;
	return true;
}
