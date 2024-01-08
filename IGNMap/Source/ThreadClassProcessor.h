//-----------------------------------------------------------------------------
//								ThreadClassProcessor.h
//								======================
//
// Traitement generique sur une liste de classes
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 07/01/2024
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"

class ThreadClassProcessor : public juce::ThreadWithProgressWindow {
public:
	std::vector<XGeoClass*> m_T;	// Liste des classes a traiter
	juce::String m_strFolderOut;			// Repertoire de destination
	juce::String m_strFileExtension;	// Extension des fichiers de sortie
	bool	m_bOnlyVisible;					// Traitement des objets visibles uniquement

	ThreadClassProcessor(juce::String windowTitle, bool onlyVisible) : ThreadWithProgressWindow(juce::translate(windowTitle), true, true)
	{
		m_bOnlyVisible = onlyVisible;
	}

	virtual bool Process(XGeoVector*) = 0;
	
	void run()
	{
		uint32_t nb_file = 0, count = 0;
		for (int i = 0; i < m_T.size(); i++) {
			if ((m_bOnlyVisible) && (!m_T[i]->Visible()))
				continue;
			nb_file += m_T[i]->NbVector();
		}
		if (nb_file == 0)
			return;

		for (int i = 0; i < m_T.size(); i++) {
			if (threadShouldExit())
				break;
			if ((m_bOnlyVisible)&&(!m_T[i]->Visible()))
				continue;
			for (int j = 0; j < m_T[i]->NbVector(); j++) {
				if (threadShouldExit())
					break;
				setProgress((double)count / (double)nb_file);
				count++;
				XGeoVector* V = m_T[i]->Vector(j);
				if ((m_bOnlyVisible) && (!V->Visible()))
					continue;
				Process(V);
			}
		}
	}
};