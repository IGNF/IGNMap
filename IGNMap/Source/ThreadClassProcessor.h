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
#include "../../XToolGeod/XGeoPref.h"

class ThreadClassProcessor : public juce::ThreadWithProgressWindow {
public:
	std::vector<XGeoClass*> m_T;	// Liste des classes a traiter
	bool	m_bOnlyVisible;					// Traitement des objets visibles uniquement
	juce::String m_strFolderOut;			// Repertoire de destination
	juce::String m_strFileExtension;	// Extension des fichiers de sortie
	juce::String m_strMifFile;				// Nom du fichier MIF eventuellement cree par le traitement
	juce::String m_strMidFile;				// Nom du fichier MID eventuellement cree par le traitement

	ThreadClassProcessor(juce::String windowTitle, bool onlyVisible) : ThreadWithProgressWindow(juce::translate(windowTitle), true, true)
	{
		m_bOnlyVisible = onlyVisible;
	}

	// Methode virtuelle a implementer pour realiser le traitement
	virtual bool Process(XGeoVector*) = 0;
	
	// Methode run du thread
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

	// Creation d'un fichier MIF/MID pour exposer les resultats du traitement
	bool CreateMifMidFile(juce::File cacheDir, juce::String name, juce::StringArray att) {
		juce::File mif = cacheDir.getNonexistentChildFile(name, ".mif");
		juce::File mid = cacheDir.getNonexistentChildFile(name, ".mid");
		mif.appendText("VERSION 300\r\nCharset \"WindowsLatin1\"");
		XGeoPref pref;
		mif.appendText(XGeoProjection::MifProjection(pref.Projection()));
		mif.appendText("\r\n");
		// Ecriture des noms d'attributs
		if ((!att.isEmpty())) {
			mif.appendText("COLUMNS " + juce::String(att.size()) + "\r\n");
			for (int i = 0; i < att.size(); i++) {
				mif.appendText(att[i] + "\r\n");
			}
			mif.appendText("DATA\r\n");
		}

		m_strMifFile = mif.getFullPathName();
		m_strMidFile = mid.getFullPathName();
		return true;
	}
};