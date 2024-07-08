//-----------------------------------------------------------------------------
//								DtmLayersViewer.h
//								=================
//
// Visualisation des classes d'objets MNT
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 21/01/2022
//-----------------------------------------------------------------------------

#include "DtmLayersViewer.h"
#include "ClassViewer.h"
#include "Utilities.h"
#include "DtmShader.h"
#include "ThreadClassProcessor.h"
#include "GeoBase.h"

//==============================================================================
// DtmViewerModel : constructeur
//==============================================================================
DtmViewerModel::DtmViewerModel()
{
	m_Base = nullptr;
	m_ActiveRow = m_ActiveColumn = -1;
}

//==============================================================================
// FindDtmClass : renvoie la ieme classe DTM de la base ou nullptr sinon
//==============================================================================
XGeoClass* DtmViewerModel::FindDtmClass(int index)
{
	if (m_Base == nullptr)
		return nullptr;
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsDTM()) {
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
int DtmViewerModel::getNumRows()
{
	if (m_Base == nullptr)
		return 0;
	int count = 0;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsDTM())
			count++;
	}
	return count;
}

//==============================================================================
// Dessin du fond
//==============================================================================
void DtmViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void DtmViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	XGeoClass* dtmClass = FindDtmClass(rowNumber);
	if (dtmClass == nullptr)
		return;

	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (dtmClass->Visible())
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (dtmClass->Selectable())
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:
		g.drawText(juce::String(dtmClass->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Zmin:
		g.drawText(juce::String(dtmClass->Zmin()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Zmax:
		g.drawText(juce::String(dtmClass->Zmax()), 0, 0, width, height, juce::Justification::centred);
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
void DtmViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	XGeoClass* dtmClass = FindDtmClass(rowNumber);
	if (dtmClass == nullptr)
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		sendActionMessage("UpdateDtmVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateDtmSelectability");
		return;
	}

	// Options
	if (columnId == Column::Options) { // Creation d'un popup menu

		std::function< void() > LayerCenter = [=]() {	// Position au centre du cadre
			XPt2D P = dtmClass->Frame().Center();
			sendActionMessage("CenterFrame:" + juce::String(P.X, 2) + ":" + juce::String(P.Y, 2)); };
		std::function< void() > LayerFrame = [=]() { // Zoom pour voir l'ensemble du cadre
			XFrame F = dtmClass->Frame();
			sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
				juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2)); };
		std::function< void() > LayerGsd = [=]() { // Zoom a la resolution native de la couche
			XGeoVector* V = dtmClass->Vector((uint32_t)0);
			if (V != nullptr)
				sendActionMessage("ZoomGsd:" + juce::String(V->Resolution(), 2)); };
		std::function< void() > LayerRemove = [=]() { // Retire la couche
			sendActionMessage("RemoveDtmClass"); };
		std::function< void() > ComputeDeltaZ = [=]() { // Calcul des deltaZ importants
			sendActionMessage("ComputeDeltaZ"); };
		std::function< void() > ViewObjects = [=]() { // Visualisation des objets de la classe
			gClassViewerMgr.AddClassViewer(dtmClass->Name(), dtmClass, this);
			};

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addItem(juce::translate("Layer GSD"), LayerGsd);
		menu.addItem(juce::translate("View Objects"), ViewObjects);
		menu.addSeparator();
		menu.addItem(juce::translate("Compute Delta Z"), ComputeDeltaZ);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void DtmViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
{
	XGeoClass* dtmClass = FindDtmClass(rowNumber);
	if (dtmClass == nullptr)
		return;

	// Nom du layer
	if (columnId == Column::Name) {
		XFrame F = dtmClass->Frame();
		sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
			juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
		return;
	}
}

//==============================================================================
// Drag&Drop des lignes pour changer l'ordre des layers
//==============================================================================
juce::var DtmViewerModel::getDragSourceDescription(const juce::SparseSet<int>& selectedRows)
{
	juce::StringArray rows;
	for (int i = 0; i < selectedRows.size(); i++)
		rows.add(juce::String(selectedRows[i]));
	return rows.joinIntoString(":");
}

//==============================================================================
// Nombre de lignes de la table
//==============================================================================
int DtmRangeModel::getNumRows()
{
	return (int)(DtmShader::m_Z.size() + 1);
}

//==============================================================================
// Dessin du fond
//==============================================================================
void DtmRangeModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void DtmRangeModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (rowNumber > DtmShader::m_Z.size())
		return;
	juce::Image icone;
	switch (columnId) {
	case Column::Altitude:
		if (rowNumber == 0) g.drawText(juce::String(DtmShader::m_Z[rowNumber]), 0, 0, width, height, juce::Justification::centred);
		if (rowNumber == 1) g.drawText(" < " + juce::String(DtmShader::m_Z[rowNumber]), 0, 0, width, height, juce::Justification::centred);
		if ((rowNumber >= 2)&&(rowNumber < DtmShader::m_Z.size()))
			g.drawText(juce::String(DtmShader::m_Z[rowNumber]), 0, 0, width, height, juce::Justification::centred);
		if (rowNumber == DtmShader::m_Z.size())
			g.drawText(" > " + juce::String(DtmShader::m_Z[rowNumber-1]), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Colour:
		g.setColour(DtmShader::m_Colour[rowNumber]);
		g.fillRect(0, 0, width, height);
		break;
	case Column::Options:// Options
		if (rowNumber == 0) {
			icone = juce::ImageCache::getFromMemory(BinaryData::Options_png, BinaryData::Options_pngSize);
			g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		}
		break;
	}
}

//==============================================================================
// Clic dans une cellule
//==============================================================================
void DtmRangeModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	if (rowNumber > DtmShader::m_Z.size())
		return;
	if (event.mods.isRightButtonDown()) {	// Clic bouton droit
		double z = DtmShader::m_Z[DtmShader::m_Z.size() - 1] + 100;
		if (rowNumber < DtmShader::m_Z.size() - 1)
			z = (DtmShader::m_Z[rowNumber] + DtmShader::m_Z[rowNumber + 1]) * 0.5;
		DtmShader::AddAltitude(z);
		sendActionMessage("UpdateRange");
		return;
	}

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	if (columnId == Column::Altitude) {
		if (rowNumber >= DtmShader::m_Z.size())
			return;
		auto altitudeSelector = std::make_unique<juce::Slider>();
		double min = 0., max = 0.;
		if (rowNumber == 0)
			min = -9999.;
		else
			min = DtmShader::m_Z[rowNumber - 1];
		if (rowNumber == (DtmShader::m_Z.size() - 1))
			max = 10000.;
		else
			max = DtmShader::m_Z[rowNumber + 1];
		
		altitudeSelector->setRange(min, max, 1.);
		altitudeSelector->setValue(DtmShader::m_Z[rowNumber]);
		altitudeSelector->setSliderStyle(juce::Slider::LinearHorizontal);
		altitudeSelector->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
		altitudeSelector->setSize(200, 50);
		altitudeSelector->addListener(this);
		juce::CallOutBox::launchAsynchronously(std::move(altitudeSelector), bounds, nullptr);
		return;
	}

	// Choix d'une couleur
	if (columnId == Column::Colour) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("background");
		colourSelector->setCurrentColour(DtmShader::m_Colour[rowNumber]);
		colourSelector->addChangeListener(this);
		colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
		colourSelector->setSize(400, 300);

		juce::CallOutBox::launchAsynchronously(std::move(colourSelector), bounds, nullptr);
		return;
	}

	// Options
	if (columnId == Column::Options) { // Creation d'un popup menu

		std::function< void() > AutomaticRange = [=]() {	// Echelle automatique
			if (m_Base == nullptr) return;
			DtmShader::AutomaticRange(m_Base->ZMin(), m_Base->ZMax());
			sendActionMessage("UpdateDtm");
			};

		juce::PopupMenu menu;
		menu.addItem(juce::translate("Automatic Range"), AutomaticRange);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// LayerViewer : constructeur
//==============================================================================
void DtmRangeModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	// Choix d'une couleur
	if ((m_ActiveColumn == Column::Colour)) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			DtmShader::m_Colour[m_ActiveRow] = cs->getCurrentColour();
			sendActionMessage("UpdateDtm");
		}
	}

}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void DtmRangeModel::sliderValueChanged(juce::Slider* slider)
{
	if (m_ActiveRow >= DtmShader::m_Z.size())
		return;
	if (m_ActiveColumn == Column::Altitude) {	// Changement de l'altitude
		DtmShader::m_Z[m_ActiveRow] = slider->getValue();
		sendActionMessage("UpdateDtm");
	}
}

