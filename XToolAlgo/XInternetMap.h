//-----------------------------------------------------------------------------
//								XInternetMap.h
//								==============
//
// Fonctions utilitaires pour Google Maps, Bing Maps, Geoportail ...
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 29/03/2024
//-----------------------------------------------------------------------------


#include <string>
#include "../XTool/XPt2D.h"

namespace XInternetMap {

	std::string GoogleMapsUrl(const XPt2D& C, double scale);
	std::string BingMapsUrl(const XPt2D& C, double scale);
}