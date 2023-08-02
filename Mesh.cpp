#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh()
{
  vertices.clear();
  faces.clear();
  edges.clear();
}

int Mesh::addVertex(vec3 pos)
{
  Vertex v;
  v.pos = pos;
  v.norm = vec3(0, 0, 0);
  vertices.push_back(v);
  return static_cast<int>(vertices.size() - 1);
}

int Mesh::addFace(int i0, int i1, int i2)
{
  Face f;
  f.index[0] = i0;
  f.index[1] = i1;
  f.index[2] = i2;
  faces.push_back(f);
  return static_cast<int>(faces.size() - 1);
}

int Mesh::addEdge(int faceIndex0, int faceIndex1, int side0, int side1)
{
  Edge e;
  e.faces[0] = faceIndex0;
  e.faces[1] = faceIndex1;
  e.side[0] = side0;
  e.side[1] = side1;
  edges.push_back(e);

  int edgeIndex = int(edges.size() - 1);

  if (faceIndex0 != -1) faces.at(faceIndex0).edgesIndices[side0] = edgeIndex;
  if (faceIndex1 != -1) faces.at(faceIndex1).edgesIndices[side1] = edgeIndex;

  return edgeIndex;
}

void Mesh::displaceVertices(float k)
{
  for (Vertex &v : vertices) v.pos = v.pos + (k * v.norm);
}

void Mesh::displaceVertex(int index, float k)
{
  vertices[index].pos = vertices[index].pos + (k * vertices[index].norm);
}

void Mesh::displaceFaces(float k)
{
  for (Face &f : faces) {
    Vertex &v0 = vertices[f.index[0]];
    Vertex &v2 = vertices[f.index[1]];
    Vertex &v1 = vertices[f.index[2]];

    v0.pos = v0.pos + (k * f.norm);
    v1.pos = v1.pos + (k * f.norm);
    v2.pos = v2.pos + (k * f.norm);
  }
}

std::vector<float> Mesh::getPositionsVector() const
{
  std::vector<float> pos;

  for (const Vertex &v : vertices) {
    pos.push_back(v.pos[0]);
    pos.push_back(v.pos[1]);
    pos.push_back(v.pos[2]);
  }

  return pos;
}

std::vector<uint> Mesh::getFacesVector() const
{
  std::vector<uint> indices;
  for (const Face &f : faces) {
    indices.push_back(f.index[0]);
    indices.push_back(f.index[1]);
    indices.push_back(f.index[2]);
  }

  return indices;
}

uint Mesh::getFaceSubdivisionLevel(int index) const
{
  uint e0SubdivisionLvl = edges.at(faces.at(index).edgesIndices[0]).subdivisions;
  uint e1SubdivisionLvl = edges.at(faces.at(index).edgesIndices[1]).subdivisions;
  uint e2SubdivisionLvl = edges.at(faces.at(index).edgesIndices[2]).subdivisions;

  return uint(maxInt3(e0SubdivisionLvl, e1SubdivisionLvl, e2SubdivisionLvl));
}

float Mesh::getAvgFacesDoubleArea() const
{
  float avgArea = 0.0f;

  for (const Face &f : faces) {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    vec3 ab = (vertices.at(v1).pos - vertices.at(v0).pos);
    vec3 bc = (vertices.at(v2).pos - vertices.at(v1).pos);

    float faceArea = length(cross(ab, bc));
    avgArea += faceArea;
  }

  avgArea /= faces.size();
  return avgArea;
}

float Mesh::getAvgEdgeLength() const
{
  float avgEdge = 0.0f;

  for (const Face &f : faces) {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    float l0 = length(vertices.at(v1).pos - vertices.at(v0).pos);
    float l1 = length(vertices.at(v2).pos - vertices.at(v1).pos);
    float l2 = length(vertices.at(v0).pos - vertices.at(v2).pos);

    avgEdge += (l0 + l1 + l2);
  }

  avgEdge /= faces.size() * 3;
  return avgEdge;
}

