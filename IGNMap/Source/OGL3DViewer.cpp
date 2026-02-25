//-----------------------------------------------------------------------------
//								OGL3DViewer.cpp
//								===============
//
// Composant pour la visualisation OpenGL d'objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 01/12/2023
//-----------------------------------------------------------------------------

#include "OGL3DViewer.h"
#include "GeoBase.h"
#include "../../XTool/XGeoBase.h"
#include "../../XTool/XGeoClass.h"
#include "../../XTool/XGeoVector.h"
#include "LasShader.h"
#include "DtmShader.h"
#include "AppUtil.h"

//==============================================================================
// OGL3DViewer
//==============================================================================
OGL3DViewer::OGL3DViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons)
  : juce::DocumentWindow(name, backgroundColour, requiredButtons)
{
  setSize(500, 500); 
  setContentOwned(&m_OGLWidget, true);
  setResizable(true, true);
}

//==============================================================================
// OGLWidget
//==============================================================================
OGLWidget::OGLWidget() : m_MapThread("OGL3DViewer")
{
  setSize(500, 500);
  setWantsKeyboardFocus(true);
  openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
  // openGL3_2 est recommande par JUCE mais probleme sur MacOS avec glLineWidth
  openGLContext.setContinuousRepainting(true);
  m_MapThread.addListener(this);
  vertexShader = nullptr;
  fragmentShader = nullptr;
  m_nNbLasVertex = m_nNbPolyVertex = m_nNbLineVertex = m_nNbDtmVertex = 0;
  m_nNbPoly = m_nNbLine = 0;
  m_LasBufferID = m_RepereID = m_PtBufferID = m_TargetID = 0;
  m_DtmBufferID = m_DtmVertexArrayID = m_DtmElementID = 0;
  m_PolyBufferID = m_PolyVertexArrayID = m_PolyElementID = 0;
  m_LineBufferID = m_LineVertexArrayID = m_LineElementID = 0;
  m_nMaxLasPt = 2000000;
  m_nMaxPolyPt = m_nMaxLinePt = 10000;
  m_bNeedUpdate = m_bNeedLasPoint = m_bAutoRotation = m_bSaveImage = m_bNeedTarget = m_bUpdateTarget = false;
  m_bTranslateView = m_bZoomInView = m_bZoomOutView = false;
  m_bDtmTriangle = m_bDtmFill = true;
  m_Base = nullptr;
  m_dX0 = m_dY0 = m_dGsd = m_dZ0 = 0.;
  m_S = XPt3D(1., 1., 1.);
  m_LasPointSize = 4.f;
  m_VectorWidth = 2.f;
  m_DtmLineWidth = 1.f;
  m_nDtmW = m_nDtmH = 600;
  m_bViewLas = m_bViewDtm = m_bViewVector = m_bViewRepere = true;
  m_dDeltaZ = m_dOffsetZ = 0.;
  m_bUpdateLasColor = m_bZLocalRange = m_bRasterLas = false;
  m_bUpdateDtmColor = m_bDtmTextured = false;
  m_bShowF1Help = false;
  m_QuickLook = juce::Image(juce::Image::ARGB, 100, 100, true);
  juce::Image image = juce::ImageCache::getFromMemory(BinaryData::Options_png, BinaryData::Options_pngSize);
  m_btnOptions.setImages(true, true, true, image, 1.f, juce::Colours::green, juce::Image(), 1.f, juce::Colours::green, 
                        juce::Image(), 1.f, juce::Colours::red);
  m_btnOptions.setToggleable(true);
  m_btnOptions.setClickingTogglesState(true);
  addAndMakeVisible(m_btnOptions);
  m_btnOptions.addListener(this);
  m_btnOptions.setBounds(0, 0, 30, 30);
  addChildComponent(m_Control3D);
  m_Control3D.setBounds(30, 0, 250, 250);
  m_Control3D.AddListener(this);
  SyncControl3D();
}

