//-----------------------------------------------------------------------------
//								LasLayersViewer.cpp
//								===================
//
// Visulisation des classes d'objets LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 27/10/2023
//-----------------------------------------------------------------------------

#include "LasLayersViewer.h"
#include "Utilities.h"
#include "LasShader.h"
#include "../XTool/XGeoVector.h"
#include "../XToolAlgo/XLasFile.h"

//==============================================================================
// FindLasClass : renvoie la ieme classe LAS de la base ou nullptr sinon
//==============================================================================
XGeoClass* LasViewerModel::FindLasClass(int index)
{
	if (m_Base == nullptr)
		return nullptr;
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsLAS()) {
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
int LasViewerModel::getNumRows()
{
	if (m_Base == nullptr)
		return 0;
	int count = 0;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsLAS())
			count++;
	}
	return count;
}

//==============================================================================
// Dessin du fond
//==============================================================================
void LasViewerModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void LasViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	XGeoClass* lasClass = FindLasClass(rowNumber);
	if (lasClass == nullptr)
		return;

	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (lasClass->Visible())
			icone = getImageFromAssets("View.png");
		else
			icone = getImageFromAssets("NoView.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (lasClass->Selectable())
			icone = getImageFromAssets("Selectable.png");
		else
			icone = getImageFromAssets("NoSelectable.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:
		g.drawText(juce::String(lasClass->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Zmin:
		g.drawText(juce::String(lasClass->Zmin()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Zmax:
		g.drawText(juce::String(lasClass->Zmax()), 0, 0, width, height, juce::Justification::centred);
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
void LasViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* lasClass = FindLasClass(rowNumber);
	if (lasClass == nullptr)
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		sendActionMessage("UpdateLasVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateLasSelectability");
		return;
	}

	// Options
	if (columnId == Column::Options) { // Creation d'un popup menu

		std::function< void() > LayerCenter = [=]() {	// Position au centre du cadre
			XPt2D P = lasClass->Frame().Center();
			sendActionMessage("CenterFrame:" + juce::String(P.X, 2) + ":" + juce::String(P.Y, 2)); };
		std::function< void() > LayerFrame = [=]() { // Zoom pour voir l'ensemble du cadre
			XFrame F = lasClass->Frame();
			sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
				juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2)); };
		std::function< void() > LayerRemove = [=]() { // Retire la couche
			sendActionMessage("RemoveLasClass"); };
		std::function< void() > ComputeDtm = [=]() { // Calcul d'un MNT
			sendActionMessage("ComputeDtm"); };

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addSeparator();
		menu.addItem(juce::translate("Compute DTM"), ComputeDtm);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void LasViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* lasClass = FindLasClass(rowNumber);
	if (lasClass == nullptr)
		return;

	// Nom du layer
	if (columnId == Column::Name) {
		XFrame F = lasClass->Frame();
		sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
			juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
		return;
	}
}

//==============================================================================
// Drag&Drop des lignes pour changer l'ordre des layers
//==============================================================================
juce::var LasViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// LasViewerModel : changeListenerCallback
//==============================================================================
void LasViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (m_Base == nullptr)
		return;
	//if (m_ActiveRow >= m_Base->GetDtmLayerCount())
	//	return;
	//GeoBase::RasterLayer* geoLayer = m_Base->GetDtmLayer(m_ActiveRow);
}

//==============================================================================
// ClassifModel : paintRowBackground
//==============================================================================
void ClassifModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// ClassifModel : paintCell
//==============================================================================
void ClassifModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (LasShader::ClassificationVisibility(rowNumber))
			icone = getImageFromAssets("View.png");
		else
			icone = getImageFromAssets("NoView.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (LasShader::ClassificationSelectable(rowNumber))
			icone = getImageFromAssets("Selectable.png");
		else
			icone = getImageFromAssets("NoSelectable.png");
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Number:
		g.drawText(juce::String(rowNumber), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Name:
		g.drawText(LasShader::ClassificationName(rowNumber), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Colour:
		g.setColour(LasShader::ClassificationColor(rowNumber));
		g.fillRect(0, 0, width, height);
		break;
	}
}

//==============================================================================
// ClassifModel : Clic dans une cellule
//==============================================================================
void ClassifModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		LasShader::ClassificationVisibility(!LasShader::ClassificationVisibility(rowNumber), rowNumber);
		sendActionMessage("UpdateLas");
		return;
	}
	// Selectable
	if (columnId == Column::Selectable) {
		LasShader::ClassificationSelectable(!LasShader::ClassificationSelectable(rowNumber), rowNumber);
		sendActionMessage("UpdateLas");
		return;
	}
	if (columnId == Column::Colour) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("LAS color " + LasShader::ClassificationName(m_ActiveRow));
		colourSelector->setCurrentColour(LasShader::ClassificationColor(m_ActiveRow));
		colourSelector->addChangeListener(this);
		colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
		colourSelector->setSize(400, 300);

		juce::CallOutBox::launchAsynchronously(std::move(colourSelector), bounds, nullptr);
		return;
	}
}

