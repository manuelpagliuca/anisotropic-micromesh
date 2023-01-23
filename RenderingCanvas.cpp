#include "RenderingCanvas.h"

#define INIT_MEM 0xDEADBEEF

using namespace std;

RenderingCanvas::RenderingCanvas(QWidget* parent) : QOpenGLWidget(parent)
{
	VAO = VBO = EBO = 0;
    shaderProgram = modelLocation = 0;
    model = glm::mat4(1.f);
}

void RenderingCanvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);  

    model = trackBall.getRotationMatrix() * trackBall.getScalingMatrix() * model;

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

    glUniform1i(wireLocation, 0); // wire off

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);

    // Drawing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    int numFaces = static_cast<int>(faces.size());
    glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_INT, 0);

    if (wireframeMode){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(wireLocation, 1); // wire on
        glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_INT, 0);
    }

    // Unbind
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    update();
}

void RenderingCanvas::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    initShaders();
    initOpenGLBuffers();
}

void RenderingCanvas::initShaders()
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

void RenderingCanvas::initOpenGLBuffers()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(INIT_MEM), nullptr, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INIT_MEM), nullptr, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderingCanvas::loadDataOnOpenGLBuffer()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * faces.size(), faces.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint RenderingCanvas::createShader(GLenum type, const GLchar* source)
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

std::string RenderingCanvas::readFile(const char* fileLocation) const
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

void RenderingCanvas::loadMeshData(const Mesh &mesh)
{
    vertices = mesh.getPositionsVector();
    faces = mesh.getFacesVector();
    loadDataOnOpenGLBuffer();
    model = glm::mat4(1.f);
    model = mesh.bbox.centering();
    update();
}

void RenderingCanvas::updateMeshData(const Mesh &mesh)
{
    vertices = mesh.getPositionsVector();
    faces = mesh.getFacesVector();
    loadDataOnOpenGLBuffer();
    update();
}

void RenderingCanvas::unloadMeshData()
{
    vertices.clear();
    faces.clear();
    loadDataOnOpenGLBuffer();
	update();
}

void RenderingCanvas::wireframePaint()
{
    wireframeMode = !wireframeMode;
    update();
}
