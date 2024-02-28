//-----------------------------------------------------------------------------
//								ExportLasDlg.cpp
//								================
//
// Dialogue d'options pour exporter les fichiers LAS/LAZ
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 25/02/2024
//-----------------------------------------------------------------------------

#include "ExportLasDlg.h"
#include "AppUtil.h"
#include "LasShader.h"
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"

//==============================================================================
// Methode run du thread
//==============================================================================
void LasExportThread::run()
{
  if (laszip_create(&m_Writer))  // Impossible de creer le Writer
    return;
  if (laszip_get_header_pointer(m_Writer, &m_Header))  // Impossible de recuperer l'entete
    return;
  // Donnees d'entete
  m_Header->file_source_ID = 0;
  m_Header->global_encoding = (1 << 0) | (1 << 4);     // see LAS specification for details
  m_Header->version_major = 1;
  m_Header->version_minor = 4;
  strncpy(m_Header->system_identifier, "Export LAS IGNMap", 32);
  strncpy(m_Header->generating_software, "IGNMap v3", 32);
  juce::Time time = juce::Time::getCurrentTime();
  m_Header->file_creation_day = time.getDayOfYear();
  m_Header->file_creation_year = time.getYear();
  m_Header->header_size = 375;
  m_Header->offset_to_point_data = 375;
  m_Header->point_data_format = 6;
  m_Header->point_data_record_length = 30;
  m_Header->number_of_point_records = 0;           // legacy 32-bit counters should be zero for new point types > 5
  for (int i = 0; i < 5; i++) {
    m_Header->number_of_points_by_return[i] = 0;   // legacy 32-bit counters should be zero for new point types > 5
  }
  m_Header->extended_number_of_point_records = 0;  // a-priori unknown number of points
  for (int i = 0; i < 15; i++) {
    m_Header->extended_number_of_points_by_return[i] = 0;
  }
  m_Header->max_x = 0.0;                           // a-priori unknown bounding box
  m_Header->min_x = 0.0;
  m_Header->max_y = 0.0;
  m_Header->min_y = 0.0;
  m_Header->max_z = 0.0;
  m_Header->min_z = 0.0;

  // Ajout de la projection
  laszip_geokey_struct key_entries[5];

  // projected coordinates
  key_entries[0].key_id = 1024; // GTModelTypeGeoKey
  key_entries[0].tiff_tag_location = 0;
  key_entries[0].count = 1;
  key_entries[0].value_offset = 1; // ModelTypeProjected

  // projection
  key_entries[1].key_id = 3072; // ProjectedCSTypeGeoKey
  key_entries[1].tiff_tag_location = 0;
  key_entries[1].count = 1;
  key_entries[1].value_offset = 2154; // Lambert 93

  // horizontal units
  key_entries[2].key_id = 3076; // ProjLinearUnitsGeoKey
  key_entries[2].tiff_tag_location = 0;
  key_entries[2].count = 1;
  key_entries[2].value_offset = 9001; // meters

  // vertical units
  key_entries[3].key_id = 4099; // VerticalUnitsGeoKey
  key_entries[3].tiff_tag_location = 0;
  key_entries[3].count = 1;
  key_entries[3].value_offset = 9001; // meters

  // vertical datum
  key_entries[4].key_id = 4096; // VerticalCSTypeGeoKey
  key_entries[4].tiff_tag_location = 0;
  key_entries[4].count = 1;
  key_entries[4].value_offset = 5720; // IGN69

  // add the geokeys (create or replace the appropriate VLR)
  if (laszip_set_geokeys(m_Writer, 5, key_entries))
    return;
  /*
  if (laszip_add_vlr(m_Writer, "LASF_Projection", 2112, 0, "intentionally empty OGC WKT", 0))
    return;
  if (laszip_add_vlr(m_Writer, "funny", 12345, 0, "just a funny VLR", 0))
    return;*/

  laszip_BOOL compress = 0;
  if (m_Compression) {
    //laszip_BOOL request = 1;
    //if (laszip_request_compatibility_mode(m_Writer, request))
    //  return;
    compress = 1;
  }

  laszip_preserve_generating_software(m_Writer, 1);
  if (laszip_open_writer(m_Writer, m_Filename.toStdString().c_str(), compress)) // Ouverture du Writer
    return;

  // Pointeur pour ecrire les points
  if (laszip_get_point_pointer(m_Writer, &m_Point))
    return;

  // Ecriture des points
  m_Count = 0;

  for (uint32_t i = 0; i < m_GeoBase->NbClass(); i++) {
    XGeoClass* C = m_GeoBase->Class(i);
    if (C == nullptr)
      continue;
    if (!C->IsLAS())
      continue;
    if (!C->Visible())
      continue;
    if (!m_Frame.Intersect(C->Frame()))
      continue;
    
    for (uint32_t j = 0; j < C->NbVector(); j++) {
      GeoLAS* las = (GeoLAS*)C->Vector(j);
      XFrame F = las->Frame();
      if (!m_Frame.Intersect(F))
        continue;
      ExportLas(las);
      if (threadShouldExit())
        return;
    }
  }

  // Fermeture du Writer
  if (laszip_get_point_count(m_Writer, &m_Count))
    return;

  if (laszip_close_writer(m_Writer))
    return;

  if (laszip_destroy(m_Writer))
    return;


}

