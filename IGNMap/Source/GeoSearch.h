//-----------------------------------------------------------------------------
//								GeoSearch.h
//								===========
//
// Recherche geographique avec la GeoPlateforme ou Nominatim
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 15/07/2024
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>

class GeoSearch {
public:
	GeoSearch();
	virtual ~GeoSearch();

	juce::String Filename() const { return m_strFilename; }
	bool Search(juce::String text);
	bool SearchIGN(juce::String text);
	bool SearchOSM(juce::String text);

protected:
	juce::String	m_strFilename;

	bool RunQuery(juce::String query);

};