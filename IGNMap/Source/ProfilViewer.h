//-----------------------------------------------------------------------------
//								ProfilViewer.h
//								==============
//
// Visualisation d'un profil altimetrique
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 27/03/2027
//-----------------------------------------------------------------------------

#pragma once

#ifndef PROFILVIEWER_H
#define PROFILVIEWER_H

#include "AppUtil.h"
#include "Utilities.h"
#include "GeoBase.h"

class ProfilViewer : public ToolWindow, public juce::ActionBroadcaster {
protected:
	class ProfilDrawer : public juce::Component {
	public:
		void paint(juce::Graphics& g) override;
		void mouseMove(const juce::MouseEvent& event) override { SetMouseX(event.x);}
		void SetMouseX(int x) {
			m_nMouseX = x;
			if ((m_nMouseX > 0) && (m_nMouseX < getWidth()))
				repaint();
		}

		std::vector<XPt3D> m_ProfilDtm;
		std::vector<XPt3D> m_ProfilLas;
		double m_dScale = 1.;
		int m_nMouseX = -1;
	};

	class ProfilComponent : public juce::Component, public juce::ActionBroadcaster, public juce::Slider::Listener {
	public:
		ProfilComponent() {
			addAndMakeVisible(m_fldX);
			addAndMakeVisible(m_fldY);
			addAndMakeVisible(m_fldZ);
			addAndMakeVisible(m_fldDist);
			addAndMakeVisible(m_fldDenivPos);
			addAndMakeVisible(m_fldDenivNeg);
			addAndMakeVisible(m_Drawer);
			addAndMakeVisible(m_lblResol);
			addAndMakeVisible(m_sldResol);
			m_fldX.SetEditor("X :", "", FieldEditor::String, 30, 90);
			m_fldY.SetEditor("Y :", "", FieldEditor::String, 30, 90);
			m_fldZ.SetEditor("Z :", "", FieldEditor::String, 30, 90);
			m_fldDist.SetEditor("Dist :", "", FieldEditor::String, 60, 60);
			m_fldDenivPos.SetEditor("Deniv + :", "", FieldEditor::String, 60, 60);
			m_fldDenivNeg.SetEditor("Deniv - :", "", FieldEditor::String, 60, 60);
			m_fldX.SetReadOnly(true);
			m_fldY.SetReadOnly(true);
			m_fldZ.SetReadOnly(true);
			m_fldDist.SetReadOnly(true);
			m_fldDenivNeg.SetReadOnly(true);
			m_fldDenivPos.SetReadOnly(true);
			m_lblResol.setText(juce::translate("Resolution :"), juce::NotificationType::dontSendNotification);
			m_sldResol.setSliderStyle(juce::Slider::LinearHorizontal);
			m_sldResol.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 80, 20);
			m_sldResol.setRange(1., 100., 1.);
			m_sldResol.setValue(10, juce::NotificationType::dontSendNotification);
			m_sldResol.addListener(this);
		}
		virtual ~ProfilComponent() { ; }
		void paint(juce::Graphics& g) override
		{
			g.fillAll(getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
		}
		void resized() override {
			auto b = getLocalBounds();
			m_Drawer.setBounds(0, 0, b.getWidth(), b.getHeight() - 90);
			m_lblResol.setBounds(b.getWidth() / 4, b.getHeight() - 90, 100, 24);
			m_sldResol.setBounds(b.getWidth() / 4 + 100, b.getHeight() - 90, 200, 24);
			m_fldX.setBounds(0, b.getHeight() - 60, b.getWidth() / 3, 24);
			m_fldY.setBounds(b.getWidth() / 3, b.getHeight() - 60, b.getWidth() / 3, 24);
			m_fldZ.setBounds((2 * b.getWidth()) / 3, b.getHeight() - 60, b.getWidth() / 3, 24);
			m_fldDist.setBounds(0, b.getHeight() - 30, b.getWidth() / 3, 24);
			m_fldDenivPos.setBounds(b.getWidth() / 3, b.getHeight() - 30, b.getWidth() / 3, 24);
			m_fldDenivNeg.setBounds((2 * b.getWidth()) / 3, b.getHeight() - 30, b.getWidth() / 3, 24);
		}
		void sliderValueChanged(juce::Slider* slider) override {
			if (slider == &m_sldResol) ComputeProfil(m_Sel, m_sldResol.getValue());
		}

		void SetBase(XGeoBase* base) { m_Base = base; }
		bool ComputeProfil(XGeoObject* sel, double resol = 10.);
		bool SetActiveIndex(int index);

	protected:
		FieldEditor m_fldX, m_fldY, m_fldZ, m_fldDist, m_fldDenivPos, m_fldDenivNeg;
		ProfilDrawer m_Drawer;
		juce::Label m_lblResol;
		juce::Slider m_sldResol;
		XGeoBase* m_Base = nullptr;
		std::vector<XPt3D> m_ProfilDtm;
		std::vector<XPt3D> m_ProfilLas;
		XGeoObject* m_Sel = nullptr;

		bool ComputeLasProfil(const XFrame& F);
	};

public:
	ProfilViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons,
		juce::ActionListener* listener, XGeoBase* base)
		: ToolWindow(name, backgroundColour, requiredButtons)
	{
		setResizable(true, true);
		setAlwaysOnTop(false);
		setContentOwned(&m_Profil, true);
		setResizeLimits(400, 450, 10000, 10000);
		m_Profil.SetBase(base);
		addActionListener(listener);
		m_Profil.addActionListener(listener);
		setWantsKeyboardFocus(true);
	}

	virtual ~ProfilViewer() { ; }

	void SetTarget(const double& X, const double& Y, const double& Z) override { ; }
	void SetSelection(void* Sel) override { m_Profil.ComputeProfil((XGeoObject*)Sel); }

	void resized() override { ToolWindow::resized();  }

private:
	ProfilComponent m_Profil;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProfilViewer)
};

#endif //PROFILVIEWER_H