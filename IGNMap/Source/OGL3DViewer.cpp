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
OGLWidget::OGLWidget()
{
  setSize(500, 500);
  setWantsKeyboardFocus(true);
  openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
  openGLContext.setContinuousRepainting(true);
  vertexShader = nullptr;
  fragmentShader = nullptr;
  m_nNbLasVertex = m_nNbPolyVertex = m_nNbLineVertex = m_nNbDtmVertex = 0;
  m_nNbPoly = m_nNbLine = 0;
  m_LasBufferID = m_RepereID = m_PtBufferID = 0;
  m_DtmBufferID = m_DtmVertexArrayID = m_DtmElementID = 0;
  m_PolyBufferID = m_PolyVertexArrayID = m_PolyElementID = 0;
  m_LineBufferID = m_LineVertexArrayID = m_LineElementID = 0;
  m_nMaxLasPt = 2000000;
  m_nMaxPolyPt = m_nMaxLinePt = 10000;
  m_bNeedUpdate = m_bNeedLasPoint = m_bAutoRotation = false;
  m_bDtmTriangle = m_bDtmFill = true;
  m_Base = nullptr;
  m_dX0 = m_dY0 = m_dGsd = m_dZ0 = 0.;
  m_S = XPt3D(1., 1., 1.);
  m_LasPointSize = 4.f;
  m_VectorWidth = 2.f;
  m_DtmLineWidth = 1.f;
  m_nDtmW = m_nDtmH = 300;
  m_bViewLas = m_bViewDtm = m_bViewVector = true;
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

  // Creation du buffer de points (du repere)pour la selection)
  openGLContext.extensions.glGenBuffers(1, &m_PtBufferID);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  CreateRepere();
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
  g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
  g.setFont(12);
  /*
  juce::String angles = "Angles : (" + juce::String(m_R.X) + " ; " + juce::String(m_R.Y) + " ; " + juce::String(m_R.Z) + ")";
  g.drawText(angles, 25, 20, 300, 30, juce::Justification::left);
  juce::String translation = "Translations : (" + juce::String(m_T.X) + " ; " + juce::String(m_T.Y) + " ; " + juce::String(m_T.Z) + ")";
  g.drawText(translation, 25, 40, 300, 30, juce::Justification::left);
  */
  juce::String help = "Point size : Q/S (Las) ; W/X (DTM) ; C/V (Vector)";
  g.drawText(help, 25, 20, 300, 30, juce::Justification::left);
  help = "Visibility: K (Vector) ; L (Las) ; M (DTM)";
  g.drawText(help, 25, 40, 300, 30, juce::Justification::left);
  juce::String nbPoint = "Points : " + juce::String(m_nNbLasVertex) + " Las";
  nbPoint += " | " + juce::String(m_nNbPolyVertex) + " Polygon";
  nbPoint += " | " + juce::String(m_nNbLineVertex) + " Line";
  g.drawText(nbPoint, 25, 60, 300, 30, juce::Justification::left);
  juce::String lastPt = "P = " + juce::String(m_LastPt.X, 2) + " ; " + juce::String(m_LastPt.Y, 2) + " ; " + juce::String(m_LastPt.Z, 2);
  g.drawText(lastPt, 25, 80, 300, 30, juce::Justification::left);
  g.drawLine(20, 20, 170, 20);
  g.drawLine(20, 110, 170, 110);
}

//==============================================================================
// Rendu OpenGL
//==============================================================================
void OGLWidget::render()
{
  using namespace ::juce::gl;
  const juce::ScopedLock lock(m_Mutex);

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

  // Dessin des polygones
  if ((m_nNbPolyVertex > 0) && (!m_bNeedUpdate) && (m_bViewVector)) {
    openGLContext.extensions.glBindVertexArray(m_PolyVertexArrayID);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PolyBufferID);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_PolyElementID);
    m_Attributes->enable();
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glLineWidth(m_VectorWidth);
    glDrawElements(GL_LINE_LOOP, m_nNbPolyVertex, GL_UNSIGNED_INT, 0);
    glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    m_Attributes->disable();
  }

  // Dessin des polylignes
  if ((m_nNbLineVertex > 0) && (!m_bNeedUpdate) && (m_bViewVector)) {
    openGLContext.extensions.glBindVertexArray(m_LineVertexArrayID);
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LineBufferID);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_LineElementID);
    m_Attributes->enable();
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glLineWidth(m_VectorWidth);
    glDrawElements(GL_LINE_STRIP, m_nNbLineVertex, GL_UNSIGNED_INT, 0);
    glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    m_Attributes->disable();
  }

  if (m_bNeedUpdate)
    UpdateBase();

  // Dessin du repere orthonorme
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_RepereID);
  m_Attributes->enable();
  glLineWidth(3.0);
  glDrawArrays(GL_LINES, 0, 6);
  m_Attributes->disable();

  // Gestion des selections de points
  if (m_bNeedLasPoint)
    Select((int)m_LastPos.x, (int)m_LastPos.y);
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  m_Attributes->enable();
  glPointSize(6.f);
  glDrawArrays(GL_POINTS, 0, 1);
  m_Attributes->disable();

  // Reset the element buffers so child Components draw correctly
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
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
  juce::Point<float> delta = event.position - m_LastPos;
  m_R.Z += (delta.getX() * 0.01);
  m_R.X += ((delta.getY() * 0.01));
  
  m_LastPos = event.position;
  repaint();  // Pour l'affichage des informations
}

