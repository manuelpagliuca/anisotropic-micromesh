#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <string>

#include <QtOpenGL>

#include <GLM/glm.hpp>

class Shader : protected QOpenGLExtraFunctions
{
public:
    Shader()
    {
        id = -1;
        uniformModelMatrix = 0;
        uniformWireframeMode = 0;
    }
    Shader(const char *vertPath, const char *fragPath);
    ~Shader();

    void createFromFiles(const char *vertexFilePath, const char *fragFilePath);
    std::string readFile(const char *fileLocation) const;
    void useShader();
    void clearShader();

    unsigned int getModelLocation() const;
    unsigned int getWireLocation() const;

private:
    void compile(const char *vertexCode, const char *fragCode);
    void addShader(const unsigned int &program, const char *shaderCode, const GLenum &shaderType);

    unsigned int id;
    unsigned int uniformModelMatrix;
    unsigned int uniformWireframeMode;
};

#endif // SHADER_H