Mesh Mesh::subdivide()
{
  Mesh subdivided = Mesh();
  subdivided.vertices = this->vertices;

  for (const Face &f : faces) {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    int m01 = subdivided.addVertex((vertices.at(v0).pos + vertices.at(v1).pos) / 2.0f);
    int m12 = subdivided.addVertex((vertices.at(v1).pos + vertices.at(v2).pos) / 2.0f);
    int m20 = subdivided.addVertex((vertices.at(v2).pos + vertices.at(v0).pos) / 2.0f);

    subdivided.addFace(m01, m12, m20);
    subdivided.addFace(v0, m01, m20);
    subdivided.addFace(m01, v1, m12);
    subdivided.addFace(m20, m12, v2);
  }

  return subdivided;
}

Mesh Mesh::nSubdivide(int n)
{
  Mesh subdivided = Mesh();
  subdivided.vertices = this->vertices;

  for (const Face &f : faces) {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    vec3 delta0 = (vertices.at(v1).pos - vertices.at(v0).pos) / float(n);
    vec3 delta1 = (vertices.at(v1).pos - vertices.at(v2).pos) / float(n);
    vec3 delta2 = (vertices.at(v2).pos - vertices.at(v0).pos) / float(n);

    int stored[2];

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n - i; j++) {
        vec3 t1 = vertices.at(v0).pos + (float(i) * delta0) + (float(j) * delta2);
        vec3 t2 = t1 + delta2;
        vec3 t3 = t2 + delta1;

        int m1 = subdivided.addVertex(t1);
        int m2 = subdivided.addVertex(t2);
        int m3 = subdivided.addVertex(t3);

        subdivided.addFace(m1, m3, m2);

        if (j > 0) subdivided.addFace(stored[0], stored[1], m3);

        stored[0] = m2;
        stored[1] = m3;
      }
    }
  }

  return subdivided;
}

Mesh Mesh::micromeshSubdivide()
{
  Mesh subdivided = Mesh();
  fixEdges();

  auto toIndex = [&](int vx, int vy) { return vy * (vy + 1) / 2 + vx; };
  auto toIndexV = [&](ivec2 v) { return v.y * (v.y + 1) / 2 + v.x; };

  for (const Face& f: faces) {
    int subLvlEdge0 = edges[f.edgesIndices[0]].subdivisions;
    int subLvlEdge1 = edges[f.edgesIndices[1]].subdivisions;
    int subLvlEdge2 = edges[f.edgesIndices[2]].subdivisions;

    int subLvlMax = maxInt3(subLvlEdge0, subLvlEdge1, subLvlEdge2);
    int n = pow(2, subLvlMax);
    int k = int(subdivided.vertices.size());

    // add microvertices
    for (int vy = 0; vy <= n; vy++) {
      for (int vx = 0; vx <= vy; vx++) { // vx+vy < n
        float c = vx / float(n);
        float a = vy / float(n);
        vec3 bary = vec3((1 - a), (a - c), c);
        subdivided.vertices.push_back(surfacePoint(f, bary));
      }
    }

    // add microfaces
    for (int fy = 0; fy < n; fy++) {
      for (int fx = 0; fx < n; fx++) {
        ivec2 v0(fx, fy), v1(fx, fy + 1), v2(fx + 1, fy + 1);

        if (fx > fy) { // flip "red" triangle
          v0 = ivec2(n, n) - v0;
          v1 = ivec2(n, n) - v1;
          v2 = ivec2(n, n) - v2;
        }

        subdivided.addFace(k + toIndexV(v0), k + toIndexV(v1), k + toIndexV(v2));
      }
    }

    if (subLvlEdge0 < subLvlMax) {
      for (int vy = 1; vy < n; vy += 2) {
        int delta = (vy < n/2) ? -1 : +1;
        subdivided.vertices[k + toIndex(0, vy)] = subdivided.vertices[k + toIndex(0, vy + delta)];
      }
    }

    if (subLvlEdge1 < subLvlMax) {
      for (int vx = 1; vx < n; vx += 2) {
        int delta = (vx < n/2) ? -1 : +1;
        subdivided.vertices[k + toIndex(vx, n)] = subdivided.vertices[k + toIndex(vx + delta, n)];
      }
    }

    if (subLvlEdge2 < subLvlMax) {
      for (int vxy = 1; vxy < n; vxy += 2) {
        int delta = (vxy < n/2) ? +1 : -1;
        subdivided.vertices[k + toIndex(vxy, vxy)] = subdivided.vertices[k + toIndex(vxy + delta, vxy + delta)];
      }
    }
  }

  subdivided.updateBoundingBox();
  subdivided.updateEdges();
  subdivided.removeDuplicatedVertices();

  return subdivided;
}

