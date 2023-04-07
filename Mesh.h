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
    glm::vec2 tex;
};

struct Face {
    unsigned int index[3];
    unsigned int edges[3];
	glm::vec3 norm;
};

struct Edge {
    unsigned int faces[2];
    unsigned int side[2];
};

class Mesh : protected QOpenGLExtraFunctions
{
public:
	Mesh();
    ~Mesh();
    BoundingBox bbox;
	std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;

    int addVertex(glm::vec3 pos);
    int addFace(int i0, int i1, int i2);

    std::vector<float> getPositionsVector() const;
    std::vector<unsigned int> getFacesVector() const;
    std::map<int, int> getDoubleAreaSubdivisionMap() const;
    float getFacesAvgDoubleArea() const;
    std::vector<float> getDeviationsFromFacesAvgDoubleArea() const;

	void draw(bool wireframe);
    void drawDirect();

	Mesh subdivide();
    Mesh nSubdivide(int subdivision);
    Mesh adaptiveSubdivide(std::map<int, int> areaToSubdivisionMap);

    void updateFaceNormals();
    void updateVertexNormals();
    void updateFaceToFaceAdjacency();
    void updateBoundingBox();
    void displaceVertices(float k);
    void displaceVertex(int index, float k);
    void displaceFace(float k);

    std::vector<std::tuple<int, float>> displaceVerticesTowardsTargetMesh(const Mesh& targetMesh);

    static Mesh parseOFF(const std::string& rawOFF);
    static Mesh parseOBJ(const std::string& rawOBJ);
    void exportOFF(const std::string& fileName) const;
    void exportOBJ(const std::string& fileName) const;

    void updateGL();

    void print() const;
};
