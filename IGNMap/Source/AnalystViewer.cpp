//-----------------------------------------------------------------------------
//								AnalystViewer.h
//								===============
//
// Visualisation des analyses vectorielles
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 31/07/2026
//-----------------------------------------------------------------------------

#include "AnalystViewer.h"
#include "../XTool/XGeoBase.h"

//==============================================================================
// Dessin du fond
//==============================================================================
void AnalystViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void AnalystViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (m_Analyse == nullptr)
		return;
	XGeoRepres* R = m_Analyse->Repres(rowNumber);
	if (R == nullptr)
		return;
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		break;
	case Column::Selectable:
		break;
	case Column::Name:// Name
		g.drawText(juce::String(R->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::PenWidth:// Width
		g.drawText(juce::String(R->Size()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::PenColour:// Pen
		g.setColour(juce::Colours::white);
		g.fillRect(0, 0, width, height);
		g.setColour(juce::Colour(R->Color()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::FillColour:// brush
		g.setColour(juce::Colours::white);
		g.fillRect(0, 0, width, height);
		g.setColour(juce::Colour(R->FillColor()));
		g.fillRect(0, 0, width, height);
		break;
	case Column::Options:// Options
		icone = juce::ImageCache::getFromMemory(BinaryData::Options_png, BinaryData::Options_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	}
}

//==============================================================================
// Clic dans une cellule
//==============================================================================
void AnalystViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	if (m_Analyse == nullptr)
		return;
	XGeoRepres* R = m_Analyse->Repres(rowNumber);
	if (R == nullptr)
		return;

	m_ActiveRow = rowNumber;
	m_ActiveColumn = columnId;
	juce::Rectangle<int> bounds;
	bounds.setCentre(event.getMouseDownScreenPosition());
	bounds.setWidth(1); bounds.setHeight(1);

	// Visibilite
	if (columnId == Column::Visibility) {
		//sendActionMessage("UpdateVectorVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		//sendActionMessage("UpdateVectorSelectability");
		return;
	}

	// Choix d'une couleur
	if ((columnId == Column::PenColour) || (columnId == Column::FillColour)) {
		auto colourSelector = std::make_unique<juce::ColourSelector>(juce::ColourSelector::showAlphaChannel
			| juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::editableColour
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace);

		colourSelector->setName("background");
		if (columnId == Column::PenColour)
			colourSelector->setCurrentColour(juce::Colour(R->Color()));
		if (columnId == Column::FillColour)
			colourSelector->setCurrentColour(juce::Colour(R->FillColor()));
		colourSelector->addChangeListener(this);
		colourSelector->setColour(juce::ColourSelector::backgroundColourId, juce::Colours::transparentBlack);
		colourSelector->setSize(400, 300);

		juce::CallOutBox::launchAsynchronously(std::move(colourSelector), bounds, nullptr);
		return;
	}

	// Choix d'une epaisseur
	if (columnId == Column::PenWidth) {
		auto widthSelector = std::make_unique<juce::Slider>();
		widthSelector->setRange(0., 20., 1.);
		widthSelector->setValue(R->Size());
		widthSelector->setSliderStyle(juce::Slider::LinearHorizontal);
		widthSelector->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
		widthSelector->setSize(200, 50);
		widthSelector->setChangeNotificationOnlyOnRelease(true);
		widthSelector->addListener(this);
		juce::CallOutBox::launchAsynchronously(std::move(widthSelector), bounds, nullptr);
		return;
	}
}

//==============================================================================
// AnalystViewerModel : changeListenerCallback
//==============================================================================
void AnalystViewerModel::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (m_Analyse == nullptr)
		return;
	XGeoRepres* R = m_Analyse->Repres(m_ActiveRow);
	if (R == nullptr)
		return;

	// Choix d'une couleur
	if ((m_ActiveColumn == Column::PenColour) || (m_ActiveColumn == Column::FillColour)) {
		if (auto* cs = dynamic_cast<juce::ColourSelector*> (source)) {
			uint32_t color = cs->getCurrentColour().getARGB();
			if (m_ActiveColumn == Column::PenColour)
				R->Color(color);
			if (m_ActiveColumn == Column::FillColour)
				R->FillColor(color);
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// Changement de valeur des sliders
//==============================================================================
void AnalystViewerModel::sliderValueChanged(juce::Slider* slider)
{
	if (m_Analyse == nullptr)
		return;
	XGeoRepres* R = m_Analyse->Repres(m_ActiveRow);
	if (R == nullptr)
		return;

	// Choix d'une epaisseur
	if (m_ActiveColumn == Column::PenWidth) {
		if (R->Size() != (int)slider->getValue()) {
			R->Size((uint8_t)slider->getValue());
			sendActionMessage("UpdateVectorRepres");
		}
	}
}

//==============================================================================
// AnalystViewerComponent : constructeur
//==============================================================================
AnalystViewerComponent::AnalystViewerComponent()
{
	m_Base = nullptr;
	m_lblLayer.setText(juce::translate("Layer :"), juce::dontSendNotification);
	m_lblClass.setText(juce::translate("Class :"), juce::dontSendNotification);
	m_lblAttribut.setText(juce::translate("Attribut :"), juce::dontSendNotification);
	m_lblDistribution.setText(juce::translate("Distribution :"), juce::dontSendNotification);
	addAndMakeVisible(m_lblLayer);
	addAndMakeVisible(m_lblClass);
	addAndMakeVisible(m_lblAttribut);
	addAndMakeVisible(m_lblDistribution);

	m_cbxLayer.addListener(this);
	addAndMakeVisible(m_cbxLayer);
	m_cbxClass.addListener(this);
	addAndMakeVisible(m_cbxClass);
	m_cbxAttribut.addListener(this);
	addAndMakeVisible(m_cbxAttribut);
	m_cbxDistribution.addItem(juce::translate("Linear distribution"), XGeoAnalyst::Fill_Lin);
	m_cbxDistribution.addItem(juce::translate("Log distribution"), XGeoAnalyst::Fill_Log);
	m_cbxDistribution.addItem(juce::translate("Constant distribution"), XGeoAnalyst::Fill_Const);
	m_cbxDistribution.addItem(juce::translate("All values"), XGeoAnalyst::All_Value);
	addAndMakeVisible(m_cbxDistribution);
	addAndMakeVisible(m_sldPlage);
	m_btnFirstColour.setButtonText(juce::translate("First value"));
	m_btnFirstColour.SetColour(juce::Colours::rosybrown);
	addAndMakeVisible(m_btnFirstColour);
	m_btnLastColour.setButtonText(juce::translate("Last value"));
	m_btnLastColour.SetColour(juce::Colours::aquamarine);
	addAndMakeVisible(m_btnLastColour);
	m_sldPlage.setRange(2., 25., 1.);
	m_sldPlage.setValue(10., juce::NotificationType::dontSendNotification);
	m_sldPlage.setSliderStyle(juce::Slider::LinearBar);
	m_sldPlage.setTextValueSuffix(juce::translate(" intervals"));
	addAndMakeVisible(m_sldPlage);
	m_btnRun.setButtonText(juce::translate("Analyze"));
	addAndMakeVisible(m_btnRun);
	m_btnRun.addListener(this);

	// Bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), AnalystViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), AnalystViewerModel::Column::Selectable, 25);
	m_Table.getHeader().addColumn(juce::translate("Name"), AnalystViewerModel::Column::Name, 190);
	m_Table.getHeader().addColumn(juce::translate("Width"), AnalystViewerModel::Column::PenWidth, 50);
	m_Table.getHeader().addColumn(juce::translate("Pen"), AnalystViewerModel::Column::PenColour, 50);
	m_Table.getHeader().addColumn(juce::translate("Brush"), AnalystViewerModel::Column::FillColour, 50);
	//m_Table.getHeader().addColumn(juce::translate(" "), AnalystViewerModel::Column::Options, 25);
	m_Table.setModel(&m_Model);
	addAndMakeVisible(m_Table);
}

//==============================================================================
// AnalystViewerComponent : Redimensionnement du composant
//==============================================================================
void AnalystViewerComponent::resized()
{
	auto w = getLocalBounds().getWidth();
	auto h = getLocalBounds().getHeight();
	m_lblLayer.setBounds(5, 40, 80, 24);
	m_cbxLayer.setBounds(100, 40, w - 110, 24);
	m_lblClass.setBounds(5, 70, 80, 24);
	m_cbxClass.setBounds(100, 70, w - 110, 24);
	m_lblAttribut.setBounds(5, 100, 80, 24);
	m_cbxAttribut.setBounds(100, 100, w - 110, 24);
	m_lblDistribution.setBounds(5, 130, 80, 24);
	m_cbxDistribution.setBounds(100, 130, w - 110, 24);

	m_btnFirstColour.setBounds(5, 160, 80, 24);
	m_sldPlage.setBounds(w / 2 - 75, 160, 150, 24);
	m_btnLastColour.setBounds(w - 90, 160, 80, 24);
	m_btnRun.setBounds(w / 2 - 50, 190, 100, 24);

	m_Table.setBounds(5, 220, w - 10, h - 225);
}

//==============================================================================
// AnalystViewerComponent : fixe la base de donnee
//==============================================================================
void AnalystViewerComponent::SetBase(XGeoBase* base)
{
	m_cbxLayer.clear(juce::NotificationType::dontSendNotification);
	m_cbxClass.clear(juce::NotificationType::dontSendNotification);
	m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
	m_Base = base;
	if (m_Base == nullptr)
		return;

	for (uint32_t i = 0; i < m_Base->NbLayer(); i++) {
		XGeoLayer* L = m_Base->Layer(i);
		bool vector_layer = false;
		for (uint32_t j = 0; j < L->NbClass(); j++) {
			XGeoClass* C = L->Class(j);
			if (C->IsVector())
				vector_layer = true;
		}
		if (vector_layer)
			m_cbxLayer.addItem(L->Name(), i + 1);
	}
}

//==============================================================================
// AnalystViewerComponent : met a jour si la base de donnee a change
//==============================================================================
void AnalystViewerComponent::UpdateBase()
{
	if (m_Base == nullptr)
		return;
	SetBase(m_Base);
}

//==============================================================================
// Reponses aux combo box
//==============================================================================
void AnalystViewerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &m_cbxLayer) {
		m_cbxClass.clear(juce::NotificationType::dontSendNotification);
		m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
		juce::String layerName = m_cbxLayer.getText();
		XGeoLayer* L = m_Base->Layer(layerName.toStdString().c_str());
		if (L == nullptr)
			return;
		for (uint32_t j = 0; j < L->NbClass(); j++) {
			XGeoClass* C = L->Class(j);
			if (C->IsVector())
				m_cbxClass.addItem(C->Name(), j + 1);
		}
	}
	if (comboBoxThatHasChanged == &m_cbxClass) {
		m_cbxAttribut.clear(juce::NotificationType::dontSendNotification);
		juce::String layerName = m_cbxLayer.getText();
		juce::String className = m_cbxClass.getText();
		XGeoClass* C = m_Base->Class(layerName.toStdString().c_str(), className.toStdString().c_str());
		if (C == nullptr)
			return;
		if (C->NbVector() < 1)
			return;
		XGeoVector* V = C->Vector((uint32_t)0);
		std::vector<std::string> Att;
		if (!V->ReadAttributes(Att))
			return;
		for (int p = 0; p < Att.size(); p+=2)
			m_cbxAttribut.addItem(Att[p], p+1);
	}
}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void AnalystViewerComponent::buttonClicked(juce::Button* button)
{
	if (button != &m_btnRun)
		return;
	XGeoAnalyst* A = new XGeoAnalyst;
	A->Layer(m_cbxLayer.getText().toStdString());
	A->Class(m_cbxClass.getText().toStdString());
	A->Attribut(m_cbxAttribut.getText().toStdString());
	A->Type((XGeoAnalyst::eType)m_cbxDistribution.getSelectedId());
	juce::Colour first = m_btnFirstColour.GetColour();
	juce::Colour last = m_btnLastColour.GetColour();

	uint32_t first_color = first.getARGB();
	uint32_t last_color = last.getARGB();

	A->SetFill((uint32_t)m_sldPlage.getValue(), first_color, last_color);
	A->Run(m_Base);

	m_Analyse.push_back(A);
	m_Model.SetAnalyse(A);
	m_Table.repaint();
}

//==============================================================================
// AnalystViewer : transmission des messages
//==============================================================================
void AnalystViewer::SetMessage(const juce::String& message)
{
	if (message == "UpdateVector")
		m_Analyst.UpdateBase();
}