Mesh Mesh::anisotropicMicromeshSubdivide()
{
  Mesh subdivided = Mesh();
  // to implement
  return *this;
}

// This function fixes the edges which are downscaled but should be higher scale
void Mesh::fixEdges()
{
  for (Edge &e: edges) {
    if (e.faces[0] == -1 || e.faces[1] == -1) continue;
    uint eMax0 = getFaceSubdivisionLevel(e.faces[0]);
    uint eMax1 = getFaceSubdivisionLevel(e.faces[1]);
    // if the edge is between two triangles N + 1 and it is N
    if (eMax0 == eMax1 && e.subdivisions + 1 == eMax0)
      e.subdivisions = eMax0;
  }
}

void Mesh::removeDuplicatedVertices()
{
  std::vector<Vertex> newVertices;
  std::vector<Face> newFaces;

  for (const Vertex &v : vertices)
    if (std::find(newVertices.begin(), newVertices.end(), v) == newVertices.end())
      newVertices.push_back(v);

  for (Face &f : faces) {
    auto it0 = std::find(newVertices.begin(), newVertices.end(), vertices.at(f.index[0]));
    auto it1 = std::find(newVertices.begin(), newVertices.end(), vertices.at(f.index[1]));
    auto it2 = std::find(newVertices.begin(), newVertices.end(), vertices.at(f.index[2]));

    uint newIndex0 = uint(std::distance(newVertices.begin(), it0));
    uint newIndex1 = uint(std::distance(newVertices.begin(), it1));
    uint newIndex2 = uint(std::distance(newVertices.begin(), it2));

    f.index[0] = newIndex0;
    f.index[1] = newIndex1;
    f.index[2] = newIndex2;
  }

  vertices = newVertices;
}

void Mesh::updateFaceNormals()
{
  for (Face &f : faces) {
    vec3 v0 = vertices[f.index[0]].pos;
    vec3 v1 = vertices[f.index[1]].pos;
    vec3 v2 = vertices[f.index[2]].pos;
    f.norm = normalize(cross(v1 - v0, v2 - v0));
  }
}

void Mesh::updateVertexNormals()
{
  for (Vertex &v : vertices) v.norm = vec3(0, 0, 0);

  for (Face &f : faces) {
    Vertex &v0 = vertices[f.index[0]];
    Vertex &v1 = vertices[f.index[1]];
    Vertex &v2 = vertices[f.index[2]];
    v0.norm += f.norm;
    v1.norm += f.norm;
    v2.norm += f.norm;
  }

  for (Vertex &v : vertices) v.norm = normalize(v.norm);
}

