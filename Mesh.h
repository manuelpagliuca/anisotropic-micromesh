#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include <QtOpenGL>
#include <GLM/vec3.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <GLM/gtx/intersect.hpp>
#include <map>
#include <utility>
#include "Utility.h"

#include "Edge.h"
#include "Vertex.h"
#include "Face.h"
#include "BoundingBox.h"
#include "Line.h"
#include "Ray.h"

using namespace glm;

class Mesh : protected QOpenGLExtraFunctions
{
public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;

    BoundingBox bbox;

    float R;     // maximal extension on maxAxis (x,y,z) of the largest bbox of all faces
    int maxAxis; // position on maxAxis (x,y or z) of the center of the bbox of this face

    Mesh();
    ~Mesh();

    void updatePosMiddleAndR();

    // Vertex
    int addVertex(vec3 pos);
    void displaceVertex(int index, float k);
    void displaceVertices(float k);
    Vertex getSurfaceVertex(const Face &f, vec3 bary) const;

    // Face
    int addFace(int i0, int i1, int i2);
    void displaceFace(int index, float k);
    void displaceFaces(float k);
    uint getFaceSubdivisionLevel(int index) const;
    float getAvgFacesDoubleArea() const;

    // Edge
    int addEdge(int faceIndex0, int faceIndex1, int side0, int side1);
    float getAvgEdgeLength() const;
    void fixEdgesSubdivisionLevels();
    void setInitialEdgeSubdivisionLevels(float targetEdgeLength);

    // Get
    std::vector<float> getPositionsVector() const;
    std::vector<uint> getFacesVector() const;

    // Subdivision
    Mesh subdivide();
    Mesh subdivideNtimes(int n);
    Mesh micromeshSubdivide();
    Mesh anisotropicMicromeshSubdivide();

    // Subdivision scheme
    bool enforceMicromesh(const Face &f);
    bool enforceAnisotropicMicromesh(const Face &f);

    // Update
    void updateFaceNormals();
    void updateVertexNormals();
    void updateBoundingBox();
    void updateEdges();
    void updateEdgesSubdivisionLevelsMicromesh(float targetEdgeLength);
    void updateEdgesSubdivisionLevelsAniso(float targetEdgeLength);

    // Utils
    void removeDuplicatedVertices();
    float minimumDistance(const vec3 &origin, const vec3 &direction, Mesh &target);
    float minimumDistanceBruteForce(const vec3 &origin, const vec3 &direction, Mesh &target);
    std::vector<float> getDisplacements(Mesh &target);
    static Mesh parseOFF(const std::string &rawOFF);
    static Mesh parseOBJ(const std::string &rawOBJ);
    void exportOFF(const std::string &fileName, QString filePath = QString("")) const;
    void exportOBJ(const std::string &fileName, QString filePath = QString("")) const;
    void intersectTriangle(int fIndex, Line line, float &minDistance);

protected:
    void updateGL() {}
    void draw(bool wireframe);
    void drawDirect();

    // Test
private:
    void setInitialEdgeSubdivisionLevelsTest();
    void sanityCheckEdge();
    bool isMicromeshScheme() const;

public:
    bool isValid() const;

    // Debug
public:
    void print() const;
    void printEdgeSubdivisions() const;
    void printOpenEdges() const;
};

#endif MESH_H
