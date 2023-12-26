//-----------------------------------------------------------------------------
//								OGL3DViewer.h
//								=============
//
// Composant pour la visualisation OpenGL d'objets geographiques
//
// Auteur : F.Becirspahic - IGN / DSI / SIMV
// License : GNU AFFERO GENERAL PUBLIC LICENSE v3
// Date de creation : 01/12/2023
//-----------------------------------------------------------------------------

#pragma once

#include <JuceHeader.h>
#include "../../XTool/XFrame.h"
#include "../../XTool/XPt2D.h"
#include "../../XTool/XPt3D.h"

class XGeoBase;
class XGeoClass;
class XGeoVector;
class GeoLAS;
class GeoDTM;

class OGLWidget : public juce::OpenGLAppComponent {
public:
  OGLWidget();
  ~OGLWidget() { shutdownOpenGL(); }

  void initialise() override;
  void shutdown() override;
  void CreateShaders();

  juce::Matrix3D<float> getProjectionMatrix() const;
  juce::Matrix3D<float> getViewMatrix() const;

  void render() override;

  void paint(juce::Graphics& g) override;
  void resized() override;
  void mouseDown(const juce::MouseEvent& event) override;
  void mouseMove(const juce::MouseEvent& event) override;
  void mouseDrag(const juce::MouseEvent& event) override;
  void mouseDoubleClick(const juce::MouseEvent& event) override;
  void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
  bool keyPressed(const juce::KeyPress& key) override;

  void LoadObjects(XGeoBase* base, XFrame* F);

protected:
  void LoadLasClass(XGeoClass* C);
  void LoadDtmClass(XGeoClass* C);
  void LoadVectorClass(XGeoClass* C);
  void UpdateBase();
  void ReinitDtm();
  void DrawLas(GeoLAS* las);
  void DrawDtm(GeoDTM* dtm);
  void DrawPolyVector(XGeoVector* V);
  void DrawLineVector(XGeoVector* V);
  void Select(int u, int v);

  void CreateRepere();

private:
  struct Vertex { // Vertex 3D
    float position[3];
    float colour[4];
  };

  struct Index {  // Indices des triangles pour les MNT
    uint32_t num[3];
  };

  XPt3D			m_T;						// Translation
  XPt3D			m_R;						// Rotation
  XPt3D     m_S;            // Facteurs d'echelle
  double    m_dX0, m_dY0, m_dZ0, m_dGsd; // Transformation terrain -> pixel
  XFrame    m_Frame;
  XGeoBase* m_Base;

  uint32_t  m_nMaxLasPt;        // Nombre maximum de points LAS
  uint32_t  m_nMaxPolyPt;        // Nombre maximum de points polygone
  uint32_t  m_nMaxLinePt;        // Nombre maximum de points polyligne
  uint32_t  m_nNbLasVertex;     // Nombre de points LAS
  uint32_t  m_nNbPolyVertex;    // Nombre de points polygone
  uint32_t  m_nNbLineVertex;    // Nombre de points polyligne
  uint32_t  m_nNbDtmVertex;     // Nombre de points DTM
  uint32_t  m_nNbPoly;          // Nombre de polygones
  uint32_t  m_nNbLine;          // Nombre de polylignes

  GLuint    m_LasBufferID;      // Points LAS
  GLuint    m_DtmBufferID;      // Points des MNT
  GLuint    m_DtmVertexArrayID;
  GLuint    m_DtmElementID;
  GLuint    m_PolyBufferID;     // Points des polygones
  GLuint    m_PolyVertexArrayID;
  GLuint    m_PolyElementID;
  GLuint    m_LineBufferID;     // Points des polylignes
  GLuint    m_LineVertexArrayID;
  GLuint    m_LineElementID;
  GLuint    m_RepereID;         // Points du repere
  GLuint    m_PtBufferID;       // Points selectionnes
  bool      m_bNeedUpdate;
  bool      m_bAutoRotation;    // Rotation automatique de la scene 3D
  bool      m_bNeedLasPoint;
  float     m_LasPointSize;     // Taille des points LAS
  float     m_VectorWidth;      // Epaisseur des lignes des donnees vectorielles
  float     m_DtmLineWidth;     // Epaisseur des lignes des MNT
  bool      m_bDtmTriangle;     // Affichage des MNT avec des triangles
  bool      m_bDtmFill;         // Remplissage des triangles des MNT
  bool      m_bViewLas;         // Affichages points LAS
  bool      m_bViewDtm;         // Affichages des MNT
  bool      m_bViewVector;      // Affichages des donnees vectorielles
  uint32_t  m_nDtmW;
  uint32_t  m_nDtmH;
  

