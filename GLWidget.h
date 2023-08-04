#ifndef GL_WIDGET_H
#define GL_WIDGET_H

#include <QtOpenGL>
#include <QtOpenGLWidgets>
#include <QOpenGLShader>
#include <QtGui>
#include <QWindow>

#include <QtGlobal>
#include "Mesh.h"
#include "Trackball.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

using namespace glm;

class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
  Q_OBJECT

public:
  explicit GLWidget(QWidget *parent);

  void loadMeshData(const Mesh &mesh);
  void updateMeshData(const Mesh &mesh);
  void unloadMeshData();

  void wireframePaint();
  void saveLastModel();
  TrackBall trackBall;

  void reloadShaders();

protected:
  void paintGL() override;
  void initializeGL() override;

private:
  std::vector<float> vertices;
  std::vector<unsigned int> faces;

  std::string readFile(const char *fileLocation) const;
  GLuint createShader(GLenum type, const GLchar *source);
  void initializeShaders();

  GLuint shaderProgram;
  GLuint vShader, fShader;
  GLint modelLocation, wireLocation;
  mat4 model;

  bool wireframeMode = false;
};

#endif // GL_WIDGET_H
