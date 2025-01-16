//-----------------------------------------------------------------------------
//								SentinelViewer.cpp
//								==================
//
// Visualisation des scenes SENTINEL
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 22/12/2024
//-----------------------------------------------------------------------------

#include "SentinelViewer.h"

#include "../XTool/XGeoBase.h"
#include "../XTool/XGeoMap.h"
#include "../../XToolGeod/XGeoPref.h"

//==============================================================================
// SentinelViewerComponent : constructeur
//==============================================================================
SentinelViewerComponent::SentinelViewerComponent() : m_Tab(juce::TabbedButtonBar::TabsAtTop)
{
	m_Base = nullptr;
	m_dX0 = m_dY0 = XGEO_NO_DATA;
	// Options d'import
	m_ImportPage.m_btnImport.setButtonText(juce::translate("Import"));
	m_ImportPage.m_btnImport.addListener(this);
	m_ImportPage.addAndMakeVisible(m_ImportPage.m_btnImport);
	m_ImportPage.m_btn10m.setButtonText("10 m");
	m_ImportPage.m_btn10m.setToggleState(true, juce::NotificationType::dontSendNotification);
	m_ImportPage.addAndMakeVisible(m_ImportPage.m_btn10m);
	m_ImportPage.m_btn20m.setButtonText("20 m");
	m_ImportPage.addAndMakeVisible(m_ImportPage.m_btn20m);
	m_ImportPage.m_btn60m.setButtonText("60 m");
	m_ImportPage.addAndMakeVisible(m_ImportPage.m_btn60m);

	// Mode de visualisation RGB, IRC, NDVI, NDWI, SWIR, URBAN
	m_ViewPage.m_cbxMode.addItem("RGB", XSentinelScene::RGB + 1);	// ID = 0 n'est pas permis
	m_ViewPage.m_cbxMode.addItem("IRC", XSentinelScene::IRC + 1);
	m_ViewPage.m_cbxMode.addItem("NDVI", XSentinelScene::NDVI + 1);
	m_ViewPage.m_cbxMode.addItem("NDWI", XSentinelScene::NDWI + 1);
	m_ViewPage.m_cbxMode.addItem("SWIR", XSentinelScene::SWIR + 1);
	m_ViewPage.m_cbxMode.addItem("URBAN", XSentinelScene::URBAN + 1);
	m_ViewPage.m_cbxMode.addSeparator();
	m_ViewPage.m_cbxMode.addItem("AOT", XSentinelScene::AOT + 1);
	m_ViewPage.m_cbxMode.addItem("B01", XSentinelScene::B01 + 1);
	m_ViewPage.m_cbxMode.addItem("B02", XSentinelScene::B02 + 1);
	m_ViewPage.m_cbxMode.addItem("B03", XSentinelScene::B03 + 1);
	m_ViewPage.m_cbxMode.addItem("B04", XSentinelScene::B04 + 1);
	m_ViewPage.m_cbxMode.addItem("B05", XSentinelScene::B05 + 1);
	m_ViewPage.m_cbxMode.addItem("B06", XSentinelScene::B06 + 1);
	m_ViewPage.m_cbxMode.addItem("B07", XSentinelScene::B07 + 1);
	m_ViewPage.m_cbxMode.addItem("B08/B8A", XSentinelScene::B8A + 1);
	m_ViewPage.m_cbxMode.addItem("B09", XSentinelScene::B09 + 1);
	m_ViewPage.m_cbxMode.addItem("B11", XSentinelScene::B11 + 1);
	m_ViewPage.m_cbxMode.addItem("B12", XSentinelScene::B12 + 1);
	m_ViewPage.m_cbxMode.addItem("SCL", XSentinelScene::SCL + 1);
	m_ViewPage.m_cbxMode.addItem("TCI", XSentinelScene::TCI + 1);
	m_ViewPage.m_cbxMode.addItem("WVP", XSentinelScene::WVP + 1);

	m_ViewPage.m_cbxMode.setSelectedId(XSentinelScene::RGB + 1);
	m_ViewPage.m_cbxMode.addListener(this);
	m_ViewPage.addAndMakeVisible(m_ViewPage.m_cbxMode);
	// Resolution de visualisation
	m_ViewPage.m_cbxResol.addItem("10 m", 10);
	m_ViewPage.m_cbxResol.addItem("20 m", 20);
	m_ViewPage.m_cbxResol.addItem("60 m", 60);
	m_ViewPage.m_cbxResol.setSelectedId(10);
	m_ViewPage.m_cbxResol.addListener(this);
	m_ViewPage.addAndMakeVisible(m_ViewPage.m_cbxResol);
	EnableViewMode(10);

	// Slider de parametrage
	m_ViewPage.m_lblParam.setText("Boost :", juce::NotificationType::dontSendNotification);
	m_ViewPage.addAndMakeVisible(m_ViewPage.m_lblParam);
	m_ViewPage.m_sldParam.setSliderStyle(juce::Slider::LinearHorizontal);
	m_ViewPage.m_sldParam.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 80, 25);
	m_ViewPage.m_sldParam.setRange(0., 100., 0.1);
	m_ViewPage.m_sldParam.setValue(0., juce::NotificationType::dontSendNotification);
	m_ViewPage.m_sldParam.addListener(this);
	m_ViewPage.addAndMakeVisible(m_ViewPage.m_sldParam);

	// Bordure
	m_ViewPage.m_tblScene.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_ViewPage.m_tblScene.setOutlineThickness(1);
	// Ajout des colonnes
	m_ViewPage.m_tblScene.getHeader().addColumn(juce::translate(" "), SentinelSceneModel::Column::Visibility, 25);
	m_ViewPage.m_tblScene.getHeader().addColumn(juce::translate(" "), SentinelSceneModel::Column::Selectable, 25);
	m_ViewPage.m_tblScene.getHeader().addColumn(juce::translate("Name"), SentinelSceneModel::Column::Name, 60);
	m_ViewPage.m_tblScene.getHeader().addColumn(juce::translate("# Scenes"), SentinelSceneModel::Column::NbScene, 60);
	m_ViewPage.m_tblScene.getHeader().addColumn(juce::translate("Date"), SentinelSceneModel::Column::Date, 100);
	m_ViewPage.m_tblScene.setModel(&m_ViewPage.m_mdlScene);
	m_ViewPage.addAndMakeVisible(m_ViewPage.m_tblScene);
	m_ViewPage.m_mdlScene.addActionListener(this);

	m_ViewPage.addAndMakeVisible(m_ViewPage.m_imcQuickView);	// Quickview

	// Bouton d'analyse
	m_AnalyzePage.m_btnAnalyze.setButtonText(juce::translate("Analyze"));
	m_AnalyzePage.m_btnAnalyze.addListener(this);
	m_AnalyzePage.addAndMakeVisible(m_AnalyzePage.m_btnAnalyze);

	// Bordure
	m_AnalyzePage.m_tblExtract.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_AnalyzePage.m_tblExtract.setOutlineThickness(1);
	m_AnalyzePage.m_tblExtract.setModel(&m_AnalyzePage.m_mdlExtract);
	m_AnalyzePage.m_tblExtract.setRowHeight(204);
	m_AnalyzePage.addAndMakeVisible(m_AnalyzePage.m_tblExtract);
	m_AnalyzePage.m_mdlExtract.addActionListener(this);
	m_AnalyzePage.m_mdlExtract.m_Result = &m_AnalyzePage.m_AnalyzeResult;

	// Dessin de l'analyse
	m_AnalyzePage.m_DrawAnalyze.m_Result = &m_AnalyzePage.m_AnalyzeResult;
	m_AnalyzePage.addAndMakeVisible(m_AnalyzePage.m_DrawAnalyze);
	m_AnalyzePage.m_DrawAnalyze.addActionListener(this);

	// Tab pour contenir les pages
	//addAndMakeVisible(m_ImportPage);
	auto colour = findColour(juce::ResizableWindow::backgroundColourId);
	m_Tab.addTab(juce::translate("Import"), colour, &m_ImportPage, false);
	m_Tab.addTab(juce::translate("View"), colour, &m_ViewPage, false);
	m_Tab.addTab(juce::translate("Analyze"), colour, &m_AnalyzePage, false);
	addAndMakeVisible(m_Tab);

	setSize(400, 450);
}

