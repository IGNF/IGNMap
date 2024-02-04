//-----------------------------------------------------------------------------
//								MainComponent.h
//								===============
//
// Composant principal de l'application
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 26/12/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "MapView.h"
#include "GeoBase.h"
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoMap.h"
#include "../../XTool/XGeoVector.h"
#include "VectorLayersViewer.h"
#include "ImageLayersViewer.h"
#include "DtmLayersViewer.h"
#include "LasLayersViewer.h"
#include "SelTreeViewer.h"
#include "ImageOptionsViewer.h"
#include "OGL3DViewer.h"
#include "MainComponentToolbarFactory.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component,
  public juce::ApplicationCommandTarget,
  public juce::MenuBarModel,
  public juce::ActionListener,
  public juce::ActionBroadcaster,
  public juce::Button::Listener,
  public juce::InterprocessConnection
{
public:
  // Liste des commandes de l'application
  enum CommandIDs
  {
    menuNew = 1, menuQuit,
    menuUndo,
    menuTranslate, menuTest,
    menuImportVectorFolder, menuImportVectorFile, menuImportImageFolder, menuImportImageFile, 
    menuImportDtmFolder, menuImportDtmFile, menuImportLasFile, menuImportLasFolder,
    menuExportImage,
    menuZoomTotal, menuZoomLevel,
    menuScale1k, menuScale10k, menuScale25k, menuScale100k, menuScale250k,
    menuShowSidePanel, menuShow3DViewer,
    menuShowVectorLayers, menuShowImageLayers, menuShowDtmLayers, menuShowLasLayers, menuShowSelection, menuShowImageOptions,
    menuAddOSM, menuAddWmtsServer,
    menuAddGeoportailOrthophoto, menuAddGeoportailOrthophotoIRC, menuAddGeoportailOrthohisto, menuAddGeoportailSatellite,
    menuAddGeoportailCartes, menuAddGeoportailPlanIGN, menuAddGeoportailParcelExpress, menuAddGeoportailSCAN50Histo,
    menuMove, menuSelect, menuZoom,
    menuSynchronize,
    menuAbout
  };

  //==============================================================================
  MainComponent();
  ~MainComponent() override;

  //==============================================================================
  void resized() override;

  // Gestion des menus
  juce::StringArray getMenuBarNames() override;
  juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& menuName) override;
  void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

  // Gestion des commandes
  ApplicationCommandTarget* getNextCommandTarget() override;
  void getAllCommands(juce::Array<juce::CommandID>& c) override;
  void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
  bool perform(const InvocationInfo& info) override;

  // Gestion des actions
  void actionListenerCallback(const juce::String& message) override;

  // Reponse aux boutons
  void buttonClicked(juce::Button*) override;

private:
  juce::ApplicationCommandManager m_CommandManager;
  std::unique_ptr<juce::MenuBarComponent> m_MenuBar;
  std::unique_ptr<MapView> m_MapView;
  std::unique_ptr<VectorLayersViewer> m_VectorViewer;
  std::unique_ptr<ImageLayersViewer> m_ImageViewer;
  std::unique_ptr<SelTreeViewer> m_SelTreeViewer;
  std::unique_ptr<ImageOptionsViewer> m_ImageOptionsViewer;
  std::unique_ptr<DtmLayersViewer> m_DtmViewer;
  std::unique_ptr<LasLayersViewer> m_LasViewer;
  std::unique_ptr<OGL3DViewer> m_OGL3DViewer;

  juce::StretchableLayoutManager m_VerticalLayout;
  std::unique_ptr<juce::StretchableLayoutResizerBar> m_VerticalDividerBar;
  std::unique_ptr<juce::ConcertinaPanel> m_Panel;
  std::unique_ptr<juce::Toolbar> m_Toolbar;
  MainComponentToolbarFactory m_ToolbarFactory;

  XGeoBase m_GeoBase;

  void Clear();
  void AboutIGNMap();
  void Translate();
  void ShowHidePanel(juce::Component* component);

  XGeoClass* ImportDataFolder(juce::String foldername, XGeoVector::eTypeVector type);

  void ImportVectorFolder();
  void ImportImageFolder();
  void ImportDtmFolder();
  void ImportLasFolder();

  bool ImportVectorFile(juce::String filename = "");
  bool ImportImageFile(juce::String rasterfile = "");
  bool ImportDtmFile(juce::String dtmfile = "");
  bool ImportLasFile(juce::String lasfile = "");

  bool ExportImage();

  bool AddOSMServer();
  bool AddWmtsServer();
  bool AddWmtsServer(std::string server, std::string layer, std::string TMS, std::string format,
                     uint32_t tileW = 256, uint32_t tileH = 256, uint32_t max_zoom = 19, std::string apikey = "");

  void Test();

  void Synchronize();
  void connectionMade() override;
  void connectionLost() override;
  void messageReceived(const juce::MemoryBlock& message) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