void Mesh::updateEdges()
{
  edges.clear();

  std::map<AvailableEdge, EdgeLocation> matingPool;
  uint faceIndex = 0;

  for (Face &f : faces) {
    for (int faceEdge = 0; faceEdge < 3; faceEdge++) {
      AvailableEdge edgeToCheck = AvailableEdge(f.index[faceEdge], f.index[(faceEdge + 1) % 3]);

      auto it = matingPool.find(edgeToCheck);

      if (it != matingPool.end()) {
        EdgeLocation matchingEdgeLocation = it->second;
        addEdge(faceIndex, matchingEdgeLocation.faceIndex, faceEdge, matchingEdgeLocation.sideIndex);
        matingPool.erase(it);
      } else {
        EdgeLocation newLocation = EdgeLocation(faceIndex, faceEdge);
        matingPool.insert({edgeToCheck.flip(), newLocation});
      }
    }
    faceIndex++;
  }

  for (auto &openEdge : matingPool) {
    EdgeLocation openEdgeLocation = openEdge.second;
    int edgeIndex = addEdge(openEdgeLocation.faceIndex, -1, openEdgeLocation.sideIndex, -1);
    faces.at(openEdgeLocation.faceIndex).edgesIndices[openEdgeLocation.sideIndex] = edgeIndex;
  }
}

void Mesh::setInitialEdgeSubdivisionLevels()
{
  for (const Face &f : faces) {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    float l0 = length(vertices.at(v1).pos - vertices.at(v0).pos);
    float l1 = length(vertices.at(v2).pos - vertices.at(v1).pos);
    float l2 = length(vertices.at(v0).pos - vertices.at(v2).pos);

    edges.at(f.edgesIndices[0]).subdivisions = nearestRoundPow2(l0);
    edges.at(f.edgesIndices[1]).subdivisions = nearestRoundPow2(l1);
    edges.at(f.edgesIndices[2]).subdivisions = nearestRoundPow2(l2);
    }
}

void Mesh::setInitialEdgeSubdivisionLevelsTest()
{
  for (Edge &e : edges) e.subdivisions = 0;
  edges.at(0).subdivisions = 6;
}

void Mesh::updateEdgesSubdivisionLevels()
{
  setInitialEdgeSubdivisionLevels();

  int count = 0;

  while (true) {
    bool changeAnything = false;

    for (Face &f : faces) {
      changeAnything |= enforceMicromesh(f);
    }

    count++;
    if (!changeAnything) break;
  }
  qDebug() << "Micromesh scheme enforced: " << count << " times.";
}

void Mesh::sanityCheckEdge()
{
  for (int edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
    Edge e = edges[edgeIndex];

    for (int edgeSide = 0; edgeSide < 2; edgeSide++) {
      int faceIndex = e.faces[edgeSide];
      int faceSide = e.side[edgeSide];
      assert(faces[faceIndex].edgesIndices[faceSide] == edgeIndex);
    }
  }

  for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
    Face f = faces[faceIndex];

    for (int faceEdge = 0; faceEdge < 3; faceEdge++) {
      int edgeIndex = f.edgesIndices[faceEdge];
      Edge e = edges[edgeIndex];
      assert(e.faces[0] == faceIndex || e.faces[1] == faceIndex);
    }
  }
}

void Mesh::updateBoundingBox()
{
  bbox.init(vertices[0].pos);
  for (Vertex &v : vertices) bbox.includeAnotherPoint(v.pos);
}

bool Mesh::enforceMicromesh(const Face &f)
{
  bool changeAnything = false;
  uint edgeSubdivision[3] = {
    edges.at(f.edgesIndices[0]).subdivisions,
    edges.at(f.edgesIndices[1]).subdivisions,
    edges.at(f.edgesIndices[2]).subdivisions
  };

  uint max = uint(maxInt3(edgeSubdivision[0], edgeSubdivision[1], edgeSubdivision[2]));

  for (uint &eSub : edgeSubdivision) {
    if (eSub < (max - 1) && (max > 0)) {
      eSub = max - 1;
      changeAnything = true;
    }
  }

  edges[f.edgesIndices[0]].subdivisions = edgeSubdivision[0];
  edges[f.edgesIndices[1]].subdivisions = edgeSubdivision[1];
  edges[f.edgesIndices[2]].subdivisions = edgeSubdivision[2];

  return changeAnything;
}

