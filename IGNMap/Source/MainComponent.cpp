//-----------------------------------------------------------------------------
//								MainComponent.cpp
//								=================
//
// Composant principal de l'application
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 26/12/2023
//-----------------------------------------------------------------------------

#include "MainComponent.h"
#include "AppUtil.h"
#include "OsmLayer.h"
#include "WmtsLayer.h"
#include "ExportImageDlg.h"
#include "ExportLasDlg.h"
#include "PrefDlg.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolImage/XTiffWriter.h"
#include "../../XToolAlgo/XInternetMap.h"
#include "AffineImage.h"
#include "../../XToolVector/XTAChantier.h"

//==============================================================================
MainComponent::MainComponent()
{
  m_MapView.reset(new MapView("View1"));
  addAndMakeVisible(m_MapView.get());
	m_MapView.get()->SetGeoBase(&m_GeoBase);
  m_MapView.get()->addActionListener(this);

  m_MenuBar.reset(new juce::MenuBarComponent(this));
  addAndMakeVisible(m_MenuBar.get());
  setApplicationCommandManagerToWatch(&m_CommandManager);
  m_CommandManager.registerAllCommandsForTarget(this);

	m_Toolbar.reset(new juce::Toolbar());
	addAndMakeVisible(m_Toolbar.get());
	m_ToolbarFactory.SetListener(this);
	m_Toolbar.get()->addDefaultItems(m_ToolbarFactory);

	m_VectorViewer.reset(new VectorLayersViewer);
	addAndMakeVisible(m_VectorViewer.get());
	m_VectorViewer.get()->SetBase(&m_GeoBase);
	m_VectorViewer.get()->addActionListener(this);
	addActionListener(m_VectorViewer.get());

	m_ImageViewer.reset(new ImageLayersViewer);
	addAndMakeVisible(m_ImageViewer.get());
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	m_ImageViewer.get()->addActionListener(this);
	addActionListener(m_ImageViewer.get());

	m_SelTreeViewer.reset(new SelTreeViewer);
	addAndMakeVisible(m_SelTreeViewer.get());
	m_SelTreeViewer.get()->SetBase(&m_GeoBase);
	m_SelTreeViewer.get()->addActionListener(this);

	m_ImageOptionsViewer.reset(new ImageOptionsViewer);
	addAndMakeVisible(m_ImageOptionsViewer.get());
	m_ImageOptionsViewer.get()->addActionListener(this);

	m_DtmViewer.reset(new DtmLayersViewer);
	addAndMakeVisible(m_DtmViewer.get());
	m_DtmViewer.get()->SetBase(&m_GeoBase);
	m_DtmViewer.get()->addActionListener(this);

	m_LasViewer.reset(new LasLayersViewer);
	addAndMakeVisible(m_LasViewer.get());
	m_LasViewer.get()->SetBase(&m_GeoBase);
	m_LasViewer.get()->addActionListener(this);

	m_OGL3DViewer.reset(new OGL3DViewer("3DViewer", juce::Colours::grey, juce::DocumentWindow::allButtons));
	m_OGL3DViewer.get()->setVisible(false);

  m_Panel.reset(new juce::ConcertinaPanel);
  addAndMakeVisible(m_Panel.get());
	m_Panel.get()->addPanel(-1, m_VectorViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_DtmViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_LasViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageOptionsViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_SelTreeViewer.get(), false);
	
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(0), new juce::TextButton(juce::translate("Vector Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(1), new juce::TextButton(juce::translate("Image Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(2), new juce::TextButton(juce::translate("DTM Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(3), new juce::TextButton(juce::translate("LAS Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(4), new juce::TextButton(juce::translate("Image Options")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(5), new juce::TextButton(juce::translate("Selection")), true);

  // set up the layout and resizer bars..
  m_VerticalLayout.setItemLayout(0, -0.2, -1.0, -0.65);
  m_VerticalLayout.setItemLayout(1, 8, 8, 8);           // the vertical divider drag-bar thing is always 8 pixels wide
  m_VerticalLayout.setItemLayout(2, 0, -0.6, -0.35);

  m_VerticalDividerBar.reset(new juce::StretchableLayoutResizerBar(&m_VerticalLayout, 1, true));
  addAndMakeVisible(m_VerticalDividerBar.get());

  // this lets the command manager use keypresses that arrive in our window to send out commands
  addKeyListener(m_CommandManager.getKeyMappings());
	setWantsKeyboardFocus(true);

  setSize(800, 600);

  juce::RuntimePermissions::request(juce::RuntimePermissions::readExternalStorage,
    [](bool granted)
    {
      if (!granted)
      {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
          "Permissions warning",
          "External storage access permission not granted, some files"
          " may be inaccessible.");
      }
    });

	// Geodesie ...
	XGeoPref pref;
	pref.Projection(XGeoProjection::Lambert93);
	pref.GoogleMode(3); // Streetview
	pref.VEMode(1);
	// Langue par defaut
	if (juce::SystemStats::getDisplayLanguage() == "fr-FR")
		Translate();
}

MainComponent::~MainComponent()
{
	if (isConnected()) {
		juce::String message = "Disconnect";
		juce::MemoryBlock block(message.getCharPointer(), message.length());
		sendMessage(block);
		disconnect();
	}
}


void MainComponent::resized()
{
  auto b = getLocalBounds();
	int menubarH = juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight();
	int toolbarW = 32;
  m_MenuBar.get()->setBounds(b.removeFromTop(menubarH));
	m_Toolbar.get()->setVertical(true);
	m_Toolbar.get()->setBounds(juce::Rectangle<int>(0, menubarH, toolbarW, b.getHeight() - menubarH));
  juce::Rectangle<int> R;
  R.setLeft(toolbarW);
  R.setRight(b.getRight());
  R.setTop(menubarH);
  R.setBottom(b.getBottom());
  //m_MapView->setBounds(R);

  Component* vcomps[] = { m_MapView.get(), m_VerticalDividerBar.get(), m_Panel.get() };

  m_VerticalLayout.layOutComponents(vcomps, 3,
    R.getX(), R.getY(), R.getWidth(), R.getHeight(),
    false,     // lay out side-by-side
    true);     // resize the components' heights as well as widths
}

//==============================================================================
// Nom des menus
//==============================================================================
juce::StringArray MainComponent::getMenuBarNames()
{
	return { juce::translate("File"), juce::translate("Edit"), juce::translate("Tools"), juce::translate("View"), "?" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String& /*menuName*/)
{
	juce::PopupMenu menu;

	if (menuIndex == 0)	// File
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuNew);
		juce::PopupMenu ImportSubMenu;
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportVectorFile);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportVectorFolder);
		ImportSubMenu.addSeparator();
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportImageFile);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportImageFolder);
		ImportSubMenu.addSeparator();
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportDtmFile);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportDtmFolder);
		ImportSubMenu.addSeparator();
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportLasFile);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuImportLasFolder);
		ImportSubMenu.addSeparator();
		juce::PopupMenu WmtsSubMenu;
		WmtsSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddOSM);
		juce::PopupMenu GeoportailSubMenu;
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailOrthophoto);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailOrthophotoIRC);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailOrthohisto);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailSatellite);
		GeoportailSubMenu.addSeparator();
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailCartes);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailPlanIGN);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailParcelExpress);
		GeoportailSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddGeoportailSCAN50Histo);
		WmtsSubMenu.addSubMenu(juce::translate("Geoplateforme (France)"), GeoportailSubMenu);
		WmtsSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddWmtsServer);
		ImportSubMenu.addSubMenu(juce::translate("Import WMTS / TMS server"), WmtsSubMenu);

		menu.addSubMenu(juce::translate("Import"), ImportSubMenu);

		juce::PopupMenu ExportSubMenu;
		ExportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuExportImage);
		ExportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuExportLas);
		menu.addSubMenu(juce::translate("Export"), ExportSubMenu);

		menu.addCommandItem(&m_CommandManager, CommandIDs::menuQuit);
	}
	else if (menuIndex == 1) // Edit
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuTranslate);
		//menu.addCommandItem(&m_CommandManager, CommandIDs::menuTest);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuPreferences);
	}
	else if (menuIndex == 2) // Tools
	{ 
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuSynchronize);
		menu.addItem(1000, "Test");
	}
	else if (menuIndex == 3)
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuShowSidePanel);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuShow3DViewer);
		juce::PopupMenu PanelSubMenu;
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowVectorLayers);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowImageLayers);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowDtmLayers);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowLasLayers);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowImageOptions);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowSelection);
		menu.addSubMenu(juce::translate("Panels"), PanelSubMenu);
		menu.addSeparator();
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuZoomTotal);
		juce::PopupMenu ScaleSubMenu;
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale1k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale10k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale25k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale100k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale250k);
		menu.addSubMenu(juce::translate("Scale"), ScaleSubMenu);
		menu.addSeparator();
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuGoogle);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuBing);
	}
	else if (menuIndex == 4) // Help
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuHelp);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuAbout);
	}

	return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/)
{
	if (menuItemID == 1000)
		Test();
}

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