//==============================================================================
// SentinelViewerComponent : fixe la base de donnee
//==============================================================================
void SentinelViewerComponent::SetBase(XGeoBase* base)
{ 
	m_Base = base;
	m_ViewPage.m_mdlScene.SetBase(base);
}

//==============================================================================
// SentinelViewerComponent : Redimensionnement du composant
//==============================================================================
void SentinelViewerComponent::resized()
{
	m_Tab.setBounds(getLocalBounds().reduced(4));
	
	auto b = getLocalBounds();
	m_ImportPage.m_btn10m.setBounds(10, 20, 90, 25);
	m_ImportPage.m_btn20m.setBounds(10, 50, 90, 25);
	m_ImportPage.m_btn60m.setBounds(10, 80, 90, 25);
	m_ImportPage.m_btnImport.setBounds(120, 50, 90, 25);

	m_ViewPage.m_cbxMode.setBounds(10, 20, 90, 25);
	m_ViewPage.m_cbxResol.setBounds(120, 20, 90, 25);
	m_ViewPage.m_lblParam.setBounds(240, 20, 60, 25);
	m_ViewPage.m_sldParam.setBounds(300, 15, 80, 35);

	m_ViewPage.m_tblScene.setBounds(10, 60, b.getWidth() - 30, /*b.getHeight() - */110);
	m_ViewPage.m_imcQuickView.setBounds(b.getWidth() / 2 - 128, 180, 256, 256);

	m_AnalyzePage.m_btnAnalyze.setBounds(b.getWidth() / 2 - 40, 20, 80, 25);
	m_AnalyzePage.m_tblExtract.setBounds(5, 50, b.getWidth() - 20, 245);
	m_AnalyzePage.m_DrawAnalyze.setBounds(5, 300, b.getWidth() - 20, b.getHeight() - 340);	
}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void SentinelViewerComponent::buttonClicked(juce::Button* button)
{
	if (button == &m_ImportPage.m_btnImport)
		return ImportScenes();
	if (button == &m_AnalyzePage.m_btnAnalyze)
		return Analyze();
}