bool Mesh::enforceAnisotropicMicromesh(const Face &f)
{
  uint edgeSubdivisions[3] = {
    edges.at(f.edgesIndices[0]).subdivisions,
    edges.at(f.edgesIndices[1]).subdivisions,
    edges.at(f.edgesIndices[2]).subdivisions
  };

  bool changeAnything = false;

  uint max = uint(maxInt3(edgeSubdivisions[0], edgeSubdivisions[1], edgeSubdivisions[2]));
  int maxCount = 0;

  int edgeSubdivisionsLowerThenMax[2] = {0, 0};
  int i = 0;

  for (uint e : edgeSubdivisions) {
    if (e == max) maxCount++;
    else if (e < max) edgeSubdivisionsLowerThenMax[i++] = e;
  }

  if (maxCount > 1) return changeAnything;

  int maxEdgeMinor = maxInt2(edgeSubdivisionsLowerThenMax[0], edgeSubdivisionsLowerThenMax[1]);

  for (uint &e : edgeSubdivisions) {
    if (e == maxEdgeMinor) {
      e = max;
      changeAnything = true;
      break;
    }
  }

  edges[f.edgesIndices[0]].subdivisions = edgeSubdivisions[0];
  edges[f.edgesIndices[1]].subdivisions = edgeSubdivisions[1];
  edges[f.edgesIndices[2]].subdivisions = edgeSubdivisions[2];

  return changeAnything;
}

bool Mesh::isMicromeshScheme() const
{
  int correctFaces = 0;

  for (const Face &f : faces) {
    int i = edges.at(f.edgesIndices[0]).subdivisions;
    int j = edges.at(f.edgesIndices[0]).subdivisions;
    int k = edges.at(f.edgesIndices[0]).subdivisions;

    int totalDelta = std::abs(i - j) + std::abs(j - k) + std::abs(i - k);

    if (totalDelta <= 2) correctFaces++;
  }

  return correctFaces == faces.size();
}

Vertex Mesh::surfacePoint(const Face &f, vec3 bary) const
{
  int v0 = f.index[0];
  int v1 = f.index[1];
  int v2 = f.index[2];

  Vertex v;
  v.pos = vertices[v0].pos * bary[0] +
          vertices[v1].pos * bary[1] +
          vertices[v2].pos * bary[2];

  v.norm = vertices[v0].norm * bary[0] +
           vertices[v1].norm * bary[1] +
           vertices[v2].norm * bary[2];

  return v;
}

float Mesh::minimumDisplacement(const vec3 &origin, const vec3 &direction, const Mesh &target)
{
  float minDisp = INF;

  for (const Face &f : target.faces) {
    vec3 v0 = target.vertices[f.index[0]].pos;
    vec3 v1 = target.vertices[f.index[1]].pos;
    vec3 v2 = target.vertices[f.index[2]].pos;

    Ray forwardRay = Ray(origin, direction);
    Ray backwardRay = Ray(origin, -direction);

    float forwardDisp, backDisp;

    bool forwardIntersect = forwardRay.intersectTriangle(v0, v1, v2, forwardDisp);
    bool backwardIntersect = backwardRay.intersectTriangle(v0, v1, v2, backDisp);

    if (forwardIntersect && backwardIntersect) {
      float smallerDisp = (abs(backDisp) < abs(forwardDisp)) ? backDisp : forwardDisp;
      minDisp = (abs(minDisp) < abs(smallerDisp)) ? minDisp : smallerDisp;
    } else if (forwardIntersect) {
      minDisp = min(minDisp, forwardDisp);
    } else if (backwardIntersect) {
      minDisp = (abs(minDisp) < abs(backDisp)) ? minDisp : backDisp;
    }
  }

  return minDisp;
}

std::vector<float> Mesh::getDisplacements(const Mesh &target)
{
  std::vector<float> displacements;

  for (const Vertex &v : vertices)
    displacements.push_back(minimumDisplacement(v.pos, v.norm, target));

  return displacements;
}

