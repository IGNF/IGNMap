//-----------------------------------------------------------------------------
//								XGeoProjection.h
//								================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/09/2007
//-----------------------------------------------------------------------------

#ifndef _XGEOPROJECTION_H
#define _XGEOPROJECTION_H

#include <string>
#include "../XTool/XBase.h"
#include "../XTool/XFrame.h"

namespace XGeoProjection {

  enum XProjCode {	Unknown = -1,
                    RGF93 = 0, Lambert1 = 1, Lambert2 = 2, Lambert3 = 3, Lambert4 = 4, Lambert2E = 5,
										Lambert93 = 6, LambertCC42 = 7, LambertCC43 = 8, LambertCC44 = 9, LambertCC45 = 10,
										LambertCC46 = 11 ,LambertCC47 = 12, LambertCC48 = 13, LambertCC49 = 14,
                    LambertCC50 = 15,
                    ETRS89LCC = 16, ETRS89LAEA = 17, ETRS89TM30 = 18, ETRS89TM31 = 19, ETRS89TM32 = 20,
                    FortDesaix = 51, SainteAnne = 52, FortMarigot = 53, RRAF = 54, RGAF09 = 55,
										CSG1967_UTM21 = 61, CSG1967_UTM22 = 62, RGFG95 = 63,
										PitonNeiges = 71, RGR92 = 72,
										SPMiquelon1950 = 81, RGSPM06 = 82,
                    Combani1950 = 91, Cadastre1997 = 92, RGM04 = 93,
                    WebMercator = 100,
                    NC_IGN72 = 110, NC_NEA74 = 111, NC_RGNC91_Lambert = 112,  NC_RGNC91_UTM57 = 113,
                    NC_RGNC91_UTM58 = 114, NC_RGNC91_UTM59 = 115};

	std::string MifProjection(XProjCode proj);
	std::string ShpProjection(XProjCode proj);

	std::string EcwDatum(XProjCode proj);
	std::string EcwProjection(XProjCode proj);

	std::string ProjectionName(XProjCode proj);

	uint32_t EPSGCode(XProjCode proj);
	XFrame FrameGeo(XProjCode proj);
	XFrame FrameProj(XProjCode proj);
}


#endif //_XGEOPROJECTION_H
