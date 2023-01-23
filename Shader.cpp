#include "Shader.h"

Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath)
{
	initializeOpenGLFunctions();
    createFromFiles(vertexFilePath, fragmentFilePath);
}

void Shader::createFromFiles(const char* vertexFilePath, const char* fragmentFilePath)
{
    std::string vertexCode = readFile(vertexFilePath);
    std::string fragmentCode = readFile(fragmentFilePath);
    compile(vertexCode.c_str(), fragmentCode.c_str());
}

std::string Shader::readFile(const char* filePath) const
{
	std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File doesn't exist.", filePath);
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

void Shader::useShader()
{
	initializeOpenGLFunctions();
	glUseProgram(id);
}

void Shader::clearShader()
{
    glDeleteProgram(id);
    id = 0;
    uniformModelMatrix = 0;
    uniformWireframeMode = 0;
}

void Shader::compile(const char* vertexCode, const char* fragmentCode)
{
	id = glCreateProgram();

    if (!id) {
		std::cerr << "Error creating shader program!" << std::endl;
		return;
	}

    addShader(id, vertexCode, GL_VERTEX_SHADER);
    addShader(id, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
    GLchar eLog[1024] = { 0 };

	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(id, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(id, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
		return;
	}

    uniformModelMatrix = glGetUniformLocation(id, "model");
    uniformWireframeMode = glGetUniformLocation(id, "wireframe");
}

void Shader::addShader(const unsigned int& program, const char* shaderCode, const GLenum& shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    int codeLength = static_cast<int>(strlen(shaderCode));

    glShaderSource(shader, 1, &shaderCode, &codeLength);
	glCompileShader(shader);

	GLint result = 0;
    GLchar eLog[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
        std::cerr << "Error compiling the " << shaderType << "shader: " << eLog << "\n";
		return;
	}

	glAttachShader(program, shader);
}

unsigned int Shader::getModelLocation() const
{
    return uniformModelMatrix;
}

unsigned int Shader::getWireLocation() const
{
    return uniformWireframeMode;
}

Shader::~Shader()
{
	clearShader();
}