//==============================================================================
// Reponses aux combo box
//==============================================================================
void SentinelViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	int resol = m_ViewPage.m_cbxResol.getSelectedId();
	int mode = m_ViewPage.m_cbxMode.getSelectedId() - 1;	// ID commence a 1 ...
	EnableViewMode(resol);
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		if (comboBoxThatHasChanged == &m_ViewPage.m_cbxResol) {
			scene->SetActiveResolution(resol);
		}
		if (comboBoxThatHasChanged == &m_ViewPage.m_cbxMode) {
			scene->SetViewMode((XSentinelScene::ViewMode)mode);
			if ((mode == XSentinelScene::ViewMode::NDVI) || (mode == XSentinelScene::ViewMode::NDWI)) {
				m_ViewPage.m_sldParam.setRange(-1., 1., 0.01);
				m_ViewPage.m_sldParam.setValue(scene->CutNDVI(), juce::NotificationType::dontSendNotification);
				if (mode == XSentinelScene::ViewMode::NDWI)
					m_ViewPage.m_sldParam.setValue(scene->CutNDWI(), juce::NotificationType::dontSendNotification);
				m_ViewPage.m_lblParam.setText("Cut :", juce::NotificationType::dontSendNotification);
			}
			else {
				m_ViewPage.m_sldParam.setRange(0, 100, 0.1);
				m_ViewPage.m_lblParam.setText("Boost :", juce::NotificationType::dontSendNotification);
			}
		}
	}
	sendActionMessage("UpdateRaster");
	m_ViewPage.m_tblScene.updateContent();
}

//==============================================================================
// SentinelViewerComponent :modification des sliders
//==============================================================================
void SentinelViewerComponent::sliderValueChanged(juce::Slider* slider)
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	int mode = m_ViewPage.m_cbxMode.getSelectedId() - 1;
	double value = slider->getValue();
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		if (mode == XSentinelScene::ViewMode::NDVI)
			scene->CutNDVI((float)value);
		if (mode == XSentinelScene::ViewMode::NDWI)
			scene->CutNDWI((float)value);
	}
	if ((mode != XSentinelScene::ViewMode::NDVI) && (mode != XSentinelScene::ViewMode::NDWI)) {
		XBaseImage::Boost_Hi = value * 0.01;
	}
	sendActionMessage("UpdateRaster");
	m_ViewPage.m_tblScene.updateContent();
}