//==============================================================================
// Initialisation OpenGL
//==============================================================================
void OGLWidget::initialise()
{
  using namespace ::juce::gl;
 
  CreateShaders();

  // Creation du buffer des points LAS
  openGLContext.extensions.glGenBuffers(1, &m_LasBufferID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, m_nMaxLasPt * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  // Creation du buffer des points DTM
  openGLContext.extensions.glGenBuffers(1, &m_DtmVertexArrayID);
  openGLContext.extensions.glGenBuffers(1, &m_DtmBufferID);
  openGLContext.extensions.glGenBuffers(1, &m_DtmElementID);
  openGLContext.extensions.glBindVertexArray(m_DtmVertexArrayID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, m_nDtmW * m_nDtmH * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DtmElementID);
  openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, (m_nDtmW - 1) * 2 * (m_nDtmH - 1) * sizeof(Index), nullptr, GL_DYNAMIC_DRAW);

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmElementID);
  Index* ptr_index = (Index*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  for (uint32_t i = 0; i < m_nDtmH - 1; i++) {
    for (uint32_t j = 0; j < m_nDtmW - 1; j++) {
      ptr_index->num[0] = j + i * m_nDtmW;
      ptr_index->num[1] = (j + 1) + i * m_nDtmW;
      ptr_index->num[2] = j + (i + 1) * m_nDtmW;
      ptr_index++;
      ptr_index->num[0] = (j + 1) + i * m_nDtmW;
      ptr_index->num[1] = (j + 1) + (i + 1) * m_nDtmW;
      ptr_index->num[2] = j + (i + 1) * m_nDtmW;
      ptr_index++;
    }
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // Creation du buffer des points des polygones
  openGLContext.extensions.glGenBuffers(1, &m_PolyVertexArrayID);
  openGLContext.extensions.glGenBuffers(1, &m_PolyBufferID);
  openGLContext.extensions.glGenBuffers(1, &m_PolyElementID);
  openGLContext.extensions.glBindVertexArray(m_PolyVertexArrayID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, m_nMaxPolyPt * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_PolyElementID);
  openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nMaxPolyPt * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  // Creation du buffer des points des polylignes
  openGLContext.extensions.glGenBuffers(1, &m_LineVertexArrayID);
  openGLContext.extensions.glGenBuffers(1, &m_LineBufferID);
  openGLContext.extensions.glGenBuffers(1, &m_LineElementID);
  openGLContext.extensions.glBindVertexArray(m_LineVertexArrayID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, m_nMaxLinePt * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LineElementID);
  openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nMaxLinePt * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

  // Creation du buffer de points du repere
  openGLContext.extensions.glGenBuffers(1, &m_RepereID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_RepereID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  // Creation du buffer de points pour la selection
  openGLContext.extensions.glGenBuffers(1, &m_PtBufferID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  // Creation du buffer de points pour la cible
  openGLContext.extensions.glGenBuffers(1, &m_TargetID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_TargetID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  CreateRepere();
  juce::OpenGLHelpers::resetErrorState();
}

//==============================================================================
// Fermeture OpenGL
//==============================================================================
void OGLWidget::shutdown()
{
  openGLContext.extensions.glDeleteBuffers(1, &m_LasBufferID);
  openGLContext.extensions.glDeleteBuffers(1, &m_DtmBufferID);
  openGLContext.extensions.glDeleteBuffers(1, &m_DtmElementID);
  openGLContext.extensions.glDeleteBuffers(1, &m_DtmVertexArrayID);
  openGLContext.extensions.glDeleteBuffers(1, &m_PolyBufferID);
  openGLContext.extensions.glDeleteBuffers(1, &m_PolyElementID);
  openGLContext.extensions.glDeleteBuffers(1, &m_PolyVertexArrayID);
  openGLContext.extensions.glDeleteBuffers(1, &m_LineBufferID);
  openGLContext.extensions.glDeleteBuffers(1, &m_LineElementID);
  openGLContext.extensions.glDeleteBuffers(1, &m_LineVertexArrayID);

  openGLContext.extensions.glDeleteBuffers(1, &m_RepereID);
  openGLContext.extensions.glDeleteBuffers(1, &m_PtBufferID);
  openGLContext.extensions.glDeleteBuffers(1, &m_TargetID);
  m_Shader.reset();
  m_Attributes.reset();
  m_Uniforms.reset();
}

//==============================================================================
// Matrice Projection
//==============================================================================
juce::Matrix3D<float> OGLWidget::getProjectionMatrix() const
{
  const juce::ScopedLock lock(m_Mutex);

  auto w = 1.0f / (0.5f + 0.1f);
  auto h = w * m_Bounds.toFloat().getAspectRatio(false);

  return juce::Matrix3D<float>::fromFrustum(-w, w, -h, h, 4.0f, 30.0f);
}

//==============================================================================
// Matrice Vue
//==============================================================================
juce::Matrix3D<float> OGLWidget::getViewMatrix() const
{
  float autoRot = 0.0;
  if (m_bAutoRotation)
    autoRot = ((float)getFrameCounter() * 0.01f);
  auto viewMatrix = juce::Matrix3D<float>::fromTranslation({ (float)m_T.X, (float)m_T.Y, -10.0f + (float)m_T.Z});
  auto rotationMatrix = juce::Matrix3D<float>::rotation({ (float)m_R.X, (float)m_R.Y, (float)m_R.Z + autoRot });
 
  juce::Matrix3D<float> scaleMatrix;
  scaleMatrix.mat[0] = (float)m_S.X;
  scaleMatrix.mat[5] = (float)m_S.Y;
  scaleMatrix.mat[10] = (float)m_S.Z;

  return viewMatrix * rotationMatrix * scaleMatrix;
}

//==============================================================================
// Creation du shader OpenGL
//==============================================================================
void OGLWidget::CreateShaders()
{
  vertexShader =
    "attribute vec4 position;\n"
    "attribute vec4 sourceColour;\n"
    "\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "\n"
    "varying vec4 destinationColour;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    destinationColour = sourceColour;\n"
    "    gl_Position = projectionMatrix * viewMatrix * position;\n"
    "}\n";

  fragmentShader =
    "varying vec4 destinationColour;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = destinationColour;\n"
    "}\n";

  std::unique_ptr<juce::OpenGLShaderProgram> newShader(new juce::OpenGLShaderProgram(openGLContext));
  juce::String statusText;

  if (newShader->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader))
    && newShader->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
    && newShader->link())
  {
    m_Attributes.reset();
    m_Uniforms.reset();

    m_Shader = std::move(newShader);
    m_Shader->use();

    m_Attributes.reset(new Attributes(*m_Shader));
    m_Uniforms.reset(new Uniforms(*m_Shader));

    statusText = "GLSL: v" + juce::String(juce::OpenGLShaderProgram::getLanguageVersion(), 2);
  }
  else
  {
    statusText = newShader->getLastError();
  }
}

//==============================================================================
// Dessin du composant
//==============================================================================
void OGLWidget::paint(juce::Graphics& g)
{
  auto b = getLocalBounds();
  g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
  g.setFont(12);
  /*
  juce::String angles = "Angles : (" + juce::String(m_R.X) + " ; " + juce::String(m_R.Y) + " ; " + juce::String(m_R.Z) + ")";
  g.drawText(angles, 25, 20, 300, 30, juce::Justification::left);
  juce::String translation = "Translations : (" + juce::String(m_T.X) + " ; " + juce::String(m_T.Y) + " ; " + juce::String(m_T.Z) + ")";
  g.drawText(translation, 25, 40, 300, 30, juce::Justification::left);
  */
  int info_origin = b.getBottom() - 125;
  juce::String help = juce::translate("Help : F1");
  g.drawText(help, 5, info_origin + 30, 300, 30, juce::Justification::left);
  juce::String nbPoint = "Points : " + juce::String(m_nNbLasVertex) + " Las";
  nbPoint += " | " + juce::String(m_nNbPolyVertex) + " Polygon";
  nbPoint += " | " + juce::String(m_nNbLineVertex) + " Line";
  g.drawText(nbPoint, 5, info_origin + 50, 300, 30, juce::Justification::left);

  // Cible
  juce::String target = "T = " + juce::String(m_Target.X, 2) + " ; " + juce::String(m_Target.Y, 2) + " ; " + juce::String(m_Target.Z, 2);
  g.setColour(juce::Colours::magenta);
  g.drawText(target, 5, info_origin + 70, 300, 30, juce::Justification::left);
  g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));

  // Point clique
  juce::String lastPt = "P = " + juce::String(m_LastPt.X, 2) + " ; " + juce::String(m_LastPt.Y, 2) + " ; " + juce::String(m_LastPt.Z, 2);
  g.drawText(lastPt, 5, info_origin + 90, 300, 30, juce::Justification::left);
  g.drawLine(0, info_origin + 30, 150, info_origin + 30);
  g.drawLine(0, info_origin + 120, 150, info_origin + 120);

  if (!m_bShowF1Help)
    return;
  b.expand(-20, -20);
  g.setOpacity(0.7f);
  g.setFillType(juce::FillType(juce::Colours::lightskyblue));
  g.fillRect(b);
  b.expand(-5, -5);
  g.setFillType(juce::FillType(juce::Colours::darkblue));
  g.setFont(18);
  help = juce::translate("Help : F1 ; Copy image : F2");
  g.drawText(help, b.getX(), b.getY() + 5, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("Point size : Q/S (Las) ; W/X (DTM) ; C/V (Vector)");
  g.drawText(help, b.getX(), b.getY() + 35, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("DTM : D (point / triangle), F (fill triangles)");
  g.drawText(help, b.getX(), b.getY() + 65, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("Visibility : J (Axis) ; K (Vector) ; L (Las) ; M (DTM)");
  g.drawText(help, b.getX(), b.getY() + 95, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("Z scale: PageUp ; PageDown");
  g.drawText(help, b.getX(), b.getY() + 125, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("Position : Left ; Right ; Up ; Down");
  g.drawText(help, b.getX(), b.getY() + 155, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("Z Position : Y ; H");
  g.drawText(help, b.getX(), b.getY() + 185, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("A : automatic rotation ;  R : reset");
  g.drawText(help, b.getX(), b.getY() + 215, b.getWidth(), 40, juce::Justification::left);
  help = juce::translate("LAS color : P palette ; O raster");
  g.drawText(help, b.getX(), b.getY() + 245, b.getWidth(), 40, juce::Justification::left);
}

//==============================================================================
// Rendu OpenGL
//==============================================================================
void OGLWidget::render()
{
  using namespace ::juce::gl;
  const juce::ScopedLock lock(m_Mutex);

  juce::OpenGLFrameBuffer* buffer = nullptr;
  juce::Image snapshotImage;
  if (m_bSaveImage) {
    snapshotImage = juce::Image(juce::OpenGLImageType().create(juce::Image::ARGB, getWidth(), getHeight(), true));
    buffer = juce::OpenGLImageType::getFrameBufferFrom(snapshotImage);
    if (buffer != nullptr)
      buffer->makeCurrentRenderingTarget();
  }

  auto desktopScale = (float)openGLContext.getRenderingScale();
  juce::OpenGLHelpers::clear(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //openGLContext.extensions.glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glEnable(GL_POLYGON_OFFSET_POINT);
  glEnable(GL_DEPTH_TEST);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glViewport(0, 0,
      juce::roundToInt(desktopScale * (float)m_Bounds.getWidth()),
      juce::roundToInt(desktopScale * (float)m_Bounds.getHeight()));

  m_Shader->use();

  if (m_Uniforms->projectionMatrix != nullptr)
    m_Uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);

  if (m_Uniforms->viewMatrix != nullptr)
    m_Uniforms->viewMatrix->setMatrix4(getViewMatrix().mat, 1, false);

  // Dessin des LAS
  if ((m_nNbLasVertex > 0) && (!m_bNeedUpdate) && (m_bViewLas)) {
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
    m_Attributes->enable();
    glPointSize(m_LasPointSize);
    glDrawArrays(GL_POINTS, 0, m_nNbLasVertex);
    m_Attributes->disable();
  }

  // Dessin des MNT
  if ((m_nNbDtmVertex > 0) && (!m_bNeedUpdate) && (m_bViewDtm)) {
    if (m_bDtmTriangle) {
      openGLContext.extensions.glBindVertexArray(m_DtmVertexArrayID);
      openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
      openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DtmElementID);
      m_Attributes->enable();
      if (!m_bDtmFill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(m_DtmLineWidth);
      glDrawElements(GL_TRIANGLES, (m_nDtmW - 1) * 2 * (m_nDtmH - 1) * 3, GL_UNSIGNED_INT, 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      m_Attributes->disable();
    }
    else {
      openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
      m_Attributes->enable();
      glPointSize(m_LasPointSize);
      glDrawArrays(GL_POINTS, 0, m_nDtmH * m_nDtmW);
      m_Attributes->disable();
    }
   }

  // Dessin des polygones et des lignes
  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(0xFFFFFFFF);
  glLineWidth(m_VectorWidth); // Ne marche pas sur MacOS
  // Dessin des polygones
  if ((m_nNbPolyVertex > 0) && (!m_bNeedUpdate) && (m_bViewVector)) {
    openGLContext.extensions.glBindVertexArray(m_PolyVertexArrayID);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyBufferID);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_PolyElementID);
    m_Attributes->enable();
    glDrawElements(GL_LINE_LOOP, m_nNbPolyVertex + m_nNbPoly, GL_UNSIGNED_INT, 0);
    m_Attributes->disable();
  }

  // Dessin des polylignes
  if ((m_nNbLineVertex > 0) && (!m_bNeedUpdate) && (m_bViewVector)) {
    openGLContext.extensions.glBindVertexArray(m_LineVertexArrayID);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineBufferID);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LineElementID);
    m_Attributes->enable();
    glDrawElements(GL_LINE_STRIP, m_nNbLineVertex + m_nNbLine, GL_UNSIGNED_INT, 0);
    m_Attributes->disable();
  }
  glDisable(GL_PRIMITIVE_RESTART);

  if (m_bNeedUpdate)
    UpdateBase();

  // Dessin du repere orthonorme
  if (m_bViewRepere) {
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_RepereID);
    m_Attributes->enable();
    glLineWidth(3.0); // Ne marche pas sur MacOS
    glDrawArrays(GL_LINES, 0, 6);
    m_Attributes->disable();
  }

  // Gestion des selections de points
  if (m_bNeedLasPoint)
    Select((int)m_LastPos.x, (int)m_LastPos.y);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  m_Attributes->enable();
  glPointSize(6.f);
  glDrawArrays(GL_POINTS, 0, 1);
  m_Attributes->disable();

  // Dessin de la cible
  if (m_bNeedTarget)
    DrawTarget();
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_TargetID);
  m_Attributes->enable();
  glLineWidth(3.0); // Ne marche pas sur MacOS
  glDrawArrays(GL_LINES, 0, 6);
  m_Attributes->disable();

  // Sauvegarde de l'image
  if (m_bSaveImage) {
    if (buffer != nullptr)
      buffer->releaseAsRenderingTarget();
    juce::File imageFile(m_strFileSave);
    if (imageFile.existsAsFile())
      imageFile.deleteFile();

    juce::FileOutputStream outputFileStream(imageFile);
    juce::PNGImageFormat imageFormatter;
    imageFormatter.writeImageToStream(snapshotImage, outputFileStream);
    m_bSaveImage = false;
  }

  // Reset the element buffers so child Components draw correctly
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
  openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//==============================================================================
// Fermeture du contexte OpenGL
//==============================================================================
/*
void OGLWidget::openGLContextClosing()
{

}*/

//==============================================================================
// Redimensionnement
//==============================================================================
void OGLWidget::resized()
{
  const juce::ScopedLock lock(m_Mutex);
  m_Bounds = getLocalBounds();
}

//==============================================================================
// Gestion de la souris
//==============================================================================
void OGLWidget::mouseDown(const juce::MouseEvent& event)
{
  m_LastPos = event.position;
}

void OGLWidget::mouseMove(const juce::MouseEvent& /*event*/)
{
  
}

void OGLWidget::mouseDrag(const juce::MouseEvent& event)
{
  if (event.mods.isAltDown()) { // Changement de X ; Y du point cible
    m_bNeedLasPoint = true;
    m_bUpdateTarget = true;
    m_bNeedTarget = true;
  }
  else { // Changement de rotation de la vue 3D
    juce::Point<float> delta = event.position - m_LastPos;
    m_R.Z += (delta.getX() * 0.01);
    m_R.X += ((delta.getY() * 0.01));
  }
  
  m_LastPos = event.position;
  repaint();  // Pour l'affichage des informations
}

void OGLWidget::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
  if (event.mods.isAltDown()) { // Changement de Z du point cible
    m_Target.Z += wheel.deltaY;
    m_bNeedTarget = true;
    repaint();
    return;
  }

  double scale = wheel.deltaY / 10.;
  m_S += XPt3D(scale, scale, scale);
  repaint();
}

void OGLWidget::mouseDoubleClick(const juce::MouseEvent& event)
{
  m_bNeedLasPoint = true;
  if (event.mods.isAltDown()) {
    m_bUpdateTarget = true;
    m_bNeedTarget = true;
  }
  if (event.mods.isShiftDown()) {
    m_bTranslateView = true;
  }
  if (event.mods.isCtrlDown()) {
    if (event.mods.isRightButtonDown())
      m_bZoomOutView = true;
    if (event.mods.isLeftButtonDown())
      m_bZoomInView = true;
  }
  repaint();
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool OGLWidget::keyPressed(const juce::KeyPress& key)
{
  if (key.getKeyCode() == juce::KeyPress::F1Key) 
    m_bShowF1Help = !m_bShowF1Help;
  if (key.getKeyCode() == juce::KeyPress::upKey)
    m_T.Y += 0.1;
  if (key.getKeyCode() == juce::KeyPress::downKey)
    m_T.Y -= 0.1;
  if (key.getKeyCode() == juce::KeyPress::leftKey)
    m_T.X -= 0.1;
  if (key.getKeyCode() == juce::KeyPress::rightKey)
    m_T.X += 0.1;
  if (key.getKeyCode() == juce::KeyPress::pageUpKey)
    m_S.Z = std::clamp(m_S.Z + 0.1, 0.1, 10.);
   if (key.getKeyCode() == juce::KeyPress::pageDownKey)
    m_S.Z = std::clamp(m_S.Z - 0.1, 0.1, 10.);
 
    if ((key.getTextCharacter() == 'R')||(key.getTextCharacter() == 'r')) {
    m_R = XPt3D();
    m_T = XPt3D();
    m_S = XPt3D(1., 1., 1.);
    m_LasPointSize = 4;
  }
  if ((key.getTextCharacter() == 'A') || (key.getTextCharacter() == 'a')) {
    if (m_bAutoRotation)
      m_R.Z += ((float)getFrameCounter() * 0.01f);
    m_bAutoRotation = (!m_bAutoRotation);
  }
  if ((key.getTextCharacter() == 'Q') || (key.getTextCharacter() == 'q'))
    m_LasPointSize += 1.f;
  if ((key.getTextCharacter() == 'S') || (key.getTextCharacter() == 's'))
    m_LasPointSize = std::clamp(m_LasPointSize - 1.f, 1.f, m_LasPointSize);
  if ((key.getTextCharacter() == 'W') || (key.getTextCharacter() == 'w'))
    m_DtmLineWidth += 1.f;
  if ((key.getTextCharacter() == 'X') || (key.getTextCharacter() == 'x'))
    m_DtmLineWidth = std::clamp(m_DtmLineWidth - 1.f, 1.f, m_DtmLineWidth);
  if ((key.getTextCharacter() == 'C') || (key.getTextCharacter() == 'c'))
    m_VectorWidth += 1.f;
  if ((key.getTextCharacter() == 'V') || (key.getTextCharacter() == 'v'))
    m_VectorWidth = std::clamp(m_VectorWidth - 1.f, 1.f, m_VectorWidth);

  if ((key.getTextCharacter() == 'D') || (key.getTextCharacter() == 'd'))
    m_bDtmTriangle = (!m_bDtmTriangle);
  if ((key.getTextCharacter() == 'F') || (key.getTextCharacter() == 'f'))
    m_bDtmFill = (!m_bDtmFill);
  if ((key.getTextCharacter() == 'J') || (key.getTextCharacter() == 'j'))
    m_bViewRepere = (!m_bViewRepere);
  if ((key.getTextCharacter() == 'K') || (key.getTextCharacter() == 'k'))
    m_bViewVector = (!m_bViewVector);
  if ((key.getTextCharacter() == 'L') || (key.getTextCharacter() == 'l'))
    m_bViewLas = (!m_bViewLas);
  if ((key.getTextCharacter() == 'M') || (key.getTextCharacter() == 'm'))
    m_bViewDtm = (!m_bViewDtm);

  if ((key.getTextCharacter() == 'Y') || (key.getTextCharacter() == 'y')) {
    m_dDeltaZ = 1.;
    m_bNeedUpdate = true;
  }
  if ((key.getTextCharacter() == 'H') || (key.getTextCharacter() == 'h')) {
    m_dDeltaZ = -1.;
    m_bNeedUpdate = true;
  }
  if ((key.getTextCharacter() == 'P') || (key.getTextCharacter() == 'p')) {
    m_bUpdateLasColor = true;
    m_bNeedUpdate = true;
  }
  if ((key.getTextCharacter() == 'O') || (key.getTextCharacter() == 'o')) {
    m_bRasterLas = !m_bRasterLas;
    if (m_bRasterLas) {
      UpdateQuickLook();
      m_bUpdateLasColor = true;
    }
    else {
      m_bUpdateLasColor = true;
      m_bNeedUpdate = true;
    }
  }
  if ((key.getTextCharacter() == 'G') || (key.getTextCharacter() == 'g')) {
    m_bDtmTextured = !m_bDtmTextured;
    if (m_bDtmTextured) {
      UpdateQuickLook();
    }
    else {
      m_bUpdateDtmColor = true;
      m_bNeedUpdate = true;
    }
  }

  if (key.getKeyCode() == juce::KeyPress::F2Key) {
    m_strFileSave = AppUtil::SaveFile("Image3DPath", juce::translate("Save Image"), "*.png;");
    if (!m_strFileSave.isEmpty())
      m_bSaveImage = true;
  }
  SyncControl3D();
  repaint();
  return true;
}

//==============================================================================
// Reponses aux boutons
//==============================================================================
void OGLWidget::buttonClicked(juce::Button* button)
{
  if (button == &m_btnOptions) {
    m_Control3D.setVisible(!m_Control3D.isVisible());
    return;
  }
  if (button == &m_Control3D.m_btnViewDtm)
    m_bViewDtm = (!m_bViewDtm);
  if (button == &m_Control3D.m_btnViewLas)
    m_bViewLas = (!m_bViewLas);
  if (button == &m_Control3D.m_btnViewVector)
    m_bViewVector = (!m_bViewVector);
  if (button == &m_Control3D.m_btnViewRepere)
    m_bViewRepere = (!m_bViewRepere);

  if (button == &m_Control3D.m_btnRasterDtm) {
    m_bDtmTextured = !m_bDtmTextured;
    if (m_bDtmTextured) {
      UpdateQuickLook();
    }
    else {
      m_bUpdateDtmColor = true;
      m_bNeedUpdate = true;
    }
  }
  if (button == &m_Control3D.m_btnMeshDtm)
    m_bDtmTriangle = (!m_bDtmTriangle);
  if (button == &m_Control3D.m_btnFillDtm)
    m_bDtmFill = (!m_bDtmFill);

  if (button == &m_Control3D.m_btnRasterLas) {
    m_bRasterLas = !m_bRasterLas;
    if (m_bRasterLas) {
      UpdateQuickLook();
      m_bUpdateLasColor = true;
    }
    else {
      m_bUpdateLasColor = true;
      m_bNeedUpdate = true;
    }
  }
  repaint();
}

//==============================================================================
// Reponses aux sliders
//==============================================================================
void OGLWidget::sliderValueChanged(juce::Slider* slider)
{
  if (slider == &m_Control3D.m_sldZFactor) {
    m_S.Z = slider->getValue();
  }
  repaint();
}

//==============================================================================
// Synchronisation des contrôles
//==============================================================================
void OGLWidget::SyncControl3D()
{
  m_Control3D.m_btnViewDtm.setToggleState(m_bViewDtm, juce::NotificationType::dontSendNotification);
  m_Control3D.m_btnViewLas.setToggleState(m_bViewLas, juce::NotificationType::dontSendNotification);
  m_Control3D.m_btnViewVector.setToggleState(m_bViewVector, juce::NotificationType::dontSendNotification);
  m_Control3D.m_btnViewRepere.setToggleState(m_bViewRepere, juce::NotificationType::dontSendNotification);

  m_Control3D.m_btnRasterDtm.setToggleState(m_bDtmTextured, juce::NotificationType::dontSendNotification);
  m_Control3D.m_btnRasterLas.setToggleState(m_bRasterLas, juce::NotificationType::dontSendNotification);
  m_Control3D.m_sldZFactor.setValue(m_S.Z, juce::NotificationType::dontSendNotification);

}


//==============================================================================
// Chargement d'objets
//==============================================================================
void OGLWidget::LoadObjects(XGeoBase* base, XFrame* F)
{
  if (base == nullptr)
    return;
  m_Frame = *F;
  m_Base = base;
  //m_bNeedUpdate = true;
  m_dX0 = F->Center().X;
  m_dY0 = F->Center().Y;
  if (F->Width() > F->Height())
    m_dGsd = F->Width() / 4.;
  else
    m_dGsd = F->Height() / 4.;
  m_dZ0 = base->Z(F->Center());
  m_dDeltaZ = m_dOffsetZ = 0.;
  UpdateQuickLook();
}

//==============================================================================
// Fixe la cible
//==============================================================================
void OGLWidget::SetTarget(const XPt3D& P)
{
  m_Target = P;
  m_bNeedTarget = true;
  repaint();
}

//==============================================================================
// Mise a jour des donnees chargees
//==============================================================================
void OGLWidget::UpdateBase()
{
  if (m_Base == nullptr)
    return;
  if (m_dDeltaZ != 0.) {  // Juste un changement d'echelle altimetrique
    MoveZ((float)m_dDeltaZ);
    m_dDeltaZ = 0.;
    return;
  }
  if (m_bUpdateLasColor) { // Juste un reetalement des couleurs des points LAS
    ChangeLasColor();
    return;
  }
  if (m_bUpdateDtmColor) {  // Juste un changement de representation des MNT
    ChangeDtmColor();
    return;
  }
  m_nNbLasVertex = 0;
  m_nNbDtmVertex = 0;
  m_nNbPolyVertex = m_nNbLineVertex = 0;
  m_nNbPoly = m_nNbLine = 0;
  ReinitDtm();
  if (m_Base == nullptr)
    return;
  const juce::ScopedLock lock(m_Mutex);
  //openGLContext.setContinuousRepainting(false);
  FindZminLas();
  for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
    XGeoClass* C = m_Base->Class(i);
    if (C == nullptr)
      continue;
    if ( (C->IsLAS()) && (C->Visible()) )
      LoadLasClass(C);
    if ((C->IsVector()) && (C->Visible()))
      LoadVectorClass(C);
  }
  DrawDtm();
  if (m_dDeltaZ != 0.)
    MoveZ((float)m_dDeltaZ);
  m_dDeltaZ = 0.;
  m_bNeedUpdate = false;
  //openGLContext.setContinuousRepainting(true);
  openGLContext.triggerRepaint();
}

//==============================================================================
// Trouve le Zmin de la zone a partir des fichiers LAS
//==============================================================================
void OGLWidget::FindZminLas()
{
  for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
    XGeoClass* C = m_Base->Class(i);
    if (C == nullptr)
      continue;
    if ((!C->IsLAS()) || (!C->Visible()))
      continue;
    if (!m_Frame.Intersect(C->Frame()))
      continue;
    for (uint32_t j = 0; j < C->NbVector(); j++) {
      GeoLAS* las = dynamic_cast<GeoLAS*>(C->Vector(j));
      XFrame F = las->Frame();
      if (!m_Frame.Intersect(F))
        continue;
      if (m_dZ0 <= XGEO_NO_DATA)
        m_dZ0 = las->Zmin();
      else
        m_dZ0 = XMin(m_dZ0, las->Zmin());
    }
  }
}

//==============================================================================
// Chargement d'un fichier LAS
//==============================================================================
void OGLWidget::LoadLasClass(XGeoClass* C)
{
  if (!m_Frame.Intersect(C->Frame()))
    return;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    GeoLAS* las = dynamic_cast<GeoLAS*>(C->Vector(i));
    XFrame F = las->Frame();
    if (!m_Frame.Intersect(F))
      continue;
    DrawLas(las);
  }
}

