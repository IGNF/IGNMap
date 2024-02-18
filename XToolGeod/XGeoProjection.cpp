//-----------------------------------------------------------------------------
//								XGeoProjection.cpp
//								==================
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 11/09/2007
//-----------------------------------------------------------------------------

#include "XGeoProjection.h"


//-----------------------------------------------------------------------------
// Renvoie la chaine de description de la projection pour MapInfo
//-----------------------------------------------------------------------------
std::string XGeoProjection::MifProjection(XProjCode proj)
{
  std::string code = "CoordSys NonEarth Units \"m\" Bounds (0, 0) (10000000, 10000000)";
	switch(proj) {
		// France Metropolitaine
		case RGF93:
			code ="CoordSys Earth Projection 1, 33";
			break;
		case Lambert1:
			code ="CoordSys Earth Projection 3, 1002, \"m\", 0, 49.5, 48.5985227778, 50.39591167, 600000, 200000 Bounds (-40092, -818525) (1264450, 486383)";
			break;
		case Lambert2:
			code = "CoordSys Earth Projection 3, 1002, \"m\", 0, 46.8,45.8989188889, 47.69601444, 600000, 200000 Bounds (-42173, -517074) (1266604, 785681)";
			break;
		case Lambert3:
			code = "CoordSys Earth Projection 3, 1002, \"m\", 0, 44.1, 43.1992913889, 44.99609389, 600000, 200000 Bounds (-45826, -217324) (1270389, 1086104)";
			break;
		case Lambert4:
			code = "CoordSys Earth Projection 3, 1002, \"m\", 0, 42.165, 41.5603877778, 42.767663333, 234.358, 185861.369 Bounds (-649267, -17367) (674434, 12884469)";
			break;
		case Lambert2E:
			code = "CoordSys Earth Projection 3, 1002, \"m\", 0, 46.8, 45.8989188889, 47.69601444, 600000, 2200000 Bounds (-42173, 1482926) (1266604, 2785681)";
			break;
		case Lambert93:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 46.5, 44, 49, 700000, 6600000 Bounds (75000, 6000000) (1275000, 7200000)";
			break;
		case LambertCC42:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 42, 41.25, 42.75, 1700000, 1200000";
			break;
		case LambertCC43:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 43, 42.25, 43.75, 1700000, 2200000";
			break;
		case LambertCC44:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 44, 43.25, 44.75, 1700000, 3200000";
			break;
		case LambertCC45:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 45, 44.25, 45.75, 1700000, 4200000";
			break;
		case LambertCC46:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 46, 45.25, 46.75, 1700000, 5200000";
			break;
		case LambertCC47:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 47, 46.25, 47.75, 1700000, 6200000";
			break;
		case LambertCC48:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 48, 47.25, 48.75, 1700000, 7200000";
			break;
		case LambertCC49:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 49, 48.25, 49.75, 1700000, 8200000";
			break;
		case LambertCC50:
			code = "CoordSys Earth Projection 3, 33, \"m\", 3, 50, 49.25, 50.75, 1700000, 9200000";
			break;

    case ETRS89LAEA:
      code = "CoordSys Earth Projection 29, 115, 7, 10, 52, 180";
      break;
    case ETRS89LCC:
      code = "CoordSys Earth Projection 3, 115, 7, 10, 52, 35, 65, 4000000, 2800000 Bounds (2122254.0, 1164627.0) (5955457.0, 5021872.0)";
      break;
    case ETRS89TM30:
      code = "CoordSys Earth Projection 8,115,7,-3,0,0.9996,500000,0";
      break;
    case ETRS89TM31:
      code = "CoordSys Earth Projection 8,115,7,3,0,0.9996,500000,0";
      break;
    case ETRS89TM32:
      code = "CoordSys Earth Projection 8,115,7,9,0,0.9996,500000,0";
      break;

		// Antille
		case FortDesaix:
			code = "CoordSys Earth Projection 8, 9999, 4, 126.93, 547.94, 130.41, 2.7867, -5.1612, 0.8584, 13.8227, 0, 7, -63, 0, 0.9996, 500000, 0";
			break;
		case SainteAnne:
			code = "CoordSys Earth Projection 8, 9999, 4, -472.29, -5.63, -304.12, -0.4362, 0.8374, -0.2563, 1.8984, 0, 7, -63, 0, 0.9996, 500000, 0";
			break;
		case FortMarigot:
			code = "CoordSys Earth Projection 8, 999, 4, 136.596, 248.148, -429.789, 7, -63, 0, 0.9996, 500000, 0";
			break;
		case RRAF:
			code = "CoordSys Earth Projection 8, 104, \"m\", -63, 0, 0.9996, 500000, 0";
			break;
    case RGAF09:
      code = "CoordSys Earth Projection 8, 104, \"m\", -63, 0, 0.9996, 500000, 0";
      break;

		// Guyane
		case CSG1967_UTM21:
			code = "CoordSys Earth Projection 8, 9999, 4, -193.066, 236.993, 105.447, -0.4814, 0.8074, -0.1276, 1.5649, 0, 7, -57, 0, 0.9996, 500000, 0";
			break;
		case CSG1967_UTM22: 
			code = "CoordSys Earth Projection 8, 9999, 4, -193.066, 236.993, 105.447, -0.4814, 0.8074, -0.1276, 1.5649, 0, 7, -51, 0, 0.9996, 500000, 0";
			break;
		case RGFG95: 
			code = "CoordSys Earth Projection 8, 104, \"m\", -51, 0, 0.9996, 500000, 0";
			break;

		// Reunion
		case PitonNeiges: 
			code = "CoordSys Earth Projection 8, 9999, 4, 789.524, -626.486, -89.904, -0.6006, -76.7946, 10.5788, -32.3241, 0, 7, 55.53333333333, -21.11666666667, 1, 160000, 50000";
			break;
		case RGR92: 
			code = "CoordSys Earth Projection 8, 104, \"m\", 57, 0, 0.9996, 500000, 10000000";
			break;

		// Saint Pierre et Miquelon
		case SPMiquelon1950:
			code = "CoordSys Earth Projection 8, 999, 7, 30, 430, 368, 7, -57, 0, 0.9996, 500000, 0";
			break;
		case RGSPM06:
			code = "CoordSys Earth Projection 8, 104, \"m\", -57, 0, 0.9996, 500000, 0";
			break;

		// Mayotte
		case Combani1950:
			code = "CoordSys Earth Projection 8, 999, 4, -382.34, -59.14, -262.41, 7, 45, 0, 0.9996, 500000, 10000000";
			break;
		case Cadastre1997:
			code = "CoordSys Earth Projection 8,9999, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 45, 0, 0.9996, 500000, 10000000";
			break;
		case RGM04:
			code = "CoordSys Earth Projection 8, 104, \"m\", 45, 0, 0.9996, 500000, 10000000";
			break;

      // Nouvelle-Caledonie
      /*
    case NC_IGN72:
      code ="";
      break;
    case NC_NEA74:
      code ="";
      break;
    case NC_RGNC91_Lambert:
      code ="";
      break;
    case NC_RGNC91_UTM57:
      code ="";
      break;
    case NC_RGNC91_UTM58:
      code ="";
      break;
    case NC_RGNC91_UTM59:
      code ="";
      break;
      */
  }
	return code;
}

