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
#include "AppUtil.h"
#include "GeoBase.h"
#include "../XTool/XGeoBase.h"
#include "../XTool/XGeoMap.h"
#include "../../XToolGeod/XGeoPref.h"

//==============================================================================
// SentinelViewerComponent : constructeur
//==============================================================================
SentinelViewerComponent::SentinelViewerComponent()
{
	m_Base = nullptr;
	// Options d'import
	m_grpImport.setText(juce::translate("Import"));
	addAndMakeVisible(m_grpImport);
	m_btnImport.setButtonText(juce::translate("Import"));
	m_btnImport.addListener(this);
	addAndMakeVisible(m_btnImport);
	m_btn10m.setButtonText("10 m");
	m_btn10m.setToggleState(true, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_btn10m);
	m_btn20m.setButtonText("20 m");
	addAndMakeVisible(m_btn20m);
	m_btn60m.setButtonText("60 m");
	addAndMakeVisible(m_btn60m);

	// Mode de visualisation RGB, IRC, NDVI, NDWI, SWIR, URBAN
	m_cbxMode.addItem("RGB", XSentinelScene::RGB + 1);	// ID = 0 n'est pas permis
	m_cbxMode.addItem("IRC", XSentinelScene::IRC + 1);
	m_cbxMode.addItem("NDVI", XSentinelScene::NDVI + 1);
	m_cbxMode.addItem("NDWI", XSentinelScene::NDWI + 1);
	m_cbxMode.addItem("SWIR", XSentinelScene::SWIR + 1);
	m_cbxMode.addItem("URBAN", XSentinelScene::URBAN + 1);
	m_cbxMode.addSeparator();
	m_cbxMode.addItem("AOT", XSentinelScene::AOT + 1);
	m_cbxMode.addItem("B01", XSentinelScene::B01 + 1);
	m_cbxMode.addItem("B02", XSentinelScene::B02 + 1);
	m_cbxMode.addItem("B03", XSentinelScene::B03 + 1);
	m_cbxMode.addItem("B04", XSentinelScene::B04 + 1);
	m_cbxMode.addItem("B05", XSentinelScene::B05 + 1);
	m_cbxMode.addItem("B06", XSentinelScene::B06 + 1);
	m_cbxMode.addItem("B07", XSentinelScene::B07 + 1);
	m_cbxMode.addItem("B08/B8A", XSentinelScene::B8A + 1);
	m_cbxMode.addItem("B09", XSentinelScene::B09 + 1);
	m_cbxMode.addItem("B11", XSentinelScene::B11 + 1);
	m_cbxMode.addItem("B12", XSentinelScene::B12 + 1);
	m_cbxMode.addItem("SCL", XSentinelScene::SCL + 1);
	m_cbxMode.addItem("TCI", XSentinelScene::TCI + 1);
	m_cbxMode.addItem("WVP", XSentinelScene::WVP + 1);

	m_cbxMode.setSelectedId(XSentinelScene::RGB + 1);
	m_cbxMode.addListener(this);
	addAndMakeVisible(m_cbxMode);
	// Resolution de visualisation
	m_cbxResol.addItem("10 m", 10);
	m_cbxResol.addItem("20 m", 20);
	m_cbxResol.addItem("60 m", 60);
	m_cbxResol.setSelectedId(10);
	m_cbxResol.addListener(this);
	addAndMakeVisible(m_cbxResol);
	EnableViewMode(10);

	// Slider de parametrage
	m_lblParam.setText("Boost :", juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblParam);
	m_sldParam.setSliderStyle(juce::Slider::LinearBar);
	m_sldParam.setRange(0., 100., 0.1);
	m_sldParam.setValue(0., juce::NotificationType::dontSendNotification);
	m_sldParam.addListener(this);
	addAndMakeVisible(m_sldParam);

	// Bordure
	m_tblScene.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_tblScene.setOutlineThickness(1);
	// Ajout des colonnes
	m_tblScene.getHeader().addColumn(juce::translate(" "), SentinelSceneModel::Column::Visibility, 25);
	m_tblScene.getHeader().addColumn(juce::translate(" "), SentinelSceneModel::Column::Selectable, 25);
	m_tblScene.getHeader().addColumn(juce::translate("Name"), SentinelSceneModel::Column::Name, 200);
	m_tblScene.getHeader().addColumn(juce::translate("Date"), SentinelSceneModel::Column::Date, 100);
	m_tblScene.setModel(&m_mdlScene);
	addAndMakeVisible(m_tblScene);
	m_mdlScene.addActionListener(this);

	setSize(400, 400);
}

//==============================================================================
// SentinelViewerComponent : Redimensionnement du composant
//==============================================================================
void SentinelViewerComponent::resized()
{
	auto b = getLocalBounds();
	m_grpImport.setBounds(5, 5, b.getWidth() - 10, 110);
	m_btn10m.setBounds(10, 20, 90, 25);
	m_btn20m.setBounds(10, 50, 90, 25);
	m_btn60m.setBounds(10, 80, 90, 25);
	m_btnImport.setBounds(120, 50, 90, 25);

	m_cbxMode.setBounds(10, 130, 90, 25);
	m_cbxResol.setBounds(120, 130, 90, 25);
	m_lblParam.setBounds(240, 130, 60, 25);
	m_sldParam.setBounds(300, 130, 80, 25);

	m_tblScene.setBounds(10, 160, getWidth() - 20, 200);
}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void SentinelViewerComponent::buttonClicked(juce::Button* button)
{
	if (button == &m_btnImport)
		return ImportScenes();
}