//==============================================================================
// SentinelViewerComponent : Active les modes de visualisation en fonction de la resolution
//==============================================================================
void SentinelViewerComponent::EnableViewMode(int resol)
{
	if (resol == 0)
		resol = m_ViewPage.m_cbxResol.getSelectedId();
	if (resol == 10) {
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::SWIR + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::URBAN + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B01 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B05 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B06 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B07 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B09 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B11 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B12 + 1, false);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::SCL + 1, false);
	}
	if ((resol == 20) || (resol == 60)) {
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::SWIR + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::URBAN + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B01 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B05 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B06 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B07 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B11 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B12 + 1, true);
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::SCL + 1, true);
	}
	if (resol == 60)
		m_ViewPage.m_cbxMode.setItemEnabled(XSentinelScene::B09 + 1, true);
}

//==============================================================================
// SentinelViewerComponent : Import de scenes Sentinel
//==============================================================================
void SentinelViewerComponent::ImportScenes()
{
	juce::String foldername = AppUtil::OpenFolder("Sentinel", juce::translate("Sentinel Folder"));
	if (foldername.isEmpty())
		return;
	//foldername = "\\\\?\\" + foldername;
	int resol = m_ViewPage.m_cbxResol.getSelectedId();
	int mode = m_ViewPage.m_cbxMode.getSelectedId() - 1;	// Les ID commencent a 1

	SentinelImportTask task;
	task.SetImportParam(m_Base, foldername, m_ImportPage.m_btn10m.getToggleState(), m_ImportPage.m_btn20m.getToggleState(), m_ImportPage.m_btn60m.getToggleState());
	task.SetActiveParam(resol, (XSentinelScene::ViewMode)mode);
	task.runThread();

	SetProjection(task.Projection());
	m_ViewPage.m_tblScene.updateContent();
	m_ViewPage.m_tblScene.repaint();
	sendActionMessage("UpdatePreferences");
}

//==============================================================================
// SentinelViewerComponent : Import d'une resolution
//==============================================================================
void SentinelViewerComponent::SetProjection(std::string projection)
{
	XGeoProjection::XProjCode projCode = Lambert93;
	if (projection == "T30") projCode = ETRS89TM30;
	if (projection == "T31") projCode = ETRS89TM31;
	if (projection == "T32") projCode = ETRS89TM32;
	if (projection == "T20") projCode = RGAF09;
	if (projection == "T21") projCode = RGSPM06;
	if (projection == "T22") projCode = RGFG95;
	if (projection == "T38") projCode = RGM04;
	if (projection == "T40") projCode = RGR92;
	XGeoPref pref;
	pref.Projection(projCode);
	pref.ProjecView(projCode);
}

//==============================================================================
// SentinelViewerComponent : Reponse aux actions
//==============================================================================
void SentinelViewerComponent::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateImageSelectability") {
		m_ViewPage.m_tblScene.repaint();
		return;
	}
	if (message == "UpdateImageVisibility") {
		m_ViewPage.m_tblScene.repaint();
		m_ViewPage.m_imcQuickView.setImage(m_ViewPage.m_mdlScene.m_QuickView);
		m_ViewPage.m_imcQuickView.repaint();
		sendActionMessage("UpdateRaster");
		return;
	}
	if (message == "UpdateExtractTable") {
		m_AnalyzePage.m_tblExtract.repaint();
		m_AnalyzePage.m_DrawAnalyze.repaint();
		return;
	}

	juce::StringArray T;
	T.addTokens(message, ":", "");

	if (T[0] == "ShowDate") {
		if (T.size() < 2)
			return;
		m_AnalyzePage.m_DrawAnalyze.SetDate(T[1].getLargeIntValue());
		m_AnalyzePage.m_DrawAnalyze.repaint();
		return;
	}
	if (T[0] == "ShowTime") {
		if (T.size() < 2)
			return;
		juce::int64 time = T[1].getLargeIntValue();
		for (size_t i = 0; i < m_AnalyzePage.m_AnalyzeResult.size() - 1; i++) {
			if ((time >= m_AnalyzePage.m_AnalyzeResult[i].Time.toMilliseconds()) && (time < m_AnalyzePage.m_AnalyzeResult[i + 1].Time.toMilliseconds())) {
				m_AnalyzePage.m_tblExtract.scrollToEnsureColumnIsOnscreen(i + 2);
				m_AnalyzePage.m_tblExtract.scrollToEnsureColumnIsOnscreen(i+1);
				break;
			}
		}
		return;
	}
}

