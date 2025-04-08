//-----------------------------------------------------------------------------
//								LasLayersViewer.cpp
//								===================
//
// Visualisation des classes d'objets LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 27/10/2023
//-----------------------------------------------------------------------------

#include "LasLayersViewer.h"
#include "ClassViewer.h"
#include "Utilities.h"
#include "AppUtil.h"
#include "LasShader.h"
#include "ThreadClassProcessor.h"
#include "GeoBase.h"
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
void LasViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void LasViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	XGeoClass* lasClass = FindLasClass(rowNumber);
	if (lasClass == nullptr)
		return;

	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (lasClass->Visible())
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (lasClass->Selectable())
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Name:
		g.drawText(juce::String(lasClass->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::NbElem:
		g.drawText(juce::String(lasClass->NbVector()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Zmin:
		g.drawText(juce::String(lasClass->Zmin()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Zmax:
		g.drawText(juce::String(lasClass->Zmax()), 0, 0, width, height, juce::Justification::centred);
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
		std::function< void() > ComputeStat = [=]() { // Statistiques
			sendActionMessage("ComputeStat"); };
		std::function< void() > ViewObjects = [=]() { // Visualisation des objets de la classe
			gClassViewerMgr.AddClassViewer(lasClass->Name(), lasClass, this);
			};


		juce::PopupMenu menu;
		menu.addItem(juce::translate("Layer Center"), LayerCenter);
		menu.addItem(juce::translate("Layer Frame"), LayerFrame);
		menu.addItem(juce::translate("View Objects"), ViewObjects);
		menu.addSeparator();
		menu.addItem(juce::translate("Compute DTM"), ComputeDtm);
		menu.addItem(juce::translate("Statistics"), ComputeStat);
		menu.addSeparator();
		menu.addItem(juce::translate("Remove"), LayerRemove);
		menu.showMenuAsync(juce::PopupMenu::Options());
	}
}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void LasViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
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
// Clic dans l'entete
//==============================================================================
void LasViewerModel::sortOrderChanged(int newSortColumnId, bool /*isForwards*/)
{
	if (newSortColumnId == Visibility)
		sendActionMessage("InvertVisibility");
	if (newSortColumnId == Selectable)
		sendActionMessage("InvertSelectable");
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
void LasViewerModel::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
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
void ClassifModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// ClassifModel : paintCell
//==============================================================================
void ClassifModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (LasShader::ClassificationVisibility((uint8_t)rowNumber))
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (LasShader::ClassificationSelectable((uint8_t)rowNumber))
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Number:
		g.drawText(juce::String(rowNumber), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Name:
		g.drawText(LasShader::ClassificationName((uint8_t)rowNumber), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Colour:
		g.setColour(juce::Colours::white);
		g.fillRect(0, 0, width, height);
		g.setColour(LasShader::ClassificationColor((uint8_t)rowNumber));
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
		sendActionMessage("UpdateLasClassificationVisibility");
		return;
	}
	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateLasClassificationSelectable");
		return;
	}
	if (columnId == Column::Colour) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("LAS color " + LasShader::ClassificationName((uint8_t)m_ActiveRow));
		colourSelector->setCurrentColour(LasShader::ClassificationColor((uint8_t)m_ActiveRow));
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
			LasShader::ClassificationColor(color, (uint8_t)m_ActiveRow);
			sendActionMessage("UpdateLasClassificationColor");
		}
	}
}

//==============================================================================
// ClassifModel : sortOrderChanged
//==============================================================================
void ClassifModel::sortOrderChanged(int newSortColumnId, bool /*isForwards*/)
{
	if (newSortColumnId == Visibility)
		sendActionMessage("InvertClassificationVisibility");
}

//==============================================================================
// LasLayersViewer : constructeur
//==============================================================================
LasLayersViewer::LasLayersViewer()
{
	m_Base = nullptr;
	m_nLasGradient = 0;

	setName("LAS Layers");
	setWantsKeyboardFocus(true);
	m_ModelLas.addActionListener(this);
	// Bordure
	m_TableLas.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableLas.setOutlineThickness(1);
	m_TableLas.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Visibility, 25);
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Selectable, 25);
	m_TableLas.getHeader().addColumn(juce::translate("Name"), LasViewerModel::Column::Name, 200);
	m_TableLas.getHeader().addColumn(juce::translate("Nb. Elem"), LasViewerModel::Column::NbElem, 50);
	m_TableLas.getHeader().addColumn(juce::translate("Zmin"), LasViewerModel::Column::Zmin, 50);
	m_TableLas.getHeader().addColumn(juce::translate("Zmax"), LasViewerModel::Column::Zmax, 50);
	m_TableLas.getHeader().addColumn(juce::translate(" "), LasViewerModel::Column::Options, 50);
	m_TableLas.setSize(377, 200);
	m_TableLas.setAutoSizeMenuOptionShown(false);
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

	// Sliders ZMin et ZMax
	m_sldZRange.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	m_sldZRange.setSliderStyle(juce::Slider::TwoValueHorizontal);
	m_sldZRange.setRange(LasShader::Zmin(), LasShader::Zmax(), 1.);
	m_sldZRange.setMinAndMaxValues(LasShader::Zmin(), LasShader::Zmax());
	m_sldZRange.setPopupDisplayEnabled(true, false, this);
	m_sldZRange.addListener(this);
	m_sldZRange.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_sldZRange);
	m_lblZRange.setText(juce::translate("Min / Max :"), juce::dontSendNotification);
	addAndMakeVisible(m_lblZRange);
	m_lblZRange.attachToComponent(&m_sldZRange, true);
	m_drwZRect.setInterceptsMouseClicks(true, true);
	m_drwZRect.addMouseListener(this, true);
	addAndMakeVisible(m_drwZRect);
	if (LasShader::Mode() != LasShader::ShaderMode::Altitude) {
		m_sldZRange.setVisible(false);
		m_lblZRange.setVisible(false);
		m_drwZRect.setVisible(false);
	}

	// Slider intensite
	m_sldIntensity.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	m_sldIntensity.setSliderStyle(juce::Slider::LinearHorizontal);
	m_sldIntensity.setRange(1., 256., 1.);
	m_sldIntensity.setValue(256. - LasShader::IntensityGain(), juce::dontSendNotification);
	m_sldIntensity.setPopupDisplayEnabled(true, false, this);
	m_sldIntensity.setSkewFactorFromMidPoint(240);
	m_sldIntensity.addListener(this);
	m_sldIntensity.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(m_sldIntensity);
	m_lblIntensity.setText(juce::translate("Gain :"), juce::dontSendNotification);
	addAndMakeVisible(m_lblIntensity);
	m_lblIntensity.attachToComponent(&m_sldIntensity, true);
	if (LasShader::Mode() != LasShader::ShaderMode::Intensity) {
		m_sldIntensity.setVisible(false);
		m_lblIntensity.setVisible(false);
	}

	// Couleurs des classifications
	m_ModelClassif.addActionListener(this);
	// Bordure
	m_TableClassif.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_TableClassif.setOutlineThickness(1);
	m_TableClassif.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_TableClassif.getHeader().addColumn(juce::translate(" "), ClassifModel::Column::Visibility, 25, 30, -1, juce::TableHeaderComponent::sortable | juce::TableHeaderComponent::visible);
	m_TableClassif.getHeader().addColumn(juce::translate(" "), ClassifModel::Column::Selectable, 25, 30, -1, juce::TableHeaderComponent::visible);
	m_TableClassif.getHeader().addColumn(juce::translate("Value"), ClassifModel::Column::Number, 50, 30, -1, juce::TableHeaderComponent::notSortable);
	m_TableClassif.getHeader().addColumn(juce::translate("Name"), ClassifModel::Column::Name, 200, 30, -1, juce::TableHeaderComponent::notSortable);
	m_TableClassif.getHeader().addColumn(juce::translate("Colour"), ClassifModel::Column::Colour, 50, 30, -1, juce::TableHeaderComponent::notSortable);
	m_TableClassif.setSize(352, 200);
	m_TableClassif.setAutoSizeMenuOptionShown(false);
	m_TableClassif.setModel(&m_ModelClassif);
	addAndMakeVisible(m_TableClassif);
	if (LasShader::Mode() != LasShader::ShaderMode::Classification)
		m_TableClassif.setVisible(false);
}