//==============================================================================
// DtmViewer : constructeur
//==============================================================================
DtmLayersViewer::DtmLayersViewer()
{
	m_Base = nullptr;
	m_Cache = GeoTools::CreateCacheDir("DTM");
	DtmShader shader;	// Necessaire pour initialiser les plages et les couleurs
	setName("DTM Layers");
	m_ModelDtm.addActionListener(this);
	// Bordure
	m_TableDtm.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableDtm.setOutlineThickness(1);
	m_TableDtm.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_TableDtm.getHeader().addColumn(juce::translate(" "), DtmViewerModel::Column::Visibility, 25);
	m_TableDtm.getHeader().addColumn(juce::translate(" "), DtmViewerModel::Column::Selectable, 25);
	m_TableDtm.getHeader().addColumn(juce::translate("Name"), DtmViewerModel::Column::Name, 200);
	m_TableDtm.getHeader().addColumn(juce::translate("Zmin"), DtmViewerModel::Column::Zmin, 50);
	m_TableDtm.getHeader().addColumn(juce::translate("Zmax"), DtmViewerModel::Column::Zmax, 50);
	m_TableDtm.getHeader().addColumn(juce::translate(" "), DtmViewerModel::Column::Options, 25);
	m_TableDtm.setSize(377, 200);
	m_TableDtm.setModel(&m_ModelDtm);
	addAndMakeVisible(m_TableDtm);
	// Slider d'opacite
	m_Opacity.setRange(0., 100., 1.);
	m_Opacity.setValue(DtmShader::m_dOpacity);
	m_Opacity.setSliderStyle(juce::Slider::LinearHorizontal);
	m_Opacity.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
	m_Opacity.setTextValueSuffix(juce::translate("% opacity"));
	m_Opacity.addListener(this);
	addAndMakeVisible(m_Opacity);

	m_ModelRange.addActionListener(this);
	// Bordure
	m_TableRange.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableRange.setOutlineThickness(1);
	// Ajout des colonnes
	m_TableRange.getHeader().addColumn(juce::translate("Altitude"), DtmRangeModel::Column::Altitude, 50);
	m_TableRange.getHeader().addColumn(juce::translate("Colour"), DtmRangeModel::Column::Colour, 50);
	m_TableRange.getHeader().addColumn(juce::translate(" "), DtmRangeModel::Column::Options, 25);
	m_TableRange.setModel(&m_ModelRange);
	addAndMakeVisible(m_TableRange);

	m_Mode.addItem(juce::translate("Altitude"), 1);
	m_Mode.addItem(juce::translate("Standard shading"), 2);
	m_Mode.addItem(juce::translate("Light shading"), 3);
	m_Mode.addItem(juce::translate("Free shading"), 4); 
	m_Mode.addItem(juce::translate("Slope"), 5);
	m_Mode.addItem(juce::translate("Colours"), 6);
	m_Mode.addItem(juce::translate("Colours + Shading"), 7);
	m_Mode.addItem(juce::translate("Contour lines"), 8);
	
	m_Mode.setSelectedId(static_cast<int>(DtmShader::m_Mode) + 1, juce::NotificationType::dontSendNotification);
	m_Mode.addListener(this);
	addAndMakeVisible(m_Mode);
	
	m_IsoStep.setRange(1., 250., 1.);
	m_IsoStep.setValue(DtmShader::m_dIsoStep);
	m_IsoStep.setSliderStyle(juce::Slider::LinearHorizontal);
	m_IsoStep.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
	m_IsoStep.addListener(this);
	m_IsoStep.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_IsoStep);

	m_Azimuth.setRange(0., 360., 1.);
	m_Azimuth.setValue(DtmShader::m_dSolarAzimuth);
	m_Azimuth.setSliderStyle(juce::Slider::Rotary);
	m_Azimuth.setRotaryParameters((float)juce::MathConstants<double>::pi, (float)(3.*juce::MathConstants<double>::pi), false);
	m_Azimuth.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	m_Azimuth.addListener(this);
	m_Azimuth.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_Azimuth);

	m_Zenith.setRange(0., 90., 1.);
	m_Zenith.setValue(DtmShader::m_dSolarZenith);
	m_Zenith.setSliderStyle(juce::Slider::LinearBarVertical);
	//m_Zenith.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	m_Zenith.setChangeNotificationOnlyOnRelease(true);
	m_Zenith.addListener(this);
	addAndMakeVisible(m_Zenith);

	comboBoxChanged(&m_Mode); // Pour fixer la visibilite des combos Azimuth, Zenith, ...
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void DtmLayersViewer::Translate()
{
	m_TableDtm.getHeader().setColumnName(DtmViewerModel::Column::Name, juce::translate("Name"));
	m_TableDtm.getHeader().setColumnName(DtmViewerModel::Column::Zmin, juce::translate("Zmin"));
	m_TableDtm.getHeader().setColumnName(DtmViewerModel::Column::Zmax, juce::translate("Zmax"));
	m_TableRange.getHeader().setColumnName(DtmRangeModel::Column::Altitude, juce::translate("Altitude"));
	m_TableRange.getHeader().setColumnName(DtmRangeModel::Column::Colour, juce::translate("Colour"));

	m_Mode.changeItemText(1, juce::translate("Altitude"));
	m_Mode.changeItemText(2, juce::translate("Standard shading"));
	m_Mode.changeItemText(3, juce::translate("Light shading"));
	m_Mode.changeItemText(4, juce::translate("Free shading"));
	m_Mode.changeItemText(5, juce::translate("Slope"));
	m_Mode.changeItemText(6, juce::translate("Colours"));
	m_Mode.changeItemText(7, juce::translate("Colours + Shading"));
	m_Mode.changeItemText(8, juce::translate("Contour lines"));

	m_Opacity.setTextValueSuffix(juce::translate("% opacity"));
}