//==============================================================================
// Dessin d'un fichier LAS
//==============================================================================
void OGLWidget::DrawLas(GeoLAS* las)
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  if (!las->ReOpen())
    return;
  if (!las->SetWorld(m_Frame, LasShader::Zmin(), LasShader::Zmax()))
    return;
  laszip_point* point = las->GetPoint();

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_vertex += m_nNbLasVertex;

  double Z0 = LasShader::Zmin();
  double deltaZ = LasShader::Zmax() - Z0;
  if (deltaZ <= 0) deltaZ = 1.;	// Pour eviter les divisions par 0
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = LasShader::Zmin();

  double X, Y, Z;
  juce::Colour col = juce::Colours::orchid;
  uint8_t data[4] = { 0, 0, 0, 255 };
  uint32_t* data_ptr = (uint32_t*)&data;
  uint8_t classification;
  bool classif_newtype = las->IsNewClassification();

  LasShader shader;
  juce::Image::BitmapData bitmap(m_QuickLook, juce::Image::BitmapData::readOnly);
  int x, y;

  while (las->GetNextPoint(&X, &Y, &Z)) {
    if (classif_newtype)
      classification = point->extended_classification;
    else
      classification = point->classification;
    if (!shader.ClassificationVisibility(classification)) continue;
    
    ptr_vertex->position[0] = (float) ((X - m_dX0) / m_dGsd);
    ptr_vertex->position[1] = (float) ((Y - m_dY0) / m_dGsd);
    ptr_vertex->position[2] = (float) ((Z - m_dZ0) / m_dGsd);
    m_dDeltaZ += ptr_vertex->position[2];

    if (m_bRasterLas) { // Affichage avec le raster
      x = (int)(bitmap.width * 0.5 + ptr_vertex->position[0] / 2. * (bitmap.width * 0.5));
      y = (int)(bitmap.height * 0.5 - ptr_vertex->position[1] / 2. * (bitmap.height * 0.5));
      *data_ptr = (uint32_t)bitmap.getPixelColour(x, y).getARGB();
    }
    else {
      switch (shader.Mode()) {
      case LasShader::ShaderMode::Altitude:
        *data_ptr = shader.AltiColorARGB((uint8_t)((Z - Z0) * 255 / deltaZ));
        break;
      case LasShader::ShaderMode::RGB:
        data[0] = (uint8_t)(point->rgb[2] / 256);
        data[1] = (uint8_t)(point->rgb[1] / 256);
        data[2] = (uint8_t)(point->rgb[0] / 256);
        // data[3] = 255; // deja fixe dans l'initialisation de data
        break;
      case LasShader::ShaderMode::IRC:
        data[0] = (uint8_t)(point->rgb[1] / 256);
        data[1] = (uint8_t)(point->rgb[0] / 256);
        data[2] = (uint8_t)(point->rgb[3] / 256);
        // data[3] = 255; // deja fixe dans l'initialisation de data
        break;
      case LasShader::ShaderMode::Classification:
        col = shader.ClassificationColor(classification);
        *data_ptr = (uint32_t)col.getARGB();
        break;
      case LasShader::ShaderMode::Intensity:
        //data[0] = point->intensity;	// L'intensite est normalisee sur 16 bits
        //memcpy(data, &point->intensity, 2 * sizeof(uint8_t));
        *data_ptr = shader.IntensityColorARGB(point->intensity);
        break;
      case LasShader::ShaderMode::Angle:
        if (point->extended_scan_angle < 0) {	// Angle en degree = extended_scan_angle * 0.006
          data[2] = (uint8_t)(255 - point->extended_scan_angle * (-0.0085));	 // Normalise sur [0; 255]
          data[1] = 0;
          data[0] = 0; // 255 - data[2];
        }
        else {
          data[2] = 0;
          data[1] = (uint8_t)(255 - point->extended_scan_angle * (0.0085));	 // Normalise sur [0; 255]
          data[0] = 0; // 255 - data[1];
        }
        break;
      default:;
      }
    }
    ptr_vertex->colour[0] = (float)data[2] / 255.f;
    ptr_vertex->colour[1] = (float)data[1] / 255.f;
    ptr_vertex->colour[2] = (float)data[0] / 255.f;
    ptr_vertex->colour[3] = (float)data[3] / 255.f;

    //memcpy(&ptr_vertex[(0+m_nNbVertex) * 7], vertex, sizeof(vertex));
    //openGLContext.extensions.glBufferSubData(GL_ARRAY_BUFFER, (6 + m_nNbVertex) * 7 * sizeof(float), sizeof(vertex), vertex);
    ptr_vertex++;
    m_nNbLasVertex++;
    if (m_nNbLasVertex >= m_nMaxLasPt)
      break;
  }
  las->CloseIfNeeded(1);
  glUnmapBuffer(GL_ARRAY_BUFFER);
  m_dDeltaZ = -(m_dDeltaZ / m_nNbLasVertex);
}

