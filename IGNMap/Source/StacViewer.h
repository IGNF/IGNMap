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
#include "Utilities.h"
#include "GeoBase.h"

class StacViewer : public ToolWindow, public juce::ActionBroadcaster {
protected:
	class StacComponent : public juce::Component {
	public:
		juce::ImageComponent m_ImageComponent;
		juce::TreeView m_Tree;
		std::unique_ptr<juce::TreeViewItem> m_RootItem;

		StacComponent() {
			setOpaque(true);
			addAndMakeVisible(m_ImageComponent);
			m_ImageComponent.setInterceptsMouseClicks(false, false);
			m_Tree.setTitle("Results");
			addAndMakeVisible(m_Tree);
			m_Tree.setColour(juce::TreeView::backgroundColourId, juce::Colours::white);
			m_Tree.setDefaultOpenness(true);
			setSize(500, 500);
			setInterceptsMouseClicks(false, true);
		}
		~StacComponent() {
			m_Tree.setRootItem(nullptr);
		}
		void paint(juce::Graphics& g) override
		{
			g.fillAll(getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
		}
		void resized() override {
			auto area = getLocalBounds();
			m_ImageComponent.setBounds(area.removeFromTop(area.getHeight() / 2).reduced(8));
			m_Tree.setBounds(area.reduced(8));
		}

	};
public:
	StacViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		setContentOwned(&m_Stac, true);
		//setInterceptsMouseClicks(false, true);
		setResizeLimits(400, 450, 10000, 10000);
		m_StacServer = "https://panoramax.ign.fr/api/";
		m_Base = base;
		m_Cache = GeoTools::CreateCacheDir("STAC");
		addActionListener(listener);
		setWantsKeyboardFocus(true);
		m_nTx = m_nTy = 0;
		m_bThumb = true;
		m_Azimuth = 0.;
	}

	virtual ~StacViewer() { m_Cache.deleteRecursively(); }

	void SetTarget(const double& X, const double& Y, const double& Z) override;
	void SetSelection(void*) override { ; }

	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDoubleClick(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	bool keyPressed(const juce::KeyPress& key) override;
	void resized() override { ToolWindow::resized(); SetImage(); }

private:
	juce::Image m_Image;
	StacComponent m_Stac;
	juce::String m_StacServer;
	juce::String m_Id;
	juce::File m_Cache;
	bool m_bThumb;
	juce::String m_Projection;
	double m_Azimuth;
	XGeoBase* m_Base;
	int m_nTx, m_nTy;
	XPt2D m_Pos;

	void SetImage();
	void ComputeSphereImage();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StacViewer)
};

#endif //STACVIEWER_H