void Mesh::exportOFF(const std::string &fileName) const
{
  std::ostringstream oss;
  std::string fileNameExt = fileName + ".off";
  std::ofstream fileStream(".\\mesh\\" + fileNameExt, std::ios::out);

  if (!fileStream.is_open()) {
    printf("Failed to open \'%s\'. File doesn't exist.", fileNameExt.c_str());
    return;
  }

  fileStream
      << "OFF"
      << "\n"
      << vertices.size() << " "
      << faces.size() << " "
      << 0 << std::endl; // todo: atm the # of edges is 0

  for (const Vertex &v : vertices)
    fileStream
        << v.pos.x << " "
        << v.pos.y << " "
        << v.pos.z << std::endl;

  for (const Face &f : faces)
    fileStream
        << 3
        << " " << f.index[0]
        << " " << f.index[1]
        << " " << f.index[2] << std::endl;

  fileStream.close();
}

void Mesh::exportOBJ(const std::string &fName) const
{
  std::ostringstream oss;
  std::string fileNameExt = fName + ".obj";
  std::ofstream fileStream(".\\mesh\\" + fileNameExt, std::ios::out);

  if (!fileStream.is_open()) {
    printf("Failed to open \'%s\'. File doesn't exist.", fileNameExt.c_str());
    return;
  }

  for (const Vertex &v : vertices) {
    fileStream << std::setprecision(6) << std::fixed
               << "vn " << v.norm.x
               << " " << v.norm.y
               << " " << v.norm.z << std::endl;
    fileStream << std::setprecision(6) << std::fixed
               << "v " << v.pos.x
               << " " << v.pos.y
               << " " << v.pos.z << std::endl;
  }

  for (const Face &f : faces)
    fileStream << "f "
               << f.index[0] + 1 << "//" << f.index[0] + 1 << " "
               << f.index[1] + 1 << "//" << f.index[1] + 1 << " "
               << f.index[2] + 1 << "//" << f.index[2] + 1 << std::endl;

  fileStream.close();
}

void Mesh::draw(bool wireframe)
{
  initializeOpenGLFunctions();
  glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
  uint numFaces = static_cast<uint>(getFacesVector().size());
  glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_INT, 0);
}

void Mesh::drawDirect()
{
  initializeOpenGLFunctions();
  glBegin(GL_TRIANGLES);

  for (const Face &f : faces) {
    vec3 v0 = vertices[f.index[0]].pos;
    vec3 v1 = vertices[f.index[1]].pos;
    vec3 v2 = vertices[f.index[2]].pos;
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
  }

  glEnd();
}

void Mesh::print() const
{
  std::cout << vertices.size() << " " << faces.size() << "\n";
  for (const Vertex &v : vertices) std::cout << to_string(v.pos) << "\n";

  std::cout << std::endl;
  for (const Face &f : faces) std::cout << f.index[0] << f.index[1] << f.index[2] << "\n";

  std::cout << std::endl;
  for (const Edge &e : edges) std::cout << e.faces[0] << e.faces[1] << "\n";
}

void Mesh::printEdgeSubdivisions() const
{
  for (const Face &f : faces) {
    qDebug()
      << "f(" << f.index[0] << " - " << f.index[1] << " - " << f.index[2] << "), s("
      << edges.at(f.index[0]).subdivisions << "/"
      << edges.at(f.index[1]).subdivisions << "/"
      << edges.at(f.index[2]).subdivisions << ")";
    }
}

void Mesh::printOpenEdges() const
{
  uint nOpenEdges = 0;
  for (const Edge &e: edges) {
    if (e.side[1] == -1) {
      Vertex v0 = vertices.at(faces.at(e.faces[0]).index[0]);
      Vertex v1 = vertices.at(faces.at(e.faces[0]).index[1]);
      Vertex v2 = vertices.at(faces.at(e.faces[0]).index[2]);

      PRINT_VECTOR(v0.pos);
      PRINT_VECTOR(v1.pos);
      PRINT_VECTOR(v2.pos);

      nOpenEdges++;
    }
  }
  qDebug() << "In total there are " << nOpenEdges << " open edges.";
}
