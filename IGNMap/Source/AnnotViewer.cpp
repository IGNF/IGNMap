//-----------------------------------------------------------------------------
//								AnnotViewer.cpp
//								===============
//
// Visualisation des annotations
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 05/08/2024
//-----------------------------------------------------------------------------

#include "AnnotViewer.h"

//==============================================================================
// AnnotViewerModel : constructeur
//==============================================================================
AnnotViewerModel::AnnotViewerModel()
{
	m_Annot = nullptr;
	m_ActiveRow = m_ActiveColumn = -1;
}

//==============================================================================
// Nombre de lignes de la table
//==============================================================================
int AnnotViewerModel::getNumRows()
{
	if (m_Annot == nullptr)
		return 0;
	return (int)m_Annot->size();
}

//==============================================================================
// Dessin du fond
//==============================================================================
void AnnotViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void AnnotViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (m_Annot == nullptr)
		return;
	if (rowNumber >= m_Annot->size())
		return;
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if ((*m_Annot)[rowNumber].Visible())
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if ((*m_Annot)[rowNumber].Selectable())
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:// Name
		//g.drawText(juce::String((*m_Annot)[rowNumber].Text()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::PenWidth:// Width
		g.drawText(juce::String((*m_Annot)[rowNumber].Repres()->Size()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::PenColour:// Pen
		g.setColour(juce::Colour((*m_Annot)[rowNumber].Repres()->Color()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::FillColour:// brush
		g.setColour(juce::Colour((*m_Annot)[rowNumber].Repres()->FillColor()));
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
void AnnotViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	if (m_Annot == nullptr)
		return;
	if (rowNumber >= m_Annot->size())
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		sendActionMessage("UpdateVectorVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateVectorSelectability");
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
			colourSelector->setCurrentColour(juce::Colour((*m_Annot)[rowNumber].Repres()->Color()));
		if (columnId == Column::FillColour)
			colourSelector->setCurrentColour(juce::Colour((*m_Annot)[rowNumber].Repres()->FillColor()));
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
		widthSelector->setValue((*m_Annot)[rowNumber].Repres()->Size());
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
			XPt2D P = (*m_Annot)[rowNumber].Frame().Center();
			sendActionMessage("CenterFrame:" + juce::String(P.X, 2) + ":" + juce::String(P.Y, 2)); };
		std::function< void() > LayerFrame = [=]() { // Zoom pour voir l'ensemble du cadre
			XFrame F = (*m_Annot)[rowNumber].Frame();
			sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
				juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2)); };
		std::function< void() > LayerRemove = [=]() { // Retire la couche
			sendActionMessage("RemoveVectorClass"); };

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
void AnnotViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
{
	if (m_Annot == nullptr)
		return;
	if (rowNumber >= m_Annot->size())
		return;

	// Nom du layer
	if (columnId == Column::Name) {
		XFrame F = (*m_Annot)[rowNumber].Frame();
		sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
			juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
		return;
	}

}

//==============================================================================
// Drag&Drop des lignes pour changer l'ordre des layers
//==============================================================================
juce::var AnnotViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// Composant custom pour l'edition des textes
//==============================================================================
juce::Component* AnnotViewerModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
																													 juce::Component* existingComponentToUpdate)
{
	if (columnId != Column::Name ) // Pas de composant custom
	{
		jassert(existingComponentToUpdate == nullptr);
		return nullptr;
	}

	auto* textLabel = static_cast<EditableTextCustomAnnotation*> (existingComponentToUpdate);
	if (textLabel == nullptr)
		textLabel = new EditableTextCustomAnnotation(m_Annot, rowNumber);
	else
		textLabel->setText((*m_Annot)[rowNumber].Text(), juce::dontSendNotification);

	return textLabel;
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
void AnnotViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (m_Annot == nullptr)
		return;
	if (m_ActiveRow >= m_Annot->size())
		return;

	// Choix d'une couleur
	if ((m_ActiveColumn == Column::PenColour) || (m_ActiveColumn == Column::FillColour)) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			uint32_t color = cs->getCurrentColour().getARGB();
			if (m_ActiveColumn == Column::PenColour)
				(*m_Annot)[m_ActiveRow].Repres()->Color(color);
			if (m_ActiveColumn == Column::FillColour)
				(*m_Annot)[m_ActiveRow].Repres()->FillColor(color);
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void AnnotViewerModel::sliderValueChanged(juce::Slider* slider)
{
	if (m_Annot == nullptr)
		return;
	if (m_ActiveRow >= m_Annot->size())
		return;


	// Choix d'une epaisseur
	if (m_ActiveColumn == Column::PenWidth) {
		if ((*m_Annot)[m_ActiveRow].Repres()->Size() != (int)slider->getValue()) {
			(*m_Annot)[m_ActiveRow].Repres()->Size((uint8_t)slider->getValue());
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// AnnotViewer : constructeur
//==============================================================================
AnnotViewer::AnnotViewer()
{
	m_Annot = nullptr;
	setTitle(juce::translate("Annotations"));
	m_Model.addActionListener(this);
	// Bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), AnnotViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), AnnotViewerModel::Column::Selectable, 25);
	m_Table.getHeader().addColumn(juce::translate("Name"), AnnotViewerModel::Column::Name, 200);
	m_Table.getHeader().addColumn(juce::translate("Width"), AnnotViewerModel::Column::PenWidth, 50);
	m_Table.getHeader().addColumn(juce::translate("Pen"), AnnotViewerModel::Column::PenColour, 50);
	m_Table.getHeader().addColumn(juce::translate("Brush"), AnnotViewerModel::Column::FillColour, 50);
	m_Table.getHeader().addColumn(juce::translate(" "), AnnotViewerModel::Column::Options, 25);
	m_Table.setSize(377, 200);
	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// AnnotViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void AnnotViewer::Translate()
{
	m_Table.getHeader().setColumnName(AnnotViewerModel::Column::Name, juce::translate("Name"));
	m_Table.getHeader().setColumnName(AnnotViewerModel::Column::PenWidth, juce::translate("Width"));
	m_Table.getHeader().setColumnName(AnnotViewerModel::Column::PenColour, juce::translate("Pen"));
	m_Table.getHeader().setColumnName(AnnotViewerModel::Column::FillColour, juce::translate("Brush"));
}

//==============================================================================
// Gestion des actions
//==============================================================================
void AnnotViewer::actionListenerCallback(const juce::String& message)
{
	if (m_Annot == nullptr)
		return;
	if (message == "NewWindow") {
		m_Table.updateContent();
		return;
	}
	if (message == "UpdateClass") {
		sendActionMessage("UpdateVector");
		return;
	}
	if (message == "UpdateVectorRepres") {
		m_Table.repaint();
		sendActionMessage("UpdateVector");
		return;
	}

	// Classes selectionnees
	std::vector<XAnnotation*> T;
	juce::SparseSet< int > S = m_Table.getSelectedRows();
	for (uint32_t i = 0; i < m_Annot->size(); i++) {
		if (S.contains(i))
			T.push_back(&((*m_Annot)[i]));
	}

	if (message == "UpdateVectorVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_Table.repaint();
		sendActionMessage("UpdateVector");
		return;
	}
	if (message == "UpdateVectorSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_Table.repaint();
		return;
	}
	if (message == "RemoveVectorClass") {
		
		m_Table.deselectAllRows();
		m_Table.repaint();
		sendActionMessage("UpdateSelectFeatures");
		sendActionMessage("UpdateVector");
		return;
	}

	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Drag&Drop
//==============================================================================
void AnnotViewer::itemDropped(const SourceDetails& details)
{
	juce::String message = details.description.toString();
	juce::StringArray T;
	T.addTokens(message, ":", "");
	if (T.size() < 1)
		return;
	int item = T[0].getIntValue();
	if (item >= m_Annot->size())
		return;
	int row = m_Table.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	if (row < 0)
		return;
	if (row == item)
		return;
	std::vector<XAnnotation>::iterator iter = m_Annot->begin() + item;
	XAnnotation A = *iter;
	m_Annot->erase(iter);
	if (item >= row)
		m_Annot->insert(m_Annot->begin() + row, A);
	else
		m_Annot->insert(m_Annot->begin() + row - 1, A);

	m_Table.updateContent();
	m_Table.repaint();
	m_Model.sendActionMessage("UpdateVector");
}

bool AnnotViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_Table)
		return false;
	return true;
}