//==============================================================================
// Redimensionnement
//==============================================================================
void DtmLayersViewer::resized()
{ 
	auto b = getLocalBounds();
	m_TableDtm.setTopLeftPosition(0, 0);
	m_TableDtm.setSize(b.getWidth(), b.getHeight() / 2 - 30);
	m_Opacity.setTopLeftPosition(0, b.getHeight() / 2 - 30);
	m_Opacity.setSize(b.getWidth(), 25);
	m_TableRange.setTopLeftPosition(0, b.getHeight() / 2);
	m_TableRange.setSize(b.getWidth()/2, b.getHeight() / 2);
	m_Mode.setTopLeftPosition(b.getWidth() / 2, b.getHeight() / 2);
	m_Mode.setSize(b.getWidth() / 2, 25);
	m_IsoStep.setTopLeftPosition(b.getWidth() / 2, b.getHeight() / 2 + 30);
	m_IsoStep.setSize(b.getWidth() / 2, 30);
	m_Azimuth.setTopLeftPosition(b.getWidth() / 2, b.getHeight() / 2 + 30);
	m_Azimuth.setSize(b.getWidth() / 4, 100);
	m_Zenith.setTopLeftPosition(3 * b.getWidth() / 4, b.getHeight() / 2 + 30);
	m_Zenith.setSize(b.getWidth() / 4, 100);
}