//==============================================================================
// SentinelViewerComponent : Fixe le point cible
//==============================================================================
void SentinelViewerComponent::SetTarget(const double& X, const double& Y, const double& /*Z*/)
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	m_dX0 = X;
	m_dY0 = Y;
	
	GeoSentinelImage* activeScene = nullptr;
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		if (!scene->Visible())
			continue;
		if (scene->Frame().IsIn(XPt2D(X, Y))) {
			activeScene = scene;
			break;
		}
	}
	if (activeScene == nullptr)
		return;
}

//==============================================================================
// SentinelViewerComponent : Analyse de la zone
//==============================================================================
void SentinelViewerComponent::Analyze()
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	m_AnalyzePage.m_tblExtract.getHeader().removeAllColumns();
	SentinelAnalyzeTask task;
	task.m_Map = map;
	task.m_P = XPt2D(m_dX0, m_dY0);
	task.runThread();
	m_AnalyzePage.m_AnalyzeResult = task.m_Result;
	m_AnalyzePage.m_DrawAnalyze.repaint();

	for (int i = 0; i < (int)m_AnalyzePage.m_AnalyzeResult.size(); i++)
		m_AnalyzePage.m_tblExtract.getHeader().addColumn(m_AnalyzePage.m_AnalyzeResult[i].Scene->Date(), i + 1, 200);	// ID doit etre different de 0
}

//==============================================================================
// SentinelSceneModel : Nombre de lignes de la table
//==============================================================================
int SentinelSceneModel::getNumRows()
{
	if (m_Base == nullptr)
		return 0;
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return 0;
	std::vector<XGeoClass*> T;
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		T.push_back(scene->Class());
	}
	std::sort(T.begin(), T.end());
	std::vector<XGeoClass*>::iterator last = std::unique(T.begin(), T.end());
	T.erase(last, T.end());
	return (int)T.size();
}

//==============================================================================
// SentinelSceneModel : Dessin du fond
//==============================================================================
void SentinelSceneModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// SentinelSceneModel : Dessin des cellules
//==============================================================================
void SentinelSceneModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	juce::String date;
	XGeoClass* geoLayer = FindScene(rowNumber, date);
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
	case Column::NbScene: // Nombre de scenes
		g.drawText(juce::String(geoLayer->NbVector()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Date:// Date
		g.drawText(date, 0, 0, width, height, juce::Justification::centred);
		break;
	}
}

//==============================================================================
// SentinelSceneModel : Clic dans une cellule
//==============================================================================
void SentinelSceneModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	juce::String date;
	XGeoClass* geoLayer = FindScene(rowNumber, date);
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
		sendActionMessage("UpdateImageVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		geoLayer->Selectable(!geoLayer->Selectable());
		sendActionMessage("UpdateImageSelectability");
		return;
	}

	// Choix d'une date
	if (columnId == Column::Date) {
		auto dateSelector = std::make_unique<juce::ComboBox>();
		dateSelector->setSize(200, 30);
		juce::StringArray T;
		for (uint32_t i = 0; i < geoLayer->NbVector(); i++) {
			GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(geoLayer->Vector(i));
			if (scene == nullptr)
				continue;
			T.add(scene->Date());
		}
		if (T.size() < 1) return;
		T.sort(true);
		dateSelector->addItemList(T, 1);
		dateSelector->setText(date, juce::NotificationType::dontSendNotification);
		dateSelector->addListener(this);
		juce::CallOutBox::launchAsynchronously(std::move(dateSelector), bounds, nullptr);
		return;
	}
}

