#ifndef MESH_H
#define MESH_H

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
private:
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  std::vector<Edge> edges;

public:
  BoundingBox bbox;

public:
  Mesh();
  ~Mesh();

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

private:
  // Test
  void setInitialEdgeSubdivisionLevelsTest();
  void sanityCheckEdge();
  bool isMicromeshScheme() const;

  // Debug
  void print() const;
  void printEdgeSubdivisions() const;
  void printOpenEdges() const;
};

#endif MESH_H
