//-----------------------------------------------------------------------------
//								AppUtil.cpp
//								===========
//
// Fonctions utilitaires
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 19/01/2024
//-----------------------------------------------------------------------------

#include "AppUtil.h"
#include <filesystem>

//==============================================================================
// Choix d'un repertoire
//==============================================================================
juce::String AppUtil::OpenFolder(juce::String optionName, juce::String mes)
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
juce::String AppUtil::OpenFile(juce::String optionName, juce::String mes, juce::String filter)
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
// Sauvegarde d'un fichier
//==============================================================================
juce::String AppUtil::SaveFile(juce::String optionName, juce::String mes, juce::String filter)
{
	juce::String path = optionName, message = mes, filters = filter;
	if (!optionName.isEmpty())
		path = GetAppOption(optionName);
	if (mes.isEmpty())
		message = juce::translate("Save a file...");
	if (filter.isEmpty())
		filters = "*";

	juce::FileChooser fc(message, path, filters, true);
	if (fc.browseForFileToSave(true)) {
		auto result = fc.getURLResult();
		auto name = result.isLocalFile() ? result.getLocalFile().getFullPathName() : result.toString(true);

		if (!optionName.isEmpty())
			SaveAppOption(optionName, name);
		return name;
	}
	return "";
}

//==============================================================================
// Gestion des options de l'application
//==============================================================================
juce::String AppUtil::GetAppOption(juce::String name)
{
	juce::PropertiesFile::Options options;
	options.applicationName = "IGNMapv3";
	options.osxLibrarySubFolder = "Application Support";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	return file->getValue(name);
}

void AppUtil::SaveAppOption(juce::String name, juce::String value)
{
	juce::PropertiesFile::Options options;
	options.applicationName = "IGNMapv3";
	options.osxLibrarySubFolder = "Application Support";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	file->setValue(name, value);
	app.saveIfNeeded();
}

//==============================================================================
// Permet de recuperer un nom de fichier sous forme de st:string en reglant les problemes UTF8 ou pas ...
//==============================================================================
std::string AppUtil::GetStringFilename(juce::String filename)
{
	std::string pathname = filename.toStdString();
#ifdef _MSC_VER
	std::filesystem::path path(filename.toWideCharPointer());
	pathname = path.string();
#endif
	return pathname;
}