//==============================================================================
//
//==============================================================================
void MainComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
	juce::Array<juce::CommandID> commands{ CommandIDs::menuNew,
		CommandIDs::menuQuit, CommandIDs::menuUndo, CommandIDs::menuTranslate, CommandIDs::menuPreferences,
		CommandIDs::menuImportVectorFile, CommandIDs::menuImportVectorFolder, 
		CommandIDs::menuImportImageFile, CommandIDs::menuImportImageFolder, CommandIDs::menuImportDtmFile,
		CommandIDs::menuImportDtmFolder, CommandIDs::menuImportLasFile, CommandIDs::menuImportLasFolder,
		CommandIDs::menuExportImage, CommandIDs::menuExportLas,
		CommandIDs::menuZoomTotal,
		CommandIDs::menuTest, CommandIDs::menuShowSidePanel,
		CommandIDs::menuShowVectorLayers, CommandIDs::menuShowImageLayers, CommandIDs::menuShowDtmLayers, 
		CommandIDs::menuShowLasLayers, CommandIDs::menuShowSelection, CommandIDs::menuShowImageOptions,
		CommandIDs::menuShow3DViewer, CommandIDs::menuAddOSM, CommandIDs::menuAddGeoportailOrthophoto,
		CommandIDs::menuAddGeoportailOrthohisto, CommandIDs::menuAddGeoportailSatellite, CommandIDs::menuAddGeoportailCartes,
		CommandIDs::menuAddGeoportailOrthophotoIRC, CommandIDs::menuAddGeoportailPlanIGN, CommandIDs::menuAddGeoportailParcelExpress,
		CommandIDs::menuAddGeoportailSCAN50Histo,
		CommandIDs::menuAddWmtsServer, CommandIDs::menuSynchronize,
		CommandIDs::menuScale1k, CommandIDs::menuScale10k, CommandIDs::menuScale25k, CommandIDs::menuScale100k, CommandIDs::menuScale250k,
		CommandIDs::menuGoogle, CommandIDs::menuBing, CommandIDs::menuHelp, CommandIDs::menuAbout };
	c.addArray(commands);
}

