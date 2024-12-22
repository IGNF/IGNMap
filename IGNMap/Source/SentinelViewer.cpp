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

//==============================================================================
// SentinelViewerComponent : constructeur
//==============================================================================
SentinelViewerComponent::SentinelViewerComponent()
{
	m_Base = nullptr;
	m_btnImport.setButtonText(juce::translate("Import"));
	m_btnImport.addListener(this);
	addAndMakeVisible(m_btnImport);
}

//==============================================================================
// SentinelViewerComponent : Redimensionnement du composant
//==============================================================================
void SentinelViewerComponent::resized()
{
	auto b = getLocalBounds();
	m_btnImport.setBounds(5, 5, 90, 25);
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
// SentinelViewerComponent : Import de scenes Sentinel
//==============================================================================
void SentinelViewerComponent::ImportScenes()
{
	juce::String foldername = AppUtil::OpenFolder("Sentinel", juce::translate("Sentinel Folder"));
	if (foldername.isEmpty())
		return;
	//foldername = "\\\\?\\" + foldername;
	juce::File folder(foldername);
	juce::Array<juce::File> T = folder.findChildFiles(juce::File::findDirectories, true, "IMG_DATA");
	for (int i = 0; i < T.size(); i++) {
		GeoSentinelImage* scene = new GeoSentinelImage;
		juce::File folder10m = T[i].getChildFile("R10m");
		if (folder10m.exists())
			ImportResol(scene, &folder10m);
		juce::File folder20m = T[i].getChildFile("R20m");
		if (folder20m.exists())
			ImportResol(scene, &folder20m);
		juce::File folder60m = T[i].getChildFile("R60m");
		if (folder60m.exists())
			ImportResol(scene, &folder60m);

		if (scene->NbImages() > 0) {
			scene->ComputeFrame();
			if (!GeoTools::RegisterObject(m_Base, scene, "SENTINEL", "Raster", scene->Name())) {
				delete scene;
			}
		}
		else
			delete scene;
	}
	sendActionMessage("UpdatePreferences");
}

//==============================================================================
// SentinelViewerComponent : Import d'une resolution
//==============================================================================
void SentinelViewerComponent::ImportResol(GeoSentinelImage* scene, juce::File* folder)
{
	juce::Array<juce::File> T;
	T = folder->findChildFiles(juce::File::findFiles, false, "*.jp2");
	T.addArray(folder->findChildFiles(juce::File::findFiles, false, "*.tif"));
	T.addArray(folder->findChildFiles(juce::File::findFiles, false, "*.cog"));
	for (int j = 0; j < T.size(); j++)
		scene->ImportImage(T[j].getFullPathName().toStdString(), T[j].getFileName().toStdString());
}