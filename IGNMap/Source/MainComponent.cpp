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
#include "TmsLayer.h"
#include "MvtLayer.h"
#include "WmtsTmsViewer.h"
#include "ExportImageDlg.h"
#include "ExportLasDlg.h"
#include "PrefDlg.h"
#include "SentinelViewer.h"
#include "ObjectViewer.h"
#include "ZoomViewer.h"
#include "StacViewer.h"
#include "StereoViewer.h"
#include "AffineImage.h"
#include "../../XToolGeod/XGeoPref.h"
#include "../../XToolImage/XTiffWriter.h"
#include "../../XToolAlgo/XInternetMap.h"
#include "../../XToolVector/XShapefileConverter.h"
#include "../../XToolImage/XJpegImage.h"

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

	m_AnnotViewer.reset(new AnnotViewer);
	addAndMakeVisible(m_AnnotViewer.get());
	m_AnnotViewer.get()->SetAnnot(m_MapView.get()->GetAnnot());
	m_AnnotViewer.get()->addActionListener(this);

  m_Panel.reset(new juce::ConcertinaPanel);
  addAndMakeVisible(m_Panel.get());
	m_Panel.get()->addPanel(-1, m_VectorViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_DtmViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_LasViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_ImageOptionsViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_AnnotViewer.get(), false);
	m_Panel.get()->addPanel(-1, m_SelTreeViewer.get(), false);
	
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(0), new juce::TextButton(juce::translate("Vector Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(1), new juce::TextButton(juce::translate("Image Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(2), new juce::TextButton(juce::translate("DTM Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(3), new juce::TextButton(juce::translate("LAS Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(4), new juce::TextButton(juce::translate("Image Options")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(5), new juce::TextButton(juce::translate("Annotations")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(6), new juce::TextButton(juce::translate("Selection")), true);

  // set up the layout and resizer bars..
  m_VerticalLayout.setItemLayout(0, -0.2, -1.0, -0.65);
  m_VerticalLayout.setItemLayout(1, 8, 8, 8);           // the vertical divider drag-bar thing is always 8 pixels wide
  m_VerticalLayout.setItemLayout(2, 0, -0.6, -0.35);

  m_VerticalDividerBar.reset(new juce::StretchableLayoutResizerBar(&m_VerticalLayout, 1, true));
  addAndMakeVisible(m_VerticalDividerBar.get());

  // this lets the command manager use keypresses that arrive in our window to send out commands
  addKeyListener(m_CommandManager.getKeyMappings());
	setWantsKeyboardFocus(true);

	const juce::Displays::Display* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
	if (display != nullptr) {
		juce::Rectangle< int > R = display->userArea;
		setSize((int)(R.getWidth() * 0.8), (int)(R.getHeight() * 0.8));
	}

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
	if (juce::SystemStats::getDisplayLanguage().containsIgnoreCase("fr"))
		Translate();
	RunCommandLine();
}

MainComponent::~MainComponent()
{
	disconnect();
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
		
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddWmtsServer);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddTmsServer);
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
		ImportSubMenu.addSubMenu(juce::translate("Geoplateforme (France)"), GeoportailSubMenu);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddOSM);
		ImportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuAddPlanIGN);

		menu.addSubMenu(juce::translate("Import"), ImportSubMenu);

		juce::PopupMenu ExportSubMenu;
		ExportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuExportVector);
		ExportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuExportImage);
		ExportSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuExportLas);
		menu.addSubMenu(juce::translate("Export"), ExportSubMenu);

		menu.addCommandItem(&m_CommandManager, CommandIDs::menuQuit);
	}
	else if (menuIndex == 1) // Edit
	{
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuUndo);
		menu.addSeparator();
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuShowAll);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuHideOut);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuSelectOut);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuSelectOutStrict);
		menu.addSeparator();
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuTranslate);
		//menu.addCommandItem(&m_CommandManager, CommandIDs::menuTest);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuPreferences);
	}
	else if (menuIndex == 2) // Tools
	{ 
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuSynchronize);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuToolSentinel);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuToolZoom);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuToolPanoramax);
		menu.addCommandItem(&m_CommandManager, CommandIDs::menuToolStereo);
#ifdef DEBUG
		menu.addItem(1000, "Test");
