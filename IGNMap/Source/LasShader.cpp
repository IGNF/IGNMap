//-----------------------------------------------------------------------------
//								LasShader.cpp
//								=============
//
// Colorisation des nuages LIDAR
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 28/10/2023
//-----------------------------------------------------------------------------

#include "LasShader.h"

bool LasShader::m_Init = false;
double LasShader::m_Opacity = 100.0;
double LasShader::m_MaxGsd = 5.0;
LasShader::ShaderMode LasShader::m_Mode = LasShader::ShaderMode::Altitude; // Mode d'affichage
juce::Colour LasShader::m_ClassifColors[256];	// Classification ASPRS
juce::Colour LasShader::m_AltiColors[256];
bool LasShader::m_ClassifVisibility[256];
bool LasShader::m_ClassifSelectable[256];

LasShader::LasShader()
{
	if (m_Init)
		return;
	// Initialisation de la palette d'altitude
  for (int i = 0; i < 64; i++)
    m_AltiColors[i] = juce::Colour((juce::uint8)0, 0, 255 - i * 2, 1.0f);
  for (int i = 64; i < 128; i++)
    m_AltiColors[i] = juce::Colour((juce::uint8)0, (i - 64) * 4, 255 - i * 2, 1.0f);
  for (int i = 128; i < 192; i++)
    m_AltiColors[i] = juce::Colour((juce::uint8)((i - 128) * 2), 255 - (i - 128) * 4, 0, 1.0f);
  for (int i = 192; i < 256; i++)
    m_AltiColors[i] = juce::Colour((juce::uint8)((i - 128) * 2), 0, 0, 1.0f);

  // Palette des classifications
  for (int i = 0; i < 256; i++)
    m_ClassifColors[i] = juce::Colour((juce::uint8)i, i, i, 1.0f);  // Par defaut gris ...
  m_ClassifColors[0] = juce::Colours::black;  // Created, Never Classified
  m_ClassifColors[1] = juce::Colours::lightgrey;  // Unclassified
  m_ClassifColors[2] = juce::Colours::sandybrown;  // Ground
  m_ClassifColors[3] = juce::Colours::lawngreen;  // Low Vegetation
  m_ClassifColors[4] = juce::Colours::forestgreen;  // Medium Vegetation
  m_ClassifColors[5] = juce::Colours::darkgreen;  // High Vegetation
  m_ClassifColors[6] = juce::Colours::red;  // Building
  m_ClassifColors[7] = juce::Colours::pink;  // Low Point (Noise)
  m_ClassifColors[8] = juce::Colours::black;  // Reserved
  m_ClassifColors[9] = juce::Colours::blue;  // Water
  m_ClassifColors[10] = juce::Colours::purple; // Rail
  m_ClassifColors[11] = juce::Colours::slategrey; // Road Surface
  m_ClassifColors[12] = juce::Colours::black; // 
  m_ClassifColors[13] = juce::Colours::yellow; // Wire – Guard (Shield)
  m_ClassifColors[14] = juce::Colours::orangered; // Wire – Conductor (Phase)
  m_ClassifColors[15] = juce::Colours::cyan; // Transmission Tower
  m_ClassifColors[16] = juce::Colours::black; // Wire-Structure Connector
  m_ClassifColors[17] = juce::Colours::darkblue; // Bridge Deck
  m_ClassifColors[18] = juce::Colours::magenta; // High Noise
  m_ClassifColors[19] = juce::Colours::orange; // Overhead Structure
  m_ClassifColors[20] = juce::Colours::lightcoral; // Ignored Ground
  m_ClassifColors[21] = juce::Colours::snow; // Snow
  m_ClassifColors[22] = juce::Colours::black; // Temporal Exclusion

  for (int i = 0; i < 256; i++)
    m_ClassifVisibility[i] = m_ClassifSelectable[i] = true;

  m_Init = true;
  
}

//==============================================================================
// Nom des classification ASPRS
//==============================================================================
juce::String LasShader::ClassificationName(unsigned char classif)
{
  switch (classif) {
  case 0: return "Created, Never Classified";
  case 1: return "Unclassified";
  case 2: return "Ground";
  case 3: return "Low Vegetation";
  case 4: return "Medium Vegetation";
  case 5: return "High Vegetation";
  case 6: return "Building";
  case 7: return "Low Point (Noise)";
  case 8: return "Reserved";
  case 9: return "Water";
  case 10: return "Rail";
  case 11: return "Road Surface";
  case 12: return "Reserved";
  case 13: return "Wire – Guard (Shield)";
  case 14: return "Wire – Conductor (Phase)";
  case 15: return "Transmission Tower";
  case 16: return "Wire-Structure Connector";
  case 17: return "Bridge Deck";
  case 18: return "High Noise";
  case 19: return "Overhead Structure";
  case 20: return "Ignored Ground";
  case 21: return "Snow";
  case 22: return "Temporal Exclusion";
  }
  return "";
}