  juce::Point<float>  m_LastPos;  // Position souris pour les drags
  XPt3D               m_LastPt;   // Point clique

private:
  //==============================================================================
  // This class just manages the attributes that the shaders use.
  struct Attributes
  {
    explicit Attributes(juce::OpenGLShaderProgram& shaderProgram)
    {
      position.reset(createAttribute(shaderProgram, "position"));
      sourceColour.reset(createAttribute(shaderProgram, "sourceColour"));
    }

    void enable()
    {
      using namespace ::juce::gl;

      if (position.get() != nullptr)
      {
        glVertexAttribPointer(position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(position->attributeID);
      }

      if (sourceColour.get() != nullptr)
      {
        glVertexAttribPointer(sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
        glEnableVertexAttribArray(sourceColour->attributeID);
      }

    }

    void disable()
    {
      using namespace ::juce::gl;

      if (position != nullptr)       glDisableVertexAttribArray(position->attributeID);
      if (sourceColour != nullptr)   glDisableVertexAttribArray(sourceColour->attributeID);
    }

    std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position, sourceColour;

  private:
    static juce::OpenGLShaderProgram::Attribute* createAttribute(juce::OpenGLShaderProgram& shader,
      const char* attributeName)
    {
      using namespace ::juce::gl;

      if (glGetAttribLocation(shader.getProgramID(), attributeName) < 0)
        return nullptr;

      return new juce::OpenGLShaderProgram::Attribute(shader, attributeName);
    }
  };

  //==============================================================================
  // This class just manages the uniform values that the demo shaders use.
  struct Uniforms
  {
    explicit Uniforms(juce::OpenGLShaderProgram& shaderProgram)
    {
      projectionMatrix.reset(createUniform(shaderProgram, "projectionMatrix"));
      viewMatrix.reset(createUniform(shaderProgram, "viewMatrix"));
    }

    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;

  private:
    static juce::OpenGLShaderProgram::Uniform* createUniform(juce::OpenGLShaderProgram& shaderProgram,
      const char* uniformName)
    {
      using namespace ::juce::gl;

      if (glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
        return nullptr;

      return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
    }
  };

  const char* vertexShader;
  const char* fragmentShader;

  std::unique_ptr<juce::OpenGLShaderProgram> m_Shader;
  std::unique_ptr<Attributes> m_Attributes;
  std::unique_ptr<Uniforms> m_Uniforms;

  juce::String m_NewVertexShader, m_NewFragmentShader;

  juce::Rectangle<int> m_Bounds;
  juce::CriticalSection m_Mutex;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OGLWidget)
};

//==============================================================================
// Classe OGL3DViewer
//==============================================================================
class OGL3DViewer : public juce::DocumentWindow {
public:
  OGL3DViewer(const juce::String& name, juce::Colour backgroundColour, int requiredButtons);
  ~OGL3DViewer() { ; }

  void closeButtonPressed() { setVisible(false); }
  
  void LoadObjects(XGeoBase* base, XFrame* F) { m_OGLWidget.LoadObjects(base, F); }

private:
  OGLWidget   m_OGLWidget;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OGL3DViewer)
};

//-----------------------------------------------------------------------------
// Fonctions issues de GLU https://www.khronos.org/opengl/wiki/GluProject_and_gluUnProject_code
//-----------------------------------------------------------------------------
extern int glhProjectf(float objx, float objy, float objz, float* modelview, float* projection, int* viewport, float* windowCoordinate);
extern int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate);
extern void MultiplyMatrices4by4OpenGL_FLOAT(float* result, float* matrix1, float* matrix2);
extern void MultiplyMatrixByVector4by4OpenGL_FLOAT(float* resultvector, const float* matrix, const float* pvector);
extern int glhInvertMatrixf2(float* m, float* out);
