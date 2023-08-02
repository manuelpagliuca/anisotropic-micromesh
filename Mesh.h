#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
#include "Ray.h"

using namespace glm;

class Mesh : protected QOpenGLExtraFunctions
{
public:
  Mesh();
  ~Mesh();

  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  std::vector<Edge> edges;
  BoundingBox bbox;

  int addVertex(vec3 pos);
  int addFace(int i0, int i1, int i2);
  int addEdge(int faceIndex0, int faceIndex1, int side0, int side1);

  void displaceVertices(float k);
  void displaceVertex(int index, float k);
  void displaceFaces(float k);

  // Data
  std::vector<float> getPositionsVector() const;
  std::vector<uint> getFacesVector() const;

  // Utils
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
  void updateEdgesSubdivisionLevels();

  void fixEdges();
  void sanityCheckEdge();

  void setInitialEdgeSubdivisionLevels();
  void setInitialEdgeSubdivisionLevelsTest();

  bool enforceMicromesh(const Face &f);
  bool enforceAnisotropicMicromesh(const Face &f);

  bool isMicromeshScheme() const;
  Vertex surfacePoint(const Face &f, vec3 bary) const;
  void removeDuplicatedVertices();

  float minimumDisplacement(const vec3 &origin, const vec3 &direction, const Mesh &target);
  std::vector<float> getDisplacements(const Mesh &target);

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
  void printOpenEdges() const;
};
