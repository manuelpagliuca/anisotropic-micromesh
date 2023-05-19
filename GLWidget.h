#pragma once

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
  void initializeShaders();

  GLuint createShader(GLenum type, const GLchar *source);
  std::string readFile(const char *fileLocation) const;

  GLuint VAO, VBO, EBO;
  GLuint shaderProgram;
  GLuint vShader, fShader;
  GLint modelLocation;
  GLint wireLocation;
  glm::mat4 model;

  bool flag = false;

  bool wireframeMode = false;
  std::vector<float> vertices;
  std::vector<unsigned int> faces;
};
