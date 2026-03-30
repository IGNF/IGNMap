//-----------------------------------------------------------------------------
//								ProfilViewer.cpp
//								================
//
// Visualisation d'un profil altimetrique
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 27/03/2027
//-----------------------------------------------------------------------------

#include "ProfilViewer.h"
#include "../XTool/XGeoBase.h"
#include "LasShader.h"

//==============================================================================
// Dessin du profil
//==============================================================================
void ProfilViewer::ProfilDrawer::paint(juce::Graphics& g)
{
  auto b = getLocalBounds();
  int orig = 10;
  int W = XRint(b.getWidth() * m_dScale), H = XRint(b.getHeight() * 0.9);

  if ((m_ProfilDtm.size() < 2)&& (m_ProfilLas.size() < 2))
    return;

  // Recherche des min et max
  double zmin = std::numeric_limits<double>::max(), zmax = std::numeric_limits<double>::min();
  for (uint32_t i = 0; i < m_ProfilDtm.size(); i++) {
    zmin = XMin(zmin, m_ProfilDtm[i].Z);
    zmax = XMax(zmax, m_ProfilDtm[i].Z);
  }
  for (uint32_t i = 0; i < m_ProfilLas.size(); i++) {
    zmin = XMin(zmin, m_ProfilLas[i].Z);
    zmax = XMax(zmax, m_ProfilLas[i].Z);
  }
  zmin = zmin - (zmax - zmin) * 0.05;
  zmax = zmax + (zmax - zmin) * 0.05;

  // Dessin des axes
  g.setColour(juce::Colours::lightgrey);
  double step = 1;
  if (fabs(zmax - zmin) > 100.) step = 10;
  if (fabs(zmax - zmin) > 1000.) step = 100;
  if (fabs(zmax - zmin) < 1.) step = 0.1;

  int Z0 = XRint(floor(zmin / step) * step);
  int D = XRint(((zmax - zmin) / (step * 5)) * step);
  if (D >= 1) {
    for (int i = Z0 - D; i < zmax + 2 * D; i += D) {
      double Zn = (i - zmin) * H / fabs(zmax - zmin);
      g.drawLine(0.f, (float)(H - Zn), (float)(W + 5), (float)(H - Zn));
      g.drawSingleLineText(juce::String(i), 0.f, (float)(H - Zn));
    }
  }
  else {  // Cas des profils "plats"
    D = 1;
    for (int i = Z0 * 10 - D; i < zmax * 10 + 2 * D; i += D) {
      double Zn = (i * 0.1 - zmin) * H / fabs(zmax - zmin);
      g.drawLine(0, H - Zn, W + 5, H - Zn);
      g.drawSingleLineText(juce::String(i * 0.1), 0, H - Zn);
    }
  }

  // Dessin des valeurs LAS
  double delta = 0., zA, zB;
  if (m_ProfilLas.size() > 1) {
    g.setColour(juce::Colours::red);
    delta = (double)(W - 2 * orig) / (double)m_ProfilLas.size();
    zA = (m_ProfilLas[0].Z - zmin) * H / fabs(zmax - zmin);
    juce::Point<float> M((float)orig, (float)(H - zA)), N;
    for (uint32_t i = 0; i < m_ProfilLas.size() - 1; i++) {
      zB = (m_ProfilLas[i + 1].Z - zmin) * H / fabs(zmax - zmin);
      N = juce::Point<float>((float)(orig + (i + 1) * delta), (float)(H - zB));
      if (M.getDistanceSquaredFrom(N) >= 1) {
        g.drawLine(M.x, M.y, N.x, N.y);
        M = N;
      }
    }
  }

  // Dessin des valeurs DTM
  if (m_ProfilDtm.size() > 1) {
    g.setColour(juce::Colours::blue);
    delta = (double)(W - 2 * orig) / (double)m_ProfilDtm.size();
    zA = (m_ProfilDtm[0].Z - zmin) * H / fabs(zmax - zmin);
    juce::Point<float> M((float)orig, (float)(H - zA)), N;
    for (uint32_t i = 0; i < m_ProfilDtm.size() - 1; i++) {
      zB = (m_ProfilDtm[i + 1].Z - zmin) * H / fabs(zmax - zmin);
      N = juce::Point<float>((float)(orig + (i + 1) * delta), (float)(H - zB));
      if (M.getDistanceSquaredFrom(N) >= 1) {
        g.drawLine(M.x, M.y, N.x, N.y);
        M = N;
      }
    }
  }

  // Dessin du curseur
  if ((m_nMouseX >= 0) && (m_nMouseX < W) && (delta > 0.)) {
    g.setColour(juce::Colours::fuchsia);
    int index = XRint((m_nMouseX - orig) / delta);
    if ((index >= 0) && (index < m_ProfilDtm.size())) {
      zA = (m_ProfilDtm[index].Z - zmin) * H / fabs(zmax - zmin);
      g.drawEllipse((float)m_nMouseX - 3.f, (float)(H - zA) - 3.f, 6.f, 6.f, 2.f);
      ProfilComponent* comp = dynamic_cast<ProfilComponent*>(getParentComponent());
      if (comp != nullptr)
        comp->SetActiveIndex(index);
    }
    if ((index >= 0) && (index < m_ProfilLas.size())) {
      zA = (m_ProfilLas[index].Z - zmin) * H / fabs(zmax - zmin);
      g.drawEllipse((float)m_nMouseX - 3.f, (float)(H - zA) - 3.f, 6.f, 6.f, 2.f);
      ProfilComponent* comp = dynamic_cast<ProfilComponent*>(getParentComponent());
      if (comp != nullptr)
        comp->SetActiveIndex(index);
    }
  }
}