//==============================================================================
// SentinelSceneModel : DoubleClic dans une cellule
//==============================================================================
void SentinelSceneModel::cellDoubleClicked(int /*rowNumber*/, int columnId, const juce::MouseEvent& /*event*/)
{
	XGeoClass* geoLayer = nullptr; // FindVectorClass(rowNumber);
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
// SentinelSceneModel : Trouve une scene et la date active
//==============================================================================
XGeoClass* SentinelSceneModel::FindScene(int number, juce::String& date)
{
	if (m_Base == nullptr)
		return nullptr;
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return nullptr;
	std::vector<XGeoClass*> T;
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		T.push_back(scene->Class());
	}
	std::sort(T.begin(), T.end());
	std::vector<XGeoClass*>::iterator last = std::unique(T.begin(), T.end());
	T.erase(last, T.end());
	if (T.size() <= number)
		return nullptr;
	XGeoClass* C = T[number];
	for (uint32_t i = 0; i < C->NbVector(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(C->Vector(i));
		if (scene == nullptr)
			continue;
		if (scene->Visible()) {
			date = scene->Date();
			break;
		}
	}

	return T[number];
}

//==============================================================================
// SentinelSceneModel : reponse aux changements de combo box
//==============================================================================
void SentinelSceneModel::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	juce::String date;
	XGeoClass* geoLayer = FindScene(m_ActiveRow, date);
	if (geoLayer == nullptr)
		return;

	std::string newDate = comboBoxThatHasChanged->getText().toStdString();
	for (uint32_t i = 0; i < geoLayer->NbVector(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(geoLayer->Vector(i));
		if (scene == nullptr)
			continue;

		if (newDate == scene->Date()) {
			scene->Visible(true);
			m_QuickView = scene->GetQuickView();
		}
		else
			scene->Visible(false);
	}
	sendActionMessage("UpdateImageVisibility");
}

//==============================================================================
// SentinelSceneModel : Dessin du fond
//==============================================================================
void SentinelExtractModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// SentinelSceneModel : Dessin des cellules
//==============================================================================
void SentinelExtractModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if ((rowNumber != 0) || (m_Result == nullptr))
		return;
	int index = columnId - 1;	// Les ID de colonnes commencent à 1
	if ((index < 0)||(index >= m_Result->size()))
		return;
	GeoSentinelImage* scene = (*m_Result)[index].Scene;
	if (scene == nullptr)
		return;
	g.drawImageAt(juce::ImageCache::getFromHashCode((juce::int64)scene), 0, 0);
	if ((*m_Result)[index].State == 0)
		return;
	if ((*m_Result)[index].State == 1) {
		g.setColour(juce::Colours::orange);
		g.drawRect(0, 0, width, height, 3);
		return;
	}
	if ((*m_Result)[index].State == 2) {
		g.setColour(juce::Colours::red);
		g.drawLine(0.f, 0.f, (float)width, (float)height, 3.f);
		g.drawLine(0.f, (float)height, (float)width, 0.f, 3.f);
		return;
	}
}

//==============================================================================
// SentinelSceneModel : Clic dans une cellule
//==============================================================================
void SentinelExtractModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	if (!event.mods.isRightButtonDown())
		return;
	if ((rowNumber != 0) || (m_Result == nullptr))
		return;
	int index = columnId - 1;	// Les ID de colonnes commencent à 1
	if ((index < 0) || (index >= m_Result->size()))
		return;
	(*m_Result)[index].State = ((*m_Result)[index].State + 1) % 3;	// 3 etats possibles
	sendActionMessage("UpdateExtractTable");
}

//==============================================================================
// SentinelSceneModel : DoubleClic dans une cellule
//==============================================================================
void SentinelExtractModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
{
	if ((rowNumber != 0) || (m_Result == nullptr))
		return;
	int index = columnId - 1;	// Les ID de colonnes commencent à 1
	if ((index < 0) || (index >= m_Result->size()))
		return;
	sendActionMessage("ShowDate:" + juce::String((*m_Result)[index].Time.toMilliseconds()));
}

//==============================================================================
// SentinelImportTask : Import d'une resolution
//==============================================================================
void SentinelImportTask::ImportResol(GeoSentinelImage* scene, juce::File* folder, int resol)
{
	juce::Array<juce::File> T;
	T = folder->findChildFiles(juce::File::findFiles, false, "*.jp2");
	T.addArray(folder->findChildFiles(juce::File::findFiles, false, "*.tif"));
	T.addArray(folder->findChildFiles(juce::File::findFiles, false, "*.cog"));
	for (int j = 0; j < T.size(); j++)
		scene->ImportImage(T[j].getFullPathName().toStdString(), T[j].getFileName().toStdString());
	scene->SetActiveResolution(resol);
}