//-----------------------------------------------------------------------------
// Renvoie la chaine de description de la projection pour ESRI
//-----------------------------------------------------------------------------
std::string XGeoProjection::ShpProjection(XProjCode proj)
{
	std::string code;
	switch(proj) {
		// France Metropolitaine
		case RGF93:
			code ="GEOGCS[\"RGF93\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
			break;
		case Lambert1:
			code ="PROJCS[\"NTF_Lambert_Zone_I\",GEOGCS[\"GCS_NTF\",DATUM[\"D_NTF\",SPHEROID[\"Clarke_1880_IGN\",6378249.2,293.46602]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",600000.0],PARAMETER[\"False_Northing\",200000.0],PARAMETER[\"Central_Meridian\",2.3372291667],PARAMETER[\"Standard_Parallel_1\",48.5985227778],PARAMETER[\"Standard_Parallel_2\",50.3959116667],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",49.5],UNIT[\"Meter\",1.0]]";
			break;
		case Lambert2:
			code ="PROJCS[\"NTF_Lambert_Zone_II\",GEOGCS[\"GCS_NTF\",DATUM[\"D_NTF\",SPHEROID[\"Clarke_1880_IGN\",6378249.2,293.46602]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",600000.0],PARAMETER[\"False_Northing\",200000.0],PARAMETER[\"Central_Meridian\",2.3372291667],PARAMETER[\"Standard_Parallel_1\",45.8989188889],PARAMETER[\"Standard_Parallel_2\",47.6960144444],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",46.8],UNIT[\"Meter\",1.0]]";
			break;
		case Lambert3:
			code ="PROJCS[\"NTF_Lambert_Zone_III\",GEOGCS[\"GCS_NTF\",DATUM[\"D_NTF\",SPHEROID[\"Clarke_1880_IGN\",6378249.2,293.46602]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",600000.0],PARAMETER[\"False_Northing\",200000.0],PARAMETER[\"Central_Meridian\",2.3372291667],PARAMETER[\"Standard_Parallel_1\",43.1992913889],PARAMETER[\"Standard_Parallel_2\",44.9960938889],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",44.1],UNIT[\"Meter\",1.0]]";
			break;
		case Lambert4:
			code ="PROJCS[\"NTF_Lambert_Zone_IV\",GEOGCS[\"GCS_NTF\",DATUM[\"D_NTF\",SPHEROID[\"Clarke_1880_IGN\",6378249.2,293.46602]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",234.358],PARAMETER[\"False_Northing\",185861.369],PARAMETER[\"Central_Meridian\",2.3372291667],PARAMETER[\"Standard_Parallel_1\",41.5603877778],PARAMETER[\"Standard_Parallel_2\",42.7676633333],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",42.165],UNIT[\"Meter\",1.0]]";
			break;
		case Lambert2E:
			code ="PROJCS[\"NTF_Lambert_II_étendu\",GEOGCS[\"GCS_NTF\",DATUM[\"D_NTF\",SPHEROID[\"Clarke_1880_IGN\",6378249.2,293.46602]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",600000.0],PARAMETER[\"False_Northing\",2200000.0],PARAMETER[\"Central_Meridian\",2.3372291667],PARAMETER[\"Standard_Parallel_1\",45.8989188889],PARAMETER[\"Standard_Parallel_2\",47.6960144444],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",46.8],UNIT[\"Meter\",1.0]]";
			break;
		case Lambert93:
			code ="PROJCS[\"RGF93_Lambert_93\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",700000.0],PARAMETER[\"False_Northing\",6600000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",44.0],PARAMETER[\"Standard_Parallel_2\",49.0],PARAMETER[\"Latitude_Of_Origin\",46.5],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC42:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_1\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",1200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",41.25],PARAMETER[\"Standard_Parallel_2\",42.75],PARAMETER[\"Latitude_Of_Origin\",42.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC43:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_2\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",2200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",42.25],PARAMETER[\"Standard_Parallel_2\",43.75],PARAMETER[\"Latitude_Of_Origin\",43.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC44:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_3\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",3200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",43.25],PARAMETER[\"Standard_Parallel_2\",44.75],PARAMETER[\"Latitude_Of_Origin\",44.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC45:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_4\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",4200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",44.25],PARAMETER[\"Standard_Parallel_2\",45.75],PARAMETER[\"Latitude_Of_Origin\",45.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC46:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_5\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",5200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",45.25],PARAMETER[\"Standard_Parallel_2\",46.75],PARAMETER[\"Latitude_Of_Origin\",46.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC47:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_6\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",6200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",46.25],PARAMETER[\"Standard_Parallel_2\",47.75],PARAMETER[\"Latitude_Of_Origin\",47.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC48:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_7\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",7200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",47.25],PARAMETER[\"Standard_Parallel_2\",48.75],PARAMETER[\"Latitude_Of_Origin\",48.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC49:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_8\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",8200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",48.25],PARAMETER[\"Standard_Parallel_2\",49.75],PARAMETER[\"Latitude_Of_Origin\",49.0],UNIT[\"Meter\",1.0]]";
			break;
		case LambertCC50:
			code ="PROJCS[\"RGF_1993_Lambert_Zone_9\",GEOGCS[\"GCS_RGF_1993\",DATUM[\"D_RGF_1993\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",1700000.0],PARAMETER[\"False_Northing\",9200000.0],PARAMETER[\"Central_Meridian\",3.0],PARAMETER[\"Standard_Parallel_1\",49.25],PARAMETER[\"Standard_Parallel_2\",50.75],PARAMETER[\"Latitude_Of_Origin\",50.0],UNIT[\"Meter\",1.0]]";
			break;

    case ETRS89LAEA:
      code ="PROJCS[\"ETRS89 / ETRS-LAEA\",GEOGCS[\"ETRS89\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Lambert_Azimuthal_Equal_Area\"],PARAMETER[\"latitude_of_origin\",52],PARAMETER[\"central_meridian\",10],PARAMETER[\"false_easting\",4321000],PARAMETER[\"false_northing\",3210000],UNIT[\"Meter\",1]]";
      break;
    case ETRS89LCC:
      code ="PROJCS[\"ETRS89 / ETRS-LCC\",GEOGCS[\"ETRS89\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"standard_parallel_1\",35],PARAMETER[\"standard_parallel_2\",65],PARAMETER[\"latitude_of_origin\",52],PARAMETER[\"central_meridian\",10],PARAMETER[\"false_easting\",4000000],PARAMETER[\"false_northing\",2800000],UNIT[\"Meter\",1]]";
      break;
    case ETRS89TM30:
      code = "PROJCS[\"ETRS89 / ETRS-TM30\",GEOGCS[\"ETRS89\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-3],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
      break;
    case ETRS89TM31:
      code = "PROJCS[\"ETRS89 / ETRS-TM31\",GEOGCS[\"ETRS89\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",3],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
      break;
    case ETRS89TM32:
      code = "PROJCS[\"ETRS89 / ETRS-TM32\",GEOGCS[\"ETRS89\",DATUM[\"D_ETRS_1989\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",9],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
      break;

		// Antille
		case FortDesaix:
			code ="PROJCS[\"Fort_Dessaix_UTM_Zone_20N\",GEOGCS[\"GCS_Fort_Desaix\",DATUM[\"<custom>\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-63.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case SainteAnne:
			code ="PROJCS[\"Sainte_Anne_UTM_Zone_20N\",GEOGCS[\"GCS_Sainte_Anne\",DATUM[\"<custom>\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-63.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case FortMarigot:
			code ="PROJCS[\"Fort_Marigot_UTM_Zone_20N\",GEOGCS[\"GCS_Fort_Marigot\",DATUM[\"<custom>\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-63.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case RRAF:
			code ="PROJCS[\"WGS_1984_UTM_Zone_20N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000],PARAMETER[\"False_Northing\",0],PARAMETER[\"Central_Meridian\",-63],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0],UNIT[\"Meter\",1]]";
			break;
    case RGAF09:
      code ="PROJCS[\"RGAF09_UTM_zone_20N\",GEOGCS[\"GCS_RGAF09\",DATUM[\"D_Reseau_Geodesique_des_Antilles_Francaises_2009\",SPHEROID[\"GRS_1980\",6378137,298.257222101]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-63],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
      break;

		// Guyane
		case CSG1967_UTM21:
			code ="PROJCS[\"CSG67_UTM_Zone_21N\",GEOGCS[\"GCS_CSG67\",DATUM[\"D_International_1924\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-57.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case CSG1967_UTM22: 
			code ="PROJCS[\"CSG67_UTM_Zone_22N\",GEOGCS[\"GCS_CSG67\",DATUM[\"D_International_1924\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-51.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case RGFG95: 
			code ="PROJCS[\"RGFG95_UTM_Zone_22N\",GEOGCS[\"GCS_RGFG95\",DATUM[\"D_GRS_1980\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-51.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;

		// Reunion
		case PitonNeiges: 
			code ="PROJCS[\"Gauss_Laborde_Réunion\",GEOGCS[\"GCS_Piton_des_Neiges\",DATUM[\"D_Piton_des_Neiges\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",160000.0],PARAMETER[\"False_Northing\",50000.0],PARAMETER[\"Central_Meridian\",55.53333333333335],PARAMETER[\"Scale_Factor\",1.0],PARAMETER[\"Latitude_Of_Origin\",-21.11666666666667],UNIT[\"Meter\",1.0]]";
			break;
		case RGR92: 
			code ="PROJCS[\"RGR92_UTM_Zone_40S\",GEOGCS[\"GCS_RGR_1992\",DATUM[\"D_RGR_1992\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",57.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;

		// Saint Pierre et Miquelon
		case SPMiquelon1950:
			code ="PROJCS[\"IGN50_Clarke1866_UTM_Zone_21N\",GEOGCS[\"GCS_IGN50_Clarke1866\",DATUM[\"D_Clarke_1866\",SPHEROID[\"Clarke_1866\",6378206.4,294.9786982]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-57.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case RGSPM06:
			code ="PROJCS[\"WGS_1984_UTM_Zone_21N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-57.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;

		// Mayotte
		case Combani1950:
			code ="PROJCS[\"IGN50_UTM_Zone_38S\",GEOGCS[\"GCS_IGN50\",DATUM[\"D_International_1924\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",45.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;
		case Cadastre1997:
			code ="PROJCS[\"Cadastre 1997 - UTM fuseau 38 Sud\",GEOGCS[\"Cadastre 1997\",DATUM[\"D_Cadastre_1997\",SPHEROID[\"International_1924\",6378388,297]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",45],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",10000000],UNIT[\"Meter\",1]]";
			break;
		case RGM04:
			code ="PROJCS[\"WGS_1984_UTM_Zone_38S\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",45.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]";
			break;

      // Nouvelle-Caledonie
    case NC_IGN72:
      code ="PROJCS[\"IGN72_Grande_Terre_UTM_58S\",GEOGCS[\"GCS_IGN72_Grande_Terre\",DATUM[\"D_IGN72_Grande_Terre\",SPHEROID[\"International_1924\",6378388.0,297.0]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",165.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",3060]]";
      break;
    case NC_NEA74:
      code ="PROJCS[\"NEA74 Noumea / UTM zone 58S\",GEOGCS[\"NEA74 Noumea\",DATUM[\"D_NEA74_Noumea\",SPHEROID[\"International_1924\",6378388,297]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",165],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",10000000],UNIT[\"Meter\",1]]";
      break;
    case NC_RGNC91_Lambert:
      code ="PROJCS[\"RGNC_1991_93_Lambert_New_Caledonia\",GEOGCS[\"GCS_RGNC_1991-93\",DATUM[\"D_Reseau_Geodesique_de_Nouvelle_Caledonie_1991-93\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",400000.0],PARAMETER[\"False_Northing\",300000.0],PARAMETER[\"Central_Meridian\",166.0],PARAMETER[\"Standard_Parallel_1\",-20.66666666666667],PARAMETER[\"Standard_Parallel_2\",-22.33333333333333],PARAMETER[\"Latitude_Of_Origin\",-21.5],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",3163]]";
      break;
    case NC_RGNC91_UTM57:
      code ="PROJCS[\"RGNC_1991-93_UTM_Zone_57S\",GEOGCS[\"GCS_RGNC_1991-93\",DATUM[\"D_Reseau_Geodesique_de_Nouvelle_Caledonie_1991-93\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",159.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",3169]]";
      break;
    case NC_RGNC91_UTM58:
      code ="PROJCS[\"RGNC_1991-93_UTM_Zone_58S\",GEOGCS[\"GCS_RGNC_1991-93\",DATUM[\"D_Reseau_Geodesique_de_Nouvelle_Caledonie_1991-93\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",165.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",3170]]";
      break;
    case NC_RGNC91_UTM59:
      code ="PROJCS[\"RGNC_1991-93_UTM_Zone_59S\",GEOGCS[\"GCS_RGNC_1991-93\",DATUM[\"D_Reseau_Geodesique_de_Nouvelle_Caledonie_1991-93\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",171.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",3171]]";
      break;

      // WebMercator
    case WebMercator:
      code = "PROJCS[\"WGS 84 / Pseudo-Mercator\",GEOGCS[\"Popular Visualisation CRS\",DATUM[\"D_Popular_Visualisation_Datum\",SPHEROID[\"Popular_Visualisation_Sphere\",6378137,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Mercator\"],PARAMETER[\"central_meridian\",0],PARAMETER[\"scale_factor\",1],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
      break;
	}
	return code;
}