//==============================================================================
// Export d'un fichier LAS
//==============================================================================
void LasExportThread::ExportLas(GeoLAS* las)
{
  laszip_I64 npoints = las->NbLasPoints();
  laszip_POINTER reader = las->GetReader();
  laszip_header* header = las->GetHeader();
  laszip_point* point = las->GetPoint();

  laszip_seek_point(reader, 0);
  double Xmin = (m_Frame.Xmin - header->x_offset) / header->x_scale_factor;
  double Xmax = (m_Frame.Xmax - header->x_offset) / header->x_scale_factor;
  double Ymin = (m_Frame.Ymin - header->y_offset) / header->y_scale_factor;
  double Ymax = (m_Frame.Ymax - header->y_offset) / header->y_scale_factor;
  double Zmin = (LasShader::Zmin() - header->z_offset) / header->z_scale_factor;
  double Zmax = (LasShader::Zmax() - header->z_offset) / header->z_scale_factor;

  uint8_t classification;
  bool classif_newtype = true;
  if (header->version_minor < 4) classif_newtype = false;
  LasShader shader;
  laszip_F64 coordinates[3];
  for (laszip_I64 i = 0; i < npoints; i++) {
    laszip_read_point(reader);
    if (classif_newtype)
      classification = point->extended_classification;
    else
      classification = point->classification;
    if (!shader.ClassificationVisibility(classification)) continue;
    if (point->X <= Xmin) continue;
    if (point->X >= Xmax) continue;
    if (point->Y <= Ymin) continue;
    if (point->Y >= Ymax) continue;
    if (point->Z < Zmin) continue;
    if (point->Z > Zmax) continue;

    coordinates[0] = point->X * header->x_scale_factor + header->x_offset;
    coordinates[1] = point->Y * header->y_scale_factor + header->y_offset;
    coordinates[2] = point->Z * header->z_scale_factor + header->z_offset;
    if (laszip_set_coordinates(m_Writer, coordinates))
      return;

    m_Point->intensity = point->intensity;
    m_Point->extended_return_number = point->extended_return_number;
    m_Point->extended_number_of_returns = point->extended_number_of_returns;
    m_Point->classification = point->classification;                // it must be set because it "fits" in 5 bits
    m_Point->extended_classification = point->extended_classification;
    m_Point->extended_scan_angle = point->extended_scan_angle;
    m_Point->extended_scanner_channel = point->extended_scanner_channel;
    m_Point->extended_classification_flags = point->extended_classification_flags; // overflag flag is set
    m_Point->gps_time = point->gps_time;

    if (laszip_write_point(m_Writer))
      return;
    if (laszip_update_inventory(m_Writer))
      return;
    m_Count++;
  }
}