void OGLWidget::mouseWheelMove(const juce::MouseEvent& /*event*/, const juce::MouseWheelDetails& wheel)
{
  double scale = wheel.deltaY / 10.;
  m_S += XPt3D(scale, scale, scale);
  repaint();
}

void OGLWidget::mouseDoubleClick(const juce::MouseEvent& /*event*/)
{
  m_bNeedLasPoint = true;
  repaint();
}

//==============================================================================
// Gestion du clavier
//==============================================================================
bool OGLWidget::keyPressed(const juce::KeyPress& key)
{
  if (key.getKeyCode() == juce::KeyPress::upKey)
    m_T.Y += 0.1;
  if (key.getKeyCode() == juce::KeyPress::downKey)
    m_T.Y -= 0.1;
  if (key.getKeyCode() == juce::KeyPress::leftKey)
    m_T.X -= 0.1;
  if (key.getKeyCode() == juce::KeyPress::rightKey)
    m_T.X += 0.1;
  if (key.getKeyCode() == juce::KeyPress::pageUpKey)
    m_S.Z += 0.1;
  if (key.getKeyCode() == juce::KeyPress::pageDownKey)
    m_S.Z -= 0.1;
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
  if ((key.getTextCharacter() == 'K') || (key.getTextCharacter() == 'k'))
    m_bViewVector = (!m_bViewVector);
  if ((key.getTextCharacter() == 'L') || (key.getTextCharacter() == 'l'))
    m_bViewLas = (!m_bViewLas);
  if ((key.getTextCharacter() == 'M') || (key.getTextCharacter() == 'm'))
    m_bViewDtm = (!m_bViewDtm);

  repaint();
  return true;
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
  m_bNeedUpdate = true;
  m_dX0 = F->Center().X;
  m_dY0 = F->Center().Y;
  if (F->Width() > F->Height())
    m_dGsd = F->Width() / 4.;
  else
    m_dGsd = F->Height() / 4.;
  m_dZ0 = base->Z(F->Center());
}