//==============================================================================
// ClassifModel : changeListenerCallback
//==============================================================================
void ClassifModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	// Choix d'une couleur pour m_ActiveRow
	if (m_ActiveColumn == Column::Colour) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			juce::Colour color = cs->getCurrentColour();
			LasShader::ClassificationColor(color, m_ActiveRow);
			sendActionMessage("UpdateLas");
		}
	}
}

//==============================================================================
// LasLayersViewer : constructeur
//==============================================================================
LasLayersViewer::LasLayersViewer()
{
	m_Base = nullptr;

	setName("LAS Layers");
	m_ModelLas.addActionListener(this);
	// Bordure
	m_TableLas.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableLas.setOutlineThickness(1);
	m_TableLas.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Visibility, 25);
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Selectable, 25);
	m_TableLas.getHeader().addColumn(juce::translate("Name"), LasViewerModel::Column::Name, 200);
	m_TableLas.getHeader().addColumn(juce::translate("Zmin"), LasViewerModel::Column::Zmin, 50);
	m_TableLas.getHeader().addColumn(juce::translate("Zmax"), LasViewerModel::Column::Zmax, 50);
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Options, 50);
	m_TableLas.setSize(377, 200);
	m_TableLas.setModel(&m_ModelLas);
	addAndMakeVisible(m_TableLas);

	// Mode d'affichage LAS
	m_Mode.addItem(juce::translate("Altitude"), 1);
	m_Mode.addItem(juce::translate("RGB"), 2);
	m_Mode.addItem(juce::translate("IRC"), 3);
	m_Mode.addItem(juce::translate("Classification"), 4);
	m_Mode.addItem(juce::translate("Intensity"), 5);
	m_Mode.addItem(juce::translate("Scan Angle"), 6);

	m_Mode.addListener(this);
	LasShader shader;
	m_Mode.setSelectedId(static_cast<int>(shader.Mode()));
	addAndMakeVisible(m_Mode);

	// Slider d'opacite
	m_Opacity.setRange(0., 100., 1.);
	m_Opacity.setValue(LasShader::Opacity());
	m_Opacity.setSliderStyle(juce::Slider::LinearHorizontal);
	m_Opacity.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
	m_Opacity.setTextValueSuffix(juce::translate("% opacity"));
	m_Opacity.addListener(this);
	m_Opacity.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_Opacity);

	// Slider d'echelle max
	m_MaxGsd.setRange(0., 20., 0.5);
	m_MaxGsd.setValue(LasShader::MaxGsd());
	m_MaxGsd.setSliderStyle(juce::Slider::LinearHorizontal);
	m_MaxGsd.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
	m_MaxGsd.setTextValueSuffix(juce::translate(" max GSD"));
	m_MaxGsd.addListener(this);
	m_MaxGsd.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_MaxGsd);

	// Couleurs des classifications
	m_ModelClassif.addActionListener(this);
	// Bordure
	m_TableClassif.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableClassif.setOutlineThickness(1);
	// Ajout des colonnes
	m_TableClassif.getHeader().addColumn(juce::translate(" "), ClassifModel::Column::Visibility, 25);
	m_TableClassif.getHeader().addColumn(juce::translate(" "), ClassifModel::Column::Selectable, 25);
	m_TableClassif.getHeader().addColumn(juce::translate("Number"), ClassifModel::Column::Number, 50);
	m_TableClassif.getHeader().addColumn(juce::translate("Name"), ClassifModel::Column::Name, 200);
	m_TableClassif.getHeader().addColumn(juce::translate("Color"), ClassifModel::Column::Colour, 50);
	m_TableClassif.setSize(352, 200);
	m_TableClassif.setModel(&m_ModelClassif);
	addAndMakeVisible(m_TableClassif);
	if (LasShader::Mode() != LasShader::ShaderMode::Classification)
		m_TableClassif.setVisible(false);
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void LasLayersViewer::UpdateColumnName()
{
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Name, juce::translate("Name"));
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Zmin, juce::translate("Zmin"));
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Zmax, juce::translate("Zmax"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Number, juce::translate("Number"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Name, juce::translate("Name"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Colour, juce::translate("Color"));
}

//==============================================================================
// Redimensionnement
//==============================================================================
void LasLayersViewer::resized()
{
	auto b = getLocalBounds();
	m_TableLas.setTopLeftPosition(0, 0);
	m_TableLas.setSize(b.getWidth(), b.getHeight() / 2 - 10);
	m_Opacity.setTopLeftPosition(0, b.getHeight() / 2);
	m_Opacity.setSize(b.getWidth(), 24);
	m_MaxGsd.setTopLeftPosition(0, b.getHeight() / 2 + 30);
	m_MaxGsd.setSize(b.getWidth(), 24);
	m_Mode.setTopLeftPosition(0, b.getHeight() / 2 + 60);
	m_Mode.setSize(b.getWidth(), 24);
	m_TableClassif.setTopLeftPosition(0, b.getHeight() / 2 + 90);
	m_TableClassif.setSize(b.getWidth(), 200);
}

//==============================================================================
// Gestion des actions
//==============================================================================
void LasLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "NewWindow") {
		m_TableLas.updateContent();
		m_TableLas.repaint();
		return;
	}	if (message == "UpdateLas") {
		repaint();
		return;
	}

	// Classes selectionnees
	std::vector<XGeoClass*> T;
	if (m_Base != nullptr) {
		juce::SparseSet< int > S = m_TableLas.getSelectedRows();
		int count = -1;
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsLAS()) {
				count++;
				if (S.contains(count))
					T.push_back(C);
			}
		}
	}

	if (message == "UpdateLasVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_TableLas.repaint();
		sendActionMessage("UpdateLas");
	}
	if (message == "UpdateLasSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_TableLas.repaint();
	}
	if (message == "RemoveLasClass") {
		m_Base->ClearSelection();
		for (int i = 0; i < T.size(); i++)
			m_Base->RemoveClass(T[i]->Layer()->Name().c_str(), T[i]->Name().c_str());
		m_TableLas.deselectAllRows();
		m_TableLas.repaint();
		sendActionMessage("UpdateSelectFeatures");
		sendActionMessage("UpdateLas");
	}
	if (message == "ComputeDtm")
		ComputeDtm(T);
}

