#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <QtOpenGL>
#include <GLM/vec3.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <map>
#include <utility>
#include "Utility.h"
#include "BoundingBox.h"

using namespace glm;

struct Vertex
{
  vec3 pos;
  vec3 norm;
  vec2 tex;
};

struct Face
{
  uint index[3];
  uint edges[3]; // 0, 1 and 2
  vec3 norm;
};

struct Edge
{
  uint faces[2];
  uint side[2]; // 0 and 1 (l and r)
  uint subdivisions = 0;
};

struct AvailableEdge
{
  uint startVertexIndex;
  uint endVertexIndex;

  AvailableEdge(uint a, uint b) : startVertexIndex(a), endVertexIndex(b) {}

  bool operator==(const AvailableEdge &o) const
  {
    return startVertexIndex == o.startVertexIndex && endVertexIndex == o.endVertexIndex;
  }

  bool operator<(const AvailableEdge &o) const
  {
    return startVertexIndex < o.startVertexIndex || (startVertexIndex == o.startVertexIndex && endVertexIndex < o.endVertexIndex);
  }

  bool operator>(const AvailableEdge &o) const
  {
    return startVertexIndex > o.startVertexIndex || (startVertexIndex == o.startVertexIndex && endVertexIndex > o.endVertexIndex);
  }

  AvailableEdge flip() const { return AvailableEdge(endVertexIndex, startVertexIndex); }
};

struct EdgeLocation
{
  uint faceIndex;
  uint sideIndex;

  EdgeLocation(uint a, uint b) : faceIndex(a), sideIndex(b) {}
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

  int addVertex(vec3 pos);
  int addFace(int i0, int i1, int i2);
  int addEdge(int faceIndex0, int faceIndex1, int side0, int side1);

  void displaceVertices(float k);
  void displaceVertex(int index, float k);
  void displaceFaces(float k);

  std::vector<float> getPositionsVector() const;
  std::vector<uint> getFacesVector() const;
  uint getFaceSubdivisionLevel(int index) const;

  float getAvgFacesDoubleArea() const;
  float getAvgEdgeLength() const;

  Mesh subdivide();
  Mesh nSubdivide(int subdivision);
  Mesh micromeshSubdivide();
  Mesh anisotropicMicromeshSubdivide();

  void updateFaceNormals();
  void updateVertexNormals();
  void updateBoundingBox();
  void updateEdges();
  void setInitialEdgeSubdivisionLevels();
  void updateEdgesSubdivisionLevels();
  void sanityCheckEdge();

  bool enforceMicromesh(const Face &f);
  bool enforceAnisotropicMicromesh(const Face &f);

  bool isMicromeshScheme() const;
  Vertex surfacePoint(const Face &f, vec3 bary) const;
  void fixEdges();

  std::vector<std::tuple<int, float>> displaceVerticesTowardsTargetMesh(const Mesh &targetMesh);

  static Mesh parseOFF(const std::string &rawOFF);
  static Mesh parseOBJ(const std::string &rawOBJ);
  void exportOFF(const std::string &fileName) const;
  void exportOBJ(const std::string &fileName) const;

protected:
  void updateGL() {}
  void draw(bool wireframe);
  void drawDirect();

  void print() const;
  void printEdgeSubdivisions() const;
};
