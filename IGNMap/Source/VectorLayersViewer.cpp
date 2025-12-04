//-----------------------------------------------------------------------------
//								VectorLayersViewer.cpp
//								======================
//
// Visualisation de classes d'objets vectoriel
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/12/2021
//-----------------------------------------------------------------------------

#include "VectorLayersViewer.h"
#include "ClassViewer.h"
#include "ThreadClassProcessor.h"
#include "AppUtil.h"
#include "../../XTool/XGeoClass.h"
#include "../../XToolVector/XShapefileConverter.h"

//==============================================================================
// LayerViewerComponent : constructeur
//==============================================================================
VectorViewerModel::VectorViewerModel()
{
	m_Base = nullptr;
	m_ActiveRow = m_ActiveColumn = -1;
}

//==============================================================================
// FindVectorClass : renvoie la ieme classe vectorielle de la base ou nullptr sinon
//==============================================================================
XGeoClass* VectorViewerModel::FindVectorClass(int index)
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
int VectorViewerModel::getNumRows()
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
void VectorViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void VectorViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	XGeoClass* geoLayer = FindVectorClass(rowNumber);
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
	case Column::PenWidth:// Width
		g.drawText(juce::String(geoLayer->Repres()->Size()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::PenColour:// Pen
		g.setColour(juce::Colours::white);
		g.fillRect(0, 0, width, height);
		g.setColour(juce::Colour(geoLayer->Repres()->Color()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::FillColour:// brush
		g.setColour(juce::Colours::white);
		g.fillRect(0, 0, width, height);
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
void VectorViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
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
			sendActionMessage("RemoveVectorClass"); };
		std::function< void() > ExportClass = [=]() { // Export de la classe
			sendActionMessage("ExportClass"); };
		std::function< void() > ViewObjects = [=]() { // Visualisation des objets de la classe
			gClassViewerMgr.AddClassViewer(geoLayer->Name(), geoLayer, this);
			};
		std::function< void() > ExportRepresVector = [=]() { // Export des representations des classes
			sendActionMessage("ExportRepresVector"); };
		std::function< void() > ImportRepresVector = [=]() { // Import des representations des classes
			sendActionMessage("ImportRepresVector"); };
		std::function< void() > Properties = [=]() { // Proprietes de la classe
			sendActionMessage("Properties"); };

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addItem(juce::translate("View Objects"), ViewObjects);
		menu.addItem(juce::translate("Properties"), Properties);
		menu.addSeparator();
		menu.addItem(juce::translate("Export Layer"), ExportClass);
		menu.addItem(juce::translate("Export Representations"), ExportRepresVector);
		menu.addItem(juce::translate("Import Representations"), ImportRepresVector);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void VectorViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
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
// Clic dans l'entete
//==============================================================================
void VectorViewerModel::sortOrderChanged(int newSortColumnId, bool /*isForwards*/)
{
	if (newSortColumnId == Visibility)
		sendActionMessage("InvertVisibility");
	if (newSortColumnId == Selectable)
		sendActionMessage("InvertSelectable");
}

//==============================================================================
// Drag&Drop des lignes pour changer l'ordre des layers
//==============================================================================
juce::var VectorViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
void VectorViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
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
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void VectorViewerModel::sliderValueChanged(juce::Slider* slider)
{
	XGeoClass* geoLayer = FindVectorClass(m_ActiveRow);
	if (geoLayer == nullptr)
		return;

	// Choix d'une epaisseur
	if (m_ActiveColumn == Column::PenWidth) {
		if (geoLayer->Repres()->Size() != (int)slider->getValue()) {
			geoLayer->Repres()->Size((uint8_t)slider->getValue());
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// VectorLayersViewer : constructeur
//==============================================================================
VectorLayersViewer::VectorLayersViewer()
{
	m_Base = nullptr;
	setTitle(juce::translate("Vector Layers"));
	setWantsKeyboardFocus(true);
	m_Model.addActionListener(this);
	// Bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), VectorViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), VectorViewerModel::Column::Selectable, 25);
	m_Table.getHeader().addColumn(juce::translate("Name"), VectorViewerModel::Column::Name, 200);
	m_Table.getHeader().addColumn(juce::translate("Width"), VectorViewerModel::Column::PenWidth, 50);
	m_Table.getHeader().addColumn(juce::translate("Pen"), VectorViewerModel::Column::PenColour, 50);
	m_Table.getHeader().addColumn(juce::translate("Brush"), VectorViewerModel::Column::FillColour, 50);
	m_Table.getHeader().addColumn(juce::translate(" "), VectorViewerModel::Column::Options, 25);
	m_Table.setSize(377, 200);
	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void VectorLayersViewer::Translate()
{
	m_Table.getHeader().setColumnName(VectorViewerModel::Column::Name, juce::translate("Name"));
	m_Table.getHeader().setColumnName(VectorViewerModel::Column::PenWidth, juce::translate("Width"));
	m_Table.getHeader().setColumnName(VectorViewerModel::Column::PenColour, juce::translate("Pen"));
	m_Table.getHeader().setColumnName(VectorViewerModel::Column::FillColour, juce::translate("Brush"));
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool VectorLayersViewer::keyPressed(const juce::KeyPress& key)
{
	if (key.getKeyCode() == juce::KeyPress::F2Key) {
		AppUtil::SaveTableComponent(&m_Table);
		return true;
	}
	return false;	// On transmet l'evenement sans le traiter
}

//==============================================================================
// Renomme la derniere classe et visualisation des objets
//==============================================================================
void VectorLayersViewer::RenameAndViewLastClass(juce::String newName)
{
	if (m_Model.getNumRows() < 1)
		return;
	XGeoClass* C = m_Model.FindVectorClass(m_Model.getNumRows() - 1);
	if (C == nullptr)
		return;
	C->Name(newName.toStdString());
	gClassViewerMgr.AddClassViewer(C->Name(), C, &m_Model);
	XGeoVector* V = C->Vector((uint32_t)0);
	if (V == nullptr)
		return;
	XFrame F = V->Frame();
	if (V->NbPt() < 2)
		sendActionMessage("CenterFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Ymax, 2));
	else {
		F += 100.;	// Pour avoir une marge et pour agrandir la zone pour les petits objets et les ponctuels
		sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
			juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
	}
}

//==============================================================================
// Gestion des actions
//==============================================================================
void VectorLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (m_Base == nullptr)
		return;
	if (message == "NewWindow") {
		gClassViewerMgr.RemoveAll();
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
	if (message == "InvertVisibility") {
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsVector()) C->Visible(!C->Visible());
		}
		m_Table.repaint();
		sendActionMessage("UpdateVector");
		return;
	}
	if (message == "InvertSelectable") {
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsVector()) C->Selectable(!C->Selectable());
		}
		m_Table.repaint();
		return;
	}

	// Classes selectionnees
	std::vector<XGeoClass*> T;
	juce::SparseSet< int > selectedRows = m_Table.getSelectedRows();
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsVector()) {
			count++;
			if (selectedRows.contains(count))
				T.push_back(C);
		}
	}
	if (T.size() < 1) {	// Aucune classe selectionnee
		sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
		return;
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
		for (int i = 0; i < T.size(); i++) {
			XGeoClass* C = m_Base->Class(T[i]->Layer()->Name().c_str(), T[i]->Name().c_str());
			gClassViewerMgr.RemoveViewer(C);
		}
		m_Base->ClearSelection();
		for (int i = 0; i < T.size(); i++)
			m_Base->RemoveClass(T[i]->Layer()->Name().c_str(), T[i]->Name().c_str());
		m_Table.deselectAllRows();
		m_Table.repaint();
		sendActionMessage("UpdateSelectFeatures");
		sendActionMessage("UpdateVector");
		return;
	}
	if (message == "ExportClass") {
		ExportClass(T);
		return;
	}
	if (message == "Properties") {
		int index = -1;
		for (int i = 0; i < m_Base->NbClass(); i++)
			if (m_Base->Class(i) == T[0])
				index = i;
		if (index >= 0)
			sendActionMessage("Properties:Class:" + juce::String(index));
	}
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
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
	m_Table.repaint();
	m_Model.sendActionMessage("UpdateVector");
}

bool VectorLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_Table)
		return false;
	return true;
}

//==============================================================================
// Export des classes en Shapefile
//==============================================================================
void VectorLayersViewer::ExportClass(std::vector<XGeoClass*> T)
{
	juce::String path;
	juce::FileChooser fc(juce::translate("Choose a directory..."), path, "*", true);
	if (!fc.browseForDirectory())
		return;
	auto result = fc.getURLResult();
	auto foldername = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString(true);

	// Thread de traitement
	class MyTask : public juce::ThreadWithProgressWindow {
	public:
		std::vector<XGeoClass*> m_T;	// Liste des classes a traiter
		std::string m_strFolderName;
		MyTask() : ThreadWithProgressWindow(juce::translate("Shapefile export ..."), true, true) { ; }
		void run()
		{
			uint32_t count = 0;
			for (int i = 0; i < m_T.size(); i++) {
				setProgress((double)count / (double)m_T.size());
				count++;
				if (!m_T[i]->Visible())
					continue;
				if (threadShouldExit())
					break;
				setStatusMessage(juce::translate("Processing ") + m_T[i]->Name());
				XShapefileConverter converter;
				converter.ConvertClass(m_T[i], m_strFolderName.c_str());
			}
		}
	};

	MyTask M;
	M.m_T = T;
	M.m_strFolderName = foldername.toStdString();
	M.runThread();
}
