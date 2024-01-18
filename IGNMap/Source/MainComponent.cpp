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
#include "OsmLayer.h"
#include "WmtsLayer.h"
#include "ExportImageDlg.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolImage/XTiffWriter.h"


//==============================================================================
MainComponent::MainComponent()
{
  m_MapView.reset(new MapView);
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
	m_VectorViewer.get()->SetActionListener(this);
	addActionListener(m_VectorViewer.get());

	m_ImageViewer.reset(new ImageLayersViewer);
	addAndMakeVisible(m_ImageViewer.get());
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	m_ImageViewer.get()->SetActionListener(this);
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
	m_DtmViewer.get()->SetActionListener(this);

	m_LasViewer.reset(new LasLayersViewer);
	addAndMakeVisible(m_LasViewer.get());
	m_LasViewer.get()->SetBase(&m_GeoBase);
	m_LasViewer.get()->SetActionListener(this);

	m_OGL3DViewer.reset(new OGL3DViewer("3DViewer", juce::Colours::grey, juce::DocumentWindow::allButtons));
	m_OGL3DViewer.get()->setVisible(false);

  m_Panel.reset(new juce::ConcertinaPanel);
  addAndMakeVisible(m_Panel.get());
	m_Panel.get()->addPanel(-1, m_VectorViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_DtmViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_LasViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_SelTreeViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageOptionsViewer.get(), false);
	
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(0), new juce::TextButton(juce::translate("Vector Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(1), new juce::TextButton(juce::translate("Image Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(2), new juce::TextButton(juce::translate("DTM Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(3), new juce::TextButton(juce::translate("LAS Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(4), new juce::TextButton(juce::translate("Selection")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(5), new juce::TextButton(juce::translate("Image Options")), true);

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
}

MainComponent::~MainComponent()
{
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

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String& menuName)
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
		menu.addSubMenu(juce::translate("Export"), ExportSubMenu);

		menu.addCommandItem(&m_CommandManager, CommandIDs::menuQuit);
	}
	else if (menuIndex == 1) // Edit
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuTranslate);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuTest);
	}
	else if (menuIndex == 2) // Tools
	{ 
		
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
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowSelection);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowImageOptions);
		menu.addSubMenu(juce::translate("Panels"), PanelSubMenu);
		menu.addSeparator();
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuZoomTotal);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuZoomLevel);
		juce::PopupMenu ScaleSubMenu;
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale1k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale10k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale25k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale100k);
		ScaleSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuScale250k);
		menu.addSubMenu(juce::translate("Scale"), ScaleSubMenu);
	}
	else if (menuIndex == 4) // Help
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuAbout);
	}

	return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{

}

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return NULL;
}