//==============================================================================
// SentinelImportTask : import d'une arborescence
//==============================================================================
void SentinelImportTask::run()
{
	if (m_Base == nullptr)	// Les parametres doivent etre definis au prealable !
		return;
	juce::File folder(m_strFoldername);
	juce::Array<juce::File> T = folder.findChildFiles(juce::File::findDirectories, true, "IMG_DATA");
	for (int i = 0; i < T.size(); i++) {
		if (threadShouldExit())
			break;
		setProgress(i / (double)T.size());
		GeoSentinelImage* scene = new GeoSentinelImage;
		if (m_b10m) {
			juce::File folder10m = T[i].getChildFile("R10m");
			if (folder10m.exists())
				ImportResol(scene, &folder10m, 10);
		}
		if (m_b20m) {
			juce::File folder20m = T[i].getChildFile("R20m");
			if (folder20m.exists())
				ImportResol(scene, &folder20m, 20);
		}
		if (m_b60m) {
			juce::File folder60m = T[i].getChildFile("R60m");
			if (folder60m.exists())
				ImportResol(scene, &folder60m, 60);
		}

		if (scene->NbImages() > 0) {
			scene->ComputeFrame();
			scene->SetViewMode(m_ViewMode);
			scene->SetActiveResolution(m_nActiveResolution);
			scene->Visible(false);
			if (!GeoTools::RegisterObject(m_Base, scene, "*SENTINEL*", "Raster", scene->Name()))
				delete scene;
			else
				m_strProjection = scene->Projection();
		}
		else
			delete scene;
	}
}

//==============================================================================
// SentinelAnalyzeTask : analyse d'une zone
//==============================================================================
void SentinelAnalyzeTask::run()
{
	if (m_Map == nullptr)
		return;
	juce::ImageCache::releaseUnusedImages();
	juce::ImageCache::setCacheTimeout(60000000);
	m_Result.clear();

	for (uint32_t i = 0; i < m_Map->NbObject(); i++) {
		if (threadShouldExit())
			break;
		setProgress(i / (double)m_Map->NbObject());
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(m_Map->Object(i));
		if (scene == nullptr)
			continue;
		if (!scene->Frame().IsIn(m_P))
			continue;

		int U0, V0, win, hin, R0, S0, wout, hout, nbBand;
		double gsd = scene->GetActiveResolution();
		double X0 = XRint(m_P.X / gsd) * gsd;
		double Y0 = XRint(m_P.Y / gsd) * gsd;
		XFrame F(X0 - gsd * 100, Y0 - gsd * 100, X0 + gsd * 100, Y0 + gsd * 100);
		if (!scene->PrepareRasterDraw(&F, gsd, U0, V0, win, hin, nbBand, R0, S0, wout, hout))
			continue;

		int factor = win / wout;
		if (factor < 1)
			factor = 1;
		int wtmp = win / factor, htmp = hin / factor;
		if ((wtmp == 0) || (htmp == 0))
			continue;
		juce::Image::PixelFormat format = juce::Image::PixelFormat::RGB;
		juce::Image tmpImage(format, wtmp, htmp, true);
		{ // Necessaire pour que bitmap soit detruit avant l'appel a drawImageAt
			juce::Image::BitmapData bitmap(tmpImage, juce::Image::BitmapData::readWrite);
			format = bitmap.pixelFormat;	// Sur Mac, on obtient toujours ARGB meme en demandant RGB !

			if (factor == 1)
				scene->GetArea(U0, V0, win, hin, bitmap.data);
			else
				scene->GetZoomArea(U0, V0, win, hin, bitmap.data, factor);

			if (format == juce::Image::PixelFormat::RGB) {
				if (nbBand == 1)
					XBaseImage::Gray2RGB(bitmap.data, wtmp * htmp);
				else
					XBaseImage::SwitchRGB2BGR(bitmap.data, wtmp * htmp);
				XBaseImage::OffsetArea(bitmap.data, wtmp * 3, bitmap.height, bitmap.lineStride);
			}
			else {
				if (nbBand == 1)
					XBaseImage::Gray2RGBA(bitmap.data, wtmp * htmp);
				else
					XBaseImage::RGB2BGRA(bitmap.data, wtmp * htmp);
				XBaseImage::OffsetArea(bitmap.data, wtmp * 4, bitmap.height, bitmap.lineStride);
			}
		}
		juce::ImageCache::addImageToCache(tmpImage, (juce::int64)scene);
		Result R;
		R.Scene = scene;
		R.Index = scene->GetIndex(X0, Y0, XSentinelScene::ViewMode::NDVI);
		if (R.Index < (-0.99)) R.State = 1;
		juce::String date = scene->Date();
		date = date.substring(0, 4) + "-" + date.substring(4, 6) + "-" + date.substring(6, 8);
		R.Time = juce::Time::fromISO8601(date);
		m_Result.push_back(R);
	}
	std::sort(m_Result.begin(), m_Result.end(), predDate);
}

