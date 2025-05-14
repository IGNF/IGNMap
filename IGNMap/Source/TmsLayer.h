//-----------------------------------------------------------------------------
//								TmsLayer.h
//								==========
//
// Gestion des flux TMS https://wiki.osgeo.org/wiki/Tile_Map_Service_Specification
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/11/2024
//-----------------------------------------------------------------------------

#pragma once
#include <JuceHeader.h>
#include <string>
#include "GeoBase.h"
#include "../../XTool/XParserXML.h"
#include "../../XTool/XFrame.h"
#include "../../XToolGeod/XGeoProjection.h"


class TmsLayer : public GeoInternetImage {
private:
	struct TileSet {
		juce::String m_strHRef;
		uint32_t m_nMinRow = 0;
		uint32_t m_nMaxRow = 0;
		uint32_t m_nMinCol = 0;
		uint32_t m_nMaxCol = 0;
		double m_dGsd = 0.;
		bool operator<(const TileSet& a) { return (this->m_dGsd > a.m_dGsd); }
		static bool predTileSet(const TileSet& a, const TileSet& b) {return (b.m_dGsd < a.m_dGsd);}
	};

protected:
	juce::String	m_strTitle;
	juce::String	m_strSRS;
	uint16_t			m_nW;		// Dimensions des dalles
	uint16_t			m_nH;
	XFrame				m_F;		// Cadre dans la projection native TMS
	juce::String	m_strFormat;
	double				m_dX0, m_dY0;	// Origine du dallage
	bool					m_bFlip;			// Indique si les numeros de dalles sont dans le sens inverse du Y
	std::vector<TileSet>	m_TileSet;
	XGeoProjection::XProjCode m_ProjCode;

	juce::String LoadTile(int x, int y, int zoomlevel);

public:
	TmsLayer();

	bool ReadServer(juce::String serverUrl);
	virtual std::string Name() { return m_strTitle.toStdString(); }

	virtual	bool ReadAttributes(std::vector<std::string>& V);
	inline virtual double Resolution() const { if (m_TileSet.size() < 1) return 10.; return m_TileSet[m_TileSet.size() - 1].m_dGsd; }

	bool LoadFrame(const XFrame& F, int zoomlevel);
	virtual juce::Image& GetAreaImage(const XFrame& F, double gsd);
};