//-----------------------------------------------------------------------------
// Renvoie le code DATUM pour ECW
//-----------------------------------------------------------------------------
std::string XGeoProjection::EcwDatum(XProjCode proj)
{
	std::string datum;
	switch(proj) {
		// France Metropolitaine
		case RGF93: datum = "RGF93"; break;
		case Lambert1: datum = "NTF"; break;
		case Lambert2: datum = "NTF"; break;
		case Lambert3: datum = "NTF"; break;
		case Lambert4: datum = "NTF"; break;
		case Lambert2E: datum = "NTF"; break;
		case Lambert93: datum = "RGF93"; break;
		case LambertCC42: datum = "RGF93"; break;
		case LambertCC43: datum = "RGF93"; break;
		case LambertCC44: datum = "RGF93"; break;
		case LambertCC45: datum = "RGF93"; break;
		case LambertCC46: datum = "RGF93"; break;
		case LambertCC47: datum = "RGF93"; break;
		case LambertCC48: datum = "RGF93"; break;
		case LambertCC49: datum = "RGF93"; break;
		case LambertCC50: datum = "RGF93"; break;

		// Antille
		case FortDesaix: datum = "MART38"; break;
		case SainteAnne: datum = "GUAD48"; break;
		case FortMarigot: datum = "STMART"; break;
		case RRAF: datum = "WGS84"; break;
    case RGAF09: datum = "WGS84"; break;

		// Guyane
		case CSG1967_UTM21: datum = ""; break;
		case CSG1967_UTM22: datum = "CSG67"; break;
		case RGFG95: datum = "RGF95"; break;

		// Reunion
		case PitonNeiges: datum = "REUN47"; break;
		case RGR92: datum = "RGR92"; break;

		// Saint Pierre et Miquelon
		case SPMiquelon1950: datum = "SPM50"; break;
		case RGSPM06: datum = "RGSPM06"; break;

		// Mayotte
		case Combani1950: datum = "MCBN50"; break;
		case Cadastre1997: datum = ""; break;
		case RGM04: datum = "RGM04"; break;

		default : datum = "";
	}
	return datum;
}
	
