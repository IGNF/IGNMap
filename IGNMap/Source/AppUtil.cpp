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
#ifdef JUCE_WINDOWS
		name = "\\\\?\\" + name;
#endif // JUCE_WINDOWS
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
	options.applicationName = juce::JUCEApplication::getInstance()->getApplicationName();
	options.osxLibrarySubFolder = "Application Support";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	return file->getValue(name);
}

void AppUtil::SaveAppOption(juce::String name, juce::String value)
{
	juce::PropertiesFile::Options options;
	options.applicationName = juce::JUCEApplication::getInstance()->getApplicationName();
	options.osxLibrarySubFolder = "Application Support";
	juce::ApplicationProperties app;
	app.setStorageParameters(options);
	juce::PropertiesFile* file = app.getUserSettings();
	file->setValue(name, value);
	app.saveIfNeeded();
}

//==============================================================================
// Permet de recuperer un nom de fichier sous forme de std:string en reglant les problemes UTF8 ou pas ...
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

//==============================================================================
// Sauvegarde un composant sous forme d'image PNG
//==============================================================================
void AppUtil::SaveComponent(juce::Component* component)
{
	juce::String filename = AppUtil::SaveFile("ComponentImagePath", juce::translate("Save Image"), "*.png;");
	if (filename.isEmpty())
		return;
	juce::Image image = component->createComponentSnapshot(component->getLocalBounds());
	juce::File file(filename);
	if (file.existsAsFile())
		file.deleteFile();
	juce::FileOutputStream outputFileStream(file);
	juce::PNGImageFormat png;
	png.writeImageToStream(image, outputFileStream);
	file.revealToUser();
}

//==============================================================================
// Sauvegarde une table sous forme d'image PNG
//==============================================================================
void AppUtil::SaveTableComponent(juce::TableListBox* table)
{
	auto b = table->getLocalBounds();
	int w = table->getHeader().getTotalWidth() + 10;
	int h = table->getNumRows() * table->getRowHeight() + table->getHeader().getHeight() + 10;
	table->setSize(w, h);
	AppUtil::SaveComponent(table);
	table->setSize(b.getWidth(), b.getHeight());
}

//==============================================================================
// Telechargement d'un fichier
//==============================================================================
juce::String AppUtil::DownloadFile(const juce::String& request, const juce::String& filename, int nb_try, int timeout)
{
	juce::URL url(request);
	juce::URL::DownloadTaskOptions options;
	std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(filename, options);
	if (task.get() == nullptr)
		return "";
	int count = 0, max_count = nb_try;
	if (max_count == 0) max_count = DownloadNbTry;
	while (task.get()->isFinished() == false)
	{
		count++;
		if (count > max_count) break;
		juce::Thread::sleep(timeout);
	}
	if (task.get()->hadError())
		return "";
	if (task.get()->statusCode() >= 400)
		return "";
	return filename;
}

//==============================================================================
// Clic sur le bouton
//==============================================================================
void ColourChangeButton::clicked()
{
	auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
		| juce::ColourSelector::showColourAtTop
		| juce::ColourSelector::editableColour
		| juce::ColourSelector::showSliders
		| juce::ColourSelector::showColourspace);

	colourSelector->setName("background");
	colourSelector->setCurrentColour(findColour(juce::TextButton::buttonColourId));
	colourSelector->addChangeListener(this);
	colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
	colourSelector->setSize(300, 400);

	juce::CallOutBox::launchAsynchronously(std::move(colourSelector), getScreenBounds(), nullptr);
}

//==============================================================================
// FieldEditor : composant pour remplir un champ
//==============================================================================
void FieldEditor::SetEditor(juce::String label, juce::String value, FieldType type, int labelW, int editorW)
{
	addAndMakeVisible(m_Label);
	m_Label.setBounds(0, 0, labelW, 24);
	m_Label.setText(label, juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_Editor);
	m_Editor.setBounds(labelW, 0, editorW, 24);
	m_Editor.setText(value);
	switch (type) {
	case Double: m_Editor.setInputRestrictions(10, "0123456789.-"); break;
	case Int: m_Editor.setInputRestrictions(10, "0123456789-"); break;
	case Uint: m_Editor.setInputRestrictions(10, "0123456789"); break;
	default:;
	}
}