//==============================================================================
// Changement de valeur des ComboBox
//==============================================================================
void LasLayersViewer::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &m_Mode) {
		LasShader shader;
		shader.Mode((LasShader::ShaderMode)(m_Mode.getSelectedId()));
		m_ModelLas.sendActionMessage("UpdateLas");
		if (LasShader::Mode() != LasShader::ShaderMode::Classification)
			m_TableClassif.setVisible(false);
		else
			m_TableClassif.setVisible(true);
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void LasLayersViewer::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &m_Opacity)
		LasShader::Opacity(slider->getValue());
	if (slider == &m_MaxGsd)
		LasShader::MaxGsd(slider->getValue());
	m_ModelLas.sendActionMessage("UpdateLas");
}

//==============================================================================
// Drag&Drop
//==============================================================================
void LasLayersViewer::itemDropped(const SourceDetails& details)
{
	juce::String message = details.description.toString();
	juce::StringArray T;
	T.addTokens(message, ":", "");
	if (T.size() < 1)
		return;
	int i;
	i = T[0].getIntValue();
	int row = m_TableLas.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	//m_Base->ReorderDtmLayer(i, row);
	//m_Table.updateContent();
	//m_Table.repaint();
	m_ModelLas.sendActionMessage("UpdateLas");
}

bool LasLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_TableLas)
		return false;
	return true;
}