//==============================================================================
// Dessin du MNT
//==============================================================================
void OGLWidget::DrawDtm()
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  DtmShader shader(m_Frame.Width() / m_nDtmW);

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  double deltaX = m_Frame.Width() / m_nDtmW;
  double deltaY = m_Frame.Height() / m_nDtmH;
  XPt3D P;
  double Zmin = m_Base->ZMin();

  juce::Colour colour;
  float opacity = (float)(DtmShader::m_dOpacity * 0.01);
 
  m_RawDtm = juce::Image(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true, juce::SoftwareImageType());
  { // Necessaire pour que bitmap soit detruit avant l'appel a ConvertImage
    juce::Image::BitmapData bitmap(m_RawDtm, juce::Image::BitmapData::writeOnly);
    double minGsd = XMin(deltaX, deltaY);
    uint32_t gridW = (uint32_t)(m_Frame.Width() / minGsd), gridH = (uint32_t)(m_Frame.Height() / minGsd);
    float* grid = new float[gridW * gridH];
    if (grid == nullptr)
      return;
    for (uint32_t i = 0; i < gridW * gridH; i++)
      grid[i] = (float)XGEO_NO_DATA;
    GeoTools::ComputeZGrid(m_Base, grid, gridW, gridH, &m_Frame);
    Zmin = XGEO_NO_DATA;
    for (uint32_t i = 0; i < gridW * gridH; i++) {
      if (grid[i] > (float)XGEO_NO_DATA) {
        if (Zmin <= XGEO_NO_DATA) Zmin = grid[i]; else Zmin = XMin(Zmin, (double)grid[i]);
      }
    }
    XBaseImage::FastZoomBil(grid, gridW, gridH, (float*)bitmap.data, bitmap.width, bitmap.height);
    delete[] grid;
  }
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = Zmin;
 
  juce::Image rgbImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  if (m_bDtmTextured)
    rgbImage = m_QuickLook.rescaled(m_nDtmW, m_nDtmH, juce::Graphics::ResamplingQuality::highResamplingQuality);
  else {
    shader.ConvertImage(&m_RawDtm, &rgbImage);
    opacity = 1.f; // On ne respecte pas l'opacite du DtmShader car il n'y a pas de fond raster
  }
  
  juce::Image::BitmapData colors(rgbImage, juce::Image::BitmapData::readWrite);
  juce::Image::BitmapData bitmap(m_RawDtm, juce::Image::BitmapData::readOnly);

  for (uint32_t i = 0; i < m_nDtmH; i++) {
    P.Y = m_Frame.Ymax - i * deltaY;
    float* ptr_Z = (float*)bitmap.getLinePointer(i);
    juce::uint32* ptr_colour = (juce::uint32*)colors.getLinePointer(i);
    for (uint32_t j = 0; j < m_nDtmW; j++) {
      P.X = m_Frame.Xmin + j * deltaX;
      P.Z = *ptr_Z;
      ptr_Z++;
      colour = juce::Colour(*ptr_colour);
      ptr_colour++;
      if (P.Z >= Zmin) {
        ptr_vertex->position[0] = (float)((P.X - m_dX0) / m_dGsd);
        ptr_vertex->position[1] = (float)((P.Y - m_dY0) / m_dGsd);
        ptr_vertex->position[2] = (float)((P.Z - m_dZ0) / m_dGsd);
        ptr_vertex->colour[0] = colour.getFloatRed();
        ptr_vertex->colour[1] = colour.getFloatGreen();
        ptr_vertex->colour[2] = colour.getFloatBlue();
        ptr_vertex->colour[3] = opacity;
        m_nNbDtmVertex++;
      }
      else {
        ptr_vertex->position[0] = (float)((P.X - m_dX0) / m_dGsd);
        ptr_vertex->position[1] = (float)((P.Y - m_dY0) / m_dGsd);
        ptr_vertex->position[2] = (float)((Zmin - m_dZ0) / m_dGsd);
        ptr_vertex->colour[0] = 0.f; 
        ptr_vertex->colour[1] = 0.f;
        ptr_vertex->colour[2] = 0.f;
        ptr_vertex->colour[3] = -1.f; // NoData -> completement transparent
      }
      ptr_vertex++;
    }
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

//==============================================================================
// Reinitialisation de la grille des MNT
//==============================================================================
void OGLWidget::ReinitDtm()
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  double deltaX = m_Frame.Width() / m_nDtmW;
  double deltaY = m_Frame.Height() / m_nDtmH;
  double X = 0., Y = 0., Z = 0.;

  for (uint32_t i = 0; i < m_nDtmH; i++) {
    Y = m_Frame.Ymax - i * deltaY;
    for (uint32_t j = 0; j < m_nDtmW; j++) {
      X = m_Frame.Xmin + j * deltaX;
      Z = 0.;
      ptr_vertex->position[0] = (float)((X - m_dX0) / m_dGsd);
      ptr_vertex->position[1] = (float)((Y - m_dY0) / m_dGsd);
      ptr_vertex->position[2] = (float)((Z) / m_dGsd);
      ptr_vertex->colour[0] = 0.f;
      ptr_vertex->colour[1] = 0.f;
      ptr_vertex->colour[2] = 0.f;
      ptr_vertex->colour[3] = (float)-1.f; // NoData -> completement transparent
      ptr_vertex++;
    }
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  m_nNbDtmVertex = 0;
}

//==============================================================================
// Translation en Z de tous les points
//==============================================================================
void OGLWidget::MoveZ(float dZ)
{
  const juce::ScopedLock lock(m_Mutex);
  //openGLContext.setContinuousRepainting(false);
  using namespace ::juce::gl;
  m_dOffsetZ += dZ;
  // Points LAS
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < m_nNbLasVertex; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Points MNT
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < m_nNbDtmVertex; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Points Polygones
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyBufferID);
  ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < m_nNbPolyVertex; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Points Polylignes
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineBufferID);
  ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < m_nNbLineVertex; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Points selectionnes
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < 1; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  // Cible
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_TargetID);
  ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  for (uint32_t i = 0; i < 6; i++) {
    ptr_vertex->position[2] += dZ;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);

  m_bNeedUpdate = false;
  //openGLContext.setContinuousRepainting(true);
  openGLContext.triggerRepaint();
}

