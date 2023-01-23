#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <QtOpenGL>
#include <GLM/vec3.hpp>
#include <GLM/gtx/string_cast.hpp>

#include "BoundingBox.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec3 tex;
};

struct Face {
	int index[3];
	glm::vec3 norm;
};

class Mesh : protected QOpenGLExtraFunctions
{
public:
	Mesh();
    BoundingBox bbox;
	std::vector<Vertex> vertices;
    std::vector<Face> faces;

    int addVertex(glm::vec3 pos);
    int addFace(int i0, int i1, int i2);

    std::vector<float> getPositionsVector() const;
    std::vector<int> getFacesVector() const;

	void draw(bool wireframe);
    void drawDirect();

	Mesh subdivide();
    void updateFaceNormals();
    void updateVertexNormals();
    void updateBoundingBox();
    void displaceVertices(float k);
    void displaceFace(float k);

    static Mesh parseOFF(const std::string& rawOFF);
    static Mesh parseOBJ(const std::string& rawOBJ);
    void exportOFF(const std::string& fileName) const;
    void exportOBJ(const std::string& fileName) const;

    void print() const;
};
