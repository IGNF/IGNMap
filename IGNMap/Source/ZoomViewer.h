//-----------------------------------------------------------------------------
//								ZoomViewer.h
//								============
//
// Zoom rapide
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 07/04/2025
//-----------------------------------------------------------------------------

#pragma once

#ifndef ZOOMVIEWER_H
#define ZOOMVIEWER_H

#include "AppUtil.h"
#include <onnxruntime_cxx_api.h>

class XGeoBase;

class ZoomViewer : public ToolWindow {
public:
	ZoomViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		m_Session = nullptr;
		m_Env = nullptr;
		setResizable(true, true);
		setAlwaysOnTop(false);
		setContentOwned(&m_ImageComponent, true);
		m_ImageComponent.setInterceptsMouseClicks(false, false);
		setResizeLimits(400, 450, 10000, 10000);
	}

	virtual ~ZoomViewer() { Clear(); }
	void Clear();
	bool LoadModel();

	void SetTarget(const double& X, const double& Y, const double& Z) override { ; }
	void SetSelection(void*) override { ; }
	bool NeedTargetImage() override { return true; }
	void SetTargetImage(const juce::Image& image) override;

	void mouseDown(const juce::MouseEvent& event) override;

private:
	Ort::Env* m_Env;
	Ort::Session* m_Session;
	juce::ImageComponent m_ImageComponent;

	bool RunModel(const juce::Image& image);

	struct IOInfo {
		std::string Type;
		std::vector<std::int64_t> Shape;
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZoomViewer)
};

#endif //ZOOMVIEWER_H