//==============================================================================
// Calcul du profil
//==============================================================================
bool ProfilViewer::ProfilComponent::ComputeProfil(XGeoObject* sel, double resol)
{
  m_Sel = nullptr;
  XGeoVector* selection = dynamic_cast<XGeoVector*>(sel);
  if ((m_Base == nullptr)||(selection == nullptr))
    return false;
  m_ProfilLas.clear();
  m_ProfilDtm.clear();
  if ((m_Base->Layer("LAS") == nullptr) && (m_Base->Layer("DTM") == nullptr))
    return false;
  if (selection->NbPt() < 2)
    return false;
  if ((selection->TypeVector() == XGeoVector::MPoint) || (selection->TypeVector() == XGeoVector::MPointZ))
    return false;
  if ((selection->TypeVector() == XGeoVector::Raster) || (selection->TypeVector() == XGeoVector::DTM))
    return false;

  if (!selection->LoadGeom2D())
    return false;
  bool lasMode = false, dtmMode = false;
  XGeoLayer* layerLAS = m_Base->Layer("LAS");
  if (layerLAS != nullptr) // On passe en mode LAS
    lasMode = true;
  XGeoLayer* layerDTM = m_Base->Layer("DTM");
  if (layerDTM != nullptr) // On passe en mode LAS
    dtmMode = true;

  XPt* Pt = selection->Pt();
  XPt2D A, B, V;
  XPt3D P;
  double d = 0.;
  double delta = resol;

 // Creation du profil en X ; Y
  A = Pt[0];
  for (uint32_t i = 0; i < selection->NbPt() - 1; i++) {
    B = Pt[i + 1];
    d = dist(A, B);
    if (d < delta)
      continue;
    V = (B - A) * delta / d;
    for (int j = 0; j < XRint(d / delta); j++) {
      P = A + V * j;
      P.Z = 0;
      if (dtmMode)
        m_ProfilDtm.push_back(P);
      if (lasMode)
        m_ProfilLas.push_back(P);
    }
    A = B;
  }
  selection->Unload();

  // Calcul du profil
  juce::MouseCursor::showWaitCursor();    // Curseur d'attente
  if (lasMode) {
    ComputeLasProfil(selection->Frame());
  }
  if (dtmMode) {
    for (size_t i = 0; i < m_ProfilDtm.size(); i++) {
      m_ProfilDtm[i].Z = m_Base->Z(m_ProfilDtm[i]);
      if (m_ProfilDtm[i].Z <= XGEO_NO_DATA)
        m_ProfilDtm[i].Z = 0;
    }
  }

  juce::MouseCursor::hideWaitCursor();  // Curseur initiale
  m_Drawer.m_ProfilLas = m_ProfilLas;
  m_Drawer.m_ProfilDtm = m_ProfilDtm;
  m_Drawer.repaint();
  m_Sel = selection;
  return true;
}