//-----------------------------------------------------------------------------
// Renvoie le code PROJECTION pour ECW
//-----------------------------------------------------------------------------
std::string XGeoProjection::EcwProjection(XProjCode proj)
{
	std::string code;
	switch(proj) {
		// France Metropolitaine
		case RGF93: code = ""; break;
		case Lambert1: code = "LM1FRE1D"; break;
		case Lambert2: code = "LM1FRE2D"; break;
		case Lambert3: code = "LM1FRE3D"; break;
		case Lambert4: code = "LM1FRE4D"; break;
		case Lambert2E: code = "LM2FRANC"; break;
		case Lambert93: code = "LMFRAN93"; break;
		case LambertCC42: code = "LM42Z1"; break;
		case LambertCC43: code = "LM43Z2"; break;
		case LambertCC44: code = "LM44Z3"; break;
		case LambertCC45: code = "LM45Z4"; break;
		case LambertCC46: code = "LM46Z5"; break;
		case LambertCC47: code = "LM47Z6"; break;
		case LambertCC48: code = "LM48Z7"; break;
		case LambertCC49: code = "LM49Z8"; break;
		case LambertCC50: code = "LM50Z9"; break;

		// Antille
		case FortDesaix: code = "NUTM20"; break;
		case SainteAnne: code = "NUTM20"; break;
		case FortMarigot: code = "NUTM20"; break;
		case RRAF: code = "NUTM20"; break;
    case RGAF09: code = "NUTM20"; break;

		// Guyane
		case CSG1967_UTM21: code = "NUTM21"; break;
		case CSG1967_UTM22: code = "NUTM22"; break;
		case RGFG95: code = "NUTM22"; break;

		// Reunion
		case PitonNeiges: code = "GLABREUN"; break;
		case RGR92: code = "SUTM40"; break;

		// Saint Pierre et Miquelon
		case SPMiquelon1950: code = "NUTM21"; break;
		case RGSPM06: code = "NUTM21"; break;

		// Mayotte
		case Combani1950: code = "SUTM38"; break;
		case Cadastre1997: code = "SUTM38"; break;
		case RGM04: code = "SUTM38"; break;

		default : code = "";
	}
	return code;
}