//==============================================================================
// Changement des couleurs des points LAS
//==============================================================================
void OGLWidget::ChangeLasColor()
{
  m_bNeedUpdate = false;
  m_bUpdateLasColor = false;
  if (m_nNbLasVertex < 1)
    return;
  LasShader shader;
  if ((shader.Mode() != LasShader::ShaderMode::Altitude)&&(!m_bRasterLas))
    return;
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  if (ptr_vertex == nullptr)
    return;
  m_bZLocalRange = !m_bZLocalRange;
  float zmin = (float)LasShader::Zmin(), zmax = (float)LasShader::Zmax();
  if (!m_bRasterLas) {  // Si on n'utilise pas le fond raster, on utilise une palette
    if (m_bZLocalRange) { // Recherche du Zmin et du Zmax local des points LAS charges
			zmin = ptr_vertex[0].position[2];
			zmax = ptr_vertex[0].position[2];
      for (uint32_t i = 1; i < m_nNbLasVertex; i++) {
        zmin = XMin(zmin, ptr_vertex[i].position[2]);
        zmax = XMax(zmax, ptr_vertex[i].position[2]);
      }
    }
  }
  float deltaZ = zmax - zmin;
  if (deltaZ <= 0) deltaZ = 1.f;	// Pour eviter les divisions par 0

  juce::Colour col = juce::Colours::orchid;
  uint8_t data[4] = { 0, 0, 0, 255 };
  uint32_t* data_ptr = (uint32_t*)&data;
  juce::Image::BitmapData bitmap(m_QuickLook, juce::Image::BitmapData::readOnly);
  int x, y;
  for (uint32_t i = 0; i < m_nNbLasVertex; i++) {
    if (m_bRasterLas) { // Affichage avec le raster
      x = (int)(bitmap.width * 0.5 + ptr_vertex->position[0] / 2. * (bitmap.width * 0.5));
      y = (int)(bitmap.height * 0.5 - ptr_vertex->position[1] / 2. * (bitmap.height * 0.5));
      *data_ptr = (uint32_t)bitmap.getPixelColour(x, y).getARGB();
    }
    else // Affichage avec une palette
    {
      if (m_bZLocalRange)
        col = shader.AltiColor((uint8_t)((ptr_vertex->position[2] - zmin) * 255 / deltaZ));
      else
        col = shader.AltiColor((uint8_t)(((ptr_vertex->position[2] - m_dOffsetZ) * m_dGsd + m_dZ0 - zmin) * 255 / deltaZ));
      *data_ptr = (uint32_t)col.getARGB();
    }

    ptr_vertex->colour[0] = (float)data[2] / 255.f;
    ptr_vertex->colour[1] = (float)data[1] / 255.f;
    ptr_vertex->colour[2] = (float)data[0] / 255.f;
    ptr_vertex->colour[3] = (float)data[3] / 255.f;
    ptr_vertex++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);
  openGLContext.triggerRepaint();
}