//==============================================================================
// Gestion des actions
//==============================================================================
void DtmLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "NewWindow") {
		m_TableDtm.updateContent();
		m_TableDtm.repaint();
		return;
	}
	if (message == "UpdateClass") {
		sendActionMessage("UpdateDtm");
		return;
	}
	if (message == "UpdateDtm") {
		sendActionMessage("UpdateDtm");
		repaint();
		return;
	}
	if (message == "UpdateRange") {
		m_TableRange.updateContent();
		return;
	}

	// Classes selectionnees
	std::vector<XGeoClass*> T;
	if (m_Base != nullptr) {
		juce::SparseSet< int > S = m_TableDtm.getSelectedRows();
		int count = -1;
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsDTM()) {
				count++;
				if (S.contains(count))
					T.push_back(C);
			}
		}
	}

	if (message == "UpdateDtmVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_TableDtm.repaint();
		sendActionMessage("UpdateDtm");
		return;
	}
	if (message == "UpdateDtmSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_TableDtm.repaint();
		return;
	}
	if (message == "RemoveDtmClass") {
		m_Base->ClearSelection();
		for (int i = 0; i < T.size(); i++)
			m_Base->RemoveClass(T[i]->Layer()->Name().c_str(), T[i]->Name().c_str());
		m_TableDtm.deselectAllRows();
		m_TableDtm.repaint();
		sendActionMessage("UpdateSelectFeatures");
		sendActionMessage("UpdateDtm");
		return;
	}
	if (message == "ComputeDeltaZ") {
		ComputeDeltaZ(T);
		return;
	}
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Changement de valeur des ComboBox
//==============================================================================
void DtmLayersViewer::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &m_Mode) {
		DtmShader::m_Mode = (DtmShader::ShaderMode)(m_Mode.getSelectedId() - 1);
		m_ModelRange.sendActionMessage("UpdateDtm");
		m_IsoStep.setVisible(false);
		m_Azimuth.setVisible(false);
		m_Zenith.setVisible(false);
		if (DtmShader::m_Mode == DtmShader::ShaderMode::Contour)
			m_IsoStep.setVisible(true);
		if (DtmShader::m_Mode == DtmShader::ShaderMode::Free_Shading) {
			m_Azimuth.setVisible(true);
			m_Zenith.setVisible(true);
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void DtmLayersViewer::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &m_Opacity) {
		DtmShader::m_dOpacity = slider->getValue();
		m_ModelRange.sendActionMessage("Repaint");
		return;
	}
	if (slider == &m_IsoStep)	// Changement de l'equidistance des isohypses
		DtmShader::m_dIsoStep = slider->getValue();
	if (slider == &m_Azimuth)
		DtmShader::m_dSolarAzimuth = slider->getValue();
	if (slider == &m_Zenith)
		DtmShader::m_dSolarZenith = slider->getValue();
	m_ModelRange.sendActionMessage("UpdateDtm");
}