//-----------------------------------------------------------------------------
// Renvoie le nom de la projection
//-----------------------------------------------------------------------------
std::string XGeoProjection::ProjectionName(XProjCode proj)
{
	std::string name;
	switch(proj) {
		// France Metropolitaine
    case RGF93: name = "WGS84 (RGF93)"; break;
		case Lambert1: name = "Lambert 1"; break;
		case Lambert2: name = "Lambert 2"; break;
		case Lambert3: name = "Lambert 3"; break;
		case Lambert4: name = "Lambert 4"; break;
		case Lambert2E: name = "Lambert 2E"; break;
		case Lambert93: name = "Lambert 93"; break;
		case LambertCC42: name = "Lambert CC42"; break;
		case LambertCC43: name = "Lambert CC43"; break;
		case LambertCC44: name = "Lambert CC44"; break;
		case LambertCC45: name = "Lambert CC45"; break;
		case LambertCC46: name = "Lambert CC46"; break;
		case LambertCC47: name = "Lambert CC47"; break;
		case LambertCC48: name = "Lambert CC48"; break;
		case LambertCC49: name = "Lambert CC49"; break;
		case LambertCC50: name = "Lambert CC50"; break;

    // Europe
    case ETRS89LCC: name = "ETRS89-LCC"; break;
    case ETRS89LAEA: name = "ETRS89-LAEA"; break;
    case ETRS89TM30: name = "ETRS89TM 30 (UTM30)"; break;
    case ETRS89TM31: name = "ETRS89TM 31 (UTM31)"; break;
    case ETRS89TM32: name = "ETRS89TM 32 (UTM32)"; break;

    // Antilles
		case FortDesaix: name = "Martinique - Fort Desaix"; break;
		case SainteAnne: name = "Guadeloupe - Sainte Anne"; break;
		case FortMarigot: name = "Guadeloupe - Fort Marigot"; break;
		case RRAF: name = "Antilles - RRAF UTM20"; break;
    case RGAF09: name = "Antilles - RGAF09 UTM20"; break;

		// Guyane
		case CSG1967_UTM21: name = "Guyane - CSG1967 UTM21"; break;
		case CSG1967_UTM22: name = "Guyane - CSG1967 UTM22"; break;
		case RGFG95: name = "Guyane - RGFG95 UTM22"; break;

		// Reunion
		case PitonNeiges: name = "Réunion - Piton des Neiges"; break;
		case RGR92: name = "Réunion - RGR92 UTM40"; break;

		// Saint Pierre et Miquelon
		case SPMiquelon1950: name = "Saint Pierre et Miquelon 1950"; break;
		case RGSPM06: name = "RGSPM06 UTM21"; break;

		// Mayotte
		case Combani1950: name = "Combani 1950"; break;
		case Cadastre1997: name = "Cadastre 1997"; break;
		case RGM04: name = "RGM04 UTM38"; break;

    // WebMercator
    case WebMercator: name = "Web Mercator"; break;

    // Nouvelle-Caledonie
    case NC_IGN72: name = "IGN72 Grande Terre"; break;
    case NC_NEA74: name = "NEA74 Nouméa"; break;
    case NC_RGNC91_Lambert: name = "RGNC1991 LambertNC"; break;
    case NC_RGNC91_UTM57: name = "RGNC1991 UTM57"; break;
    case NC_RGNC91_UTM58: name = "RGNC1991 UTM58"; break;
    case NC_RGNC91_UTM59: name = "RGNC1991 UTM59"; break;

		default : name = "";
	}
	return name;
}