//==============================================================================
// LayerViewer : fixe la base de donnees
//==============================================================================
void LasLayersViewer::SetBase(XGeoBase* base)
{ 
	m_Base = base;
	m_ModelLas.SetBase(base);
	m_TableLas.updateContent();
	double zmin = std::numeric_limits<double>::max();
	double zmax = std::numeric_limits<double>::min();
	int count = 0;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (!C->IsLAS())
			continue;
		count++;
		zmin = XMin(C->Zmin(), zmin);
		zmax = XMax(C->Zmax(), zmax);
	}
	if (count < 1)
		return;
	zmin = floor(zmin);
	zmax = ceil(zmax);
	LasShader::Zmin(zmin);
	LasShader::Zmax(zmax);
	m_sldZRange.setRange(zmin, zmax, 1.);
	m_sldZRange.setMinAndMaxValues(zmin, zmax);
}

//==============================================================================
// LayerViewer : mise a jour du nom des colonnes (pour la traduction)
//==============================================================================
void LasLayersViewer::Translate()
{
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Name, juce::translate("Name"));
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Zmin, juce::translate("Nb. Elem"));
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Zmin, juce::translate("Zmin"));
	m_TableLas.getHeader().setColumnName(LasViewerModel::Column::Zmax, juce::translate("Zmax"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Number, juce::translate("Value"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Name, juce::translate("Name"));
	m_TableClassif.getHeader().setColumnName(ClassifModel::Column::Colour, juce::translate("Colour"));

	m_Mode.changeItemText(1, juce::translate("Altitude"));
	m_Mode.changeItemText(2, juce::translate("RGB"));
	m_Mode.changeItemText(3, juce::translate("IRC"));
	m_Mode.changeItemText(4, juce::translate("Classification"));
	m_Mode.changeItemText(5, juce::translate("Intensity"));
	m_Mode.changeItemText(6, juce::translate("Scan Angle"));

	m_Opacity.setTextValueSuffix(juce::translate("% opacity"));
}

//==============================================================================
// LayerViewer : mise a jour du gradient altimetrique
//==============================================================================
void LasLayersViewer::UpdateAltiColors()
{
	auto b = getLocalBounds();
	juce::ColourGradient gradient = juce::ColourGradient::horizontal(LasShader::AltiColor((uint8_t)0), 0.f,
		LasShader::AltiColor((uint8_t)255), (float)b.getWidth());
	for (int i = 1; i < 255; i++)
		gradient.addColour((double)i / 255, LasShader::AltiColor((uint8_t)i));
	m_drwZRect.setFill(juce::FillType(gradient));
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
	m_sldZRange.setTopLeftPosition(100, b.getHeight() / 2 + 90);
	m_sldZRange.setSize(b.getWidth() - 100, 24);
	m_sldIntensity.setTopLeftPosition(100, b.getHeight() / 2 + 90);
	m_sldIntensity.setSize(b.getWidth() - 100, 24);
	// Palette coloree des altitudes
	UpdateAltiColors();
	m_drwZRect.setRectangle(juce::Parallelogram<float>(juce::Rectangle<float>(0.f, b.getHeight() / 2 + 120.f, (float)b.getWidth(), 24.f)));
}

//==============================================================================
// Gestion des actions
//==============================================================================
void LasLayersViewer::actionListenerCallback(const juce::String& message)
{
	if (m_Base == nullptr)
		return;
	if (message == "NewWindow") {
		m_TableLas.updateContent();
		m_TableLas.repaint();
		return;
	}
	if (message == "UpdateLasOptions") {
		sendActionMessage("UpdateLas");
		repaint();
		return;
	}
	if (message == "UpdateLasClassificationColor") {
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "UpdateClass") {
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "InvertVisibility") {
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsLAS()) C->Visible(!C->Visible());
		}
		m_TableLas.repaint();
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "InvertSelectable") {
		for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
			XGeoClass* C = m_Base->Class(i);
			if (C->IsLAS()) C->Selectable(!C->Selectable());
		}
		m_TableLas.repaint();
		return;
	}
	if (message == "InvertClassificationVisibility") {
		for (int i = 0; i < 256; i++) {
			LasShader::ClassificationVisibility(!LasShader::ClassificationVisibility((uint8_t)i), (uint8_t)i);
		}
		m_TableClassif.repaint();
		sendActionMessage("UpdateLas");
		return;
	}

	// Classes selectionnees
	std::vector<XGeoClass*> T;
	juce::SparseSet< int > selectedRows = m_TableLas.getSelectedRows();
	int count = -1;
	for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
		XGeoClass* C = m_Base->Class(i);
		if (C->IsLAS()) {
			count++;
			if (selectedRows.contains(count))
				T.push_back(C);
		}
	}

	if (message == "UpdateLasVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_TableLas.repaint();
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "UpdateLasSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_TableLas.repaint();
		return;
	}
	if (message == "RemoveLasClass") {
		juce::String removeLas = "RemoveLasClass";
		for (int i = 0; i < T.size(); i++)
			removeLas += (juce::String(":") + T[i]->Layer()->Name().c_str() + juce::String(":") + T[i]->Name().c_str());
		sendActionMessage(removeLas);
		m_TableLas.deselectAllRows();
		m_TableLas.repaint();
		return;
	}
	if (message == "UpdateLasClassificationVisibility") {
		juce::SparseSet< int > S = m_TableClassif.getSelectedRows();
		for (int i = 0; i < S.size(); i++) {
			LasShader::ClassificationVisibility(!LasShader::ClassificationVisibility((uint8_t)S[i]), (uint8_t)S[i]);
		}
		m_TableClassif.repaint();
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "UpdateLasClassificationSelectable") {
		juce::SparseSet< int > S = m_TableClassif.getSelectedRows();
		for (int i = 0; i < S.size(); i++) {
			LasShader::ClassificationVisibility(!LasShader::ClassificationSelectable((uint8_t)S[i]), (uint8_t)S[i]);
		}
		m_TableClassif.repaint();
		sendActionMessage("UpdateLas");
		return;
	}
	if (message == "ComputeDtm") {
		ComputeDtm(T);
		return;
	}
	if (message == "ComputeStat") {
		ComputeStat(T);
		return;
	}
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Changement de valeur des ComboBox
//==============================================================================
void LasLayersViewer::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &m_Mode) {
		LasShader shader;
		shader.Mode((LasShader::ShaderMode)(m_Mode.getSelectedId()));
		m_ModelLas.sendActionMessage("UpdateLasOptions");
		bool ZRangeVisible = false, IntensityVisible = false, ClassifVisible = false;
		if (LasShader::Mode() == LasShader::ShaderMode::Classification) 
			ClassifVisible = true;
		if (LasShader::Mode() == LasShader::ShaderMode::Altitude)
			ZRangeVisible = true;
		if (LasShader::Mode() == LasShader::ShaderMode::Intensity)
			IntensityVisible = true;
		m_TableClassif.setVisible(ClassifVisible);
		m_sldZRange.setVisible(ZRangeVisible);
		m_lblZRange.setVisible(ZRangeVisible);
		m_sldIntensity.setVisible(IntensityVisible);
		m_lblIntensity.setVisible(IntensityVisible);

		m_drwZRect.setVisible(ZRangeVisible || IntensityVisible);
	}
}