#endif // DEBUG
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
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowAnnotations);
		PanelSubMenu.addCommandItem(&m_CommandManager, CommandIDs::menuShowSelection);
		menu.addSubMenu(juce::translate("Panels"), PanelSubMenu);
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
		CommandIDs::menuShowAll, CommandIDs::menuHideOut, CommandIDs::menuSelectOut, CommandIDs::menuSelectOutStrict,
		CommandIDs::menuImportVectorFile, CommandIDs::menuImportVectorFolder, 
		CommandIDs::menuImportImageFile, CommandIDs::menuImportImageFolder, CommandIDs::menuImportDtmFile,
		CommandIDs::menuImportDtmFolder, CommandIDs::menuImportLasFile, CommandIDs::menuImportLasFolder,
		CommandIDs::menuExportVector, CommandIDs::menuExportImage, CommandIDs::menuExportLas,
		CommandIDs::menuTest, CommandIDs::menuShowSidePanel,
		CommandIDs::menuShowVectorLayers, CommandIDs::menuShowImageLayers, CommandIDs::menuShowDtmLayers, 
		CommandIDs::menuShowLasLayers, CommandIDs::menuShowSelection, CommandIDs::menuShowImageOptions, CommandIDs::menuShowAnnotations,
		CommandIDs::menuShow3DViewer, CommandIDs::menuAddOSM, CommandIDs::menuAddPlanIGN, CommandIDs::menuAddGeoportailOrthophoto,
		CommandIDs::menuAddGeoportailOrthohisto, CommandIDs::menuAddGeoportailSatellite, CommandIDs::menuAddGeoportailCartes,
		CommandIDs::menuAddGeoportailOrthophotoIRC, CommandIDs::menuAddGeoportailPlanIGN, CommandIDs::menuAddGeoportailParcelExpress,
		CommandIDs::menuAddGeoportailSCAN50Histo,
		CommandIDs::menuAddWmtsServer, CommandIDs::menuAddTmsServer, CommandIDs::menuSynchronize,
		CommandIDs::menuGoogle, CommandIDs::menuBing,
		CommandIDs::menuToolSentinel, CommandIDs::menuToolZoom, CommandIDs::menuToolPanoramax, CommandIDs::menuToolStereo,
		CommandIDs::menuHelp, CommandIDs::menuAbout };
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
		result.addDefaultKeypress('n', juce::ModifierKeys::commandModifier);
		break;
	case CommandIDs::menuQuit:
		result.setInfo(juce::translate("Quit"), juce::translate("Quit IGNMap"), "Menu", 0);
		result.addDefaultKeypress('q', juce::ModifierKeys::commandModifier);
		break;
	case CommandIDs::menuUndo:
		result.setInfo(juce::translate("Undo"), juce::translate("Undo"), "Menu", 0);
		result.addDefaultKeypress('z', juce::ModifierKeys::commandModifier);
		break;
	case CommandIDs::menuShowAll:
		result.setInfo(juce::translate("Show all"), juce::translate("Show all the objects"), "Menu", 0);
		break;
	case CommandIDs::menuHideOut:
		result.setInfo(juce::translate("Hide Frame"), juce::translate("Hide objects outside frame"), "Menu", 0); 
		break;
	case CommandIDs::menuSelectOut:
		result.setInfo(juce::translate("Hide Selection"), juce::translate("Hide objects outside selection"), "Menu", 0); 
		break;
	case CommandIDs::menuSelectOutStrict:
		result.setInfo(juce::translate("Hide Selection (strict)"), juce::translate("Hide objects outside frame (strict)"), "Menu", 0);
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
	case CommandIDs::menuAddPlanIGN:
		result.setInfo(juce::translate("Import Plan IGN"), juce::translate("Import Plan IGN"), "Menu", 0);
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
	case CommandIDs::menuAddTmsServer:
		result.setInfo(juce::translate("TMS Server"), juce::translate("TMS Server"), "Menu", 0);
		break;
	case CommandIDs::menuExportVector:
		result.setInfo(juce::translate("Export vector"), juce::translate("Export vector"), "Menu", 0);
		break;
	case CommandIDs::menuExportImage:
		result.setInfo(juce::translate("Export image"), juce::translate("Export image"), "Menu", 0);
		break;
	case CommandIDs::menuExportLas:
		result.setInfo(juce::translate("Export LAS"), juce::translate("Export LAS"), "Menu", 0);
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
	case CommandIDs::menuShowAnnotations:
		result.setInfo(juce::translate("View Annotations Panel"), juce::translate("View Annotations Panel"), "Menu", 0);
		if (m_AnnotViewer.get() != nullptr)
			result.setTicked(m_AnnotViewer.get()->isVisible());
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
	case CommandIDs::menuToolSentinel:
		result.setInfo(juce::translate("Sentinel"), juce::translate("Sentinel"), "Menu", 0);
		break;
	case CommandIDs::menuToolZoom:
		result.setInfo(juce::translate("Zoom"), juce::translate("Zoom"), "Menu", 0);
		break;
	case CommandIDs::menuToolPanoramax:
		result.setInfo(juce::translate("Panoramax"), juce::translate("Panoramax"), "Menu", 0);
		break;
	case CommandIDs::menuToolStereo:
		result.setInfo(juce::translate("Stereoscopic View"), juce::translate("Stereoscopic View"), "Menu", 0);
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
		Clear();
		juce::JUCEApplication::quit();
		break;
	case CommandIDs::menuUndo:
		m_GeoBase.ClearSelection();
		actionListenerCallback("UpdateSelectFeatures");
		break;
	case CommandIDs::menuShowAll:
		ShowHideObjects(false);
		break;
	case CommandIDs::menuHideOut:
		ShowHideObjects(true);
		break;
	case CommandIDs::menuSelectOut:
		ShowHideObjects(true, true);
		break;
	case CommandIDs::menuSelectOutStrict:
		ShowHideObjects(true, true, true);
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
	case CommandIDs::menuAddPlanIGN:
		AddMvtServer("https://data.geopf.fr/tms/1.0.0/PLAN.IGN", "pbf", "https://data.geopf.fr/annexes/ressources/vectorTiles/styles/PLAN.IGN/standard.json", 256, 256, 18);
		break;
	case CommandIDs::menuAddGeoportailOrthophoto:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS", "PM_0_19", "jpeg", 256, 256, 20);
		break;
	case CommandIDs::menuAddGeoportailOrthophotoIRC:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS.IRC", "PM_6_19", "jpeg", 256, 256, 20);
		break;
	case CommandIDs::menuAddGeoportailOrthohisto:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS.1950-1965", "PM_0_18", "png", 256, 256, 18);
		break;
	case CommandIDs::menuAddGeoportailSatellite:
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHO-SAT.SPOT.2024", "PM_0_17", "jpeg", 256, 256, 17);
		//AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS.ORTHO-EXPRESS.2024", "PM_0_19", "jpeg", 256, 256, 19);
		break;
	case CommandIDs::menuAddGeoportailCartes:
		AddWmtsServer("data.geopf.fr/private/wmts", "GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR", "PM", "jpeg", 256, 256, 16, "ign_scan_ws");
		break;
	case CommandIDs::menuAddGeoportailPlanIGN:
		AddWmtsServer("data.geopf.fr/wmts", "GEOGRAPHICALGRIDSYSTEMS.PLANIGNV2", "PM_0_19", "png", 256, 256, 19);
		break;
	case CommandIDs::menuAddGeoportailParcelExpress:
		AddWmtsServer("data.geopf.fr/wmts", "CADASTRALPARCELS.PARCELLAIRE_EXPRESS", "PM_0_19", "png", 256, 256, 19);
		break;
	case CommandIDs::menuAddGeoportailSCAN50Histo:
		AddWmtsServer("data.geopf.fr/wmts", "GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN50.1950", "PM_3_15", "jpeg", 256, 256, 15);
		break;
	case CommandIDs::menuAddWmtsServer:
		AddWmtsServer();
		break;
	case CommandIDs::menuAddTmsServer:
		AddTmsServer();
		break;
	case CommandIDs::menuExportVector:
		ExportVector();
		break;
	case CommandIDs::menuExportImage:
		ExportImage();
		break;
	case CommandIDs::menuExportLas:
		ExportLas();
		break;
	case CommandIDs::menuShowSidePanel:
		return ShowHideSidePanel();
	case CommandIDs::menuShow3DViewer:
		if (m_OGL3DViewer.get() == nullptr)
			Create3DView();
		else
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
	case CommandIDs::menuShowAnnotations:
		ShowHidePanel(m_AnnotViewer.get());
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
	case CommandIDs::menuToolSentinel: 
		OpenTool("Sentinel");
		break;
	case CommandIDs::menuToolZoom:
		OpenTool("Zoom");
		break;
	case CommandIDs::menuToolPanoramax:
		OpenPanoramax();
		break;
	case CommandIDs::menuToolStereo:
		OpenTool("Stereo");
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
	if (message == "UpdateSelection") {
		m_MapView.get()->RenderMap(true, false, false, false, false, true);
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
		XGeoVector* V = nullptr;
		if (m_GeoBase.NbSelection() > 0)
			V = m_GeoBase.Selection(0);
		for (size_t i = 0; i < m_ToolWindows.size(); i++)
			m_ToolWindows[i]->SetSelection(V);
		return;
	}
	if (message == "CloseAnnotation") {
		m_AnnotViewer.get()->Update();
		return;
	}
	if (message == "UpdatePreferences") {
		GeoTools::UpdateProjection(&m_GeoBase);
		XFrame F = m_GeoBase.Frame();
		m_MapView.get()->SetFrame(F);
		m_MapView.get()->CenterView(F.Center().X, F.Center().Y);
		return;
	}
	if (message == "ExportRepresVector") {
		juce::String filename = AppUtil::SaveFile("RepresPath", juce::translate("Save Representation file"), "*.xml;");
		if (filename.isEmpty())
			return;
		std::ofstream out(AppUtil::GetStringFilename(filename));
		if (!out.good())
			return;
		XGeoRepres repres;
		out << "<ignmap_style>" << std::endl;
		for (uint32_t i = 0; i < m_GeoBase.NbClass(); i++) {
			XGeoClass* C = m_GeoBase.Class(i);
			repres = *(C->Repres());
			repres.Name(C->Name().c_str());
			repres.XmlWrite(&out);
		}
		out << "</ignmap_style>" << std::endl;
		return;
	}
	if (message == "ImportRepresVector") {
		juce::String filename = AppUtil::OpenFile("RepresPath", juce::translate("Open Representation file"), "*.xml;");
		if (filename.isEmpty())
			return;
		XParserXML parser;
		if (!parser.Parse(AppUtil::GetStringFilename(filename)))
			return;
		std::vector<XParserXML> vec;
		parser.FindAllSubParsers("/ignmap_style/xgeorepres", &vec);
		XGeoRepres repres;
		for (uint32_t i = 0; i < vec.size(); i++) {
			if (!repres.XmlRead(&vec[i]))
				continue;
			juce::String name = repres.Name();
			for (uint32_t j = 0; j < m_GeoBase.NbClass(); j++) {
				XGeoClass* C = m_GeoBase.Class(j);
				if (name.compareIgnoreCase(C->Name()) == 0)
					*(C->Repres()) = repres;
			}
		}
		m_GeoBase.SortClass();
		m_MapView.get()->RenderMap(false, false, false, true, false, true);
		m_VectorViewer.get()->SetBase(&m_GeoBase);
		return;
	}
	if (message == "AddWmtsLayer") {
		m_MapView.get()->SetFrame(m_GeoBase.Frame());
		m_MapView.get()->RenderMap(false, true, false, false, false, true);
		m_ImageViewer.get()->SetBase(&m_GeoBase);
		m_Panel.get()->expandPanelFully(m_Panel.get()->getPanel(1), true);
		return;
	}

	juce::StringArray T;
	T.addTokens(message, ":", "");

	if (T[0] == "Update3DView") {
		if (T.size() < 5)
			return;
		if (m_OGL3DViewer.get() == nullptr)
			Create3DView();
		XFrame F;
		F.Xmin = T[1].getDoubleValue();
		F.Xmax = T[2].getDoubleValue();
		F.Ymin = T[3].getDoubleValue();
		F.Ymax = T[4].getDoubleValue();
		if (m_OGL3DViewer.get() != nullptr) {
			m_OGL3DViewer.get()->setVisible(true);
			m_OGL3DViewer.get()->toFront(true);
			m_OGL3DViewer.get()->LoadObjects(&m_GeoBase, &F);
			m_OGL3DViewer.get()->SetQuickLook(m_MapView.get()->GetSelImage());
			m_OGL3DViewer.get()->SetTarget(m_MapView.get()->GetTarget());
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
	if (T[0] == "SetSelectionFrame") {
		if (T.size() < 5)
			return;
		XFrame F;
		F.Xmin = T[1].getDoubleValue();
		F.Xmax = T[2].getDoubleValue();
		F.Ymin = T[3].getDoubleValue();
		F.Ymax = T[4].getDoubleValue();
		m_MapView.get()->SetSelectionFrame(F);
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
		XPt3D target;
		target.X = T[1].getDoubleValue();
		target.Y = T[2].getDoubleValue();
		target.Z = T[3].getDoubleValue();
		if (m_OGL3DViewer.get() != nullptr)
			m_OGL3DViewer.get()->SetTarget(target);
		if (m_MapView.get() != nullptr)
			m_MapView.get()->SetTarget(target, false);
		for (size_t i = 0; i < m_ToolWindows.size(); i++) {
			m_ToolWindows[i]->SetTarget(target.X, target.Y, target.Z);
			if (m_ToolWindows[i]->NeedTargetImage())
				if (m_MapView.get() != nullptr)
					m_ToolWindows[i]->SetTargetImage(m_MapView.get()->GetTargetImage());
		}
	}
	if (T[0] == "UpdateTargetPoly") {
		if (((T.size() - 1) % 3 != 0) || (T.size() == 1))
			return;
		std::vector<XPt3D> target;
		for (size_t i = 0; i < (T.size() - 1) / 3; i++) {
			target.push_back(XPt3D(T[3*i + 1].getDoubleValue(), T[3*i + 2].getDoubleValue(), T[3*i + 3].getDoubleValue()));
		}
		if (m_MapView.get() != nullptr)
			m_MapView.get()->SetTargetPoly(target, false);
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
	if (T[0] == "AddImageInObject") {
		if (T.size() < 1)
			return;
		int index = T[1].getIntValue();
		if (!GeoTools::AddImageInObect(&m_GeoBase, index))
			return;
		actionListenerCallback("UpdateSelectFeatures");
		m_MapView.get()->SetFrame(m_GeoBase.Frame());
		m_MapView.get()->RenderMap(false, true, false, false, false, true);
		m_ImageViewer.get()->SetBase(&m_GeoBase);
	}
	if (T[0] == "Properties") {
		if (T.size() < 2)
			return;
		int index = T[2].getIntValue();
		if (T[1] == "Object")
			ShowProperties((uint32_t)index, true);
		if (T[1] == "Class")
			ShowProperties((uint32_t)index, false);
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
	switch (tlb->getItemId()) {
		case MainComponentToolbarFactory::Move: m_MapView.get()->SetMouseMode(MapView::Move); break;
		case MainComponentToolbarFactory::Select: m_MapView.get()->SetMouseMode(MapView::Select); break;
		case MainComponentToolbarFactory::Zoom: m_MapView.get()->SetMouseMode(MapView::Zoom); break;
		case MainComponentToolbarFactory::Select3D: m_MapView.get()->SetMouseMode(MapView::Select3D); break;
		case MainComponentToolbarFactory::Polyline: m_MapView.get()->SetMouseMode(MapView::Polyline); break;
		case MainComponentToolbarFactory::Polygone: m_MapView.get()->SetMouseMode(MapView::Polygone); break;
		case MainComponentToolbarFactory::Rectangle: m_MapView.get()->SetMouseMode(MapView::Rectangle); break;
		case MainComponentToolbarFactory::Text: m_MapView.get()->SetMouseMode(MapView::Text); break;
		case MainComponentToolbarFactory::Gsd: m_MapView.get()->ZoomGsd(tlb->getButtonText().getDoubleValue()); break;
		case MainComponentToolbarFactory::Search: Search(tlb->getButtonText()); tlb->setButtonText(""); break;
		case MainComponentToolbarFactory::Layer: SetDefaultLayers(tlb->getButtonText()); tlb->setButtonText(""); break;
		case MainComponentToolbarFactory::Scale:
		{
			juce::String scale = tlb->getButtonText();
			tlb->setButtonText("");
			if (scale.isNotEmpty()) {
				if (scale == "Total")
					m_MapView.get()->ZoomWorld();
				else
					m_MapView.get()->ZoomScale(scale.getDoubleValue());
			}
			break;
		}
	}
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
	ClearSearch();
	gWmtsTmsViewerMgr.RemoveAll();
	for (size_t i = 0; i < m_ToolWindows.size(); i++)
		delete m_ToolWindows[i];
	m_ToolWindows.clear();
}

void MainComponent::ClearSearch()
{
	for (size_t i = 0; i < m_Search.size(); i++)
		delete m_Search[i];
	m_Search.clear();
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
// Gestion de la ligne de commande eventuelle
//==============================================================================
void MainComponent::RunCommandLine()
{
	juce::StringArray T = juce::JUCEApplication::getCommandLineParameterArray();
	if (T.size() == 0) return;
	if (T.size() <= 1) {
		filesDropped(T, 0, 0);
		return;
	}
	int index = 0;
	while (index < T.size()) {
		if (T[index] == "-i") { // Input d'un fichier image
			index++;
			if (index >= T.size())	// Nom du fichier image absent
				break;
			ImportImageFile(T[index]);
			continue;
		}
		if (T[index] == "-v") { // Input d'un fichier vectoriel
			index++;
			if (index >= T.size())	// Nom du fichier vectoriel absent
				break;
			ImportVectorFile(T[index]);
			continue;
		}
		if (T[index] == "-r") { // Input d'un fichier image avec rotation
			index++;
			if (T.size() < index + 4)	// Parametres absents
				break;
			juce::String filename = T[index];
			double X = T[index + 1].getDoubleValue(), Y = T[index + 2].getDoubleValue();
			double gsd = T[index + 3].getDoubleValue(), rot = 180. - T[index + 4].getDoubleValue();
			GeoTools::AddRotationImage(&m_GeoBase, filename, rot * XPI / 180., X, Y, gsd);
			index += 4;
			continue;
		}
		return;	// Si on arrive la, c'est qu'il y a un probleme ...
	};
	if (m_GeoBase.NbClass() == 1) {
		XGeoClass* C = m_GeoBase.Class(0);
		if (C != nullptr) {
			if (C->NbVector() == 1) {
				m_GeoBase.SelectFeature(C->Vector((uint32_t)0));
				ShowProperties(0, true);
			}
		}
	}
	ShowHideSidePanel();
	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->ZoomWorld();
}

//==============================================================================
// Bascule pour afficher ou cacher le panneau lateral
//==============================================================================
bool MainComponent::ShowHideSidePanel()
{
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
	return true;
}

//==============================================================================
// A propos
//==============================================================================
void MainComponent::AboutIGNMap()
{
	juce::String version = "0.1.4";
	juce::String info = "Compilation : " + juce::String(__DATE__) + ", " + juce::String(__TIME__);
	juce::String message = "IGNMap 3 Version : " + version + "\n\n" + info + "\n\n";
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
	if (!image->AnalyzeImage(AppUtil::GetStringFilename(filename))) {
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
// Import d'un repertoire de fichiers MNT
//==============================================================================
void MainComponent::ImportDtmFolder()
{
	juce::String folderName = AppUtil::OpenFolder("DtmFolderPath");
	if (folderName.isEmpty())
		return;
	if (ImportDataFolder(folderName, XGeoVector::DTM) != nullptr) {
		m_DtmViewer.get()->SetBase(&m_GeoBase);
		m_MapView.get()->RenderMap(false, false, true, false, false, true);
	}
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
	if (!dtm->OpenDtm(AppUtil::GetStringFilename(filename).c_str(), tmpFile.getFullPathName().toStdString().c_str())) {
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
	XGeoClass* C = GeoTools::ImportDataFolder(folderName, &m_GeoBase, type);
	if (C != nullptr)
		m_MapView.get()->SetFrame(m_GeoBase.Frame());
	return C;
}

//==============================================================================
// Import d'un repertoire de fichiers LAS/LAZ
//==============================================================================
void MainComponent::ImportLasFolder()
{
	juce::String folderName = AppUtil::OpenFolder("LasFolderPath");
	if (folderName.isEmpty())
		return;
	if(ImportDataFolder(folderName, XGeoVector::LAS) != nullptr) {
		m_LasViewer.get()->SetBase(&m_GeoBase); // Le LasViewer appele la mise a jour de la vue
		m_MapView.get()->RenderMap(false, false, false, false, true, true);
	}
}

//==============================================================================
// Ajout d'une couche LAS
//==============================================================================
bool MainComponent::ImportLasFile(juce::String lasfile)
{
	juce::String filename = lasfile;
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("LasPath", juce::translate("Open LAS file"), "*.las;*.laz;*.copc;");
	if (filename.isEmpty())
		return false;
	juce::File file(filename);
	juce::String name = file.getFileNameWithoutExtension();

	GeoLAS* las = new GeoLAS;
	
	if (!las->Open(AppUtil::GetStringFilename(filename))) {
		delete las;
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "IGNMap",
			filename + juce::translate(" : this file cannot be opened"), "OK");
		return false;
	}

	if (!GeoTools::RegisterObject(&m_GeoBase, las, name.toStdString().c_str(), "LAS", name.toStdString().c_str())) {
		delete las;
		return false;
	}

	m_LasViewer.get()->SetBase(&m_GeoBase); // Le LasViewer appele la mise a jour de la vue
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
	gWmtsTmsViewerMgr.AddWmtsViewer("WMTS", this, &m_GeoBase);
	return true;
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

	WmtsLayerWebMerc* wmts = new WmtsLayerWebMerc(server, layer, TMS, format, tileW, tileH, max_zoom, apikey);
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
// Ajout d'un serveur TMS
//==============================================================================
bool MainComponent::AddTmsServer()
{
	gWmtsTmsViewerMgr.AddTmsViewer("TMS", this, &m_GeoBase);
	return true;
}

//==============================================================================
// Ajout d'un serveur Mapbox Vector Tile
//==============================================================================
bool MainComponent::AddMvtServer(std::string url, std::string ext, std::string style, uint32_t tileW, uint32_t tileH, uint32_t max_zoom)
{
	XGeoPref pref;
	XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

	MvtLayer* mvt = new MvtLayer(url, ext, tileW, tileH, max_zoom);
	mvt->SetFrame(F);
	mvt->LoadStyle(style);
	if (!GeoTools::RegisterObject(&m_GeoBase, mvt, "MVT", "MVT", url)) {
		delete mvt;
		return false;
	}

	m_MapView.get()->SetFrame(m_GeoBase.Frame());
	m_MapView.get()->RenderMap(false, true, false, false, false, true);
	m_ImageViewer.get()->SetBase(&m_GeoBase);
	return true;
}

//==============================================================================
// Export vectoriel
//==============================================================================
bool MainComponent::ExportVector()
{
	juce::String foldername = AppUtil::OpenFolder("ExportVector", juce::translate("Destination folder"));
	if (foldername.isEmpty())
		return false;
	for (uint32_t i = 0; i < m_GeoBase.NbLayer(); i++) {
		XGeoLayer* layer = m_GeoBase.Layer(i);
		if (!layer->Visible())
			continue;
		bool visible = false;
		for (uint32_t j = 0; j < layer->NbClass(); j++) {
			XGeoClass* C = layer->Class(j);
			if (C->Visible()) {
				visible = true;
				break;
			}
		}
		if (!visible)
			continue;
		juce::String layer_folder = foldername + juce::File::getSeparatorString() + layer->Name();
		juce::File folder(layer_folder);
		folder.createDirectory();
	}
	
	class ExportTask : public GeoTask {
	public:
		XGeoBase* Base;
		std::string Foldername;
		void run() {
			XShapefileConverter converter;
			converter.ConvertVisibleOnly(true);
			converter.ConvertBase(Base, Foldername.c_str(), this);
		}
	};
	ExportTask task;
	task.Base = &m_GeoBase;
	task.Foldername = foldername.toStdString();
	task.runThread();
	
	juce::File(foldername).revealToUser();
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
	dlg->addActionListener(this);
	juce::DialogWindow::LaunchOptions options;
	options.content.setOwned(dlg);
	options.content->setSize(410, 300);
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
	options.content->setSize(410, 300);
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
	m_AnnotViewer.get()->Translate();
	m_ToolbarFactory.Translate();
	for (int i = 0; i < m_Panel.get()->getNumPanels(); i++)
		m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(i), nullptr, true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(0), new juce::TextButton(juce::translate("Vector Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(1), new juce::TextButton(juce::translate("Image Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(2), new juce::TextButton(juce::translate("DTM Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(3), new juce::TextButton(juce::translate("LAS Layers")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(4), new juce::TextButton(juce::translate("Image Options")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(5), new juce::TextButton(juce::translate("Annotations")), true);
	m_Panel.get()->setCustomPanelHeader(m_Panel.get()->getPanel(6), new juce::TextButton(juce::translate("Selection")), true);
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
	AddMvtServer("https://data.geopf.fr/tms/1.0.0/IGNF_NUAGES-DE-POINTS-LIDAR-HD-produit", "pbf", "", 256, 256, 16);

	/* Creation d'un differentiel MNS*/
	/*
	XFrame F = m_MapView.get()->GetViewFrame();
	int Xmin = XRint(F.Xmin);
	int Xmax = XRint(F.Xmax);
	int Ymin = XRint(F.Ymin);
	int Ymax = XRint(F.Ymax);
	int step = 5;
	int nbX = (Xmax - Xmin) / step;
	int nbY = (Ymax - Ymin) / step;
	double* ZminA = new double[nbX * nbY];
	double* ZmaxA = new double[nbX * nbY];
	double* ZminB = new double[nbX * nbY];
	double* ZmaxB = new double[nbX * nbY];
	for (int i = 0; i < nbX * nbY; i++)
		ZminA[i] = ZmaxA[i] = ZminB[i] = ZmaxB[i] = -9999.;

	bool first_pass = true;
	double zmin, zmax, zmean;
	XFrame cell;
	for (uint32_t i = 0; i < m_GeoBase.NbClass(); i++) {
		XGeoClass* C = m_GeoBase.Class(i);
		if (C == nullptr)
			continue;
		if (!C->IsDTM())
			continue;
		if (!C->Visible())
			continue;
		for (uint32_t j = 0; j < C->NbVector(); j++) {
			GeoDTM* dtm = (GeoDTM*)C->Vector(j);
			if (!dtm->Visible())
				continue;
			if (!F.Intersect(dtm->Frame()))
				continue;
			for (int line = 0; line < nbY; line++) {
				for (int col = 0; col < nbX; col++) {
					cell.Xmin = Xmin + col * step;
					cell.Xmax = Xmin + (col + 1) * step;
					cell.Ymax = Ymax - line * step;
					cell.Ymin = Ymax - (line + 1) * step;
					dtm->ZFrame(cell, &zmax, &zmin, &zmean);
					if (first_pass) {
						ZminA[line * nbX + col] = zmin;
						ZmaxA[line * nbX + col] = zmax;
					}
					else {
						ZminB[line * nbX + col] = zmin;
						ZmaxB[line * nbX + col] = zmax;
					}
				}
			}
		}
		first_pass = false;
	}

	XTiffWriter tiff;
	tiff.SetGeoTiff(Xmin, Ymax, step);
	float* buf = new float[nbX * nbY];
	for (int i = 0; i < nbX * nbY; i++)
		buf[i] = 0.f;
	
	for (int line = 0; line < nbY; line++) {
		for (int col = 0; col < nbX; col++) {
			if ((ZminA[line * nbX + col] < -999.) || (ZminB[line * nbX + col] < -999.))
				continue;

			if (ZmaxA[line * nbX + col] < ZminB[line * nbX + col]) {
				buf[line * nbX + col] = ZminB[line * nbX + col] - ZmaxA[line * nbX + col];
			}
			if (ZmaxB[line * nbX + col] < ZminA[line * nbX + col]) {
				buf[line * nbX + col] = ZminA[line * nbX + col] - ZmaxB[line * nbX + col];
			}
		}
	}

	tiff.Write("c:\\Temp\\test_DCHAN.tif", nbX, nbY, 1, 32, (uint8_t*)buf);

	delete[] buf;
	delete[] ZminA;
	delete[] ZminB;
	delete[] ZmaxA;
	delete[] ZmaxB;
	*/

	/* Creation d'anaglyphe
	XFrame F = m_MapView.get()->GetViewFrame();
	juce::Image image = m_MapView.get()->GetImage();
	double gsd = m_MapView.get()->GetGsd();
	double X0 = F.Xmin, Y0 = F.Ymax;
	uint32_t w = image.getWidth(), h = image.getHeight();
	float* grid = new (std::nothrow) float[w * h];
	if (grid == nullptr)
		return;
	for (uint32_t i = 0; i < w * h; i++)
		grid[i] = -1000.f;
	if (!GeoTools::ComputeZGrid(&m_GeoBase, grid, w, h, &F)) {
		delete[] grid;
		return;
	}

	juce::Image::BitmapData bitmap(image, juce::Image::BitmapData::readWrite);

	float Z0 = std::numeric_limits<float>::max(), zMax = std::numeric_limits<float>::min();
	for (uint32_t i = 0; i < w * h; i++) {
		if (grid[i] < -999.)
			continue;
		Z0 = XMin(Z0, grid[i]);
		zMax = XMax(zMax, grid[i]);
	}
	//juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, "Z : ", juce::String(Z0, 2) + " : " + juce::String(zMax, 2), "OK");

	double SL_X = F.Xmin, SL_Y = F.Center().Y, SL_Z = zMax + 1 * F.Width();
	double SR_X = F.Xmax, SR_Y = F.Center().Y, SR_Z = zMax + 1 * F.Width();
	uint8_t* ima_buf = new (std::nothrow) uint8_t[w * h * 3];
	if (ima_buf == nullptr) {
		delete[] grid;
		return;
	}
	memset(ima_buf, 0, w * h * 3);

	double k, xM, yM, zM, zMB, xT, yT;
	int u, v, pos;
	uint32_t factor = 4, zFactor = 1;
	juce::Colour color;
	for (int i = 0; i < h * factor; i++) {
		for (int j = factor * w - 1; j >= 0; j--) {
			zM = grid[(i / factor) * w + j / factor] * zFactor;
			
			//if ((j / factor - 1) >= 0) {
			//	zMB = grid[i * w + j / factor - 1];
			//	zM = (zM * (factor - j % factor) + zMB * (j % factor)) / factor;
			//}
			xM = X0 + (j * gsd) / factor;
			yM = Y0 - (i * gsd) / factor;
			k = (Z0 - SL_Z) / (zM - SL_Z);
			xT = SL_X + (xM - SL_X) * k;
			yT = SL_Y + (yM - SL_Y) * k;
			u = XRint((xT - X0) / gsd);
			v = XRint((Y0 - yT) / gsd);
			if ((u >= 0) && (u < w) && (v >= 0) && (v < h)) {
				pos = v * w * 3 + u * 3;
				color = bitmap.getPixelColour(j / factor, i / factor);
				ima_buf[pos] = color.getRed();
				ima_buf[pos+1] = color.getGreen();
				ima_buf[pos + 2] = color.getBlue();
			}
		}
	}
	XTiffWriter tiffL;
	tiffL.SetGeoTiff(X0, Y0, gsd);
	tiffL.Write("C:\\TEMP\\Ima_L.tif", w, h, 3, 8, ima_buf);

	memset(ima_buf, 0, w * h * 3);
	for (uint32_t i = 0; i < h * factor; i++) {
		for (uint32_t j = 0; j < factor * w; j++) {
			zM = grid[(i / factor) * w + j / factor] * zFactor;
			//if ((j / factor + 1) < w) {
			//	zMB = grid[i * w + j / factor + 1];
			//	zM = (zM * (factor - j % factor) + zMB * (j % factor)) / factor;
			//}
			xM = X0 + (j * gsd) / factor;
			yM = Y0 - (i * gsd) / factor;
			k = (Z0 - SR_Z) / (zM - SR_Z);
			xT = SR_X + (xM - SR_X) * k;
			yT = SR_Y + (yM - SR_Y) * k;
			u = XRint((xT - X0) / gsd);
			v = XRint((Y0 - yT) / gsd);
			if ((u >= 0) && (u < w) && (v >= 0) && (v < h)) {
				pos = v * w * 3 + u * 3;
				color = bitmap.getPixelColour(j / factor, i / factor);
				ima_buf[pos] = color.getRed();
				ima_buf[pos + 1] = color.getGreen();
				ima_buf[pos + 2] = color.getBlue();
			}
		}
	}
	XTiffWriter tiffR;
	tiffR.SetGeoTiff(X0, Y0, gsd);
	tiffR.Write("C:\\TEMP\\Ima_R.tif", w, h, 3, 8, ima_buf);

	delete[] ima_buf;
	delete[] grid;

	StereoViewer* viewer = dynamic_cast<StereoViewer*>(OpenTool("Stereo"));
	if (viewer != nullptr) {
		viewer->OpenImage("C:\\TEMP\\Ima_L.tif", true, 0);
		viewer->OpenImage("C:\\TEMP\\Ima_R.tif", false, 0);
		viewer->SetPseudoOrientation(XPt3D(SL_X, SL_Y, SL_Z), XPt3D(SR_X, SR_Y, SR_Z), XPt3D(X0, Y0, Z0), gsd);
	}

	*/ // Fin creation d'anaglyphe

	/*
	std::ofstream ori;
	ori.open("C:\\TEMP\\ignmap.ori", std::ios::out);
	ori.setf(std::ios::fixed); ori.precision(2);
	ori << X0 << " " << Y0 << " " << Z0 << " " << gsd << std::endl;
	ori << "Ima_L.tif " << SL_X << " " << SL_Y << " " << SL_Z << std::endl;
	ori << "Ima_R.tif " << SR_X << " " << SR_Y << " " << SR_Z << std::endl;

	return;
	*/

	/* Recherche d'un Z sur la Geoplateforme
	GeoSearch search;
	XPt3D P = m_MapView.get()->GetTarget();
	XGeoPref pref;
	double lon, lat;
	pref.ConvertDeg(pref.Projection(), XGeoProjection::RGF93, P.X, P.Y, lon, lat);
	double z = search.GetAltitude(lon, lat);
	juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,"Altitude", juce::String(z, 2), "OK");
	return;
	*/

	/*
	juce::String filename;
	if (filename.isEmpty())
		filename = AppUtil::OpenFile("RasterPath");
	if (filename.isEmpty())
		return;
	XJpegImage image;
	image.Open(AppUtil::GetStringFilename(filename));
	*/

	/*
	juce::StringArray T = juce::JUCEApplication::getCommandLineParameterArray();
	if (T.size() < 5)
		return;
	if (T[0] != "-ortho")
		return;
	double X0 = T[1].getDoubleValue();
	double Y0 = T[2].getDoubleValue();
	double gsd = T[3].getDoubleValue();
	juce::String filename = T[4];

	XGeoPref pref;
	pref.Projection(XGeoProjection::Lambert93);
	XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
	pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

	WmtsLayerWebMerc* wmts = new WmtsLayerWebMerc("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS", "PM", "jpeg", 256, 256, 20);
	wmts->SetFrame(F);
	XFrame exportFrame(X0, Y0 - 1000., X0 + 1000., Y0);
	juce::Image image = wmts->GetAreaImage(exportFrame, gsd);

	juce::File file(filename);
	juce::FileOutputStream stream(file);
	juce::PNGImageFormat png;
	png.writeImageToStream(image, stream);
	
	juce::JUCEApplication::quit();
	*/
}

//==============================================================================
// Affiche les proprietes d'un objet
//==============================================================================
void MainComponent::ShowProperties(uint32_t index, bool typeVector)
{
	XGeoObject* obj = nullptr;
	if (typeVector)
		obj = m_GeoBase.Selection(index);
	else 
		obj = m_GeoBase.Class(index);
	if (obj == nullptr)
		return;
	juce::String title = obj->Name();
	if (title.isEmpty()) title = "ObjectViewer";
	ObjectViewer* viewer = nullptr;
	for (size_t i = 0; i < m_ToolWindows.size(); i++) {
		viewer = dynamic_cast<ObjectViewer*>(m_ToolWindows[i]);
		if (viewer != nullptr) {
			m_ToolWindows[i]->setVisible(true);
			m_ToolWindows[i]->toFront(true);
			m_ToolWindows[i]->setTitle(title);
			break;
		}
	}
	if (viewer == nullptr) {
		viewer = new ObjectViewer(title, juce::Colours::grey, juce::DocumentWindow::allButtons, this);
		viewer->setVisible(true);
		m_ToolWindows.push_back(viewer);
	}
	viewer->SetSelection(obj);
}

//==============================================================================
// Gestion de l'affichage des objets hors cadre / hors selection
//==============================================================================
void MainComponent::ShowHideObjects(bool hide, bool select, bool strict)
{
	juce::MouseCursor::showWaitCursor();
	if (!hide) 	// On affiche tout
		m_GeoBase.ShowAll();
	else {
		if (select) { // On cache les objets dans la selection
			if (m_GeoBase.NbSelection() > 0) {
				XGeoVector* V = m_GeoBase.Selection(0);
				if (strict)
					m_GeoBase.HideOutStrict(V);
				else
					m_GeoBase.HideOut(V);
			}
		}
		else {	// On cache les objets hors cadre
			XFrame F = m_MapView.get()->GetSelectionFrame();
			if (F.IsEmpty())
				F = m_MapView.get()->GetViewFrame();
			m_GeoBase.HideOut(&F);
		}
	}
	m_MapView.get()->RenderMap(true, true, true, true, true, true);
	juce::MouseCursor::hideWaitCursor();
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
	createPipe("IGNMap", -1, false);
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
		if (T.size() < 4)
			return;
		double X = T[1].getDoubleValue();
		double Y = T[2].getDoubleValue();
		double Z = T[3].getDoubleValue();
		m_MapView.get()->SetTarget(XPt3D(X, Y, Z), false);
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
		if (ext.equalsIgnoreCase(".jp2") || ext.equalsIgnoreCase(".tif") || ext.equalsIgnoreCase(".cog") || ext.equalsIgnoreCase(".webp") || ext.equalsIgnoreCase(".jpg"))
			ImportImageFile(filename);
		if (ext.equalsIgnoreCase(".las") || ext.equalsIgnoreCase(".laz") || ext.equalsIgnoreCase(".copc"))
			ImportLasFile(filename);
		if (ext.equalsIgnoreCase(".shp") || ext.equalsIgnoreCase(".mif") || ext.equalsIgnoreCase(".gpkg") || ext.equalsIgnoreCase(".json")
			|| ext.equalsIgnoreCase(".xml"))
			ImportVectorFile(filename);
		if (ext.equalsIgnoreCase(".asc"))
			ImportDtmFile(filename);
	}
}

//==============================================================================
// Recherche d'un lieu
//==============================================================================
void MainComponent::Search(juce::String query)
{
	if (query.length() < 3)
		return;
	GeoSearch* search = new GeoSearch;
		if (!search->Search(query)) {
			delete search;
			return;
		}
	if (!ImportVectorFile(search->Filename())) {
		delete search;
		return;
	}
	m_Search.push_back(search);
	m_VectorViewer.get()->RenameAndViewLastClass(query);
}

//==============================================================================
// Fixe les couches par defaut
//==============================================================================
void MainComponent::SetDefaultLayers(juce::String layers)
{
	if (layers.isEmpty())
		return;
	m_MapView.get()->setMouseCursor(juce::MouseCursor(juce::MouseCursor::WaitCursor));
	m_GeoBase.ClearSelection();
	actionListenerCallback("UpdateSelectFeatures");
	if (layers == "Empty") {
		m_GeoBase.RemoveClass("MVT", "https://data.geopf.fr/tms/1.0.0/PLAN.IGN");
		m_GeoBase.RemoveClass("WMTS", "ORTHOIMAGERY.ORTHOPHOTOS");
		m_ImageViewer.get()->SetBase(&m_GeoBase);
		actionListenerCallback("UpdateRaster");
	}
	if (layers == "Ortho") {
		m_GeoBase.RemoveClass("MVT", "https://data.geopf.fr/tms/1.0.0/PLAN.IGN");
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS", "PM_0_19", "jpeg", 256, 256, 20);
	}
	if (layers == "Carto") {
		m_GeoBase.RemoveClass("WMTS", "ORTHOIMAGERY.ORTHOPHOTOS");
		AddMvtServer("https://data.geopf.fr/tms/1.0.0/PLAN.IGN", "pbf", 
			"https://data.geopf.fr/annexes/ressources/vectorTiles/styles/PLAN.IGN/standard.json", 256, 256, 18);
	}
	if (layers == "Ortho+Carto") {
		AddWmtsServer("data.geopf.fr/wmts", "ORTHOIMAGERY.ORTHOPHOTOS", "PM_0_19", "jpeg", 256, 256, 20);
		m_GeoBase.RemoveClass("MVT", "https://data.geopf.fr/tms/1.0.0/PLAN.IGN");
		AddMvtServer("https://data.geopf.fr/tms/1.0.0/PLAN.IGN", "pbf",
			"https://data.geopf.fr/annexes/ressources/vectorTiles/styles/PLAN.IGN/toponymes.json", 256, 256, 18);
	}
	
	m_MapView.get()->setMouseCursor(juce::MouseCursor(juce::MouseCursor::NormalCursor));
}


//==============================================================================
// Ouverture d'une fenetre outil
//==============================================================================
ToolWindow* MainComponent::OpenTool(juce::String toolName)
{
	for (size_t i = 0; i < m_ToolWindows.size(); i++) {
		if (m_ToolWindows[i]->getName() == toolName) {
			m_ToolWindows[i]->setVisible(true);
			m_ToolWindows[i]->toFront(true);
			return m_ToolWindows[i];
		}
	}
	ToolWindow* tool = nullptr;
	if (toolName == "Sentinel")
		tool = new SentinelViewer("Sentinel", juce::Colours::grey, juce::DocumentWindow::allButtons, this, &m_GeoBase);
	if (toolName == "Zoom")
		tool = new ZoomViewer("Zoom", juce::Colours::grey, juce::DocumentWindow::allButtons, this, &m_GeoBase);
	if (toolName == "Panoramax")
		tool = new StacViewer("Panoramax", juce::Colours::grey, juce::DocumentWindow::allButtons, this, &m_GeoBase);
	if (toolName == "Stereo")
		tool = new StereoViewer("Stereo", juce::Colours::grey, juce::DocumentWindow::allButtons, this, &m_GeoBase);

	if (tool != nullptr) {
		tool->setVisible(true);
		m_ToolWindows.push_back(tool);
		return tool;
	}

	return nullptr;
}

//==============================================================================
// Ouverture de l'outil Panoramax
//==============================================================================
void MainComponent::OpenPanoramax()
{
	/* Connexion Panoramax */
	std::string url = "https://panoramax.ign.fr/api/map";
	if (m_GeoBase.Class("Panoramax", url.c_str()) == nullptr) {
		XGeoPref pref;
		XFrame F, geoF = XGeoProjection::FrameGeo(pref.Projection());
		pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmin, geoF.Ymin, F.Xmin, F.Ymin);
		pref.ConvertDeg(XGeoProjection::RGF93, pref.Projection(), geoF.Xmax, geoF.Ymax, F.Xmax, F.Ymax);

		std::string ext = "mvt";
		std::string style = "https://panoramax.ign.fr/api/map/style.json";
		int zoom = 15;

		MvtLayer* mvt = new MvtLayer(url, ext, 256, 256, zoom);
		mvt->SetFrame(F);
		mvt->LoadStyle(style);
		if (!GeoTools::RegisterObject(&m_GeoBase, mvt, "Panoramax", "Panoramax", url)) {
			delete mvt;
			return;
		}

		m_MapView.get()->SetFrame(m_GeoBase.Frame());
		m_MapView.get()->RenderMap(false, true, false, false, false, true);
		m_ImageViewer.get()->SetBase(&m_GeoBase);
	}
	
	OpenTool("Panoramax");
}