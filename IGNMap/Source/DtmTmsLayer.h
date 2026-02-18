//-----------------------------------------------------------------------------
//								DtmTmsLayer.h
//								=============
//
// Gestion des flux MNT diffuses en TMS 
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 13/02/2026
//-----------------------------------------------------------------------------

#pragma once
#include "GeoBase.h"
#include "../../XToolGeod/XGeoProjection.h"

//-----------------------------------------------------------------------------
// DtmTmsLayer
//-----------------------------------------------------------------------------
class DtmTmsLayer : public GeoDTM, public GeoCacheImage {
protected:
	juce::String	m_strServer;
	juce::String	m_strKey;
	juce::String	m_strRequest;  // Derniere requete HTTP
	juce::String	m_strFormat;
	juce::String	m_strEncoding;	// Terrarium

	uint16_t			m_nTileW;		// Dimensions des dalles
	uint16_t			m_nTileH;
	XFrame				m_F;		// Cadre dans la projection native TMS
	double				m_dX0, m_dY0;	// Origine du dallage
	bool					m_bFlip;			// Indique si les numeros de dalles sont dans le sens inverse du Y
	XGeoProjection::XProjCode m_ProjCode;
	uint16_t			m_nMaxZoom;  // Niveau de zoom maximum de la pyramide
	float*				m_SourceGrid;  // Grille en WebMercator
	uint32_t			m_SourceW, m_SourceH;	// Dimension de la grille en WebMercator
	juce::Image		m_ProjImage;    // Image en projection

	juce::String LoadTile(int x, int y, int zoomlevel);
	bool ConvertTile(const juce::Image& image, float* grid);
	juce::Image LoadWebp(juce::String filename);

public:
	DtmTmsLayer(std::string server, std::string apikey = "", std::string format = "png", std::string encoding = "Terrarium", 
		uint16_t tileW = 256, uint16_t tileH = 256, uint16_t max_zoom = 15);

	virtual ~DtmTmsLayer();

	void SetFrame(const XFrame& F) { m_Frame = F; }
	inline double Swath(uint32_t zoom_level) const { return 6378137. * 2 * XPI / pow(2, zoom_level); } // Largeur d'une dalle a l'Equateur
	inline double Resol(uint32_t zoom_level) const { return Swath(zoom_level) / m_nTileW; }
	virtual	bool ReadAttributes(std::vector<std::string>& V);

	bool LoadFrame(const XFrame& F, int zoomlevel);
	virtual bool ComputeZGrid(float* grid, uint32_t w, uint32_t h, XFrame* F);
};

//-----------------------------------------------------------------------------
// DtmTmsComponent
//-----------------------------------------------------------------------------
class DtmTmsComponent : public juce::Component, public juce::ComboBox::Listener, public juce::Button::Listener,
		public juce::ActionBroadcaster {
protected:
	XGeoBase* m_Base;
	juce::ComboBox m_cbxTitle;
	juce::TextEditor m_edtServer, m_edtKey, m_edtTileSize, m_edtZoomMax;
	juce::ComboBox m_cbxFormat, m_cbxEncoding;
	juce::Label m_lblServer, m_lblKey, m_lblFormat, m_lblEncoding, m_lblTileSize, m_lblZoomMax;
	juce::TextButton m_btnOK;
public:
	DtmTmsComponent(XGeoBase* base);
	virtual void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	virtual void buttonClicked(juce::Button* button) override;

	std::string Server() { return m_edtServer.getText().toStdString(); }
	std::string ApiKey() { return m_edtKey.getText().toStdString(); }
	std::string Format() { return m_cbxFormat.getText().toLowerCase().toStdString(); }
	std::string Encoding() { return m_cbxEncoding.getText().toStdString(); }
	int TileSize() { return m_edtTileSize.getText().getIntValue();}
	int ZoomMax() { return m_edtZoomMax.getText().getIntValue(); }

};