//==============================================================================
// Changement des couleurs des MNT
//==============================================================================
void OGLWidget::ChangeDtmColor()
{
  m_bNeedUpdate = false;
  m_bUpdateDtmColor = false;

  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  if (ptr_vertex == nullptr)
    return;
  //m_bDtmTextured = !m_bDtmTextured;
  XPt3D P;
  double Zmin = m_Base->ZMin();

  juce::Colour colour;
  float opacity = (float)(DtmShader::m_dOpacity * 0.01);

  juce::Image rgbImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  if (m_bDtmTextured) {
    rgbImage = m_QuickLook.rescaled(m_nDtmW, m_nDtmH, juce::Graphics::ResamplingQuality::highResamplingQuality);
    opacity = 1.f;
  }
  else {
    DtmShader shader(m_Frame.Width() / m_nDtmW);
    shader.ConvertImage(&m_RawDtm, &rgbImage);
    opacity = 1.f; // On ne respecte pas l'opacite du DtmShader car il n'y a pas de fond raster
  }

  juce::Image::BitmapData colors(rgbImage, juce::Image::BitmapData::readWrite);
  juce::Image::BitmapData bitmap(m_RawDtm, juce::Image::BitmapData::readOnly);

  for (uint32_t i = 0; i < m_nDtmH; i++) {
    float* ptr_Z = (float*)bitmap.getLinePointer(i);
    juce::uint32* ptr_colour = (juce::uint32*)colors.getLinePointer(i);
    for (uint32_t j = 0; j < m_nDtmW; j++) {
      P.Z = *ptr_Z;
      ptr_Z++;
      colour = juce::Colour(*ptr_colour);
      ptr_colour++;
      if (P.Z >= Zmin) {
        ptr_vertex->colour[0] = colour.getFloatRed();
        ptr_vertex->colour[1] = colour.getFloatGreen();
        ptr_vertex->colour[2] = colour.getFloatBlue();
        ptr_vertex->colour[3] = opacity;
      }
      else {
        ptr_vertex->colour[0] = 0.f;
        ptr_vertex->colour[1] = 0.f;
        ptr_vertex->colour[2] = 0.f;
        ptr_vertex->colour[3] = -1.f; // NoData -> completement transparent
      }
      ptr_vertex++;
    }
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

//==============================================================================
// Chargement d'un fichier vectoriel
//==============================================================================
void OGLWidget::LoadVectorClass(XGeoClass* C)
{
  if (!m_Frame.Intersect(C->Frame()))
    return;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    XGeoVector* V = C->Vector(i);
    if (!V->Is3D())
      continue;
    if (!m_Frame.Intersect(V->Frame()))
      continue;
    if ((V->TypeVector() == XGeoVector::PolyZ) || (V->TypeVector() == XGeoVector::MPolyZ))
      DrawPolyVector(V);
    if ((V->TypeVector() == XGeoVector::LineZ) || (V->TypeVector() == XGeoVector::MLineZ))
      DrawLineVector(V);
  }
}

//==============================================================================
// Dessin d'un polygone
//==============================================================================
void OGLWidget::DrawPolyVector(XGeoVector* V)
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;

  if ((V->NbPt() + m_nNbPolyVertex) >= m_nMaxPolyPt) // Trop de points charges
    return;
  if ((V->NbPt() + m_nNbPolyVertex + m_nNbPoly) >= m_nMaxPolyPt) // Trop d'index charges
    return;

  if (!V->LoadGeom())
    return;
  if (V->Zmin() < -99.)
    return;
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = V->Zmin();
  uint32_t color = V->Repres()->Color();
  uint8_t* ptr_color = (uint8_t*)&color;
  float red = ptr_color[2] / 255.f;
  float green = ptr_color[1] / 255.f;
  float blue = ptr_color[0] / 255.f;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_vertex += m_nNbPolyVertex;

  XPt* P = V->Pt();
  double* Z = V->Z();
  for (uint32_t i = 0; i < V->NbPt(); i++) {
    ptr_vertex->position[0] = (float)((P->X - m_dX0) / m_dGsd);
    ptr_vertex->position[1] = (float)((P->Y - m_dY0) / m_dGsd);
    ptr_vertex->position[2] = (float)((*Z - m_dZ0) / m_dGsd);
    ptr_vertex->colour[0] = red;
    ptr_vertex->colour[1] = green;
    ptr_vertex->colour[2] = blue;
    ptr_vertex->colour[3] = (float)1.f;
    ptr_vertex++;
    P++;
    Z++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyElementID);
  GLuint* ptr_index = (GLuint*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_index += m_nNbPolyVertex;
  ptr_index += m_nNbPoly;
  if (V->NbPart() <= 1) {
    for (uint32_t i = 0; i < V->NbPt(); i++) {
      *ptr_index = m_nNbPolyVertex + i;
      ptr_index++;
    }
   }
  else {
    for (uint32_t i = 0; i < V->NbPt(); i++) {
      for (uint32_t j = 1; j < V->NbPart(); j++)
        if (i == V->Part(j)) {
          *ptr_index = 0xFFFFFFFF;  // Primitive restart
          ptr_index++;
        }
      *ptr_index = m_nNbPolyVertex + i;
      ptr_index++;
    }
  }
  *ptr_index = 0xFFFFFFFF;  // Primitive restart
  ptr_index++;
 
  glUnmapBuffer(GL_ARRAY_BUFFER);

  m_nNbPolyVertex += V->NbPt();
  m_nNbPoly += V->NbPart();
  V->Unload();
}

//==============================================================================
// Dessin d'une polyligne
//==============================================================================
void OGLWidget::DrawLineVector(XGeoVector* V)
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;

  if ((V->NbPt() + m_nNbLineVertex) >= m_nMaxLinePt) // Trop de points charges
    return;
  if ((V->NbPt() + m_nNbLineVertex + m_nNbLine) >= m_nMaxLinePt) // Trop d'index charges
    return;

  if (!V->LoadGeom())
    return;
  if (V->Zmin() < -99.)
    return;
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = V->Zmin();
  uint32_t color = V->Repres()->Color();
  uint8_t* ptr_color = (uint8_t*)&color;
  float red = ptr_color[2] / 255.f;
  float green = ptr_color[1] / 255.f;
  float blue = ptr_color[0] / 255.f;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_vertex += m_nNbLineVertex;

  XPt* P = V->Pt();
  double* Z = V->Z();
  for (uint32_t i = 0; i < V->NbPt(); i++) {
    ptr_vertex->position[0] = (float)((P->X - m_dX0) / m_dGsd);
    ptr_vertex->position[1] = (float)((P->Y - m_dY0) / m_dGsd);
    ptr_vertex->position[2] = (float)((*Z - m_dZ0) / m_dGsd);
    ptr_vertex->colour[0] = red;
    ptr_vertex->colour[1] = green;
    ptr_vertex->colour[2] = blue;
    ptr_vertex->colour[3] = (float)1.f;
    ptr_vertex++;
    P++;
    Z++;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineElementID);
  GLuint* ptr_index = (GLuint*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_index += m_nNbLineVertex;
  ptr_index += m_nNbLine;
  if (V->NbPart() <= 1) {
    for (uint32_t i = 0; i < V->NbPt(); i++) {
      *ptr_index = m_nNbLineVertex + i;
      ptr_index++;
    }
  }
  else {
    for (uint32_t i = 0; i < V->NbPt(); i++) {
      for (uint32_t j = 1; j < V->NbPart(); j++)
        if (i == V->Part(j)) {
          *ptr_index = 0xFFFFFFFF;  // Primitive restart
          ptr_index++;
        }
      *ptr_index = m_nNbLineVertex + i;
      ptr_index++;
    }
  }
  *ptr_index = 0xFFFFFFFF;  // Primitive restart
  ptr_index++;

  glUnmapBuffer(GL_ARRAY_BUFFER);

  m_nNbLineVertex += V->NbPt();
  m_nNbLine += V->NbPart();
  V->Unload();
}

//==============================================================================
// Dessin du repere
//==============================================================================
void OGLWidget::CreateRepere()
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;

  float myVertex[]{
  0, 0, 0, 1., 0., 0., 0.5,
  1, 0, 0, 1., 0., 0., 0.5,
  0, 0, 0, 0., 1., 0., 0.5,
  0, 1, 0, 0., 1., 0., 0.5,
  0, 0, 0, 0., 0., 1., 0.5,
  0, 0, 1, 0., 0., 1., 0.5,
  };
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_RepereID);
  openGLContext.extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(myVertex), myVertex);
}