//==============================================================================
// SentinelAnalyzeDraw : dessin d'une analyse
//==============================================================================
void SentinelAnalyzeDraw::paint(juce::Graphics& g)
{
	if (m_Result == nullptr)
		return;
	g.setColour(juce::Colours::floralwhite);
	g.fillRect(0, 0, getWidth(), getHeight());
	double X0 = 5., Y0 = 5.;
	double W = getWidth() - 2 * X0;
	double H = getHeight() - 2 * Y0;
	g.setColour(juce::Colours::lightgrey);
	g.drawLine((float)X0, (float)(Y0 + H * 0.5), (float)W, (float)(Y0 + H * 0.5), 3.f);
	for (int y = -10; y <= 10; y += 2) {
		g.drawLine((float)X0, (float)(Y0 + H * 0.5 - y * H * 0.05), (float)W, (float)(Y0 + H * 0.5 - y * H * 0.05), 1.f);
	}

	if (m_Result->size() < 2)
		return;
	juce::int64 deltaT = (*m_Result)[m_Result->size() - 1].Time.toMilliseconds() - (*m_Result)[0].Time.toMilliseconds();
	if (deltaT <= 0)
		return;

	if (m_ShowDate > 0) {
		g.setColour(juce::Colours::lightpink);
		juce::int64 T = m_ShowDate - (*m_Result)[0].Time.toMilliseconds();
		float x = (float)(X0 + (T * W) / deltaT);
		g.drawLine(x, Y0, x, Y0 + H, 3.f);
	}

	float lastX = 0.f, lastY = 0.f;
	for (size_t i = 0; i < m_Result->size(); i++) {
		juce::int64 T = (*m_Result)[i].Time.toMilliseconds() - (*m_Result)[0].Time.toMilliseconds();
		float x = (float)(X0 + (T * W) / deltaT);
		float y = (float)(Y0 + H * 0.5 - (*m_Result)[i].Index * (H * 0.5));
		int state = (*m_Result)[i].State;

		if ((lastX > 0.f) && (lastY > 0.f) && (state == 0)) {
			g.setColour(juce::Colours::grey);
			g.drawLine(lastX, lastY, x, y, 1.f);
		}

		switch ((*m_Result)[i].State) {
		case 1: g.setColour(juce::Colours::orange); break;
		case 2: g.setColour(juce::Colours::red); break;
		default : g.setColour(juce::Colours::green);
		}
		g.drawEllipse(x - 3.f, y - 3.f, 6.f, 6.f, 3.f);
		if (state == 0) {
			lastX = x;
			lastY = y;
		}
	}
}

//==============================================================================
// SentinelAnalyzeDraw : double-clic dans le dessin
//==============================================================================
void SentinelAnalyzeDraw::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (m_Result == nullptr)
		return;
	if (m_Result->size() < 2)
		return;
	double X0 = 5.;
	double W = getWidth() - 2 * X0;
	
	juce::int64 deltaT = (*m_Result)[m_Result->size() - 1].Time.toMilliseconds() - (*m_Result)[0].Time.toMilliseconds();
	if (deltaT <= 0)
		return;
	SetDate(juce::int64(((event.position.getX() - X0) * deltaT) / W) + (*m_Result)[0].Time.toMilliseconds());
	sendActionMessage("ShowTime:" + juce::String(m_ShowDate));
	repaint();
}