//==============================================================================
// Calcul de MNT a partir des fichiers LAS
//==============================================================================
void LasLayersViewer::ComputeDtm(std::vector<XGeoClass*> T)
{
	std::unique_ptr<juce::AlertWindow> asyncAlertWindow;
	asyncAlertWindow = std::make_unique<juce::AlertWindow>(juce::translate("Compute DTM/DSM"),
		juce::translate("This tool allows to compute a DTM or a DSM for each LAS file"),
		juce::MessageBoxIconType::QuestionIcon);

	asyncAlertWindow->addTextEditor("GSD", "1.0", juce::translate("GSD:"));
	juce::TextEditor* gsd_editor = asyncAlertWindow->getTextEditor("GSD");
	gsd_editor->setInputRestrictions(5, "0123456789.");
	asyncAlertWindow->addComboBox("Algo", { "Z minimum", "Z average", "Z maximum" }, juce::translate("Algorithm:"));
	asyncAlertWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	asyncAlertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	if(asyncAlertWindow->runModalLoop() == 0)
		return;

	juce::String path;
	juce::FileChooser fc(juce::translate("Choose a directory..."), path, "*", true);
	if (!fc.browseForDirectory())
		return;
	auto result = fc.getURLResult();
	auto foldername = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString(true);
	auto algoIndexChosen = asyncAlertWindow->getComboBoxComponent("Algo")->getSelectedItemIndex();
	auto gsd_text = asyncAlertWindow->getTextEditorContents("GSD");
	double gsd = gsd_text.getDoubleValue();

	// Thread de traitement
	class MyTask : public juce::ThreadWithProgressWindow {
	public:
		std::vector<XGeoClass*> T;
		double GSD = 1.;
		XLasFile::AlgoDtm algo = XLasFile::ZMinimum;
		juce::String folder_out;

		MyTask() : ThreadWithProgressWindow(juce::translate("Compute DTM/DSM ..."), true, true) { ; }
		void run()
		{
			uint32_t nb_file = 0, count = 0;
			for (int i = 0; i < T.size(); i++) 
				nb_file += T[i]->NbVector();
			if (nb_file == 0)
				return;

			for (int i = 0; i < T.size(); i++) {
				if (threadShouldExit())
					break;
				for (int j = 0; j < T[i]->NbVector(); j++) {
					if (threadShouldExit())
						break;
					setProgress((double)count / (double)nb_file);
					count++;
					XGeoVector* V = T[i]->Vector(j);
					if (V->TypeVector() != XGeoVector::LAS)
						continue;
					XLasFile las;
					if (!las.Open(V->Filename()))
						continue;
					juce::File file(V->Filename());
					juce::String file_out = folder_out + juce::File::getSeparatorString() + file.getFileNameWithoutExtension() + ".tif";
					setStatusMessage(juce::translate("Processing ") + file.getFileNameWithoutExtension());
					las.ComputeDtm(file_out.toStdString(), GSD, algo);
				}
			}
		}
	};

	MyTask M;
	M.GSD = gsd;
	M.T = T;
	M.folder_out = foldername;
	M.algo = (XLasFile::AlgoDtm)(algoIndexChosen + 1);
	M.runThread();
}