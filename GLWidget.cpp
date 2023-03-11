#include "GLWidget.h"

using namespace std;

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
	VAO = VBO = EBO = 0;
    shaderProgram = modelLocation = 0;
    model = glm::mat4(1.f);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);  

    model = trackBall.getRotationMatrix() * trackBall.getScalingMatrix() * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(wireLocation, 0); // wireframe starts disabled
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, vertices.data());
    glEnableVertexAttribArray(0);

    // Drawing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, faces.data());

    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(wireLocation, 1); // wire on
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, faces.data());
    }

    glDisableVertexAttribArray(0);

    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    initShaders();
}

void GLWidget::initShaders()
{
    std::string vertexShaderSrc = readFile("./shaders/shader.vert");
    std::string fragShaderSrc = readFile("./shaders/shader.frag");

    GLuint vert = createShader(GL_VERTEX_SHADER, vertexShaderSrc.c_str());
    GLuint frag = createShader(GL_FRAGMENT_SHADER, fragShaderSrc.c_str());
    GLuint prg = glCreateProgram();

    glAttachShader(prg, vert);
    glAttachShader(prg, frag);
    glLinkProgram(prg);

    GLint success;
    glGetProgramiv(prg, GL_LINK_STATUS, &success);

    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(prg, 512, nullptr, infoLog);
        std::cout << "error： program link failed: " << infoLog << std::endl;
    }
    shaderProgram = prg;

    glDeleteShader(vert);
    glDeleteShader(frag);

    modelLocation = glGetUniformLocation(shaderProgram, "model");
    wireLocation = glGetUniformLocation(shaderProgram, "wireframe");

    glUseProgram(shaderProgram);
}

void GLWidget::loadMeshData(const Mesh &mesh)
{
    vertices.clear();
    faces.clear();
    vertices = mesh.getPositionsVector();
    faces = mesh.getFacesVector();
    model = glm::mat4(1.f);
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
    vertices = mesh.getPositionsVector();
    faces = mesh.getFacesVector();
    update();
}

GLuint GLWidget::createShader(GLenum type, const GLchar* source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success){
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "error： shader compilation failed: " << infoLog << source << std::endl;
	}
	return shader;
}

std::string GLWidget::readFile(const char* fileLocation) const
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
