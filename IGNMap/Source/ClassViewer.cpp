//-----------------------------------------------------------------------------
//								ClassViewer.cpp
//								===============
//
// Visualisation des objets d'une classe
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/05/2024
//-----------------------------------------------------------------------------

#include "ClassViewer.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"

//==============================================================================
// renvoie la ieme vecteur de la classe ou nullptr sinon
//==============================================================================
XGeoVector* ClassViewerModel::FindVector(int index)
{
	if (m_Class == nullptr)
		return nullptr;
	return m_Class->Vector(index);
}

//==============================================================================
// Nombre de lignes de la table
//==============================================================================
int ClassViewerModel::getNumRows()
{
	if (m_Class == nullptr)
		return 0;
	return (int)m_Class->NbVector();
}

//==============================================================================
// Dessin du fond
//==============================================================================
void ClassViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void ClassViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	XGeoVector* V = FindVector(rowNumber);
	if (V == nullptr)
		return;

	std::vector<std::string> Att;
	juce::Image icone;
	switch (columnId) {
	case Column::Visibility:
		if (V->Visible())
			icone = juce::ImageCache::getFromMemory(BinaryData::View_png, BinaryData::View_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoView_png, BinaryData::NoView_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	case Column::Selectable:
		if (V->Selectable())
			icone = juce::ImageCache::getFromMemory(BinaryData::Selectable_png, BinaryData::Selectable_pngSize);
		else
			icone = juce::ImageCache::getFromMemory(BinaryData::NoSelectable_png, BinaryData::NoSelectable_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
	default:
		V->ReadAttributes(Att);
		g.drawText(juce::String(Att[(columnId - 1) * 2 + 1]), 0, 0, width, height, juce::Justification::centredLeft);
		/*
	case Column::Name:
		g.drawText(juce::String(dtmClass->Name()), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Zmin:
		g.drawText(juce::String(dtmClass->Zmin()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Zmax:
		g.drawText(juce::String(dtmClass->Zmax()), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Options:// Options
		icone = juce::ImageCache::getFromMemory(BinaryData::Options_png, BinaryData::Options_pngSize);
		g.drawImageAt(icone, (width - icone.getWidth()) / 2, (height - icone.getHeight()) / 2);
		break;
		*/
	}
}

//==============================================================================
// Clic dans une cellule
//==============================================================================
void ClassViewerModel::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
	// Visibilite
	if (columnId == Column::Visibility) {
		sendActionMessage("UpdateObjectVisibility");
		return;
	}

	// Selectable
	if (columnId == Column::Selectable) {
		sendActionMessage("UpdateObjectSelectability");
		return;
	}

}

//==============================================================================
// DoubleClic dans une cellule
//==============================================================================
void ClassViewerModel::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& /*event*/)
{
	XGeoVector* V = FindVector(rowNumber);
	if (V == nullptr)
		return;
}

//==============================================================================
// ClassViewer : constructeur
//==============================================================================
ClassViewer::ClassViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons, 
												 XGeoClass* C, juce::ActionListener* listener)
																				: juce::DocumentWindow(name, backgroundColour, requiredButtons)
{
	m_Model.SetClass(C);
	m_Model.addActionListener(this);
	if (listener != nullptr)
		addActionListener(listener);
	// Ajout d'une bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate(" "), ClassViewerModel::Column::Visibility, 25);
	m_Table.getHeader().addColumn(juce::translate(" "), ClassViewerModel::Column::Selectable, 25);
	XGeoVector* V = C->Vector((uint32_t)0);
	if (V != nullptr) {
		std::vector<std::string> Att;
		if (V->ReadAttributes(Att)) {
			for (int i = 0; i < Att.size() / 2; i++)
				m_Table.getHeader().addColumn(Att[2*i], i + ClassViewerModel::Column::Attribut, 100); // ColumnID doit etre different de 0
		}
	}

	m_Table.setSize(600, 200);

	m_Table.setModel(&m_Model);

	setContentOwned(&m_Table, true);
	setResizable(true, true);
	setAlwaysOnTop(false);
}

//==============================================================================
// Gestion des actions
//==============================================================================
void ClassViewer::actionListenerCallback(const juce::String& message)
{
	// Objets selectionnees
	std::vector<XGeoVector*> T;
	juce::SparseSet< int > S = m_Table.getSelectedRows();
	for (int i = 0; i < S.size(); i++) {
		XGeoVector* V = m_Model.FindVector(S[i]);
		if (V != nullptr)
			T.push_back(V);
	}

	if (message == "UpdateObjectVisibility") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Visible(!T[i]->Visible());
		m_Table.repaint();
		sendActionMessage("UpdateClass");
		return;
	}
	if (message == "UpdateObjectSelectability") {
		for (int i = 0; i < T.size(); i++)
			T[i]->Selectable(!T[i]->Selectable());
		m_Table.repaint();
		return;
	}
}