//==============================================================================
// Fixe la cible
//==============================================================================
void OGLWidget::DrawTarget()
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  Vertex M[6];
  // Couleur
  for (int i = 0; i < 6; i++) {
    M[i].colour[0] = M[i].colour[3] = M[i].colour[2] = 1.f;
    M[i].colour[1] = 0.f;
  }

  float X = (float)((m_Target.X - m_dX0) / m_dGsd);
  float Y = (float)((m_Target.Y - m_dY0) / m_dGsd);
  float Z = (float)((m_Target.Z - m_dZ0) / m_dGsd + m_dOffsetZ);
  // Axe Z
  M[0].position[0] = X;
  M[0].position[1] = Y;
  M[0].position[2] = (float)(1.0);
  M[1].position[0] = X;
  M[1].position[1] = Y;
  M[1].position[2] = (float)(-1.0);
  // Axe X
  M[2].position[0] = X - 0.1f;
  M[2].position[1] = Y;
  M[2].position[2] = Z;
  M[3].position[0] = X + 0.1f;
  M[3].position[1] = Y;
  M[3].position[2] = Z;
  // Axe Y
  M[4].position[0] = X;
  M[4].position[1] = Y - 0.1f;
  M[4].position[2] = Z;
  M[5].position[0] = X;
  M[5].position[1] = Y + 0.1f;
  M[5].position[2] = Z;

  m_bNeedTarget = false;
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_TargetID);
  openGLContext.extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M), &M);
}

//==============================================================================
// Selection
//==============================================================================
void OGLWidget::Select(int u, int v)
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  juce::Matrix3D<float> V = getViewMatrix();
  juce::Matrix3D<float> P = getProjectionMatrix();
 
  GLfloat* modelview = V.mat;
  GLfloat* projection = P.mat;
  GLint viewport[4];
  viewport[0] = viewport[1] = 0;
  auto desktopScale = (float)openGLContext.getRenderingScale();
  viewport[2] = juce::roundToInt(desktopScale * (float)getWidth());
  viewport[3] = juce::roundToInt(desktopScale * (float)getHeight());

  GLfloat winX = 0.0, winY = 0.0, winZ = 0.0;
  winX = (GLfloat)u * desktopScale;
  winY = (GLfloat)(viewport[3] - v * desktopScale);

  // Z du DEPTH buffer
  glReadPixels((GLint)winX, (GLint)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

  // Passage en coordonnees normalisees
  Vertex M;
  M.colour[0] = M.colour[3] = M.colour[2] = 1.f;
  M.colour[1] = 0.f;
  glhUnProjectf(winX, winY, winZ, modelview, projection, viewport, M.position);
  XPt3D A(M.position[0], M.position[1], M.position[2] - m_dOffsetZ);
  A *= m_dGsd;
  A += XPt3D(m_dX0, m_dY0, m_dZ0);
  m_bNeedLasPoint = false;
  if (m_bUpdateTarget) {  // Mise a jour de la cible
    m_Target = A;
    m_bUpdateTarget = false;
    sendActionMessage("UpdateTargetPos:" + juce::String(m_Target.X, 2) + ":" + juce::String(m_Target.Y, 2)
      + ":" + juce::String(m_Target.Z, 2));
  }
  else {
    m_LastPt = A;
    if (m_bTranslateView || m_bZoomInView || m_bZoomOutView) {
      TranslateView();
      M.position[0] = M.position[1] = M.position[2] = 0.f;
    }
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
    openGLContext.extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M), &M);
  }
}

//==============================================================================
// Translation de la vue
//==============================================================================
void OGLWidget::TranslateView()
{
  XFrame newFrame3D;
  double factor = 0.5;
  if (m_bZoomInView) factor = 0.25;
  if (m_bZoomOutView) factor = 2;
  double W = m_Frame.Width() * factor, H = m_Frame.Height() * factor;
  newFrame3D.Xmin = m_LastPt.X - W;
  newFrame3D.Xmax = m_LastPt.X + W;
  newFrame3D.Ymin = m_LastPt.Y - H;
  newFrame3D.Ymax = m_LastPt.Y + H;
  m_bTranslateView = m_bZoomInView = m_bZoomOutView = false;
  sendActionMessage("Translate3DView:" + juce::String(newFrame3D.Xmin) + ":" + juce::String(newFrame3D.Xmax) + ":" +
    juce::String(newFrame3D.Ymin) + ":" + juce::String(newFrame3D.Ymax));
  LoadObjects(m_Base, &newFrame3D);
  m_bNeedUpdate = false;  // L'update sera effectue apres la mise a jour du quick look
  UpdateQuickLook();
}

//==============================================================================
// Mise a jour du quick look
//==============================================================================
void OGLWidget::UpdateQuickLook()
{
  if (m_Frame.IsEmpty())
    return;
  if (m_bRasterLas || m_bDtmTextured) {
    juce::MouseCursor::showWaitCursor();
    m_MapThread.SetGeoBase(m_Base);
    double gsd = m_Frame.Width() / 1000.;
    m_MapThread.SetWorld(m_Frame.Xmin, m_Frame.Ymax, gsd,
      (int)(m_Frame.Width() / gsd), (int)(m_Frame.Height() / gsd), true);
    m_MapThread.SetUpdate(false, true, false, false, false);
    m_MapThread.startThread(juce::Thread::Priority::high);
  } else
    m_bNeedUpdate = true;
}

//==============================================================================
// Fin du thread de mise a jour du quick look
//==============================================================================
void OGLWidget::exitSignalSent()
{
  //m_bUpdateDtmColor = true;
  //m_bUpdateLasColor = true;
  juce::MouseCursor::hideWaitCursor();
  m_bNeedUpdate = true;
  m_QuickLook = m_MapThread.GetRaster();
  //repaint();
}

//-----------------------------------------------------------------------------
// Fonctions issues de GLU https://www.khronos.org/opengl/wiki/GluProject_and_gluUnProject_code
//-----------------------------------------------------------------------------
int glhProjectf(float objx, float objy, float objz, float* modelview, float* projection, int* viewport, float* windowCoordinate)
{
  // Transformation vectors
  float fTempo[8];
  // Modelview transform
  fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12]; // w is always 1
  fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
  fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
  fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
  // Projection transform, the final row of projection matrix is always [0 0 -1 0]
  // so we optimize for that.
  fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
  fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
  fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
  fTempo[7] = -fTempo[2];
  // The result normalizes between -1 and 1
  if (fTempo[7] == 0.0) // The w value
    return 0;
  fTempo[7] = (float)(1.0 / fTempo[7]);
  // Perspective division
  fTempo[4] *= fTempo[7];
  fTempo[5] *= fTempo[7];
  fTempo[6] *= fTempo[7];
  // Window coordinates
  // Map x, y to range 0-1
  windowCoordinate[0] = (fTempo[4] * 0.5f + 0.5f) * viewport[2] + viewport[0];
  windowCoordinate[1] = (fTempo[5] * 0.5f + 0.5f) * viewport[3] + viewport[1];
  // This is only correct when glDepthRange(0.0, 1.0)
  windowCoordinate[2] = (1.0f + fTempo[6]) * 0.5f;	// Between 0 and 1
  return 1;
}

int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate)
{
  // Transformation matrices
  float m[16], A[16];
  float in[4], out[4];
  // Calculation for inverting a matrix, compute projection x modelview
  // and store in A[16]
  MultiplyMatrices4by4OpenGL_FLOAT(A, projection, modelview);
  // Now compute the inverse of matrix A
  if (glhInvertMatrixf2(A, m) == 0)
    return 0;
  // Transformation of normalized coordinates between -1 and 1
  in[0] = (winx - (float)viewport[0]) / (float)viewport[2] * 2.0f - 1.0f;
  in[1] = (winy - (float)viewport[1]) / (float)viewport[3] * 2.0f - 1.0f;
  in[2] = 2.0f * winz - 1.0f;
  in[3] = 1.0f;
  // Objects coordinates
  MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
  if (out[3] == 0.0)
    return 0;
  out[3] = 1.0f / out[3];
  objectCoordinate[0] = out[0] * out[3];
  objectCoordinate[1] = out[1] * out[3];
  objectCoordinate[2] = out[2] * out[3];
  return 1;
}

