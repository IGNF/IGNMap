//-----------------------------------------------------------------------------
//								GeoSearch.cpp
//								=============
//
// Recherche geographique avec la GeoPlateforme ou Nominatim
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 15/07/2024
//-----------------------------------------------------------------------------

#include "GeoSearch.h"


//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
GeoSearch::GeoSearch()
{
  juce::File tmpDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory);
  juce::File cache = tmpDir.getNonexistentChildFile("geosearch", ".json");
  m_strFilename = cache.getFullPathName();
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
GeoSearch::~GeoSearch()
{
  juce::File file(m_strFilename);
  file.deleteFile();
}

//-----------------------------------------------------------------------------
// Recherche sur la Geoplateforme IGN
//-----------------------------------------------------------------------------
bool GeoSearch::SearchIGN(juce::String text)
{
  juce::String query = "https://data.geopf.fr/geocodage/search?q=";
  query += text;

  juce::URL url(query);
  juce::URL::DownloadTaskOptions options;
  std::unique_ptr< juce::URL::DownloadTask > task = url.downloadToFile(m_strFilename, options);
  if (task.get() == nullptr)
    return false;
  int count = 0;
  while (task.get()->isFinished() == false)
  {
    juce::Thread::sleep(50);
    count++;
    if (count > 100) return false;
  }
  return true;
}