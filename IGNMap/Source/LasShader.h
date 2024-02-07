//-----------------------------------------------------------------------------
//								LasShader.h
//								===========
//
// Colorisation des nuages LIDAR
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 28/10/2023
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>

class LasShader {
public:
	LasShader();

	enum class ShaderMode { Null = 0, Altitude = 1, RGB = 2, IRC = 3, Classification = 4, Intensity = 5, Angle = 6 };

	static ShaderMode Mode() { return m_Mode; }
	static void Mode(ShaderMode mode) { m_Mode = mode; }
	static double Opacity() { return m_Opacity; }
	static void Opacity(double opacity) { m_Opacity = opacity; }
	static double MaxGsd() { return m_MaxGsd; }
	static void MaxGsd(double gsd) { m_MaxGsd = gsd; }
	static double Zmin() { return m_Zmin; }
	static void Zmin(double z) { m_Zmin = z; }
	static double Zmax() { return m_Zmax; }
	static void Zmax(double z) { m_Zmax = z; }
	static juce::Colour ClassificationColor(unsigned char classif) { return m_ClassifColors[classif]; }
	static juce::Colour AltiColor(unsigned char alti) { return m_AltiColors[alti]; }
	static juce::String ClassificationName(unsigned char classif);
	static bool ClassificationVisibility(unsigned char classif) { return m_ClassifVisibility[classif]; }
	static bool ClassificationSelectable(unsigned char classif) { return m_ClassifSelectable[classif]; }

	static void ClassificationColor(juce::Colour color, unsigned char classif) { m_ClassifColors[classif] = color; }
	static void ClassificationVisibility(bool flag, unsigned char classif) { m_ClassifVisibility[classif] = flag; }
	static void ClassificationSelectable(bool flag, unsigned char classif) { m_ClassifSelectable[classif] = flag; }

protected:
	static bool m_Init;
	static ShaderMode m_Mode; // Mode d'affichage
	static juce::Colour m_ClassifColors[256];	// Classification ASPRS
	static juce::Colour m_AltiColors[256];			// Palette altimetrique
	static double m_Opacity;
	static double m_MaxGsd;		// GSD maximale d'affichage des points du LAS
	static double m_Zmin;			// Zmin que l'on considere
	static double m_Zmax;			// Zmax que l'on considere
	static bool m_ClassifVisibility[256];
	static bool m_ClassifSelectable[256];
};