//==============================================================================
// Drag&Drop
//==============================================================================
void DtmLayersViewer::itemDropped(const SourceDetails& details)
{
	juce::String message = details.description.toString();
	juce::StringArray T;
	T.addTokens(message, ":", "");
	if (T.size() < 1)
		return;
	int item;
	item = T[0].getIntValue();
	int row = m_TableDtm.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	int index = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (!C->IsDTM())
			continue;
		index++;
		if (index == item)
			C->Repres()->ZOrder(row * 10 + 5);
		else
			C->Repres()->ZOrder(index * 10);
	}
	m_Base->SortClass();
	m_TableDtm.repaint();
	m_ModelDtm.sendActionMessage("UpdateDtm");
}

bool DtmLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_TableDtm)
		return false;
	return true;
}

//==============================================================================
// Calcul des delta Z importants en noeuds successifs
//==============================================================================
void DtmLayersViewer::ComputeDeltaZ(std::vector< XGeoClass*> T)
{
	std::unique_ptr<juce::AlertWindow> asyncAlertWindow;
	asyncAlertWindow = std::make_unique<juce::AlertWindow>(juce::translate("Compute Delta Z"),
		juce::translate("This tool allows to find the delta Z between successive nodes"),
		juce::MessageBoxIconType::QuestionIcon);

	asyncAlertWindow->addTextEditor("DeltaZ", "5.0", juce::translate("Delta Z :"));
	juce::TextEditor* deltaZ_editor = asyncAlertWindow->getTextEditor("DeltaZ");
	deltaZ_editor->setInputRestrictions(5, "0123456789.");
	asyncAlertWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	asyncAlertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	if (asyncAlertWindow->runModalLoop() == 0)
		return;
	double deltaZ = asyncAlertWindow->getTextEditorContents("DeltaZ").getDoubleValue();

	// Thread de traitement
	class MyTask : public ThreadClassProcessor {
	public:
		double deltaZ = 5.;

		MyTask() : ThreadClassProcessor(juce::translate("Compute Delta Z ..."), true) { ; }

		virtual bool Process(XGeoVector* V)
		{
			if (V->TypeVector() != XGeoVector::DTM)
				return false;
			XGeoFDtm* dtm = dynamic_cast<XGeoFDtm*>(V);
			if (dtm == nullptr)
				return false;
			std::vector<XPt3D> DZ;
			if (!dtm->DeltaMax(deltaZ, DZ))
				return false;
			std::ofstream mif, mid;
			mif.open(m_strMifFile.toStdString(), std::ios::out | std::ios::app);
			mid.open(m_strMidFile.toStdString(), std::ios::out | std::ios::app);
			mif.setf(std::ios::fixed); mif.precision(2);
			mid.setf(std::ios::fixed); mid.precision(2);
			for (size_t i = 0; i < DZ.size(); i++) {
				mif << "POINT " << DZ[i].X << " " << DZ[i].Y << std::endl;
				mid << DZ[i].Z << std::endl;
			}
			return true;
		}
	};

	MyTask M;
	M.deltaZ = deltaZ;
	M.m_T = T;
	juce::StringArray Att;
	Att.add("DeltaZ decimal (10,2)");
	M.CreateMifMidFile(m_Cache, juce::String("DeltaZ_") + juce::String(deltaZ,2) , Att);
	M.runThread();
	GeoTools::ImportMifMid(M.m_strMifFile, m_Base);
	GeoTools::ColorizeClasses(m_Base);
	sendActionMessage("UpdateVector");
}