void MultiplyMatrices4by4OpenGL_FLOAT(float* result, float* matrix1, float* matrix2)
{
  result[0] = matrix1[0] * matrix2[0] +
    matrix1[4] * matrix2[1] +
    matrix1[8] * matrix2[2] +
    matrix1[12] * matrix2[3];
  result[4] = matrix1[0] * matrix2[4] +
    matrix1[4] * matrix2[5] +
    matrix1[8] * matrix2[6] +
    matrix1[12] * matrix2[7];
  result[8] = matrix1[0] * matrix2[8] +
    matrix1[4] * matrix2[9] +
    matrix1[8] * matrix2[10] +
    matrix1[12] * matrix2[11];
  result[12] = matrix1[0] * matrix2[12] +
    matrix1[4] * matrix2[13] +
    matrix1[8] * matrix2[14] +
    matrix1[12] * matrix2[15];
  result[1] = matrix1[1] * matrix2[0] +
    matrix1[5] * matrix2[1] +
    matrix1[9] * matrix2[2] +
    matrix1[13] * matrix2[3];
  result[5] = matrix1[1] * matrix2[4] +
    matrix1[5] * matrix2[5] +
    matrix1[9] * matrix2[6] +
    matrix1[13] * matrix2[7];
  result[9] = matrix1[1] * matrix2[8] +
    matrix1[5] * matrix2[9] +
    matrix1[9] * matrix2[10] +
    matrix1[13] * matrix2[11];
  result[13] = matrix1[1] * matrix2[12] +
    matrix1[5] * matrix2[13] +
    matrix1[9] * matrix2[14] +
    matrix1[13] * matrix2[15];
  result[2] = matrix1[2] * matrix2[0] +
    matrix1[6] * matrix2[1] +
    matrix1[10] * matrix2[2] +
    matrix1[14] * matrix2[3];
  result[6] = matrix1[2] * matrix2[4] +
    matrix1[6] * matrix2[5] +
    matrix1[10] * matrix2[6] +
    matrix1[14] * matrix2[7];
  result[10] = matrix1[2] * matrix2[8] +
    matrix1[6] * matrix2[9] +
    matrix1[10] * matrix2[10] +
    matrix1[14] * matrix2[11];
  result[14] = matrix1[2] * matrix2[12] +
    matrix1[6] * matrix2[13] +
    matrix1[10] * matrix2[14] +
    matrix1[14] * matrix2[15];
  result[3] = matrix1[3] * matrix2[0] +
    matrix1[7] * matrix2[1] +
    matrix1[11] * matrix2[2] +
    matrix1[15] * matrix2[3];
  result[7] = matrix1[3] * matrix2[4] +
    matrix1[7] * matrix2[5] +
    matrix1[11] * matrix2[6] +
    matrix1[15] * matrix2[7];
  result[11] = matrix1[3] * matrix2[8] +
    matrix1[7] * matrix2[9] +
    matrix1[11] * matrix2[10] +
    matrix1[15] * matrix2[11];
  result[15] = matrix1[3] * matrix2[12] +
    matrix1[7] * matrix2[13] +
    matrix1[11] * matrix2[14] +
    matrix1[15] * matrix2[15];
}

void MultiplyMatrixByVector4by4OpenGL_FLOAT(float* resultvector, const float* matrix, const float* pvector)
{
  resultvector[0] = matrix[0] * pvector[0] + matrix[4] * pvector[1] + matrix[8] * pvector[2] + matrix[12] * pvector[3];
  resultvector[1] = matrix[1] * pvector[0] + matrix[5] * pvector[1] + matrix[9] * pvector[2] + matrix[13] * pvector[3];
  resultvector[2] = matrix[2] * pvector[0] + matrix[6] * pvector[1] + matrix[10] * pvector[2] + matrix[14] * pvector[3];
  resultvector[3] = matrix[3] * pvector[0] + matrix[7] * pvector[1] + matrix[11] * pvector[2] + matrix[15] * pvector[3];
}

#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

// This code comes directly from GLU except that it is for float
int glhInvertMatrixf2(float* m, float* out)
{
  float wtmp[4][8];
  float m0, m1, m2, m3, s;
  float* r0, * r1, * r2, * r3;
	r0 = wtmp[0]; r1 = wtmp[1]; r2 = wtmp[2]; r3 = wtmp[3];
	r0[0] = MAT(m, 0, 0); r0[1] = MAT(m, 0, 1);
	r0[2] = MAT(m, 0, 2); r0[3] = MAT(m, 0, 3);
	r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;
	r1[0] = MAT(m, 1, 0); r1[1] = MAT(m, 1, 1);
	r1[2] = MAT(m, 1, 2); r1[3] = MAT(m, 1, 3);
	r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;
	r2[0] = MAT(m, 2, 0); r2[1] = MAT(m, 2, 1);
	r2[2] = MAT(m, 2, 2); r2[3] = MAT(m, 2, 3);
	r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;
	r3[0] = MAT(m, 3, 0); r3[1] = MAT(m, 3, 1);
	r3[2] = MAT(m, 3, 2); r3[3] = MAT(m, 3, 3);
	r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;
  /* choose pivot - or die */
  if (fabsf(r3[0]) > fabsf(r2[0]))
    SWAP_ROWS_FLOAT(r3, r2);
  if (fabsf(r2[0]) > fabsf(r1[0]))
    SWAP_ROWS_FLOAT(r2, r1);
  if (fabsf(r1[0]) > fabsf(r0[0]))
    SWAP_ROWS_FLOAT(r1, r0);
  if (0.0 == r0[0])
    return 0;
  /* eliminate first variable */
  m1 = r1[0] / r0[0];
  m2 = r2[0] / r0[0];
  m3 = r3[0] / r0[0];
  s = r0[1];
  r1[1] -= m1 * s;
  r2[1] -= m2 * s;
  r3[1] -= m3 * s;
  s = r0[2];
  r1[2] -= m1 * s;
  r2[2] -= m2 * s;
  r3[2] -= m3 * s;
  s = r0[3];
  r1[3] -= m1 * s;
  r2[3] -= m2 * s;
  r3[3] -= m3 * s;
  s = r0[4];
  if (s != 0.0) {
    r1[4] -= m1 * s;
    r2[4] -= m2 * s;
    r3[4] -= m3 * s;
  }
  s = r0[5];
  if (s != 0.0) {
    r1[5] -= m1 * s;
    r2[5] -= m2 * s;
    r3[5] -= m3 * s;
  }
  s = r0[6];
  if (s != 0.0) {
    r1[6] -= m1 * s;
    r2[6] -= m2 * s;
    r3[6] -= m3 * s;
  }
  s = r0[7];
  if (s != 0.0) {
    r1[7] -= m1 * s;
    r2[7] -= m2 * s;
    r3[7] -= m3 * s;
  }
  /* choose pivot - or die */
  if (fabsf(r3[1]) > fabsf(r2[1]))
    SWAP_ROWS_FLOAT(r3, r2);
  if (fabsf(r2[1]) > fabsf(r1[1]))
    SWAP_ROWS_FLOAT(r2, r1);
  if (0.0 == r1[1])
    return 0;
  /* eliminate second variable */
  m2 = r2[1] / r1[1];
  m3 = r3[1] / r1[1];
  r2[2] -= m2 * r1[2];
  r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3];
  r3[3] -= m3 * r1[3];
  s = r1[4];
  if (0.0 != s) {
    r2[4] -= m2 * s;
    r3[4] -= m3 * s;
  }
  s = r1[5];
  if (0.0 != s) {
    r2[5] -= m2 * s;
    r3[5] -= m3 * s;
  }
  s = r1[6];
  if (0.0 != s) {
    r2[6] -= m2 * s;
    r3[6] -= m3 * s;
  }
  s = r1[7];
  if (0.0 != s) {
    r2[7] -= m2 * s;
    r3[7] -= m3 * s;
  }
  /* choose pivot - or die */
  if (fabsf(r3[2]) > fabsf(r2[2]))
    SWAP_ROWS_FLOAT(r3, r2);
  if (0.0 == r2[2])
    return 0;
  /* eliminate third variable */
  m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
	r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6]; r3[7] -= m3 * r2[7];
  /* last check */
  if (0.0 == r3[3])
    return 0;
  s = 1.0f / r3[3];		/* now back substitute row 3 */
  r3[4] *= s;
  r3[5] *= s;
  r3[6] *= s;
  r3[7] *= s;
  m2 = r2[3];			/* now back substitute row 2 */
  s = 1.0f / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2); r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2); r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
	r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
  m0 = r0[3];
	r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0;
	r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;
  m1 = r1[2];			/* now back substitute row 1 */
  s = 1.0f / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
	r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0;
	r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;
  m0 = r0[1];			/* now back substitute row 0 */
  s = 1.0f / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);
  MAT(out, 0, 0) = r0[4];
	MAT(out, 0, 1) = r0[5]; MAT(out, 0, 2) = r0[6];
	MAT(out, 0, 3) = r0[7]; MAT(out, 1, 0) = r1[4];
	MAT(out, 1, 1) = r1[5]; MAT(out, 1, 2) = r1[6];
	MAT(out, 1, 3) = r1[7]; MAT(out, 2, 0) = r2[4];
	MAT(out, 2, 1) = r2[5]; MAT(out, 2, 2) = r2[6];
	MAT(out, 2, 3) = r2[7]; MAT(out, 3, 0) = r3[4];
	MAT(out, 3, 1) = r3[5]; MAT(out, 3, 2) = r3[6];
  MAT(out, 3, 3) = r3[7];
  return 1;
}
#undef SWAP_ROWS_DOUBLE
#undef SWAP_ROWS_FLOAT
#undef MAT