//-----------------------------------------------------------------------------
// Renvoie le code EPSG de la projection
//-----------------------------------------------------------------------------
uint32_t XGeoProjection::EPSGCode(XProjCode proj)
{
	switch(proj) {
		// France Metropolitaine
		case RGF93: return 4171;
		case Lambert1: return 27561;
		case Lambert2: return 27562;
		case Lambert3: return 27563;
		case Lambert4: return 27564;
		case Lambert2E: return 27572;
		case Lambert93: return 2154;
		case LambertCC42: return 3942;
		case LambertCC43: return 3943;
		case LambertCC44: return 3944;
		case LambertCC45: return 3945;
		case LambertCC46: return 3946;
		case LambertCC47: return 3947;
		case LambertCC48: return 3948;
		case LambertCC49: return 3949;
		case LambertCC50: return 3950;

    // Europe
    case ETRS89LCC: return 3034;
    case ETRS89LAEA: return 3035;
    case ETRS89TM30: return 3042;
    case ETRS89TM31: return 3043;
    case ETRS89TM32: return 3044;

    // Antille
		case FortDesaix: return 2973;
		case SainteAnne: return 2970;
		case FortMarigot: return 2969;
		case RRAF: return 2989;
    case RGAF09: return 5490;

		// Guyane
		case CSG1967_UTM21: return 3312;
		case CSG1967_UTM22: return 2971;
		case RGFG95: return 2972;

		// Reunion
		case PitonNeiges: return 3727;
		case RGR92: return 2975;

		// Saint Pierre et Miquelon
		case SPMiquelon1950: return 2987;
		case RGSPM06: return 0;

		// Mayotte
		case Combani1950: return 2980;
		case Cadastre1997: return 0;
		case RGM04: return 0;

    // WebMercator
    case WebMercator: return 3857;

    // Nouvelle-Caledonie
    case NC_IGN72: return 3060;
    case NC_NEA74: return 2998;
    case NC_RGNC91_Lambert: return 3163;
    case NC_RGNC91_UTM57: return 3169;
    case NC_RGNC91_UTM58: return 3170;
    case NC_RGNC91_UTM59: return 3171;

		//default : return 0;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Renvoie le cadre (en geographique) de la projection
//-----------------------------------------------------------------------------
XFrame XGeoProjection::FrameGeo(XProjCode proj)
{
  XFrame France(-7.000, 40.5000, 11.5000, 51.5400);
  XFrame Antille(-63.154, 14.000, -60.500, 18.125);
  XFrame Guyane(-54.600, 2.111, -51.622, 5.800);
  XFrame Reunion(55.200, -21.50, 55.900, -20.700);
  XFrame StPierre(-56.500, 46.700, -56.000 , 47.200);
  XFrame Mayotte(44.800, -13.100, 45.4, -12.500);
  XFrame Caledonie(163, -23, 169, -19);
  XFrame Monde(-180, -80, 180, 80);


	switch(proj) {
	// France Metropolitaine
	case RGF93: return France;
	case Lambert1: return France;
	case Lambert2: return France;
	case Lambert3: return France;
	case Lambert4: return France;
	case Lambert2E: return France;;
	case Lambert93: return France;
	case LambertCC42: return France;
	case LambertCC43: return France;
	case LambertCC44: return France;
	case LambertCC45: return France;
	case LambertCC46: return France;
	case LambertCC47: return France;
	case LambertCC48: return France;
	case LambertCC49: return France;
	case LambertCC50: return France;

    // Europe
  case ETRS89LCC: return France;
  case ETRS89LAEA: return France;
  case ETRS89TM30: return France;
  case ETRS89TM31: return France;
  case ETRS89TM32: return France;

    // Antilles
	case FortDesaix: return Antille;
	case SainteAnne: return Antille;
	case FortMarigot: return Antille;
	case RRAF: return Antille;
  case RGAF09: return Antille;

		// Guyane
	case CSG1967_UTM21: return Guyane;
	case CSG1967_UTM22: return Guyane;
	case RGFG95: return Guyane;

		// Reunion
	case PitonNeiges: return Reunion;
	case RGR92: return Reunion;

		// Saint Pierre et Miquelon
	case SPMiquelon1950: return StPierre;
	case RGSPM06: return StPierre;

		// Mayotte
	case Combani1950: return Mayotte;
	case Cadastre1997: return Mayotte;
	case RGM04: return Mayotte;

    // Nouvelle-Caledonie
  case NC_IGN72: return Caledonie;
  case NC_NEA74: return Caledonie;
  case NC_RGNC91_Lambert: return Caledonie;
  case NC_RGNC91_UTM57: return Caledonie;
  case NC_RGNC91_UTM58: return Caledonie;
  case NC_RGNC91_UTM59: return Caledonie;

  case WebMercator : return Monde;

	default : return XFrame();
	}
	return XFrame();
}

//-----------------------------------------------------------------------------
// Renvoie le cadre (en projection) de la projection
//-----------------------------------------------------------------------------
XFrame XGeoProjection::FrameProj(XProjCode proj)
{
  XFrame France(-300000, 6100000, 1400000, 7250000);
  XFrame Antille(450000, 1550000, 750000, 2010000);
	XFrame Guyane(100000, 200000, 500000, 700000);
	XFrame Reunion(300000, 7630000, 400000, 7700000);
	XFrame StPierre(530000, 5172000, 575000, 5228000);
	XFrame Mayotte(495000, 8560000, 540000, 8612000);
  XFrame WebMercatorFrance(-600000, 5000000, 1050000, 6640000);
  XFrame Caledonie(142750, 159000, 631000, 5155000);

	switch(proj) {
	// France Metropolitaine
  case RGF93: return France;
	case Lambert1: return France;
	case Lambert2: return France;
	case Lambert3: return France;
	case Lambert4: return France;
	case Lambert2E: return France;;
	case Lambert93: return France;
	case LambertCC42: return France;
	case LambertCC43: return France;
	case LambertCC44: return France;
	case LambertCC45: return France;
	case LambertCC46: return France;
	case LambertCC47: return France;
	case LambertCC48: return France;
	case LambertCC49: return France;
	case LambertCC50: return France;

    // Europe
  case ETRS89LCC: return France;
  case ETRS89LAEA: return France;
  case ETRS89TM30: return France;
  case ETRS89TM31: return France;
  case ETRS89TM32: return France;

    // Antilles
	case FortDesaix: return Antille;
	case SainteAnne: return Antille;
	case FortMarigot: return Antille;
	case RRAF: return Antille;
  case RGAF09: return Antille;

		// Guyane
	case CSG1967_UTM21: return Guyane;
	case CSG1967_UTM22: return Guyane;
	case RGFG95: return Guyane;

		// Reunion
	case PitonNeiges: return Reunion;
	case RGR92: return Reunion;

		// Saint Pierre et Miquelon
	case SPMiquelon1950: return StPierre;
	case RGSPM06: return StPierre;

		// Mayotte
	case Combani1950: return Mayotte;
	case Cadastre1997: return Mayotte;
	case RGM04: return Mayotte;

    // WebMercator
  case WebMercator: return WebMercatorFrance;

    // Nouvelle-Caledonie
  case NC_IGN72: return Caledonie;
  case NC_NEA74: return Caledonie;
  case NC_RGNC91_Lambert: return Caledonie;
  case NC_RGNC91_UTM57: return Caledonie;
  case NC_RGNC91_UTM58: return Caledonie;
  case NC_RGNC91_UTM59: return Caledonie;

	default : return XFrame();
	}
	return XFrame();
}
