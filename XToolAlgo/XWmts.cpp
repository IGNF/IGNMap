//-----------------------------------------------------------------------------
//								XWmts.cpp
//								=========
//
// Gestion des flux WMTS
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 15/11/2012
//-----------------------------------------------------------------------------

#include "XWmts.h"
#include <algorithm>
#include "../../XTool/XParserXML.h"
#include "../../XTool/XXml.h"

//-----------------------------------------------------------------------------
// XTileMatrix : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XTileMatrix::XmlRead(XParserXML* parser, uint32_t num)
{
  XParserXML tile = parser->FindSubParser("/TileMatrix", num);
  if (tile.IsEmpty())
    return false;
  m_strId = tile.ReadNode("/TileMatrix/ows:Identifier");
  m_dScaleDenom = tile.ReadNodeAsDouble("/TileMatrix/ScaleDenominator");
  m_nTileW = tile.ReadNodeAsUInt32("/TileMatrix/TileWidth");
  m_nTileH = tile.ReadNodeAsUInt32("/TileMatrix/TileHeight");
  m_nMatrixW = tile.ReadNodeAsUInt32("/TileMatrix/MatrixWidth");
  m_nMatrixH = tile.ReadNodeAsUInt32("/TileMatrix/MatrixHeight");

  std::string pos = tile.ReadNode("/TileMatrix/TopLeftCorner");
  if (std::sscanf(pos.c_str(), "%lf %lf", &m_dXTL, &m_dYTL) != 2)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
// XTileMatrixSet : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XTileMatrixSet::XmlRead(XParserXML* parser, uint32_t num)
{
  XParserXML tileset = parser->FindSubParser("/TileMatrixSet", num);
  if (tileset.IsEmpty())
    return false;
  m_strId = XXml::XmlToOem(tileset.ReadNode("/TileMatrixSet/ows:Identifier"));
  m_strCrs = XXml::XmlToOem(tileset.ReadNode("/TileMatrixSet/ows:SupportedCRS"));
  //std::transform(m_strCrs.begin(), m_strCrs.end(), m_strCrs.begin(), toupper);
  std::transform(m_strCrs.begin(), m_strCrs.end(), m_strCrs.begin(), [](char c) {return static_cast<char>(std::toupper(c)); });

  XParserXML tile;
  uint32_t nb_tile = 0;
  while (true) {
    tile = tileset.FindSubParser("/TileMatrixSet/TileMatrix", nb_tile);
    if (tile.IsEmpty())
      break;
    XTileMatrix tm;
    if (tm.XmlRead(&tile))
      m_T.push_back(tm);
    nb_tile++;
  }
  std::sort(m_T.begin(), m_T.end(), XTileMatrix::predTileMatrix);

  return true;
}


//-----------------------------------------------------------------------------
// XTileMatrixSet : Resolution des Tiles
//-----------------------------------------------------------------------------
std::vector<double> XTileMatrixSet::GSD()
{
  std::vector<double> resol;
  for (int i = 0; i < m_T.size(); i++)
    resol.push_back(m_T[i].GSD());
  std::sort(resol.begin(), resol.end());
  return resol;
}

//-----------------------------------------------------------------------------
// XTileMatrixLimits : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XTileMatrixLimits::XmlRead(XParserXML* parser, uint32_t num)
{
  XParserXML tileset = parser->FindSubParser("/TileMatrixLimits", num);
  if (tileset.IsEmpty())
    return false;

  m_strId = tileset.ReadNode("/TileMatrixLimits/TileMatrix");
  m_nMinTileRow = tileset.ReadNodeAsUInt32("/TileMatrixLimits/MinTileRow");
  m_nMaxTileRow = tileset.ReadNodeAsUInt32("/TileMatrixLimits/MaxTileRow");
  m_nMinTileCol = tileset.ReadNodeAsUInt32("/TileMatrixLimits/MinTileCol");
  m_nMaxTileCol = tileset.ReadNodeAsUInt32("/TileMatrixLimits/MaxTileCol");

  return true;
}

//-----------------------------------------------------------------------------
// XTileMatrixSetLink : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XTileMatrixSetLink::XmlRead(XParserXML* parser, uint32_t num)
{
  XParserXML tileset = parser->FindSubParser("/TileMatrixSetLink", num);
  if (tileset.IsEmpty())
    return false;

  m_strTileMatrixSetId = XXml::XmlToOem(tileset.ReadNode("/TileMatrixSetLink/TileMatrixSet"));
  XParserXML limits;
  limits = tileset.FindSubParser("/TileMatrixSetLink/TileMatrixSetLimits");
  if (limits.IsEmpty())
    return false;
  XParserXML tile;
  uint32_t nb_limits = 0;
  while (true) {
    tile = limits.FindSubParser("/TileMatrixSetLimits/TileMatrixLimits", nb_limits);
    if (tile.IsEmpty())
      break;
    XTileMatrixLimits tileLimits;
    if (tileLimits.XmlRead(&tile))
      m_L.push_back(tileLimits);
    nb_limits++;
  }
  std::sort(m_L.begin(), m_L.end(), XTileMatrixLimits::predTileMatrixLimits);

  return true;
}

//-----------------------------------------------------------------------------
// XWmtsLayer : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XWmtsLayer::XmlRead(XParserXML* parser, uint32_t num)
{
  XParserXML layer = parser->FindSubParser("/Layer", num);
  if (layer.IsEmpty())
    return false;

  m_strId = XXml::XmlToOem(layer.ReadNode("/Layer/ows:Identifier"));
  m_strTitle = XXml::XmlToOem(layer.ReadNode("/Layer/ows:Title"));
  m_strAbstract = XXml::XmlToOem(layer.ReadNode("/Layer/ows:Abstract"));
  m_strFormat = XXml::XmlToOem(layer.ReadNode("/Layer/Format"));

  uint32_t nb_tileMatrixSetLink = 0;
  XParserXML tileMatrixSetLink;
  while (true) {
    tileMatrixSetLink = layer.FindSubParser("/Layer/TileMatrixSetLink", nb_tileMatrixSetLink);
    if (tileMatrixSetLink.IsEmpty())
      break;
    nb_tileMatrixSetLink++;
    XTileMatrixSetLink tm;
    if (tm.XmlRead(&tileMatrixSetLink))
      m_SetLink.push_back(tm);
  }
  if (m_SetLink.size() < 1)
    return false;

  return true;
}

//-----------------------------------------------------------------------------
// XWmtsCapabilities : Lecture dans un fichier XML issu d'un GetConfig
//-----------------------------------------------------------------------------
bool XWmtsCapabilities::XmlRead(XParserXML* parser, uint32_t /*num*/)
{
  XParserXML capabilities = parser->FindSubParser("/Capabilities");
  if (capabilities.IsEmpty())
    return false;
  XParserXML contents = capabilities.FindSubParser("/Capabilities/Contents");
  if (contents.IsEmpty())
    return false;

  // Lecture des TileMatrixSet
  uint32_t nb_tileMatrixSet = 0;
  XParserXML tileMatrixSet;
  while (true) {
    tileMatrixSet = contents.FindSubParser("/Contents/TileMatrixSet", nb_tileMatrixSet);
    if (tileMatrixSet.IsEmpty())
      break;
    nb_tileMatrixSet++;
    XTileMatrixSet tm;
    if (tm.XmlRead(&tileMatrixSet))
      m_MatrixSet.push_back(tm);
  }
  if (m_MatrixSet.size() < 1)
    return false;

  // Lecture des Layers
  uint32_t nb_layer = 0;
  XParserXML layer;
  while (true) {
    layer = contents.FindSubParser("/Contents/Layer", nb_layer);
    if (layer.IsEmpty())
      break;
    nb_layer++;
    XWmtsLayer wmtsLayer;
    if (wmtsLayer.XmlRead(&layer))
      m_Layer.push_back(wmtsLayer);
  }
  if (m_Layer.size() < 1)
    return false;

  return true;
}

//-----------------------------------------------------------------------------
// XWmtsCapabilities : recupere un layer et la pyramide associee
//-----------------------------------------------------------------------------
bool XWmtsCapabilities::SetLayerTMS(XWmtsLayerTMS* layer, std::string layerId, std::string tmsId)
{
  if (layer == nullptr)
    return false;
  // Recherche du layer
  int indexLayer = -1;
  for (int i = 0; i < m_Layer.size(); i++) {
    if (layerId == m_Layer[i].m_strId) {
      indexLayer = i;
      break;
    }
  }
  if (indexLayer < 0)
    return false;
  // Recherche de la pyramide dans le layer
  int indexLink = -1;
  for (int i = 0; i < m_Layer[indexLayer].m_SetLink.size(); i++) {
    if (tmsId == m_Layer[indexLayer].m_SetLink[i].Id()) {
      indexLink = i;
      break;
    }
  }
  if (indexLink < 0)
    return false;
  // Recherche de la pyramide dans la collection de pyramide
  int indexTms = -1;
  for (int i = 0; i < m_MatrixSet.size(); i++) {
    if (tmsId == m_MatrixSet[i].Id()) {
      indexTms = i;
      break;
    }
  }
  if (indexTms < 0)
    return false;
  // Construction de l'objet
  layer->m_strId = m_Layer[indexLayer].m_strId;
  layer->m_strFormat = m_Layer[indexLayer].m_strFormat;
  layer->m_strAbstract = m_Layer[indexLayer].m_strAbstract;
  layer->m_strTitle = m_Layer[indexLayer].m_strTitle;

  layer->m_TMS = m_MatrixSet[indexTms];

  // Construction des limites
  layer->m_Limits.clear();
  for (int i = 0; i < layer->m_TMS.NbTile(); i++) {
    XTileMatrixLimits L;
    layer->m_Limits.push_back(L);
    for (int j = 0; j < m_Layer[indexLayer].m_SetLink[indexLink].NbLimits(); j++) {
      XTileMatrixLimits limits = m_Layer[indexLayer].m_SetLink[indexLink].Limits(j);
      if (limits.Id() == layer->m_TMS.Tile(i).Id())
        layer->m_Limits[i] = limits;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// XWmtsCapabilities : recupere les informations sur tous les layers
//-----------------------------------------------------------------------------
bool XWmtsCapabilities::GetLayerInfo(std::vector<LayerInfo>& L)
{
  L.clear();
  for (size_t i = 0; i < m_Layer.size(); i++) {
    for (size_t j = 0; j < m_Layer[i].m_SetLink.size(); j++) {
      LayerInfo info;
      info.Id = m_Layer[i].m_strId;
      info.Title = m_Layer[i].m_strTitle;
      info.TMS = m_Layer[i].m_SetLink[j].Id();
      // Recherche de la pyramide dans la collection de pyramide
      for (size_t k = 0; k < m_MatrixSet.size(); k++) {
        if (info.TMS == m_MatrixSet[k].Id()) {
          info.Projection = m_MatrixSet[k].Crs();
          L.push_back(info);
          break;
        }
      }
    }
  }
  return true;
}