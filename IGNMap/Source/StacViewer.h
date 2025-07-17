//-----------------------------------------------------------------------------
//								StacViewer.h
//								============
//
// Utilisation d'un catalogue STAC pour trouver une image proche d'un point
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 23/06/2025
//-----------------------------------------------------------------------------

#pragma once

#ifndef STACVIEWER_H
#define STACVIEWER_H

#include "AppUtil.h"
#include "GeoBase.h"

class StacViewer : public ToolWindow, public juce::ActionBroadcaster {
public:
	StacViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		setContentOwned(&m_ImageComponent, true);
		m_ImageComponent.setInterceptsMouseClicks(false, false);
		setResizeLimits(400, 450, 10000, 10000);
		m_StacServer = "https://panoramax.ign.fr/api/";
		m_Base = base;
		m_Class = nullptr;
		m_Cache = GeoTools::CreateCacheDir("STAC");
		addActionListener(listener);
		setWantsKeyboardFocus(true);
		m_nTx = 0;
	}

	virtual ~StacViewer() { m_Cache.deleteRecursively(); }

	void SetTarget(const double& X, const double& Y, const double& Z) override;
	void SetSelection(void*) override { ; }

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	bool keyPressed(const juce::KeyPress& key) override;

private:
	juce::Image m_Image;
	juce::ImageComponent m_ImageComponent;
	juce::String m_StacServer;
	XGeoBase* m_Base;
	XGeoClass* m_Class;
	juce::File m_Cache;
	int m_nTx;

	void SetImage();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StacViewer)
};

#endif //STACVIEWER_H