//==============================================================================
// Clic souris
//==============================================================================
void LasLayersViewer::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (!m_drwZRect.hitTest(event.getMouseDownX(), event.getMouseDownY()))
		return;
	m_nLasGradient += 1;
	m_nLasGradient = m_nLasGradient % 6;
	SetAltiColors();
	UpdateAltiColors();
	repaint();
	m_ModelLas.sendActionMessage("UpdateLasOptions");
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool LasLayersViewer::keyPressed(const juce::KeyPress& key)
{
	if (key.getKeyCode() == juce::KeyPress::F2Key) {
		AppUtil::SaveTableComponent(&m_TableClassif);
		return true;
	}
	return false;	// On transmet l'evenement sans le traiter
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
	if (slider == &m_sldZRange) {
		LasShader::Zmin(slider->getMinValue());
		LasShader::Zmax(slider->getMaxValue());
		//m_drwZRect.repaint();
		//UpdateAltiColors();
	}
	if (slider == &m_sldIntensity) {
		LasShader::IntensityGain(256 - slider->getValue());
	}

	m_ModelLas.sendActionMessage("UpdateLasOptions");
}

//==============================================================================
// Drag&Drop
//==============================================================================
void LasLayersViewer::itemDropped(const SourceDetails& /*details*/)
{
	//juce::String message = details.description.toString();
	//juce::StringArray T;
	//T.addTokens(message, ":", "");
	//if (T.size() < 1)
	//	return;
	//int i;
	//i = T[0].getIntValue();
	//int row = m_TableLas.getRowContainingPosition(details.localPosition.x, details.localPosition.y);
	////m_Base->ReorderDtmLayer(i, row);
	////m_Table.updateContent();
	////m_Table.repaint();
	//m_ModelLas.sendActionMessage("UpdateLas");
}

