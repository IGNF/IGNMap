//-----------------------------------------------------------------------------
//								WmtsViewer.cpp
//								==============
//
// Visualisation des layers d'un serveur WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 30/11/2024
//-----------------------------------------------------------------------------

#include "WmtsViewer.h"

WmtsViewerMgr gWmtsViewerMgr;	// Le manager de toutes les fenetres ClassViewer

//==============================================================================
// Dessin du fond
//==============================================================================
void WmtsViewerModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	g.setColour(juce::Colours::lightblue);
	if (rowIsSelected)
		g.drawRect(g.getClipBounds());
	g.setColour(juce::Colours::white);
}

//==============================================================================
// Dessin des cellules
//==============================================================================
void WmtsViewerModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	if (rowNumber >= m_Proxy.size())
		return;

	std::vector<std::string> Att;
	juce::Image icone;
	switch (columnId) {
	case Column::Id:
		g.drawText(juce::String(m_Proxy[rowNumber].Id), 0, 0, width, height, juce::Justification::centredLeft);
		break;
	case Column::Title:
		g.drawText(juce::String(m_Proxy[rowNumber].Title), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::Projection:
		g.drawText(juce::String(m_Proxy[rowNumber].Projection), 0, 0, width, height, juce::Justification::centred);
		break;
	case Column::TMS:
		g.drawText(juce::String(m_Proxy[rowNumber].TMS), 0, 0, width, height, juce::Justification::centred);
		break;
	default:;
	}
}

//==============================================================================
// Tri des colonnes
//==============================================================================
void WmtsViewerModel::sortOrderChanged(int newSortColumnId, bool isForwards)
{
	juce::MouseCursor::showWaitCursor();

	juce::MouseCursor::hideWaitCursor();
	sendActionMessage("UpdateSort");
}

//==============================================================================
// ClassViewer : constructeur
//==============================================================================
WmtsViewer::WmtsViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
												juce::ActionListener* listener)
	: juce::DocumentWindow(name, backgroundColour, requiredButtons)
{
	m_Model.addActionListener(this);
	if (listener != nullptr)
		addActionListener(listener);
	// Ajout d'une bordure
	m_Table.setColour(juce::ListBox::outlineColourId, juce::Colours::grey);
	m_Table.setOutlineThickness(1);
	m_Table.setMultipleSelectionEnabled(true);
	// Ajout des colonnes
	m_Table.getHeader().addColumn(juce::translate("ID"), WmtsViewerModel::Id, 100);
	m_Table.getHeader().addColumn(juce::translate("Title"), WmtsViewerModel::Title, 100);
	m_Table.getHeader().addColumn(juce::translate("Projection"), WmtsViewerModel::Projection, 100);
	m_Table.getHeader().addColumn(juce::translate("TMS"), WmtsViewerModel::TMS, 100);
	m_Table.setSize(600, 200);

	m_Table.setModel(&m_Model);

	setContentOwned(&m_Table, true);
	setResizable(true, true);
	setAlwaysOnTop(false);
}

//==============================================================================
// WmtsViewer : fermeture de la fenetre
//==============================================================================
void WmtsViewer::closeButtonPressed()
{
	gWmtsViewerMgr.RemoveViewer(this);
	delete this;
}

//==============================================================================
// Gestion des actions
//==============================================================================
void WmtsViewer::actionListenerCallback(const juce::String& message)
{
	if (message == "UpdateSort") {
		m_Table.repaint();
		return;
	}

	// Objets selectionnees
	juce::SparseSet< int > S = m_Table.getSelectedRows();
	for (int i = 0; i < S.size(); i++) {
		
	}

	sendActionMessage(message);	// On transmet les messages que l'on ne traite pas
}

//==============================================================================
// Ajout d'un Viewer au gestionnaire de Viewer
//==============================================================================
void WmtsViewerMgr::AddWmtsViewer(const juce::String& name, juce::ActionListener* listener)
{
	WmtsViewer* viewer = new WmtsViewer(name, juce::Colours::grey, juce::DocumentWindow::allButtons, listener);
	viewer->setVisible(true);
	m_Viewer.push_back(viewer);
}

//==============================================================================
// Retrait d'un Viewer du gestionnaire de Viewer
//==============================================================================
void WmtsViewerMgr::RemoveViewer(WmtsViewer* viewer)
{
	m_Viewer.remove(viewer);
}

//==============================================================================
// Retrait de tous les Viewers
//==============================================================================
void WmtsViewerMgr::RemoveAll()
{
	std::list<WmtsViewer*>::iterator iter;
	for (iter = m_Viewer.begin(); iter != m_Viewer.end(); iter++) {
		delete (*iter);
	}
	m_Viewer.clear();
}