//==============================================================================
//
//==============================================================================
void MainComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
	juce::Array<juce::CommandID> commands{ CommandIDs::menuNew,
		CommandIDs::menuQuit, CommandIDs::menuUndo, CommandIDs::menuTranslate,
		CommandIDs::menuImportVectorFile, CommandIDs::menuImportVectorFolder, 
		CommandIDs::menuImportImageFile, CommandIDs::menuImportImageFolder, CommandIDs::menuImportDtmFile,
		CommandIDs::menuImportDtmFolder, CommandIDs::menuImportLasFile, CommandIDs::menuImportLasFolder,
		CommandIDs::menuExportImage,
		CommandIDs::menuZoomTotal, CommandIDs::menuZoomLevel,
		CommandIDs::menuTest, CommandIDs::menuShowSidePanel,
		CommandIDs::menuShowVectorLayers, CommandIDs::menuShowImageLayers, CommandIDs::menuShowDtmLayers, 
		CommandIDs::menuShowLasLayers, CommandIDs::menuShowSelection, CommandIDs::menuShowImageOptions,
		CommandIDs::menuShow3DViewer, CommandIDs::menuAddOSM, CommandIDs::menuAddGeoportailOrthophoto,
		CommandIDs::menuAddGeoportailOrthohisto, CommandIDs::menuAddGeoportailSatellite, CommandIDs::menuAddGeoportailCartes,
		CommandIDs::menuAddGeoportailOrthophotoIRC, CommandIDs::menuAddGeoportailPlanIGN, CommandIDs::menuAddGeoportailParcelExpress,
		CommandIDs::menuAddGeoportailSCAN50Histo,
		CommandIDs::menuAddWmtsServer,
		CommandIDs::menuScale1k, CommandIDs::menuScale10k, CommandIDs::menuScale25k, CommandIDs::menuScale100k, CommandIDs::menuScale250k,
		CommandIDs::menuAbout };
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
		result.setInfo(juce::translate("New Window"), "Places the menu bar inside the application window", "Menu", 0);
		result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
		break;
	case CommandIDs::menuQuit:
		result.setInfo(juce::translate("Quit"), "Uses a burger menu", "Menu", 0);
		result.addDefaultKeypress('q', juce::ModifierKeys::ctrlModifier);
		break;
	case CommandIDs::menuTranslate:
		result.setInfo(juce::translate("Translate"), juce::translate("Load a translation file"), "Menu", 0);
		break;
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
	case CommandIDs::menuZoomTotal:
		result.setInfo(juce::translate("Zoom total"), juce::translate("Zoom total"), "Menu", 0);
		break;
	case CommandIDs::menuZoomLevel:
		result.setInfo(juce::translate("Zoom level"), juce::translate("Zoom level"), "Menu", 0);
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
		result.setInfo(juce::translate("View DTM Layers Panek"), juce::translate("View DTM Layers Panel"), "Menu", 0);
		if (m_DtmViewer.get() != nullptr)
			result.setTicked(m_DtmViewer.get()->isVisible());
		break;
	case CommandIDs::menuShowLasLayers:
		result.setInfo(juce::translate("View LAS Layers Panek"), juce::translate("View LAS Layers Panel"), "Menu", 0);
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
		Clear();
		sendActionMessage("NewWindow");
		break;
	case CommandIDs::menuQuit:
		juce::JUCEApplication::quit();
		break;
	case CommandIDs::menuTranslate:
		Translate();
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
	case CommandIDs::menuZoomTotal:
		m_MapView.get()->ZoomWorld();
		break;
	case CommandIDs::menuZoomLevel:
		m_MapView.get()->ZoomLevel();
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
	if (message == "UpdateSelectFeatures") {
		m_SelTreeViewer.get()->SetBase(&m_GeoBase);
		m_ImageOptionsViewer.get()->SetGeoBase(&m_GeoBase);
		m_MapView.get()->RenderMap(true, false, false, false, false);
		return;
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
			m_OGL3DViewer.get()->LoadObjects(&m_GeoBase, &F);
		}
		return;
	}

	if (T[0] == "SelectFeature") {
		if (T.size() < 3)
			return;
		//GIntBig id = T[1].getLargeIntValue();
		int idLayer = T[2].getIntValue();
		//m_Base.SelectFeatureFields(idLayer, id);
		//m_FeatureViewer.get()->SetBase(&m_Base);
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
	if (T[0] == "DrawFrame") {
		if (T.size() < 5)
			return;
		XFrame F;
		F.Xmin = T[1].getDoubleValue();
		F.Xmax = T[2].getDoubleValue();
		F.Ymin = T[3].getDoubleValue();
		F.Ymax = T[4].getDoubleValue();
		m_MapView.get()->DrawFrame(F);
	}
	if (T[0] == "CenterFrame") {
		if (T.size() < 3)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		m_MapView.get()->CenterView(X, Y);
	}
	if (T[0] == "UpdateGroundPos") {
		if (T.size() < 3)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		m_ImageOptionsViewer.get()->SetGroundPos(X, Y);
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
	if (tlb->getItemId() == m_ToolbarFactory.Move) {
		m_MapView.get()->SetMouseMode(MapView::Move);
	}
	if (tlb->getItemId() == m_ToolbarFactory.Select) {
		m_MapView.get()->SetMouseMode(MapView::Select);
	}
	if (tlb->getItemId() == m_ToolbarFactory.Zoom) {
		m_MapView.get()->SetMouseMode(MapView::Zoom);
	}
	if (tlb->getItemId() == m_ToolbarFactory.Select3D) {
		m_MapView.get()->SetMouseMode(MapView::Select3D);
	}
	if (tlb->getItemId() == m_ToolbarFactory.Gsd) {
		double gsd = tlb->getButtonText().getDoubleValue();
		m_MapView.get()->ZoomGsd(gsd);
	}
}

//==============================================================================
// Choix d'un repertoire
//==============================================================================
juce::String MainComponent::OpenFolder(juce::String optionName, juce::String mes)
{
	juce::String path, message;
	if (!optionName.isEmpty())
		path = GetAppOption(optionName);
	if (mes.isEmpty())
		message = juce::translate("Choose a directory...");
	
	juce::FileChooser fc(message, path, "*", true);
	if (fc.browseForDirectory()) {
		auto result = fc.getURLResult();
		auto name = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString(true);

		if (!optionName.isEmpty())
			SaveAppOption(optionName, name);
		return name;
	}
	return "";
}

//==============================================================================
// Choix d'un fichier
//==============================================================================
juce::String MainComponent::OpenFile(juce::String optionName, juce::String mes, juce::String filter)
{
	juce::String path = optionName, message = mes, filters = filter;
	if (!optionName.isEmpty())
		path = GetAppOption(optionName);
	if (mes.isEmpty())
		message = juce::translate("Choose a file...");
	if (filter.isEmpty())
		filters = "*";
	
	juce::FileChooser fc(message, path, filters, true);
	if (fc.browseForFileToOpen()) {
		auto result = fc.getURLResult();
		auto name = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString(true);

		if (!optionName.isEmpty())
			SaveAppOption(optionName, name);
		return name;
	}
	return "";
}


//==============================================================================
// Choix d'un fichier vecteur
//==============================================================================
void MainComponent::ImportVectorFolder()
{
	juce::String folderName = OpenFolder("VectorFolderPath");
	if (folderName.isEmpty())
		return;
	int nb_total, nb_imported;
	GeoBase::ImportVectorFolder(folderName, &m_GeoBase, nb_total, nb_imported);
	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(true, false, false, true, false, true);
	m_VectorViewer.get()->SetBase(&m_GeoBase);
}

//==============================================================================
// Import d'un repertoire d'images
//==============================================================================
void MainComponent::ImportImageFolder()
{
	juce::String folderName = OpenFolder("ImageFolderPath");
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
// Gestion des options de l'application
//==============================================================================
juce::String MainComponent::GetAppOption(juce::String name)
{
	juce::PropertiesFile::Options options;
	options.applicationName = "GdalMap";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	return file->getValue(name);
}

void MainComponent::SaveAppOption(juce::String name, juce::String value)
{
	juce::PropertiesFile::Options options;
	options.applicationName = "GdalMap";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	file->setValue(name, value);
	app.saveIfNeeded();
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
// A propos
//==============================================================================
void MainComponent::AboutIGNMap()
{
	juce::String version = "0.0.1";
	juce::String info = "02/01/2024";
	juce::String message = "IGNMap 3 Version : " + version + "\n" + info + "\n";
	message += "JUCE Version : " + juce::String(JUCE_MAJOR_VERSION) + "."
		+ juce::String(JUCE_MINOR_VERSION) + "." + juce::String(JUCE_BUILDNUMBER);
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
		juce::translate("About IGNMap"), message, "OK");
}

//==============================================================================
// Ajout d'une couche vectorielle
//==============================================================================
bool MainComponent::ImportVectorFile(juce::String filename)
{
	if (filename.isEmpty())
		filename = OpenFile("VectorPath", juce::translate("Open vector file"), "*.shp;*.mif;*.gpkg");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String extension = file.getFileExtension();
	extension = extension.toLowerCase();
	bool flag = false;
	if (extension == ".shp")
		flag = GeoBase::ImportShapefile(filename, &m_GeoBase);
	if (extension == ".gpkg")
		flag = GeoBase::ImportGeoPackage(filename, &m_GeoBase);
	if (extension == ".mif")
		flag = GeoBase::ImportMifMid(filename, &m_GeoBase);
	if (flag == false) {
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}
	GeoBase::ColorizeClasses(&m_GeoBase);

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
		filename = OpenFile("RasterPath");
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
	if (!GeoBase::RegisterObject(&m_GeoBase, image, name.toStdString().c_str(), "Raster", name.toStdString().c_str())) {
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
	juce::String folderName = OpenFolder("DtmFolderPath");
	if (folderName.isEmpty())
		return;
	XGeoClass* C = ImportDataFolder(folderName, XGeoVector::DTM);
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
		filename = OpenFile("DtmPath");
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

	if (!GeoBase::RegisterObject(&m_GeoBase, dtm, name.toStdString().c_str(), "DTM", name.toStdString().c_str())) {
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
	juce::String folderName = OpenFolder("LasFolderPath");
	if (folderName.isEmpty())
		return;
	XGeoClass* C = ImportDataFolder(folderName, XGeoVector::LAS);
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
		filename = OpenFile("LasPath");
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

	if (!GeoBase::RegisterObject(&m_GeoBase, las, name.toStdString().c_str(), "LAS", name.toStdString().c_str())) {
		delete las;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, false, false, false, true, true);
	m_LasViewer.get()->SetBase(&m_GeoBase);

	return true;
}

//==============================================================================
// Ajout d'une couche TMS OSM
//==============================================================================
bool MainComponent::AddOSMServer()
{
	XGeoPref pref;
	pref.Projection(XGeoProjection::Lambert93);
	XFrame F = XGeoProjection::FrameProj(pref.Projection());

	OsmLayer* osm = new OsmLayer("tile.openstreetmap.org");
	osm->SetFrame(F);
	if (!GeoBase::RegisterObject(&m_GeoBase, osm, "OSM", "OSM", "tile.openstreetmap.org")) {
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
	pref.Projection(XGeoProjection::Lambert93);
	XFrame F = XGeoProjection::FrameProj(pref.Projection());

	WmtsLayer* wmts = new WmtsLayer(server, layer, TMS, format, tileW, tileH, max_zoom, apikey);
	wmts->SetFrame(F);
	if (!GeoBase::RegisterObject(&m_GeoBase, wmts, "WMTS", "WMTS", layer)) {
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
	ExportImageDlg* dlg = new ExportImageDlg(&m_GeoBase, 743000.0, 6750500, 743500, 6751000, 1.);
	juce::DialogWindow::LaunchOptions options;
	options.content.setOwned(dlg);

	juce::Rectangle<int> area(0, 0, 410, 300);

	options.content->setSize(area.getWidth(), area.getHeight());
	options.dialogTitle = juce::translate("Export Image");
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
	juce::String filename = OpenFile();
	if (filename.isEmpty())
		return;
	juce::File file(filename);
	if (!file.exists())
		return;
	juce::LocalisedStrings::setCurrentMappings(new juce::LocalisedStrings(file, true));
	//m_FeatureViewer.get()->UpdateColumnName();
	//m_LayerViewer.get()->UpdateColumnName();
	//m_RasterLayerViewer.get()->UpdateColumnName();
	m_VectorViewer.get()->UpdateColumnName();
	m_ImageViewer.get()->UpdateColumnName();
	m_DtmViewer.get()->UpdateColumnName();
	m_LasViewer.get()->UpdateColumnName();
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
}