//==============================================================================
// Index actif du profil
//==============================================================================
bool ProfilViewer::ProfilComponent::SetActiveIndex(int index)
{
  if (index < 0) {
    m_fldX.SetValue("");
    m_fldY.SetValue("");
    m_fldZ.SetValue("");
    m_fldDist.SetValue("");
    m_fldDenivPos.SetValue("");
    m_fldDenivNeg.SetValue("");
    return false;
  }
  juce::String strX, strY, strZ, strDist, strDenivPos, strDenivNeg;
  double distance = 0., deniv_pos = 0., deniv_neg = 0., deniv = 0., Z0 = 0.;
  if (index < m_ProfilDtm.size()) {
    for (int k = 0; k < index; k++) {
      distance += dist((XPt2D)m_ProfilDtm[k + 1], (XPt2D)m_ProfilDtm[k]);
      deniv = m_ProfilDtm[k + 1].Z - m_ProfilDtm[k].Z;
      if (deniv > 0) deniv_pos += deniv; else deniv_neg += deniv;
    }
    strX = juce::String(m_ProfilDtm[index].X, 2);
    strY = juce::String(m_ProfilDtm[index].Y, 2);
    strZ = juce::String(m_ProfilDtm[index].Z, 2) + " | ";
    strDist = juce::String(distance, 2) + " | ";
    strDenivPos = juce::String(deniv_pos, 2) + " | ";
    strDenivNeg = juce::String(deniv_neg, 2) + " | ";
    Z0 = m_ProfilDtm[index].Z;
  }
  if (index < m_ProfilLas.size()) {
    for (int k = 0; k < index; k++) {
      distance += dist((XPt2D)m_ProfilLas[k + 1], (XPt2D)m_ProfilLas[k]);
      deniv = m_ProfilLas[k + 1].Z - m_ProfilLas[k].Z;
      if (deniv > 0) deniv_pos += deniv; else deniv_neg += deniv;
    }
    strX = juce::String(m_ProfilLas[index].X, 2);
    strY = juce::String(m_ProfilLas[index].Y, 2);
    strZ += juce::String(m_ProfilLas[index].Z, 2);
    strDist += juce::String(distance, 2);
    strDenivPos += juce::String(deniv_pos, 2);
    strDenivNeg += juce::String(deniv_neg, 2);
    Z0 = m_ProfilLas[index].Z;
  }

  m_fldX.SetValue(strX);
  m_fldY.SetValue(strY);
  m_fldZ.SetValue(strZ);
  m_fldDist.SetValue(strDist);
  m_fldDenivPos.SetValue(strDenivPos);
  m_fldDenivNeg.SetValue(strDenivNeg);

  sendActionMessage("UpdateTargetPos:" + strX + ":" + strY + ":" + juce::String(Z0, 2));

  return true;
}

//==============================================================================
// Calcul d'un profil sur les nuages de points LAS
//==============================================================================
bool ProfilViewer::ProfilComponent::ComputeLasProfil(const XFrame& F)
{
  XGeoLayer* layerLAS = m_Base->Layer("LAS");
  if (layerLAS == nullptr) // Pas de LAS
    return false;
  std::vector<double> Tdist_min;
  for (int k = 0; k < m_ProfilLas.size(); k++)
    Tdist_min.push_back(std::numeric_limits<double>::max());

  for (uint32_t i = 0; i < layerLAS->NbClass(); i++) {
    XGeoClass* C = layerLAS->Class(i);
    if (!C->Visible())
      continue;
    if (!C->Frame().Intersect(F))
      continue;
    for (uint32_t j = 0; j < C->NbVector(); j++) {
      GeoLAS* las = dynamic_cast<GeoLAS*>(C->Vector(j));
      if (las == nullptr) continue;
      if (!F.Intersect(las->Frame()))
        continue;
      las->ReOpen();
      if (!las->SetWorld(F, LasShader::Zmin(), LasShader::Zmax()))
        continue;
      laszip_point* point = las->GetPoint();

      double d2;
      XPt3D lasPoint;
      uint8_t classification;
      bool classif_newtype = las->IsNewClassification();

      while (las->GetNextPoint(&lasPoint.X, &lasPoint.Y, &lasPoint.Z)) {
        if (classif_newtype)
          classification = point->extended_classification;
        else
          classification = point->classification;
        if (!LasShader::ClassificationVisibility(classification)) continue;

        for (int k = 0; k < m_ProfilLas.size(); k++) {
          d2 = dist_plani2(m_ProfilLas[k], lasPoint);
          if (d2 < Tdist_min[k]) {
            m_ProfilLas[k].Z = lasPoint.Z;
            Tdist_min[k] = d2;
          }
        }
      } // endwhile
      las->CloseIfNeeded(1);
    } //endfor j
  } // ednfor i
  return true;
}

//==============================================================================
// Export des profils en CSV
//==============================================================================
bool ProfilViewer::ProfilComponent::ExportCsvFile()
{
  if ((m_ProfilDtm.size() < 1) && (m_ProfilLas.size() < 1))
    return false;
  juce::String filename = AppUtil::SaveFile("CSVProfil", juce::translate("Save Profile"), "*.csv");
  if (filename.isEmpty())
    return false;
  std::ofstream out;
  out.open(filename.toStdString(), std::ios::out);
  out.setf(std::ios::fixed); out.precision(2);
  bool dtm_mode = true, las_mode = true;
  int nb_point = m_ProfilDtm.size();
  if (nb_point < 1)
    nb_point = m_ProfilLas.size();
  if (m_ProfilDtm.size() < 1) dtm_mode = false;
  if (m_ProfilLas.size() < 1) las_mode = false;

  out << "X;Y";
  if (dtm_mode) out << ";ZDTM";
  if (las_mode) out << ";ZLAS";
  out << std::endl;
  for (int i = 0; i < nb_point; i++) {
    if (dtm_mode)
      out << m_ProfilDtm[i].X << ";" << m_ProfilDtm[i].Y;
    else
      out << m_ProfilLas[i].X << ";" << m_ProfilLas[i].Y;
    if (dtm_mode)
      out << ";" << m_ProfilDtm[i].Z;
    if (las_mode)
      out << ";" << m_ProfilLas[i].Z;
    out << std::endl;
  }
  return true;
}