bool LasLayersViewer::isInterestedInDragSource(const SourceDetails& details)
{
	if (details.sourceComponent.get() != &m_TableLas)
		return false;
	return true;
}

//==============================================================================
// Fixe les couleurs de la palette altimetrique
//==============================================================================
void LasLayersViewer::SetAltiColors()
{
	std::vector<juce::Colour> T;
	switch (m_nLasGradient) {
	case 1 : T.push_back(juce::Colours::darkblue); T.push_back(juce::Colours::green); T.push_back(juce::Colours::lightcoral);
		break;
	case 2 : T.push_back(juce::Colours::aqua); T.push_back(juce::Colours::darkorange); T.push_back(juce::Colours::lawngreen);
		break;
	case 3 : T.push_back(juce::Colours::darkgreen); T.push_back(juce::Colours::lightskyblue); T.push_back(juce::Colours::rebeccapurple);
		break;
	case 4 : T.push_back(juce::Colours::purple); T.push_back(juce::Colours::yellow); T.push_back(juce::Colours::red);
		break;
	case 5 : T.push_back(juce::Colours::purple); T.push_back(juce::Colours::blue); T.push_back(juce::Colours::green);
		T.push_back(juce::Colours::yellow); T.push_back(juce::Colours::orange); T.push_back(juce::Colours::red); T.push_back(juce::Colours::white);
		break;
		default: m_nLasGradient = 0;
	}
	if (m_nLasGradient > 0) {
		LasShader shader;
		int nb_grad = (int)T.size() - 1;
		double nb_step = 256. / nb_grad;
		int cmpt = 0;
		for (int step = 0; step < T.size() - 1; step++) {
			juce::ColourGradient gradient(T[step], 0.f, 0.f, T[step+1], (float)(256. / nb_grad), (float)(256. / nb_grad), false);
			for (int i = 0; i < nb_step; i++) {
				shader.AltiColor(gradient.getColourAtPosition(i / nb_step), (uint8_t)XMin(cmpt, 255));
				cmpt++;
			}
		}
	}
	else
		LasShader::InitAltiColor();
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

	asyncAlertWindow->addTextEditor("GSD", "1.0", juce::translate("GSD"));
	juce::TextEditor* gsd_editor = asyncAlertWindow->getTextEditor("GSD");
	gsd_editor->setInputRestrictions(5, "0123456789.");
	asyncAlertWindow->addComboBox("Algo", { juce::translate("Z minimum"), juce::translate("Z average"), 
		juce::translate("Z maximum"), juce::translate("StdDev"), juce::translate("Height"), juce::translate("Density") }, juce::translate("Algorithm"));
	asyncAlertWindow->addComboBox("FillHole", { juce::translate("Yes"), juce::translate("No") }, juce::translate("Fill NOZ Hole"));
	asyncAlertWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	asyncAlertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	if(asyncAlertWindow->runModalLoop() == 0)
		return;

	juce::String foldername = AppUtil::OpenFolder("Las2Dtm", juce::translate("Choose a directory..."));
	if (foldername.isEmpty())
		return;
	auto algoIndexChosen = asyncAlertWindow->getComboBoxComponent("Algo")->getSelectedItemIndex();
	auto fillHole = asyncAlertWindow->getComboBoxComponent("FillHole")->getSelectedItemIndex();
	auto gsd_text = asyncAlertWindow->getTextEditorContents("GSD");
	double gsd = gsd_text.getDoubleValue();

	// Thread de traitement
	class MyTask : public ThreadClassProcessor {
	public:
		double GSD = 1.;
		XLasFile::AlgoDtm algo = XLasFile::ZMinimum;
		bool fill_hole = true;
		bool classif_visibility[256];

		MyTask() : ThreadClassProcessor(juce::translate("Compute DTM/DSM ..."), true) 
		{ LasShader shader;
		for (int i = 0; i < 256; i++) classif_visibility[i] = shader.ClassificationVisibility((uint8_t)i);
		}

		virtual bool Process(XGeoVector* V)
		{
			if (V->TypeVector() != XGeoVector::LAS)
				return false;
			XLasFile las;
			if (!las.Open(V->Filename()))
				return false;
			juce::File file(V->Filename());
			juce::String file_out = m_strFolderOut + juce::File::getSeparatorString() + file.getFileNameWithoutExtension() + ".tif";
			setStatusMessage(juce::translate("Processing ") + file.getFileNameWithoutExtension());
			return las.ComputeDtm(file_out.toStdString(), GSD, algo, classif_visibility, fill_hole);
		}
	};

	MyTask M;
	M.GSD = gsd;
	M.m_T = T;
	M.m_strFolderOut = foldername;
	M.algo = (XLasFile::AlgoDtm)(algoIndexChosen + 1);
	if (fillHole > 0)
		M.fill_hole = false;
	M.runThread();
}

