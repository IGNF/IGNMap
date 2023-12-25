//==============================================================================
// LayerViewer.cpp
//
// Author : F.Becirspahic
// Date : 19/12/2021
//==============================================================================

#include "VectorLayersViewer.h"
#include "Utilities.h"
#include "../../XTool/XGeoClass.h"

//==============================================================================
// LayerViewerComponent : constructeur
//==============================================================================
LayerViewerModel::LayerViewerModel()
{
	m_Base = nullptr;
	m_ActiveRow = m_ActiveColumn = -1;
}

//==============================================================================
// FindDtmClass : renvoie la ieme classe DTM de la base ou nullptr sinon
//==============================================================================
XGeoClass* LayerViewerModel::FindVectorClass(int index)
{
	if (m_Base == nullptr)
		return nullptr;
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsVector()) {
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
int LayerViewerModel::getNumRows()
{
	if (m_Base == nullptr)
		return 0;
	int count = 0;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsVector())
			count++;
	}
	return count;
}

//==============================================================================
// Dessin du fond
//==============================================================================
void LayerViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void LayerViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	XGeoClass* geoLayer = FindVectorClass(rowNumber);
	if (geoLayer == nullptr)
		return;
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (geoLayer->Visible())
			icone = getImageFromAssets("View.png");
		else
			icone = getImageFromAssets("NoView.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (geoLayer->Selectable())
			icone = getImageFromAssets("Selectable.png");
		else
			icone = getImageFromAssets("NoSelectable.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:// Name
		g.drawText(juce::String(geoLayer->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::PenWidth:// Width
		g.drawText(juce::String(geoLayer->Repres()->Size()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::PenColour:// Pen
		g.setColour(juce::Colour(geoLayer->Repres()->Color()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::FillColour:// brush
		g.setColour(juce::Colour(geoLayer->Repres()->FillColor()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::Options:// Options
		icone = getImageFromAssets("Options.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	}
}

//==============================================================================
// Clic dans une cellule
//==============================================================================
void LayerViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* geoLayer = FindVectorClass(rowNumber);
	if (geoLayer == nullptr)
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		geoLayer->Visible(!geoLayer->Visible());
		if (geoLayer->IsVector())
			sendActionMessage("UpdateVector");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		geoLayer->Selectable(!geoLayer->Selectable());
		sendActionMessage("UpdateSelectable");
		return;
	}

	// Choix d'une couleur
	if ((columnId == Column::PenColour) || (columnId == Column::FillColour)) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("background");
		if (columnId == Column::PenColour)
			colourSelector->setCurrentColour(juce::Colour(geoLayer->Repres()->Color()));
		if (columnId == Column::FillColour)
			colourSelector->setCurrentColour(juce::Colour(geoLayer->Repres()->FillColor()));
		colourSelector->addChangeListener(this);
		colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
		colourSelector->setSize(400, 300);

		juce::CallOutBox::launchAsynchronously(std::move(colourSelector), bounds, nullptr);
		return;
	}

	// Choix d'une epaisseur
	if (columnId == Column::PenWidth) {
		auto widthSelector = std::make_unique<juce::Slider>();
		widthSelector->setRange(0., 20., 1.);
		widthSelector->setValue(geoLayer->Repres()->Size());
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
		std::function< void() > LayerRemove = [=]() { // Retire la couche
			sendActionMessage("RemoveVectorClass:" + geoLayer->Layer()->Name() + ":" + geoLayer->Name()); };

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void LayerViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* geoLayer = FindVectorClass(rowNumber);
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
juce::var LayerViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
void LayerViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	XGeoClass* geoLayer = FindVectorClass(m_ActiveRow);
	if (geoLayer == nullptr)
		return;

	// Choix d'une couleur
	if ((m_ActiveColumn == Column::PenColour) || (m_ActiveColumn == Column::FillColour)) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			uint32_t color = cs->getCurrentColour().getARGB();
			if (m_ActiveColumn == Column::PenColour)
				geoLayer->Repres()->Color(color);
			if (m_ActiveColumn == Column::FillColour)
				geoLayer->Repres()->FillColor(color);
			sendActionMessage("UpdateVector");
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void LayerViewerModel::sliderValueChanged(juce::Slider* slider)
{
	XGeoClass* geoLayer = FindVectorClass(m_ActiveRow);
	if (geoLayer == nullptr)
		return;

	// Choix d'une epaisseur
	if (m_ActiveColumn == Column::PenWidth) {
		if (geoLayer->Repres()->Size() != (int)slider->getValue()) {
			geoLayer->Repres()->Size((int)slider->getValue());
			sendActionMessage("UpdateVector");
		}
	}
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
VectorLayersViewer::VectorLayersViewer()
{
	m_Base = nullptr;
	setTitle(juce::translate("Vector Layers"));
	m_Model.addActionListener(this);
	// Bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), LayerViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), LayerViewerModel::Column::Selectable, 25);
	m_Table.getHeader().addColumn(juce::translate("Name"), LayerViewerModel::Column::Name, 200);
	m_Table.getHeader().addColumn(juce::translate("Width"), LayerViewerModel::Column::PenWidth, 50);
	m_Table.getHeader().addColumn(juce::translate("Pen"), LayerViewerModel::Column::PenColour, 50);
	m_Table.getHeader().addColumn(juce::translate("Brush"), LayerViewerModel::Column::FillColour, 50);
	m_Table.getHeader().addColumn(juce::translate(" "), LayerViewerModel::Column::Options, 25);
	m_Table.setSize(377, 200);
	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void VectorLayersViewer::UpdateColumnName()
{
	m_Table.getHeader().setColumnName(LayerViewerModel::Column::Name, juce::translate("Name"));
	m_Table.getHeader().setColumnName(LayerViewerModel::Column::PenWidth, juce::translate("Width"));
	m_Table.getHeader().setColumnName(LayerViewerModel::Column::PenColour, juce::translate("Pen"));
	m_Table.getHeader().setColumnName(LayerViewerModel::Column::FillColour, juce::translate("Brush"));
}

//==============================================================================
// Gestion des actions
//==============================================================================
void VectorLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateVector") {
		repaint();
	}
	if (message == "UpdateRaster") {
		repaint();
	}
	if (message == "UpdateDtm") {
		repaint();
	}
	if (message == "NewWindow") {
		m_Table.updateContent();
		m_Table.repaint();
	}
	if (message == "UpdateSelectable") {
		m_Table.repaint();
	}
}

//==============================================================================
// Drag&Drop
//==============================================================================
void VectorLayersViewer::itemDropped(const SourceDetails& details)
{
	juce::String message = details.description.toString();
	juce::StringArray T;
	T.addTokens(message, ":", "");
	if (T.size() < 1)
		return;
	int item = T[0].getIntValue();
	int row = m_Table.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	int index = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (!C->IsVector())
			continue;
		index++;
		if (index == item)
			C->Repres()->ZOrder(row * 10 + 5);
		else
			C->Repres()->ZOrder(index * 10);
	}
	m_Base->SortClass();
	m_Model.sendActionMessage("UpdateVector");
}

bool VectorLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_Table)
		return false;
	return true;
}
