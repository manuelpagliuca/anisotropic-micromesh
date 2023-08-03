#include "Headers/GLWidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
  shaderProgram = 0;
  modelLocation = viewLocation = projLocation = 0;
  lightPositionLocation = lightColorLocation = 0;
  shininessLocation = 0;

  model = mat4(1.f);
  view = mat4(1.f);
  proj = mat4(1.f);
  lightPosition = vec3(-20.0f, 10.0f, 1.0f);
  lightColor = vec3(1.0f);
  shininess = 10.0f;
}

void GLWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.f, 0.f, 0.f, 1.f);

  model = trackBall.getRotationMatrix() * trackBall.getScalingMatrix() * model;

  glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(model));
  glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projLocation, 1, GL_FALSE, value_ptr(proj));
  glUniform3fv(lightPositionLocation, 1, value_ptr(lightPosition));
  glUniform3fv(lightColorLocation, 1, value_ptr(lightColor));
  glUniform1f(shininessLocation, shininess);

  // Wireframe starts disabled
  glUniform1i(wireLocation, 0);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, vertices.data());
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, normals.data());
  glEnableVertexAttribArray(1);

  // Drawing
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size()), GL_UNSIGNED_INT, faces.data());
  glEnable(GL_LIGHTING);

  if (wireframeMode) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform1i(wireLocation, 1); // wire on
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size()), GL_UNSIGNED_INT, faces.data());
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  update();
}

void GLWidget::initializeGL()
{
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  initializeShaders();
}

void GLWidget::reloadShaders()
{
  glUseProgram(0);
  glDeleteProgram(shaderProgram);
  initializeGL();
}

void GLWidget::initializeShaders()
{
  std::string vertexShaderSrc = readFile("../Shaders/shader.vert");
  std::string fragShaderSrc = readFile("../Shaders/shader.frag");

  vShader = createShader(GL_VERTEX_SHADER, vertexShaderSrc.c_str());
  fShader = createShader(GL_FRAGMENT_SHADER, fragShaderSrc.c_str());
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vShader);
  glAttachShader(shaderProgram, fShader);
  glLinkProgram(shaderProgram);

  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cout << "error： program link failed: " << infoLog << std::endl;
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);

  modelLocation = glGetUniformLocation(shaderProgram, "model");
  viewLocation = glGetUniformLocation(shaderProgram, "view");
  projLocation = glGetUniformLocation(shaderProgram, "projection");
  wireLocation = glGetUniformLocation(shaderProgram, "wireframe");

  glUseProgram(shaderProgram);
}

void GLWidget::loadMeshData(const Mesh &mesh)
{
  vertices.clear();
  faces.clear();
  vertices = mesh.getPositionsData();
  normals = mesh.getFaceNormalsData();
  faces = mesh.getFacesData();
  model = mat4(1.f);
  model = mesh.bbox.centering();
  update();
}

void GLWidget::unloadMeshData()
{
  vertices.clear();
  faces.clear();
  update();
}

void GLWidget::updateMeshData(const Mesh &mesh)
{
  vertices = mesh.getPositionsData();
  faces = mesh.getFacesData();
  update();
}

GLuint GLWidget::createShader(GLenum type, const GLchar *source)
{
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cout << "error： shader compilation failed: " << infoLog << source << std::endl;
  }
  return shader;
}

std::string GLWidget::readFile(const char *fileLocation) const
{
  std::string content;
  std::ifstream fileStream(fileLocation, std::ios::in);

  if (!fileStream.is_open()) {
    printf("Failed to read %s! File doesn't exist.", fileLocation);
    return "";
  }

  std::string line = "";

  while (!fileStream.eof()) {
    std::getline(fileStream, line);
    content.append(line + "\n");
  }

  fileStream.close();
  return content;
}

void GLWidget::wireframePaint()
{
  wireframeMode = !wireframeMode;
  update();
}
