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
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"

ClassViewerMgr gClassViewerMgr;	// Le manager de toutes les fenetres ClassViewer

//==============================================================================
// Renvoie la ieme vecteur de la classe ou nullptr sinon
//==============================================================================
XGeoVector* ClassViewerModel::FindVector(int index)
{
	if (m_Class == nullptr)
		return nullptr;
	if (index >= m_Proxy.size())
		return nullptr;
	return m_Proxy[index];
}

//==============================================================================
// Fixe la classe
//==============================================================================
void ClassViewerModel::SetClass(XGeoClass* C)
{ 
	m_Class = C;
	m_Proxy.clear();
	for (uint32_t i = 0; i < m_Class->NbVector(); i++)
		m_Proxy.push_back(m_Class->Vector(i));
}

//==============================================================================
// Nombre de lignes de la table
//==============================================================================
int ClassViewerModel::getNumRows()
{
	if (m_Class == nullptr)
		return 0;
	return (int)m_Proxy.size();
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
		if (((columnId - ClassViewerModel::Column::Attribut) * 2 + 1) < Att.size())
			g.drawText(juce::String(Att[(columnId - ClassViewerModel::Column::Attribut) * 2 + 1]), 0, 0, width, height, juce::Justification::centredLeft);
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
void ClassViewerModel::cellClicked(int /*rowNumber*/, int columnId, const juce::MouseEvent& /*event*/)
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
void ClassViewerModel::cellDoubleClicked(int rowNumber, int /*columnId*/, const juce::MouseEvent& /*event*/)
{
	XGeoVector* V = FindVector(rowNumber);
	if (V == nullptr)
		return;
	XFrame F = V->Frame();
	F += 100.;	// Pour avoir une marge et pour agrandir la zone pour les petits objets et les ponctuels
	sendActionMessage("ZoomFrame:" + juce::String(F.Xmin, 2) + ":" + juce::String(F.Xmax, 2) + ":" +
		juce::String(F.Ymin, 2) + ":" + juce::String(F.Ymax, 2));
}

//==============================================================================
// Tri des colonnes
//==============================================================================
void ClassViewerModel::sortOrderChanged(int newSortColumnId, bool isForwards)
{
	if ((newSortColumnId == Column::Visibility) || (newSortColumnId == Column::Selectable))
		return;
	std::multimap<std::string, XGeoVector*> map;
	std::vector<std::string> Att;
	XGeoVector* V;
	if (m_Proxy.size())
		juce::MouseCursor::showWaitCursor();
	for (size_t i = 0; i < m_Proxy.size(); i++) {
		V = m_Proxy[i];
		Att.clear();
		V->ReadAttributes(Att);
		map.emplace(Att[2 * (newSortColumnId - ClassViewerModel::Column::Attribut) + 1], V);
	}
	std::multimap<std::string, XGeoVector*>::iterator iter;
	m_Proxy.clear();
	for (iter = map.begin(); iter != map.end(); iter++) 
		m_Proxy.push_back(iter->second);
	if (!isForwards)
		std::reverse(m_Proxy.begin(), m_Proxy.end());
	juce::MouseCursor::hideWaitCursor();
	sendActionMessage("UpdateSort");
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
// ClassViewer : fermeture de la fenetre
//==============================================================================
void ClassViewer::closeButtonPressed() 
{ 
	gClassViewerMgr.RemoveViewer(this);
	delete this; 
}

//==============================================================================
// Gestion des actions
//==============================================================================
void ClassViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateSort") {
		m_Table.repaint();
		return;
	}

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
	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Ajout d'un Viewer au gestionnaire de Viewer
//==============================================================================
void ClassViewerMgr::AddClassViewer(const juce::String& name, XGeoClass* C, juce::ActionListener* listener)
{
	ClassViewer* viewer = new ClassViewer(name, juce::Colours::grey, juce::DocumentWindow::allButtons, C, listener);
	viewer->setVisible(true);
	m_Viewer.push_back(viewer);
}

//==============================================================================
// Retrait d'un Viewer du gestionnaire de Viewer
//==============================================================================
void ClassViewerMgr::RemoveViewer(ClassViewer* viewer)
{
	m_Viewer.remove(viewer);
}

//==============================================================================
// Retrait de tous les Viewers d'une classe donnee du gestionnaire de Viewer
//==============================================================================
void ClassViewerMgr::RemoveViewer(XGeoClass* C)
{
	std::list<ClassViewer*>::iterator iter;
	for (iter = m_Viewer.begin(); iter != m_Viewer.end(); iter++) {
		if ((*iter)->GetClass() != C)
			continue;
		delete (*iter);
		m_Viewer.erase(iter);
		RemoveViewer(C);
		break;
	}
}

//==============================================================================
// Retrait de tous les Viewers
//==============================================================================
void ClassViewerMgr::RemoveAll()
{
	std::list<ClassViewer*>::iterator iter;
	for (iter = m_Viewer.begin(); iter != m_Viewer.end(); iter++) {
		delete (*iter);
	}
	m_Viewer.clear();
}