//==============================================================================
// Constructeur
//==============================================================================
ExportLasDlg::ExportLasDlg(XGeoBase* base, double xmin, double ymin, double xmax,
                           double ymax) : m_ExportThread("ExportThread")
{
  m_Base = base;

  addAndMakeVisible(m_lblXmin);
  m_lblXmin.setBounds(10, 50, 50, 24);
  m_lblXmin.setText(juce::translate("Xmin : "), juce::NotificationType::dontSendNotification);
  addAndMakeVisible(m_edtXmin);
  m_edtXmin.setBounds(60, 50, 100, 24);
  m_edtXmin.setText(juce::String(xmin, 2));

  addAndMakeVisible(m_lblXmax);
  m_lblXmax.setBounds(350, 50, 50, 24);
  m_lblXmax.setText(juce::translate(" : Xmax"), juce::NotificationType::dontSendNotification);
  addAndMakeVisible(m_edtXmax);
  m_edtXmax.setBounds(240, 50, 100, 24);
  m_edtXmax.setText(juce::String(xmax, 2));

  addAndMakeVisible(m_lblYmax);
  m_lblYmax.setBounds(100, 10, 50, 24);
  m_lblYmax.setText(juce::translate("Ymax : "), juce::NotificationType::dontSendNotification);
  addAndMakeVisible(m_edtYmax);
  m_edtYmax.setBounds(150, 10, 100, 24);
  m_edtYmax.setText(juce::String(ymax, 2));

  addAndMakeVisible(m_lblYmin);
  m_lblYmin.setBounds(260, 90, 50, 24);
  m_lblYmin.setText(juce::translate(" : Ymin"), juce::NotificationType::dontSendNotification);
  addAndMakeVisible(m_edtYmin);
  m_edtYmin.setBounds(150, 90, 100, 24);
  m_edtYmin.setText(juce::String(ymin, 2));

  addAndMakeVisible(m_btnLaz);
  m_btnLaz.setButtonText(juce::translate("LAZ Compression"));
  m_btnLaz.setBounds(140, 130, 120, 24);

  addAndMakeVisible(m_btnExport);
  m_btnExport.setButtonText(juce::translate("Export"));
  m_btnExport.setBounds(160, 170, 80, 30);
  m_btnExport.addListener(this);

  m_progressBar = new juce::ProgressBar(m_dProgress);
  addAndMakeVisible(m_progressBar);
  m_progressBar->setBounds(100, 210, 200, 30);

  addAndMakeVisible(m_edtFilename);
  m_edtFilename.setBounds(10, 260, 380, 24);
  m_edtFilename.setText("");
  m_edtFilename.setReadOnly(true);
}

//==============================================================================
// Destructeur
//==============================================================================
ExportLasDlg::~ExportLasDlg()
{
  delete m_progressBar;
  stopTimer();
  m_ExportThread.stopThread(5000);
}

//==============================================================================
// Validation de l'export
//==============================================================================
void ExportLasDlg::buttonClicked(juce::Button* button)
{
  if (button != &m_btnExport)
    return;
  if (m_Base == nullptr)
    return;
  if (m_btnExport.getButtonText() == juce::translate("Cancel")) { // Annulation
    stopTimer();
    m_ExportThread.signalThreadShouldExit();
    if (m_ExportThread.isThreadRunning())
      m_ExportThread.stopThread(-1);
    m_dProgress = 0.;
    m_btnExport.setButtonText(juce::translate("Export"));
    juce::File file(m_strFilename);
    file.deleteFile();
    return;
  }

  m_btnExport.setButtonText(juce::translate("Cancel"));

  m_strFilename = m_edtFilename.getText();
  m_Frame.Xmin = m_edtXmin.getText().getDoubleValue();
  m_Frame.Xmax = m_edtXmax.getText().getDoubleValue();
  m_Frame.Ymin = m_edtYmin.getText().getDoubleValue();
  m_Frame.Ymax = m_edtYmax.getText().getDoubleValue();
  
  m_dProgress = 0.;

  // Creation du fichier LAS
  juce::String ext = "*.las";
  if (m_btnLaz.getToggleState())
    ext = "*.laz";
  m_strFilename = AppUtil::SaveFile("ExportLasFile", juce::translate("File to save"), ext);
  if (m_strFilename.isEmpty())
    return;

  m_ExportThread.signalThreadShouldExit();
  if (m_ExportThread.isThreadRunning()) {
    if (!m_ExportThread.stopThread(-1))
      return;
  }

  // Lancement du thread d'export
  m_ExportThread.SetGeoBase(m_Base);
  m_ExportThread.SetExportFrame(m_Frame);
  m_ExportThread.SetFilename(m_strFilename);
  if (m_btnLaz.getToggleState())
    m_ExportThread.SetCompression(true);
  m_ExportThread.startThread();

  startTimerHz(10);
}

//==============================================================================
// Callback du Timer
//==============================================================================
void ExportLasDlg::timerCallback()
{
  if (m_ExportThread.isThreadRunning()) {
    m_dProgress++;
    return;
  }
  m_dProgress = 1.;
  stopTimer();
  m_btnExport.setButtonText(juce::translate("Export"));
  juce::File file(m_strFilename);
  file.revealToUser();
  juce::Component* parent = getParentComponent();
  if (parent != nullptr)
    delete parent;
  return;
}