//==============================================================================
// Reponses aux combo box
//==============================================================================
void SentinelViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	int resol = m_cbxResol.getSelectedId();
	int mode = m_cbxMode.getSelectedId() - 1;	// ID commence a 1 ...
	EnableViewMode(resol);
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		if (comboBoxThatHasChanged == &m_cbxResol) {
			scene->SetActiveResolution(resol);
		}
		if (comboBoxThatHasChanged == &m_cbxMode) {
			scene->SetViewMode((XSentinelScene::ViewMode)mode);
			if ((mode == XSentinelScene::ViewMode::NDVI) || (mode == XSentinelScene::ViewMode::NDWI)) {
				m_sldParam.setRange(-1., 1., 0.01);
				m_sldParam.setValue(scene->CutNDVI(), juce::NotificationType::dontSendNotification);
				if (mode == XSentinelScene::ViewMode::NDWI)
					m_sldParam.setValue(scene->CutNDWI(), juce::NotificationType::dontSendNotification);
				m_lblParam.setText("Cut :", juce::NotificationType::dontSendNotification);
			}
			else {
				m_sldParam.setRange(0, 100, 0.1);
				m_lblParam.setText("Boost :", juce::NotificationType::dontSendNotification);
			}
		}
	}
	sendActionMessage("UpdateRaster");
	m_tblScene.updateContent();
}

//==============================================================================
// SentinelViewerComponent :modification des sliders
//==============================================================================
void SentinelViewerComponent::sliderValueChanged(juce::Slider* slider)
{
	XGeoMap* map = m_Base->Map("*SENTINEL*");
	if (map == nullptr)
		return;
	int mode = m_cbxMode.getSelectedId() - 1;
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
		XBaseImage::MinValue = value * 32;
		XBaseImage::MaxValue = 256*256 - value * 32;
	}
	sendActionMessage("UpdateRaster");
	m_tblScene.updateContent();
}

//==============================================================================
// SentinelViewerComponent : Active les modes de visualisation en fonction de la resolution
//==============================================================================
void SentinelViewerComponent::EnableViewMode(int resol)
{
	if (resol == 0)
		resol = m_cbxResol.getSelectedId();
	if (resol == 10) {
		m_cbxMode.setItemEnabled(XSentinelScene::SWIR + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::URBAN + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B01 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B05 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B06 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B07 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B09 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B11 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::B12 + 1, false);
		m_cbxMode.setItemEnabled(XSentinelScene::SCL + 1, false);
	}
	if ((resol == 20) || (resol == 60)) {
		m_cbxMode.setItemEnabled(XSentinelScene::SWIR + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::URBAN + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B01 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B05 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B06 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B07 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B11 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::B12 + 1, true);
		m_cbxMode.setItemEnabled(XSentinelScene::SCL + 1, true);
	}
	if (resol == 60)
		m_cbxMode.setItemEnabled(XSentinelScene::B09 + 1, true);
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
	int resol = m_cbxResol.getSelectedId();
	int mode = m_cbxMode.getSelectedId() - 1;	// Les ID commencent a 1
	juce::File folder(foldername);
	juce::Array<juce::File> T = folder.findChildFiles(juce::File::findDirectories, true, "IMG_DATA");
	for (int i = 0; i < T.size(); i++) {
		GeoSentinelImage* scene = new GeoSentinelImage;
		if (m_btn10m.getToggleState()) {
			juce::File folder10m = T[i].getChildFile("R10m");
			if (folder10m.exists())
				ImportResol(scene, &folder10m, 10);
		}
		if (m_btn20m.getToggleState()) {
			juce::File folder20m = T[i].getChildFile("R20m");
			if (folder20m.exists())
				ImportResol(scene, &folder20m, 20);
		}
		if (m_btn60m.getToggleState()) {
			juce::File folder60m = T[i].getChildFile("R60m");
			if (folder60m.exists())
				ImportResol(scene, &folder60m, 60);
		}

		if (scene->NbImages() > 0) {
			scene->ComputeFrame();
			scene->SetViewMode((XSentinelScene::ViewMode)mode);
			scene->SetActiveResolution(resol);
			scene->Visible(false);
			if (!GeoTools::RegisterObject(m_Base, scene, "*SENTINEL*", "Raster", scene->Name()))
				delete scene;
			else
				SetProjection(scene->Projection());
		}
		else
			delete scene;
	}
	m_tblScene.updateContent();
	sendActionMessage("UpdatePreferences");
}

//==============================================================================
// SentinelViewerComponent : Import d'une resolution
//==============================================================================
void SentinelViewerComponent::ImportResol(GeoSentinelImage* scene, juce::File* folder, int resol)
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
		m_tblScene.repaint();
		return;
	}
	if (message == "UpdateImageVisibility") {
		m_tblScene.repaint();
		sendActionMessage("UpdateRaster");
		return;
	}
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
	for (int i = 0; i < map->NbObject(); i++) {
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


	// Choix d'une epaisseur
	if (columnId == Column::Date) {
		auto dateSelector = std::make_unique<juce::ComboBox>();
		dateSelector->setSize(200, 30);
		for (int i = 0; i < geoLayer->NbVector(); i++) {
			GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(geoLayer->Vector(i));
			if (scene == nullptr)
				continue;
			dateSelector->addItem(scene->Date(), i + 1);
		}
		dateSelector->setText(date, juce::NotificationType::dontSendNotification);
		dateSelector->addListener(this);
		juce::CallOutBox::launchAsynchronously(std::move(dateSelector), bounds, nullptr);
		return;
	}

	
}

//==============================================================================
// SentinelSceneModel : DoubleClic dans une cellule
//==============================================================================
void SentinelSceneModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
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
	for (int i = 0; i < map->NbObject(); i++) {
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
	for (int i = 0; i < C->NbVector(); i++) {
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

		if (newDate == scene->Date())
			scene->Visible(true);
		else
			scene->Visible(false);
	}
	sendActionMessage("UpdateImageVisibility");
}