//==============================================================================
// Mise a jour des donnees chargees
//==============================================================================
void OGLWidget::UpdateBase()
{
  m_nNbLasVertex = 0;
  m_nNbDtmVertex = 0;
  m_nNbPolyVertex = m_nNbLineVertex = 0;
  m_nNbPoly = m_nNbLine = 0;
  ReinitDtm();
  if (m_Base == nullptr)
    return;
  const juce::ScopedLock lock(m_Mutex);
  openGLContext.setContinuousRepainting(false);
  for (uint32_t i = 0; i < m_Base->NbClass(); i++) {
    XGeoClass* C = m_Base->Class(i);
    if (C == nullptr)
      continue;
    if ( (C->IsLAS()) && (C->Visible()) )
      LoadLasClass(C);
    //if ((C->IsDTM()) && (C->Visible()))
    //  LoadDtmClass(C);
    if ((C->IsVector()) && (C->Visible()))
      LoadVectorClass(C);
  }
  DrawDtm();
  m_bNeedUpdate = false;
  openGLContext.setContinuousRepainting(true);
  openGLContext.triggerRepaint();
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

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_LasBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  ptr_vertex += m_nNbLasVertex;

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
  bool classif_newtype = true;
  if (header->version_minor < 4) classif_newtype = false;
  LasShader shader;
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

    X = point->X * header->x_scale_factor + header->x_offset;
    Y = point->Y * header->y_scale_factor + header->y_offset;
    Z = point->Z * header->z_scale_factor + header->z_offset;
    ptr_vertex->position[0] = (float) ((X - m_dX0) / m_dGsd);
    ptr_vertex->position[1] = (float) ((Y - m_dY0) / m_dGsd);
    ptr_vertex->position[2] = (float) ((Z - m_dZ0) / m_dGsd);

    switch (shader.Mode()) {
    case LasShader::ShaderMode::Altitude:
      col = shader.AltiColor((uint8_t)((Z - Z0) * 255 / deltaZ));
      *data_ptr = (uint32_t)col.getARGB();
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
      memcpy(data, &point->intensity, 2 * sizeof(uint8_t));
      break;
    case LasShader::ShaderMode::Angle:
      if (point->extended_scan_angle < 0) {	// Angle en degree = extended_scan_angle * 0.006
        data[2] = (uint8_t)(255 - point->extended_scan_angle * (-0.0085));	 // Normalise sur [0; 255]
        data[1] = 0;
        data[0] = 255 - data[0];
      }
      else {
        data[2] = 0;
        data[1] = (uint8_t)(255 - point->extended_scan_angle * (0.0085));	 // Normalise sur [0; 255]
        data[0] = 255 - data[1];
      }
      break;
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
  laszip_seek_point(reader, 0);
  las->CloseIfNeeded();
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

//==============================================================================
// Chargement d'un fichier MNT
//==============================================================================
void OGLWidget::LoadDtmClass(XGeoClass* C)
{
  if (!m_Frame.Intersect(C->Frame()))
    return;
  for (uint32_t i = 0; i < C->NbVector(); i++) {
    GeoDTM* dtm = dynamic_cast<GeoDTM*>(C->Vector(i));
    XFrame F = dtm->Frame();
    if (!m_Frame.Intersect(F))
      continue;
    DrawDtm(dtm);
  }
}

//==============================================================================
// Dessin d'un fichier MNT
//==============================================================================
void OGLWidget::DrawDtm(GeoDTM* dtm)
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  DtmShader shader;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  double deltaX = m_Frame.Width() / m_nDtmW;
  double deltaY = m_Frame.Height() / m_nDtmH;
  double X = 0., Y = 0., Z = 0.;
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = dtm->Zmin();

  juce::Colour colour;
  float opacity = (float)(DtmShader::m_dOpacity * 0.01);
  /*
  for (uint32_t i = 0; i < m_nDtmH; i++) {
    Y = m_Frame.Ymax - i * deltaY;
    for (uint32_t j = 0; j < m_nDtmW; j++) {
      X = m_Frame.Xmin + j * deltaX;
      Z = dtm->Z(X, Y);
      if (Z > XGEO_NO_DATA) {
        colour = DtmShader::Colour(Z);
        ptr_vertex->position[0] = (float)((X - m_dX0) / m_dGsd);
        ptr_vertex->position[1] = (float)((Y - m_dY0) / m_dGsd);
        ptr_vertex->position[2] = (float)((Z - m_dZ0) / m_dGsd);
        ptr_vertex->colour[0] = colour.getFloatRed();
        ptr_vertex->colour[1] = colour.getFloatGreen();
        ptr_vertex->colour[2] = colour.getFloatBlue();
        ptr_vertex->colour[3] = opacity;
        m_nNbDtmVertex++;
      }
      ptr_vertex++;
    }
  }*/
  juce::Image rawImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  juce::Image::BitmapData bitmap(rawImage, juce::Image::BitmapData::readWrite);
  for (int i = 0; i < bitmap.height; i++) {
    Y = m_Frame.Ymax - i * deltaY;
    float* ptr_Z = (float*)bitmap.getLinePointer(i);
    for (int j = 0; j < bitmap.width; j++) {
      X = m_Frame.Xmin + j * deltaX;
      Z = dtm->Z(X, Y);
      *ptr_Z = (float)Z;
      ptr_Z++;
    }
  }
  juce::Image rgbImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  shader.ConvertImage(&rawImage, &rgbImage);
  juce::Image::BitmapData colors(rgbImage, juce::Image::BitmapData::readWrite);

  for (uint32_t i = 0; i < m_nDtmH; i++) {
    Y = m_Frame.Ymax - i * deltaY;
    float* ptr_Z = (float*)bitmap.getLinePointer(i);
    juce::uint32* ptr_colour = (juce::uint32*)colors.getLinePointer(i);
    for (uint32_t j = 0; j < m_nDtmW; j++) {
      X = m_Frame.Xmin + j * deltaX;
      Z = *ptr_Z;
      ptr_Z++;
      colour = juce::Colour(*ptr_colour);
      ptr_colour++;
      if (Z >= m_dZ0) {
        ptr_vertex->position[0] = (float)((X - m_dX0) / m_dGsd);
        ptr_vertex->position[1] = (float)((Y - m_dY0) / m_dGsd);
        ptr_vertex->position[2] = (float)((Z - m_dZ0) / m_dGsd);
        ptr_vertex->colour[0] = colour.getFloatRed();
        ptr_vertex->colour[1] = colour.getFloatGreen();
        ptr_vertex->colour[2] = colour.getFloatBlue();
        ptr_vertex->colour[3] = opacity;
        m_nNbDtmVertex++;
      }
      ptr_vertex++;
    }
  }

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

//==============================================================================
// Dessin du MNT
//==============================================================================
void OGLWidget::DrawDtm()
{
  const juce::ScopedLock lock(m_Mutex);
  using namespace ::juce::gl;
  DtmShader shader;

  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_DtmBufferID);
  Vertex* ptr_vertex = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  double deltaX = m_Frame.Width() / m_nDtmW;
  double deltaY = m_Frame.Height() / m_nDtmH;
  XPt3D P;
  double Zmin = m_Base->ZMin();
  if (m_dZ0 <= XGEO_NO_DATA)
    m_dZ0 = Zmin;

  juce::Colour colour;
  float opacity = (float)(DtmShader::m_dOpacity * 0.01);
 
  juce::Image rawImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  juce::Image::BitmapData bitmap(rawImage, juce::Image::BitmapData::readWrite);
  for (int i = 0; i < bitmap.height; i++) {
    P.Y = m_Frame.Ymax - i * deltaY;
    float* ptr_Z = (float*)bitmap.getLinePointer(i);
    for (int j = 0; j < bitmap.width; j++) {
      P.X = m_Frame.Xmin + j * deltaX;
      P.Z = m_Base->Z(P);
      *ptr_Z = (float)P.Z;
      ptr_Z++;
    }
  }
  juce::Image rgbImage(juce::Image::PixelFormat::ARGB, m_nDtmW, m_nDtmH, true);
  shader.ConvertImage(&rawImage, &rgbImage);
  juce::Image::BitmapData colors(rgbImage, juce::Image::BitmapData::readWrite);

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
  winX = (GLfloat)u;
  winY = (GLfloat)(viewport[3] - v);

  // Z du DEPTH buffer
  glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

  // Passage en coordonnees normalisees
  Vertex M;
  M.colour[0] = M.colour[3] = 1.f;
  M.colour[1] = M.colour[2] = 0.f;
  glhUnProjectf(winX, winY, winZ, modelview, projection, viewport, M.position);
  m_LastPt = XPt3D(M.position[0], M.position[1], M.position[2]);
  m_LastPt *= m_dGsd;
  m_LastPt += XPt3D(m_dX0, m_dY0, m_dZ0);
  m_bNeedLasPoint = false;
  openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, m_PtBufferID);
  openGLContext.extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(M), &M);
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
  windowCoordinate[0] = (fTempo[4] * 0.5 + 0.5) * viewport[2] + viewport[0];
  windowCoordinate[1] = (fTempo[5] * 0.5 + 0.5) * viewport[3] + viewport[1];
  // This is only correct when glDepthRange(0.0, 1.0)
  windowCoordinate[2] = (1.0 + fTempo[6]) * 0.5;	// Between 0 and 1
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
  in[0] = (winx - (float)viewport[0]) / (float)viewport[2] * 2.0 - 1.0;
  in[1] = (winy - (float)viewport[1]) / (float)viewport[3] * 2.0 - 1.0;
  in[2] = 2.0 * winz - 1.0;
  in[3] = 1.0;
  // Objects coordinates
  MultiplyMatrixByVector4by4OpenGL_FLOAT(out, m, in);
  if (out[3] == 0.0)
    return 0;
  out[3] = 1.0 / out[3];
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
  r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
  r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
    r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
    r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
    r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
    r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
    r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
    r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
    r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
    r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
    r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
    r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
    r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
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
  r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
    r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
  /* last check */
  if (0.0 == r3[3])
    return 0;
  s = 1.0 / r3[3];		/* now back substitute row 3 */
  r3[4] *= s;
  r3[5] *= s;
  r3[6] *= s;
  r3[7] *= s;
  m2 = r2[3];			/* now back substitute row 2 */
  s = 1.0 / r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
    r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
    r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
    r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
  m1 = r1[2];			/* now back substitute row 1 */
  s = 1.0 / r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
    r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
    r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
  m0 = r0[1];			/* now back substitute row 0 */
  s = 1.0 / r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
    r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);
  MAT(out, 0, 0) = r0[4];
  MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
  MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
  MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
  MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
  MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
  MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
  MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
  MAT(out, 3, 3) = r3[7];
  return 1;
}
#undef SWAP_ROWS_DOUBLE
#undef SWAP_ROWS_FLOAT
#undef MAT