//==============================================================================
//
//==============================================================================
void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case CommandIDs::menuNew:
		result.setInfo(juce::translate("New Window"), juce::translate("Clear the content of IGNMap"), "Menu", 0);
		result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
		break;
	case CommandIDs::menuQuit:
		result.setInfo(juce::translate("Quit"), juce::translate("Quit IGNMap"), "Menu", 0);
		result.addDefaultKeypress('q', juce::ModifierKeys::ctrlModifier);
		break;
	case CommandIDs::menuTranslate:
		result.setInfo(juce::translate("Translate"), juce::translate("Load a translation file"), "Menu", 0);
		break;
	case CommandIDs::menuPreferences:
		result.setInfo(juce::translate("Preferences"), juce::translate("Application settings"), "Menu", 0);
		break;
	case CommandIDs::menuImportVectorFile:
		result.setInfo(juce::translate("Import a vector file"), juce::translate("Import a vector file"), "Menu", 0);
		break;
	case CommandIDs::menuImportVectorFolder:
		result.setInfo(juce::translate("Import a vector folder"), juce::translate("Import a vector folder"), "Menu", 0);
		break;
	case CommandIDs::menuImportImageFile:
		result.setInfo(juce::translate("Import an image file"), juce::translate("Import an image file"), "Menu", 0);
		break;
	case CommandIDs::menuImportImageFolder:
		result.setInfo(juce::translate("Import an image folder"), juce::translate("Import an image folder"), "Menu", 0);
		break;
	case CommandIDs::menuImportDtmFile:
		result.setInfo(juce::translate("Import a DTM file"), juce::translate("Import a DTM file"), "Menu", 0);
		break;
	case CommandIDs::menuImportDtmFolder:
		result.setInfo(juce::translate("Import a DTM folder"), juce::translate("Import a DTM folder"), "Menu", 0);
		break;
	case CommandIDs::menuImportLasFile:
		result.setInfo(juce::translate("Import a LAS/LAZ file"), juce::translate("Import a LAS/LAZ file"), "Menu", 0);
		break;
	case CommandIDs::menuImportLasFolder:
		result.setInfo(juce::translate("Import a LAS/LAZ folder"), juce::translate("Import a LAS/LAZ folder"), "Menu", 0);
		break;
	case CommandIDs::menuAddOSM:
		result.setInfo(juce::translate("Import OSM data"), juce::translate("Import OSM data"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailOrthophoto:
		result.setInfo(juce::translate("Orthophoto"), juce::translate("Orthophoto"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailOrthophotoIRC:
		result.setInfo(juce::translate("Orthophoto IRC"), juce::translate("Orthophoto IRC"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailOrthohisto:
		result.setInfo(juce::translate("Historic Orthophoto"), juce::translate("Historic Orthophoto"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailSatellite:
		result.setInfo(juce::translate("Satellite"), juce::translate("Satellite"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailCartes:
		result.setInfo(juce::translate("Cartography"), juce::translate("Cartography"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailPlanIGN:
		result.setInfo(juce::translate("PlanIGNV2"), juce::translate("PlanIGNV2"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailParcelExpress:
		result.setInfo(juce::translate("PCI"), juce::translate("PCI"), "Menu", 0);
		break;
	case CommandIDs::menuAddGeoportailSCAN50Histo:
		result.setInfo(juce::translate("Historic Scan50"), juce::translate("Historic Scan50"), "Menu", 0);
		break;
	case CommandIDs::menuAddWmtsServer:
		result.setInfo(juce::translate("WMTS Server"), juce::translate("WMTS Server"), "Menu", 0);
		break;
	case CommandIDs::menuExportImage:
		result.setInfo(juce::translate("Export image"), juce::translate("Export image"), "Menu", 0);
		break;
	case CommandIDs::menuExportLas:
		result.setInfo(juce::translate("Export LAS"), juce::translate("Export LAS"), "Menu", 0);
		break;
	case CommandIDs::menuZoomTotal:
		result.setInfo(juce::translate("Zoom total"), juce::translate("Zoom total"), "Menu", 0);
		break;
	case CommandIDs::menuScale1k:
		result.setInfo(juce::translate("1 : 1000"), juce::translate("1 : 1000"), "Menu", 0);
		break;
	case CommandIDs::menuScale10k:
		result.setInfo(juce::translate("1 : 10 000"), juce::translate("1 : 10 000"), "Menu", 0);
		break;
	case CommandIDs::menuScale25k:
		result.setInfo(juce::translate("1 : 25 000"), juce::translate("1 : 25 000"), "Menu", 0);
		break;
	case CommandIDs::menuScale100k:
		result.setInfo(juce::translate("1 : 100 000"), juce::translate("1 : 100 000"), "Menu", 0);
		break;
	case CommandIDs::menuScale250k:
		result.setInfo(juce::translate("1 : 250 000"), juce::translate("1 : 250 000"), "Menu", 0);
		break;
	case CommandIDs::menuShowSidePanel:
		result.setInfo(juce::translate("View Side Panel"), juce::translate("View Side Panel"), "Menu", 0);
		if (m_Panel.get() != nullptr)
			result.setTicked(m_Panel.get()->isVisible());
		break;
	case CommandIDs::menuShow3DViewer:
		result.setInfo(juce::translate("View 3D Viewer"), juce::translate("View 3D Viewer"), "Menu", 0);
		if (m_OGL3DViewer.get() != nullptr)
			result.setTicked(m_OGL3DViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowVectorLayers:
		result.setInfo(juce::translate("View Vector Layers Panel"), juce::translate("View Vector Layers Panel"), "Menu", 0);
		if (m_VectorViewer.get() != nullptr)
			result.setTicked(m_VectorViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowImageLayers:
		result.setInfo(juce::translate("View Image Layers Panel"), juce::translate("View Image Layers Panel"), "Menu", 0);
		if (m_ImageViewer.get() != nullptr)
			result.setTicked(m_ImageViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowDtmLayers:
		result.setInfo(juce::translate("View DTM Layers Panel"), juce::translate("View DTM Layers Panel"), "Menu", 0);
		if (m_DtmViewer.get() != nullptr)
			result.setTicked(m_DtmViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowLasLayers:
		result.setInfo(juce::translate("View LAS Layers Panel"), juce::translate("View LAS Layers Panel"), "Menu", 0);
		if (m_LasViewer.get() != nullptr)
			result.setTicked(m_LasViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowSelection:
		result.setInfo(juce::translate("View Selection Panel"), juce::translate("View Selection Panel"), "Menu", 0);
		if (m_SelTreeViewer.get() != nullptr)
			result.setTicked(m_SelTreeViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowImageOptions:
		result.setInfo(juce::translate("View Image Options Panel"), juce::translate("View Image Options Panel"), "Menu", 0);
		if (m_ImageOptionsViewer.get() != nullptr)
			result.setTicked(m_ImageOptionsViewer.get()->isVisible());
		break;
	case CommandIDs::menuSynchronize:
		result.setInfo(juce::translate("Synchronize"), juce::translate("Synchronize with another IGNMap"), "Menu", 0);
		break;
	case CommandIDs::menuGoogle:
		result.setInfo(juce::translate("Google Maps"), juce::translate("Google Maps"), "Menu", 0);
		break;
	case CommandIDs::menuBing:
		result.setInfo(juce::translate("Bing Maps"), juce::translate("Bing Maps"), "Menu", 0);
		break;
	case CommandIDs::menuHelp:
		result.setInfo(juce::translate("Help"), juce::translate("Help"), "Menu", 0);
		break;
	case CommandIDs::menuAbout:
		result.setInfo(juce::translate("About IGNMap"), juce::translate("About IGNMap"), "Menu", 0);
		break;
	default:
		result.setInfo("Test", "Test menu", "Menu", 0);
		break;
	}
}

//==============================================================================
// Resultat d'une commande
//==============================================================================
bool MainComponent::perform(const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case CommandIDs::menuNew:
		NewWindow();
		break;
	case CommandIDs::menuQuit:
		juce::JUCEApplication::quit();
		break;
	case CommandIDs::menuTranslate:
		Translate();
		break;
	case CommandIDs::menuPreferences:
		Preferences();
		break;
	case CommandIDs::menuTest:
		Test();
		break;
	case CommandIDs::menuImportVectorFile:
		ImportVectorFile();
		break;
	case CommandIDs::menuImportVectorFolder:
		ImportVectorFolder();
		break;
	case CommandIDs::menuImportImageFile:
		ImportImageFile();
		break;
	case CommandIDs::menuImportImageFolder:
		ImportImageFolder();
		break;
	case CommandIDs::menuImportDtmFile:
		ImportDtmFile();
		break;
	case CommandIDs::menuImportDtmFolder:
		ImportDtmFolder();
		break;
	case CommandIDs::menuImportLasFile:
		ImportLasFile();
		break;
	case CommandIDs::menuImportLasFolder:
		ImportLasFolder();
		break;
	case CommandIDs::menuAddOSM:
		AddOSMServer();
		break;
	case CommandIDs::menuAddGeoportailOrthophoto:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS", "PM", "jpeg", 256, 256, 20);
		break;
	case CommandIDs::menuAddGeoportailOrthophotoIRC:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS.IRC", "PM", "jpeg", 256, 256, 20);
		break;
	case CommandIDs::menuAddGeoportailOrthohisto:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS.1950-1965", "PM", "png", 256, 256, 18);
		break;
	case CommandIDs::menuAddGeoportailSatellite:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHO-SAT.SPOT.2021", "PM", "jpeg", 256, 256, 17);
		break;
	case CommandIDs::menuAddGeoportailCartes:
		AddWmtsServer("data.geopf.fr/private/wmts", "GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR", "PM", "jpeg", 256, 256, 16, "ign_scan_ws");
		break;
	case CommandIDs::menuAddGeoportailPlanIGN:
		AddWmtsServer("data.geopf.fr/wmts", "GEOGRAPHICALGRIDSYSTEMS.PLANIGNV2", "PM", "png", 256, 256, 19);
		break;
	case CommandIDs::menuAddGeoportailParcelExpress:
		AddWmtsServer("data.geopf.fr/wmts", "CADASTRALPARCELS.PARCELLAIRE_EXPRESS", "PM", "png", 256, 256, 19);
		break;
	case CommandIDs::menuAddGeoportailSCAN50Histo:
		AddWmtsServer("data.geopf.fr/wmts", "GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN50.1950", "PM", "jpeg", 256, 256, 15);
		break;
	case CommandIDs::menuAddWmtsServer:
		AddWmtsServer();
		break;
	case CommandIDs::menuExportImage:
		ExportImage();
		break;
	case CommandIDs::menuExportLas:
		ExportLas();
		break;
	case CommandIDs::menuZoomTotal:
		m_MapView.get()->ZoomWorld();
		break;
	case CommandIDs::menuScale1k:
		m_MapView.get()->ZoomScale(1000);
		break;
	case CommandIDs::menuScale10k:
		m_MapView.get()->ZoomScale(10000);
		break;
	case CommandIDs::menuScale25k:
		m_MapView.get()->ZoomScale(25000);
		break;
	case CommandIDs::menuScale100k:
		m_MapView.get()->ZoomScale(100000);
		break;
	case CommandIDs::menuScale250k:
		m_MapView.get()->ZoomScale(250000);
		break;
	case CommandIDs::menuShowSidePanel:
		if ((m_VerticalDividerBar.get() == nullptr) || (m_Panel.get() == nullptr))
			return false;
		if (m_Panel.get()->isVisible()) {
			m_VerticalDividerBar.get()->setVisible(false);
			m_Panel.get()->setVisible(false);
			m_VerticalLayout.setItemLayout(0, -1., -1., -1.);
		}
		else {
			m_VerticalDividerBar.get()->setVisible(true);
			m_Panel.get()->setVisible(true);
			m_VerticalLayout.setItemLayout(0, -0.2, -1.0, -0.65);
		}
		resized();
		break;
	case CommandIDs::menuShow3DViewer:
		if (m_OGL3DViewer.get() == nullptr)
			return false;
		m_OGL3DViewer.get()->setVisible(!m_OGL3DViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowVectorLayers:
		ShowHidePanel(m_VectorViewer.get());
		break;
	case CommandIDs::menuShowImageLayers:
		ShowHidePanel(m_ImageViewer.get());
		break;
	case CommandIDs::menuShowDtmLayers:
		ShowHidePanel(m_DtmViewer.get());
		break;
	case CommandIDs::menuShowLasLayers:
		ShowHidePanel(m_LasViewer.get());
		break;
	case CommandIDs::menuShowSelection:
		ShowHidePanel(m_SelTreeViewer.get());
		break;
	case CommandIDs::menuShowImageOptions:
		ShowHidePanel(m_ImageOptionsViewer.get());
		break;
	case CommandIDs::menuSynchronize:
		Synchronize();
		break;
	case CommandIDs::menuGoogle:
		juce::URL(XInternetMap::GoogleMapsUrl(m_MapView.get()->GetTarget(), m_MapView.get()->GetGsd())).launchInDefaultBrowser();
		break;
	case CommandIDs::menuBing:
		juce::URL(XInternetMap::BingMapsUrl(m_MapView.get()->GetTarget(), m_MapView.get()->GetGsd())).launchInDefaultBrowser();
		break;
	case CommandIDs::menuHelp:
		juce::URL("https://github.com/IGNF/IGNMap/blob/master/Documentation/Documentation.md").launchInDefaultBrowser();
		break;
	case CommandIDs::menuAbout:
		AboutIGNMap();
		break;
	default:
		return false;
	}

	return true;
}

//==============================================================================
// Gestion des actions
//==============================================================================
void MainComponent::actionListenerCallback(const juce::String& message)
{
	if (m_MapView == nullptr)
		return;
	if (message == "UpdateVector") {
		m_MapView.get()->RenderMap(true, false, false, true, false, true);
		return;
	}
	if (message == "UpdateRaster") {
		m_MapView.get()->RenderMap(false, true, false, false, false, true);
		return;
	}
	if (message == "UpdateDtm") {
		m_MapView.get()->RenderMap(false, false, true, false, false, true);
		return;
	}
	if (message == "UpdateLas") {
		m_MapView.get()->RenderMap(false, false, false, false, true, true);
		return;
	}
	if (message == "Repaint") {
		m_MapView.get()->RenderMap(false, false, false, false, false);
		return;
	}
	if (message == "StopMapThread") {
		m_MapView.get()->StopThread();
		return;
	}
	if (message == "UpdateSelectFeatures") {
		m_SelTreeViewer.get()->SetBase(&m_GeoBase);
		m_ImageOptionsViewer.get()->SetGeoBase(&m_GeoBase);
		m_MapView.get()->RenderMap(true, false, false, false, false);
		return;
	}
	if (message == "UpdatePreferences") {
		GeoTools::UpdateProjection(&m_GeoBase);
		XFrame F = m_GeoBase.Frame();
		m_MapView.get()->SetFrame(F);
		m_MapView.get()->CenterView(F.Center().X, F.Center().Y);
	}

	juce::StringArray T;
	T.addTokens(message, ":", "");

	if (T[0] == "Update3DView") {
		if (T.size() < 5)
			return;
		XFrame F;
		F.Xmin = T[1].getDoubleValue();
		F.Xmax = T[2].getDoubleValue();
		F.Ymin = T[3].getDoubleValue();
		F.Ymax = T[4].getDoubleValue();
		if (m_OGL3DViewer.get() != nullptr) {
			m_OGL3DViewer.get()->setVisible(true);
			m_OGL3DViewer.get()->toFront(true);
			m_OGL3DViewer.get()->LoadObjects(&m_GeoBase, &F);
		}
		return;
	}

	if (T[0] == "ZoomFrame") {
		if (T.size() < 5)
			return;
		XFrame F;
		F.Xmin = T[1].getDoubleValue();
		F.Xmax = T[2].getDoubleValue();
		F.Ymin = T[3].getDoubleValue();
		F.Ymax = T[4].getDoubleValue();
		m_MapView.get()->ZoomFrame(F, 10.);
	}
	if (T[0] == "CenterFrame") {
		if (T.size() < 3)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		m_MapView.get()->CenterView(X, Y);
	}
	if (T[0] == "ZoomGsd") {
		if (T.size() < 1)
			return;
		m_MapView.get()->ZoomGsd(T[1].getDoubleValue());
	}
	if (T[0] == "UpdateGroundPos") {
		if (T.size() < 3)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		m_ImageOptionsViewer.get()->SetGroundPos(X, Y);
	}
	if (T[0] == "UpdateTargetPos") {
		if (isConnected()) {
			juce::MemoryBlock block(message.getCharPointer(), message.length());
			sendMessage(block);
		}
	}
	if (T[0] == "CenterView") {
		if (isConnected()) {
			juce::MemoryBlock block(message.getCharPointer(), message.length());
			sendMessage(block);
		}
	}
	if (T[0] == "RemoveLasClass") {
		if (((T.size() - 1) % 2 != 0)||(T.size() == 1))
			return;
		m_MapView.get()->StopThread();
		m_GeoBase.ClearSelection();
		for (int i = 0; i < (T.size() - 1)/2; i++)
			m_GeoBase.RemoveClass(T[2*i + 1].getCharPointer(), T[2*i + 2].getCharPointer());

		actionListenerCallback("UpdateSelectFeatures");
		actionListenerCallback("UpdateLas");
	}
}

//==============================================================================
// Reponses aux boutons de la toolbar
//==============================================================================
void MainComponent::buttonClicked(juce::Button* button)
{
	juce::ToolbarButton* tlb = dynamic_cast<juce::ToolbarButton*>(button);
	if (tlb == nullptr)
		return;
	if (tlb->getItemId() == m_ToolbarFactory.Move)
		m_MapView.get()->SetMouseMode(MapView::Move);
	if (tlb->getItemId() == m_ToolbarFactory.Select)
		m_MapView.get()->SetMouseMode(MapView::Select);
	if (tlb->getItemId() == m_ToolbarFactory.Zoom)
		m_MapView.get()->SetMouseMode(MapView::Zoom);
	if (tlb->getItemId() == m_ToolbarFactory.Select3D)
		m_MapView.get()->SetMouseMode(MapView::Select3D);

	if (tlb->getItemId() == m_ToolbarFactory.Gsd) {
		double gsd = tlb->getButtonText().getDoubleValue();
		m_MapView.get()->ZoomGsd(gsd);
	}
	if (tlb->getItemId() == m_ToolbarFactory.Polyline)
		m_MapView.get()->SetMouseMode(MapView::Polyline);
	if (tlb->getItemId() == m_ToolbarFactory.Polygone)
		m_MapView.get()->SetMouseMode(MapView::Polygone);
	if (tlb->getItemId() == m_ToolbarFactory.Rectangle)
		m_MapView.get()->SetMouseMode(MapView::Rectangle);
	if (tlb->getItemId() == m_ToolbarFactory.Text)
		m_MapView.get()->SetMouseMode(MapView::Text);
}

//==============================================================================
// Retire tous les jeux de donnees charges
//==============================================================================
void MainComponent::Clear()
{
	m_MapView.get()->Clear();
	m_GeoBase.Clear();
	m_MapView.get()->SetGeoBase(&m_GeoBase);
	m_VectorViewer.get()->SetBase(&m_GeoBase);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	m_DtmViewer.get()->SetBase(&m_GeoBase);
	m_LasViewer.get()->SetBase(&m_GeoBase);
	m_SelTreeViewer.get()->SetBase(&m_GeoBase);
	m_ImageOptionsViewer.get()->SetImage(nullptr);
}

//==============================================================================
// Retire tous les jeux de donnees charges
//==============================================================================
void MainComponent::NewWindow()
{
	Clear();
	sendActionMessage("NewWindow");
	juce::Component* component = m_Toolbar.get()->getChildComponent(MainComponentToolbarFactory::Move);
	if (component != nullptr) {
		juce::ToolbarButton* button = dynamic_cast<juce::ToolbarButton*>(component);
		if (button != nullptr)
			button->triggerClick();
	}
}

//==============================================================================
// A propos
//==============================================================================
void MainComponent::AboutIGNMap()
{
	juce::String version = "0.0.5";
	juce::String info = "04/06/2024";
	juce::String message = "IGNMap 3 Version : " + version + "\n" + info + "\n";
	message += "JUCE Version : " + juce::String(JUCE_MAJOR_VERSION) + "."
		+ juce::String(JUCE_MINOR_VERSION) + "." + juce::String(JUCE_BUILDNUMBER) + "\n";
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, juce::translate("About IGNMap"), message, "OK");
}

//==============================================================================
// Choix d'un fichier vecteur
//==============================================================================
void MainComponent::ImportVectorFolder()
{
	juce::String folderName = AppUtil::OpenFolder("VectorFolderPath");
	if (folderName.isEmpty())
		return;
	int nb_total, nb_imported;
	GeoTools::ImportVectorFolder(folderName, &m_GeoBase, nb_total, nb_imported);
	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(true, false, false, true, false, true);
	m_VectorViewer.get()->SetBase(&m_GeoBase);
}

//==============================================================================
// Import d'un repertoire d'images
//==============================================================================
void MainComponent::ImportImageFolder()
{
	juce::String folderName = AppUtil::OpenFolder("ImageFolderPath");
	if (folderName.isEmpty())
		return;
	XGeoClass* C = ImportDataFolder(folderName, XGeoVector::Raster);
	if (C == nullptr)
		return;
	C->Repres()->Name(C->Name().c_str());
	C->Repres()->ZOrder(0);
	C->Repres()->Transparency(0);
	C->Repres()->Color(0xFFFFFFFF);
	C->Repres()->FillColor(0xFFFFFFFF);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
}

//==============================================================================
// Ajout d'une couche vectorielle
//==============================================================================
bool MainComponent::ImportVectorFile(juce::String filename)
{
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("VectorPath", juce::translate("Open vector file"), "*.shp;*.mif;*.gpkg;*.dxf;*.json;*.xml");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String extension = file.getFileExtension();
	extension = extension.toLowerCase();
	bool flag = false;
	if (extension == ".shp")
		flag = GeoTools::ImportShapefile(filename, &m_GeoBase);
	if (extension == ".gpkg")
		flag = GeoTools::ImportGeoPackage(filename, &m_GeoBase);
	if (extension == ".mif")
		flag = GeoTools::ImportMifMid(filename, &m_GeoBase);
	if (extension == ".dxf")
		flag = GeoTools::ImportDxf(filename, &m_GeoBase);
	if (extension == ".json")
		flag = GeoTools::ImportGeoJson(filename, &m_GeoBase);
	if (extension == ".xml")
		flag = GeoTools::ImportTA(filename, &m_GeoBase);
	if (flag == false) {
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}
	GeoTools::ColorizeClasses(&m_GeoBase);

	m_GeoBase.UpdateFrame();

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(true, false, false, true, false, true);
	m_VectorViewer.get()->SetBase(&m_GeoBase);

	return true;
}

//==============================================================================
// Ajout d'une couche raster
//==============================================================================
bool MainComponent::ImportImageFile(juce::String rasterfile)
{
	juce::String filename = rasterfile;
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("RasterPath");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();

	GeoFileImage* image = new GeoFileImage;
	if (!image->AnalyzeImage(filename.toStdString())) {
		delete image;
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}
	if (!GeoTools::RegisterObject(&m_GeoBase, image, name.toStdString().c_str(), "Raster", name.toStdString().c_str())) {
		delete image;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);

	return true;
}

//==============================================================================
// Import d'un repertoire de fichiers LAS/LAZ
//==============================================================================
void MainComponent::ImportDtmFolder()
{
	juce::String folderName = AppUtil::OpenFolder("DtmFolderPath");
	if (folderName.isEmpty())
		return;
	ImportDataFolder(folderName, XGeoVector::DTM);
	m_DtmViewer.get()->SetBase(&m_GeoBase);
	m_MapView.get()->RenderMap(false, false, true, false, false, true);
}

//==============================================================================
// Ajout d'une couche MNT
//==============================================================================
bool MainComponent::ImportDtmFile(juce::String dtmfile)
{
	juce::String filename = dtmfile;
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("DtmPath");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();

	GeoDTM* dtm = new GeoDTM;
	juce::File tmpFile = juce::File::createTempFile("tif");
	if (!dtm->OpenDtm(filename.toStdString().c_str(), tmpFile.getFullPathName().toStdString().c_str())) {
		delete dtm;
		tmpFile.deleteFile();
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}

	if (!GeoTools::RegisterObject(&m_GeoBase, dtm, name.toStdString().c_str(), "DTM", name.toStdString().c_str())) {
		delete dtm;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, false, true, false, false, true);
	m_DtmViewer.get()->SetBase(&m_GeoBase);

	return true;
}

//==============================================================================
// Ajout d'un repertoire de donnees LAS, DTM ou Raster
//==============================================================================
XGeoClass* MainComponent::ImportDataFolder(juce::String folderName, XGeoVector::eTypeVector type)
{
	if ((type != XGeoVector::DTM) && (type != XGeoVector::LAS) && (type != XGeoVector::Raster))
		return nullptr;
	std::string layerName = "DTM";
	if (type == XGeoVector::LAS) layerName = "LAS";
	if (type == XGeoVector::Raster) layerName = "Raster";

	juce::File folder = folderName;
	
	XGeoMap* map = new XGeoMap;
	map->Name(folderName.toStdString());
	m_GeoBase.AddMap(map);
	XGeoClass* data_class = m_GeoBase.AddClass(layerName.c_str(), folder.getFileName().toStdString().c_str());
	if (data_class == nullptr)
		return nullptr;
	m_GeoBase.SortClass();

	juce::Array<juce::File> T;
	if (type == XGeoVector::LAS) {
		T = folder.findChildFiles(juce::File::findFiles, false, "*.las");
		T.addArray(folder.findChildFiles(juce::File::findFiles, false, "*.laz"));
	}
	if (type == XGeoVector::DTM) {
		T = folder.findChildFiles(juce::File::findFiles, false, "*.asc");
		T.addArray(folder.findChildFiles(juce::File::findFiles, false, "*.tif"));
	}
	if (type == XGeoVector::Raster) {
		T = folder.findChildFiles(juce::File::findFiles, false, "*.jp2");
		T.addArray(folder.findChildFiles(juce::File::findFiles, false, "*.tif"));
		T.addArray(folder.findChildFiles(juce::File::findFiles, false, "*.cog"));
		T.addArray(folder.findChildFiles(juce::File::findFiles, false, "*.webp"));
	}

	class MyTask : public juce::ThreadWithProgressWindow {
	public:
		XGeoMap* map = nullptr;
		XGeoClass* data_class = nullptr;
		juce::Array<juce::File>* T = nullptr;
		XGeoVector::eTypeVector type;
		MyTask() : ThreadWithProgressWindow("busy...", true, true) { type = XGeoVector::LAS; }
		void run()
		{
			for (int i = 0; i < T->size(); i++)
			{
				if (threadShouldExit())
					break;
				setProgress(i / (double)T->size());
				XGeoVector* V = nullptr;
				if (type == XGeoVector::LAS) {
					GeoLAS* las = new GeoLAS;
					if (!las->Open((*T)[i].getFullPathName().toStdString())) {
						delete las;
						continue;
					}
					V = las;
					las->CloseIfNeeded();	// Pour eviter d'utiliser trop de descripteurs de fichiers
				}
				if (type == XGeoVector::DTM) {
					GeoDTM* dtm = new GeoDTM;
					juce::File tmpFile = juce::File::createTempFile("tif");
					if (!dtm->OpenDtm((*T)[i].getFullPathName().toStdString().c_str(), tmpFile.getFullPathName().toStdString().c_str())) {
						delete dtm;
						continue;
					}
					V = dtm;
				}
				if (type == XGeoVector::Raster) {
					GeoFileImage* image = new GeoFileImage;
					if (!image->AnalyzeImage((*T)[i].getFullPathName().toStdString())) {
						delete image;
						continue;
					}
					V = image;
				}

				data_class->Vector(V);
				V->Class(data_class);
				map->AddObject(V);
			}
		}
	};

	MyTask m;
	m.data_class = data_class;
	m.map = map;
	m.T = &T;
	m.type = type;
	m.runThread();

	m_GeoBase.UpdateFrame();
	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	return data_class;
}

//==============================================================================
// Import d'un repertoire de fichiers LAS/LAZ
//==============================================================================
void MainComponent::ImportLasFolder()
{
	juce::String folderName = AppUtil::OpenFolder("LasFolderPath");
	if (folderName.isEmpty())
		return;
	ImportDataFolder(folderName, XGeoVector::LAS);
	m_LasViewer.get()->SetBase(&m_GeoBase);
	m_MapView.get()->RenderMap(false, false, false, false, true, true);
}

//==============================================================================
// Ajout d'une couche LAS
//==============================================================================
bool MainComponent::ImportLasFile(juce::String lasfile)
{
	juce::String filename = lasfile;
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("LasPath");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();

	GeoLAS* las = new GeoLAS;
	
	if (!las->Open(filename.toStdString())) {
		delete las;
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}

	if (!GeoTools::RegisterObject(&m_GeoBase, las, name.toStdString().c_str(), "LAS", name.toStdString().c_str())) {
		delete las;
		return false;
	}

	m_LasViewer.get()->SetBase(&m_GeoBase); 
	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, false, false, false, true, true);

	return true;
}

//==============================================================================
// Ajout d'une couche TMS OSM
//==============================================================================
bool MainComponent::AddOSMServer()
{
	XGeoPref pref;
	XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

	OsmLayer* osm = new OsmLayer("tile.openstreetmap.org");
	osm->SetFrame(F);
	if (!GeoTools::RegisterObject(&m_GeoBase, osm, "OSM", "OSM", "tile.openstreetmap.org")) {
		delete osm;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	return true;
}

//==============================================================================
// Ajout d'un serveur WMTS
//==============================================================================
bool MainComponent::AddWmtsServer()
{
	juce::AlertWindow alert(juce::translate("Add a WMTS server"), juce::translate("URL of the WMTS server"), juce::AlertWindow::QuestionIcon);
	alert.addButton(juce::translate("Cancel"), 0);
	alert.addButton(juce::translate("OK"), 1);
	alert.addTextEditor("Server", "https://wxs.ign.fr/ortho/geoportail/wmts", juce::translate("Server : "));
	alert.addTextEditor("Layer", "", juce::translate("Layer : "));
	alert.addTextEditor("TMS", "", juce::translate("TMS : "));
	alert.addTextEditor("Format", "jpeg", juce::translate("Format : "));


	if (alert.runModalLoop() == 0)
		return false;
	AddWmtsServer(alert.getTextEditorContents("Server").toStdString(), alert.getTextEditorContents("Layer").toStdString(),
		alert.getTextEditorContents("TMS").toStdString(), alert.getTextEditorContents("Format").toStdString());
	return false;
}

//==============================================================================
// Ajout d'un serveur WMTS
//==============================================================================
bool MainComponent::AddWmtsServer(std::string server, std::string layer, std::string TMS, std::string format, 
																	uint32_t tileW, uint32_t tileH, uint32_t max_zoom, std::string apikey)
{
	XGeoPref pref;
	XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

	WmtsLayer* wmts = new WmtsLayer(server, layer, TMS, format, tileW, tileH, max_zoom, apikey);
	wmts->SetFrame(F);
	if (!GeoTools::RegisterObject(&m_GeoBase, wmts, "WMTS", "WMTS", layer)) {
		delete wmts;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	return true;
}

//==============================================================================
// Export sous forme d'image
//==============================================================================
bool MainComponent::ExportImage()
{
	m_MapView.get()->StopThread();
	XFrame F = m_MapView.get()->GetSelectionFrame();
	double gsd = m_MapView.get()->GetGsd();
	ExportImageDlg* dlg = new ExportImageDlg(&m_GeoBase, XRint(F.Xmin), XRint(F.Ymin), XRint(F.Xmax), XRint(F.Ymax), XRint(gsd));
	juce::DialogWindow::LaunchOptions options;
	options.content.setOwned(dlg);

	juce::Rectangle<int> area(0, 0, 410, 300);

	options.content->setSize(area.getWidth(), area.getHeight());
	options.dialogTitle = juce::translate("Export Image");
	options.dialogBackgroundColour = juce::Colour(0xff0e345a);
	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = false;
	options.resizable = false;

	options.launchAsync();
	return true;
}

//==============================================================================
// Export sous forme d'un nuage de points LAS
//==============================================================================
bool MainComponent::ExportLas()
{
	m_MapView.get()->StopThread();
	XFrame F = m_MapView.get()->GetSelectionFrame();
	ExportLasDlg* dlg = new ExportLasDlg(&m_GeoBase, XRint(F.Xmin), XRint(F.Ymin), XRint(F.Xmax), XRint(F.Ymax));
	juce::DialogWindow::LaunchOptions options;
	options.content.setOwned(dlg);

	juce::Rectangle<int> area(0, 0, 410, 300);

	options.content->setSize(area.getWidth(), area.getHeight());
	options.dialogTitle = juce::translate("Export LAS");
	options.dialogBackgroundColour = juce::Colour(0xff0e345a);
	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = false;
	options.resizable = false;

	options.runModal();
	return true;
}

//==============================================================================
// Chargement d'un fichier de traduction
//==============================================================================
void MainComponent::Translate()
{
	/*
	juce::String filename = AppUtil::OpenFile();
	if (filename.isEmpty())
		return;
	juce::File file(filename);
	if (!file.exists())
		return;
	juce::LocalisedStrings::setCurrentMappings(new juce::LocalisedStrings(file, true));
	*/
	juce::LocalisedStrings* actualLoc = juce::LocalisedStrings::getCurrentMappings();
	if (actualLoc != nullptr)
		juce::LocalisedStrings::setCurrentMappings(nullptr);
	else {
		int size;
		const char* data = BinaryData::getNamedResource("Translation_fr_txt", size);
		if (data == nullptr)
			return;
		juce::String fileContents = juce::String::createStringFromData(data, size);
		juce::LocalisedStrings::setCurrentMappings(new juce::LocalisedStrings(fileContents, true));
	}

	m_VectorViewer.get()->Translate();
	m_ImageViewer.get()->Translate();
	m_DtmViewer.get()->Translate();
	m_LasViewer.get()->Translate();
	m_ImageOptionsViewer.get()->Translate();
	for (int i = 0; i < m_Panel.get()->getNumPanels(); i++)
		m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(i), nullptr, true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(0), new juce::TextButton(juce::translate("Vector Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(1), new juce::TextButton(juce::translate("Image Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(2), new juce::TextButton(juce::translate("DTM Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(3), new juce::TextButton(juce::translate("LAS Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(4), new juce::TextButton(juce::translate("Image Options")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(5), new juce::TextButton(juce::translate("Selection")), true);
	for (int i = 0; i < m_Panel.get()->getNumPanels(); i++)	// Necessaire pour rafraichir les titres des panneaux
		m_Panel.get()->expandPanelFully(m_Panel.get()->getPanel(i), false);
}

//==============================================================================
// Preferences de l'application
//==============================================================================
void MainComponent::Preferences()
{
	PrefDlg* dlg = new PrefDlg;
	dlg->addActionListener(this);
	juce::DialogWindow::LaunchOptions options;
	options.content.setOwned(dlg);

	juce::Rectangle<int> area = dlg->PreferedSize();
	options.content->setSize(area.getWidth(), area.getHeight());
	options.dialogTitle = juce::translate("Preferences");
	options.dialogBackgroundColour = juce::Colours::grey;
	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = false;
	options.resizable = false;
	options.launchAsync();
}

//==============================================================================
// Change la visibilite d'un panneau
//==============================================================================
void MainComponent::ShowHidePanel(juce::Component* component)
{
	if (component->isVisible()) {
		m_Panel.get()->setPanelSize(component, 0, true);
		m_Panel.get()->setPanelHeaderSize(component, 0);
		component->setVisible(false);
	}
	else {
		m_Panel.get()->setPanelHeaderSize(component, 20);
		component->setVisible(true);
		m_Panel.get()->expandPanelFully(component, true);
	}
}

//==============================================================================
// Methode test ... 
//==============================================================================
void MainComponent::Test()
{
	/*
	XTiffWriter writer;
	writer.SetGeoTiff(500000., 6500000., 1.);
	writer.WriteTiled("D:\\Test_tile.tif", 1000, 1000, 1, 8, nullptr, 0, 256, 256);
	std::ofstream file;
	file.open("D:\\Test_tile.tif", std::ios::out | std::ios::binary | std::ios::app);
	file.seekp(0, std::ios_base::end);

	uint8_t* area = new uint8_t[256 * 256];
	for (int i = 0; i < 16; i++) {
		::memset(area, i * 16, 256 * 256 * sizeof(uint8_t));
		file.write((const char*)area, 256 * 256 * sizeof(uint8_t));
	}
	file.close();
	delete[] area;
	*/
	/*
	auto bounds = m_MapView.get()->getBounds();
	juce::AffineTransform transfo;
	//transfo.rotated(0.5, bounds.getCentreX(), bounds.getCentreY());
	m_MapView.get()->setTransform(transfo.rotated(0.5, bounds.getCentreX(), bounds.getCentreY()));
	*/

	if (m_GeoBase.NbSelection() != 1)
		return;
	XGeoVector* V = m_GeoBase.Selection(0);
	XTACliche* cliche = dynamic_cast<XTACliche*>(V);
	if (cliche == nullptr)
		return;
	juce::String filename;
	filename = AppUtil::OpenFile("RasterPath");
	if (filename.isEmpty())
		return;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();
	RotationImage* affine = new RotationImage();
	if (!affine->AnalyzeImage(filename.toStdString())) {
		delete affine;
		return;
	}
	XPt2D S = cliche->Centroide();
	double gsd = cliche->Resol();
	XPt2D C = XPt2D(affine->GetImageW() * 0.5, affine->GetImageH() * 0.5);
	if (!cliche->IsDigital()) {
		gsd = (cliche->Resol() * 1000 * 0.24) / XMin(affine->GetImageW(), affine->GetImageH());
		C = XPt2D(affine->GetImageW() * 0.5, affine->GetImageH() - affine->GetImageW() * 0.5);
	}

	affine->SetPosition(S.X, S.Y, gsd);
	affine->SetImageCenter(C.X, C.Y);
	affine->SetRotation(XPI - (cliche->Cap() / 180.) * XPI);

	if (!GeoTools::RegisterObject(&m_GeoBase, affine, "ROTATION", "Raster", name.toStdString().c_str())) {
		delete affine;
		return;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);


	
	/*
	XGeoMap* map = m_GeoBase.Map("ROTATION");
	if (map != NULL) {
		for (uint32_t i = 0; i < map->NbObject(); i++) {
			RotationImage* image = dynamic_cast<RotationImage*>(map->Object(i));
			if (image != nullptr) {
				image->AddRotation(XPI4 / 4);
			}
		}
		m_MapView.get()->SetFrame(m_GeoBase.Frame());
		m_MapView.get()->RenderMap(false, true, false, false, false, true);
		return;
	}

	juce::String filename;
	filename = AppUtil::OpenFile("RasterPath");
	if (filename.isEmpty())
		return;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();
	RotationImage* affine = new RotationImage();
	if (!affine->AnalyzeImage(filename.toStdString())) {
		delete affine;
		return;
	}
	// 465593,69 ; 6872112,785 93.04 
	affine->SetPosition(668000., 6840000., 0.5);
	affine->SetImageCenter(affine->GetImageW(), affine->GetImageH());
	affine->SetRotation(XPI4);
	//affine->SetPosition(465593.69, 6872112.785, 0.5);
	//affine->SetRotation((93.04 / 180) * XPI - XPI2);
	
	if (!GeoTools::RegisterObject(&m_GeoBase, affine, "ROTATION", "Raster", name.toStdString().c_str())) {
		delete affine;
		return ;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	*/
}

//==============================================================================
// Synchronisation de 2 instances d'IGNMap
//==============================================================================
void MainComponent::Synchronize()
{
	if (isConnected()) {
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
			juce::translate("Synchronization"), juce::translate("Already connected"), "OK");
		return;
	}
	if (!createPipe("IGNMap", -1, true)) {
		if (connectToPipe("IGNMap", -1)) {
			juce::Rectangle< int > R = getParentMonitorArea();
			if (getParentComponent()!=nullptr)
				getParentComponent()->setBounds(R.getWidth() / 2, 0, R.getWidth() / 2, R.getHeight());
			juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
				juce::translate("Synchronization"), juce::translate("Connected with ") + getConnectedHostName(), "OK");
		}
	}
	else {
		juce::Rectangle< int > R = getParentMonitorArea();
		if (getParentComponent() != nullptr)
			getParentComponent()->setBounds(0, 0, R.getWidth() / 2, R.getHeight());
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
			juce::translate("Synchronization"), juce::translate("Connection ready ... waiting for another IGNMap"), "OK");
	}
}

//==============================================================================
// Methodes de InterprocessConnection 
//==============================================================================
void MainComponent::connectionMade()
{
}

void MainComponent::connectionLost()
{
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
		juce::translate("Synchronization"), juce::translate("Connection lost !"), "OK");
}

void MainComponent::messageReceived(const juce::MemoryBlock& message)
{
	juce::String chaine = message.toString();
	if (chaine == "Disconnect") {
		disconnect();
		return;
	}
	juce::StringArray T;
	T.addTokens(chaine, ":", "");
	if (T[0] == "CenterView") {
		if (T.size() < 4)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		double scale = T[3].getDoubleValue();
		m_MapView.get()->CenterView(X, Y, scale, false);
		return;
	}
	if (T[0] == "UpdateTargetPos") {
		if (T.size() < 3)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		m_MapView.get()->SetTarget(X, Y, false);
		return;
	}

}

//==============================================================================
// Gestion du Drag&Drop de fichiers externes
//==============================================================================
void MainComponent::filesDropped(const juce::StringArray& filenames, int /*x*/, int /*y*/)
{
	for (int i = 0; i < filenames.size(); i++) {
		juce::String filename = filenames[i];
		juce::File file(filename);
		juce::String ext = file.getFileExtension();
		if (ext.equalsIgnoreCase(".jp2") || ext.equalsIgnoreCase(".tif") || ext.equalsIgnoreCase(".cog"))
			ImportImageFile(filename);
		if (ext.equalsIgnoreCase(".las") || ext.equalsIgnoreCase(".laz") || ext.equalsIgnoreCase(".copc"))
			ImportLasFile(filename);
		if (ext.equalsIgnoreCase(".shp") || ext.equalsIgnoreCase(".mif") || ext.equalsIgnoreCase(".gpkg"))
			ImportVectorFile(filename);
		if (ext.equalsIgnoreCase(".asc"))
			ImportDtmFile(filename);
	}
}