//==============================================================================
// Calcul des statistiques des fichiers LAS
//==============================================================================
void LasLayersViewer::ComputeStat(std::vector<XGeoClass*> T)
{
	juce::String foldername = AppUtil::OpenFolder("Statistics", juce::translate("Choose a directory..."));
	if (foldername.isEmpty())
		return;

	// Thread de traitement
	class MyTask : public ThreadClassProcessor {
	public:

		MyTask() : ThreadClassProcessor(juce::translate("Compute LAS Statistics ..."), true)
		{
			
		}

		virtual bool Process(XGeoVector* V)
		{
			if (V->TypeVector() != XGeoVector::LAS)
				return false;
			XLasFile las;
			if (!las.Open(V->Filename()))
				return false;
			juce::File file(V->Filename());
			juce::String file_out = m_strFolderOut + juce::File::getSeparatorString() + file.getFileNameWithoutExtension() + ".txt";
			setStatusMessage(juce::translate("Processing ") + file.getFileNameWithoutExtension());
			std::ofstream mif, mid;
			mif.open(m_strMifFile.toStdString(), std::ios::out | std::ios::app);
			mid.open(m_strMidFile.toStdString(), std::ios::out | std::ios::app);
			mif.setf(std::ios::fixed); mif.precision(2);
			mid.setf(std::ios::fixed); mid.precision(2);
			return las.StatLas(file_out.toStdString(), &mif, &mid);
		}
	};

	MyTask M;
	M.m_T = T;
	M.m_strFolderOut = foldername;

	juce::StringArray Att;
	Att.add("NAME char (255)");
	Att.add("ZMIN decimal (10,2)"); Att.add("ZMAX decimal (10,2)");
	Att.add("CLASS_0 Integer"); Att.add("NON_CLASS Integer");
	Att.add("SOL Integer"); Att.add("VEG_BAS Integer");
	Att.add("VEG_MOY Integer"); Att.add("VEG_HAU Integer");
	Att.add("BATI Integer"); Att.add("EAU Integer");
	Att.add("PONT Integer"); Att.add("SURSOL Integer");
	Att.add("ARTEFACTS Integer"); Att.add("VIRTUEL Integer");
	Att.add("DIVERS_BAT Integer"); Att.add("Autres Integer");

	M.CreateMifMidFile(foldername, juce::String("StatLAS"), Att);
	M.runThread();
	GeoTools::ImportMifMid(M.m_strMifFile, m_Base);
	GeoTools::ColorizeClasses(m_Base);
	sendActionMessage("UpdateVector");
}
