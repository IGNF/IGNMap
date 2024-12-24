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
	m_cbxMode.addItem("RGB", XSentinelScene::RGB);
	m_cbxMode.addItem("IRC", XSentinelScene::IRC);
	m_cbxMode.addItem("NDVI", XSentinelScene::NDVI);
	m_cbxMode.addItem("NDWI", XSentinelScene::NDWI);
	m_cbxMode.addItem("SWIR", XSentinelScene::SWIR);
	m_cbxMode.addItem("URBAN", XSentinelScene::URBAN);
	m_cbxMode.setSelectedId(XSentinelScene::RGB);
	m_cbxMode.addListener(this);
	addAndMakeVisible(m_cbxMode);
	// Resolution de visualisation
	m_cbxResol.addItem("10 m", 10);
	m_cbxResol.addItem("20 m", 20);
	m_cbxResol.addItem("60 m", 60);
	m_cbxResol.setSelectedId(10);
	m_cbxResol.addListener(this);
	addAndMakeVisible(m_cbxResol);
	// Date
	m_cbxDate.addListener(this);
	addAndMakeVisible(m_cbxDate);

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
	m_cbxDate.setBounds(220, 130, 90, 25);
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
	int mode = m_cbxMode.getSelectedId();
	std::string date = m_cbxDate.getText().toStdString();
	for (uint32_t i = 0; i < map->NbObject(); i++) {
		GeoSentinelImage* scene = dynamic_cast<GeoSentinelImage*>(map->Object(i));
		if (scene == nullptr)
			continue;
		if (comboBoxThatHasChanged == &m_cbxResol) {
			scene->SetActiveResolution(resol);
		}
		if (comboBoxThatHasChanged == &m_cbxMode) {
			scene->SetViewMode((XSentinelScene::ViewMode)mode);
		}
		if (comboBoxThatHasChanged == &m_cbxDate) {
			if (date == scene->Date())
				scene->Visible(true);
			else
				scene->Visible(false);
		}
	}
	sendActionMessage("UpdateRaster");
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
	int mode = m_cbxMode.getSelectedId();
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
				m_cbxDate.addItem(scene->Date(), m_cbxDate.getNumItems() + 1);
		}
		else
			delete scene;
	}
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