//==============================================================================
// FrameComponent : composant pour visualiser et modifier un cadre
//==============================================================================
FrameComponent::FrameComponent(double xmin, double ymin, double xmax, double ymax) :
	m_btnKm("Km", juce::DrawableButton::ImageRaw)
{
	addAndMakeVisible(m_lblXmin);
	m_lblXmin.setBounds(10, 50, 50, 24);
	m_lblXmin.setText(juce::translate("Xmin : "), juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_edtXmin);
	m_edtXmin.setBounds(60, 50, 100, 24);
	m_edtXmin.setText(juce::String(xmin, 2));

	addAndMakeVisible(m_lblXmax);
	m_lblXmax.setBounds(350, 50, 50, 24);
	m_lblXmax.setText(juce::translate(" : Xmax"), juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_edtXmax);
	m_edtXmax.setBounds(240, 50, 100, 24);
	m_edtXmax.setText(juce::String(xmax, 2));

	addAndMakeVisible(m_lblYmax);
	m_lblYmax.setBounds(100, 10, 50, 24);
	m_lblYmax.setText(juce::translate("Ymax : "), juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_edtYmax);
	m_edtYmax.setBounds(150, 10, 100, 24);
	m_edtYmax.setText(juce::String(ymax, 2));

	addAndMakeVisible(m_lblYmin);
	m_lblYmin.setBounds(260, 90, 50, 24);
	m_lblYmin.setText(juce::translate(" : Ymin"), juce::NotificationType::dontSendNotification);
	addAndMakeVisible(m_edtYmin);
	m_edtYmin.setBounds(150, 90, 100, 24);
	m_edtYmin.setText(juce::String(ymin, 2));

	addAndMakeVisible(m_btnView);
	auto image = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
	m_btnView.setImages(false, false, true, image, 1.f, juce::Colours::transparentWhite,
		image, 0.5f, juce::Colours::transparentWhite, image, 1.f, juce::Colours::transparentWhite);
	m_btnView.setBounds(160, 50, 40, 24);
	m_btnView.addListener(this);

	addAndMakeVisible(m_btnKm);
	juce::DrawableText text_on, text_over;
	text_on.setText("Km"); text_over.setText("Km");
	text_on.setColour(juce::Colours::red); text_over.setColour(juce::Colours::blue);
	m_btnKm.setImages(&text_on, &text_over);
	m_btnKm.setBounds(200, 50, 40, 24);
	m_btnKm.addListener(this);
}

//==============================================================================
// FrameComponent : Clic des boutons
//==============================================================================
void FrameComponent::buttonClicked(juce::Button* button)
{
	if (button == &m_btnView)
		ViewFrame();
	if (button == &m_btnKm)
		RoundFrame();
}

//==============================================================================
// FrameComponent : Affichage de la zone d'export
//==============================================================================
void FrameComponent::ViewFrame()
{
	double xmin = m_edtXmin.getText().getDoubleValue();
	double xmax = m_edtXmax.getText().getDoubleValue();
	double ymin = m_edtYmin.getText().getDoubleValue();
	double ymax = m_edtYmax.getText().getDoubleValue();
	juce::Component* parent = getParentComponent();
	juce::ActionBroadcaster* broadcaster = dynamic_cast<juce::ActionBroadcaster*>(parent);
	if (broadcaster != nullptr) {
		broadcaster->sendActionMessage("SetSelectionFrame:" + juce::String(xmin) + ":" + juce::String(xmax) + ":" +
			juce::String(ymin) + ":" + juce::String(ymax));
		broadcaster->sendActionMessage("ZoomFrame:" + juce::String(xmin) + ":" + juce::String(xmax) + ":" +
			juce::String(ymin) + ":" + juce::String(ymax));
	}
}

//==============================================================================
// FrameComponent : Arrondi au km de la zone d'export
//==============================================================================
void FrameComponent::RoundFrame()
{
	double xmin = m_edtXmin.getText().getDoubleValue();
	xmin = floor(xmin / 1000.) * 1000.;
	m_edtXmin.setText(juce::String(xmin, 2));
	double xmax = m_edtXmax.getText().getDoubleValue();
	xmax = ceil(xmax / 1000.) * 1000.;
	m_edtXmax.setText(juce::String(xmax, 2));
	double ymin = m_edtYmin.getText().getDoubleValue();
	ymin = floor(ymin / 1000.) * 1000.;
	m_edtYmin.setText(juce::String(ymin, 2));
	double ymax = m_edtYmax.getText().getDoubleValue();
	ymax = ceil(ymax / 1000.) * 1000.;
	m_edtYmax.setText(juce::String(ymax, 2));
	juce::Component* parent = getParentComponent();
	juce::ActionBroadcaster* broadcaster = dynamic_cast<juce::ActionBroadcaster*>(parent);
	if (broadcaster != nullptr)
		broadcaster->sendActionMessage("SetSelectionFrame:" + juce::String(xmin) + ":" + juce::String(xmax) + ":" +
																		juce::String(ymin) + ":" + juce::String(ymax));
}

//==============================================================================
// FrameComponent : Recuperation du cadre
//==============================================================================
void FrameComponent::GetFrame(double& xmin, double& ymin, double& xmax, double& ymax)
{
	xmin = m_edtXmin.getText().getDoubleValue();
	xmax = m_edtXmax.getText().getDoubleValue();
	ymin = m_edtYmin.getText().getDoubleValue();
	ymax = m_edtYmax.getText().getDoubleValue();
}
