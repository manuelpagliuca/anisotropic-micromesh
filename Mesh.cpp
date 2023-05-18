#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh()
{
  vertices.clear();
  faces.clear();
  edges.clear();
}

int Mesh::addVertex(glm::vec3 pos)
{
  Vertex v;
  v.pos = pos;
  v.norm = glm::vec3(0, 0, 0);

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

int Mesh::addEdge(int i0, int i1, int s0, int s1)
{
  Edge e;
  e.faces[0] = i0;
  e.faces[1] = i1;
  e.side[0] = s0;
  e.side[1] = s1;

  edges.push_back(e);

  return static_cast<int>(edges.size() - 1);
}

void Mesh::print() const
{
  std::cout << vertices.size() << " " << faces.size() << "\n";
  for (const Vertex &v : vertices) std::cout << glm::to_string(v.pos) << "\n";

  std::cout << std::endl;
  for (const Face &f : faces) std::cout << f.index[0] << f.index[1] << f.index[2] << "\n";

  std::cout << std::endl;
  for (const Edge &e : edges) std::cout << e.faces[0] << e.faces[1] << "\n";
}

void Mesh::printEdgeSubdivisions() const
{
  for (const Face &f : faces)
  {
    qDebug()
      << edges.at(f.index[0]).subdivisions << "/"
      << edges.at(f.index[1]).subdivisions << "/"
      << edges.at(f.index[2]).subdivisions;
  }
}

Mesh Mesh::subdivide()
{
  Mesh subdivided = Mesh();
  subdivided.vertices = this->vertices;

  for (const Face &f : faces)
  {
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

Vertex Mesh::surfacePoint(const Face &f, glm::vec3 bary) const
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

Mesh Mesh::adaptiveSubdivide()
{
  Mesh subdivided = Mesh();

  for (const Face& f: faces) {
    int n = 1 << maxInt3(edges[f.edges[0]].subdivisions, edges[f.edges[1]].subdivisions, edges[f.edges[2]].subdivisions);
    int k = subdivided.vertices.size();

    // add microvertices
    for (int vy = 0; vy <= n; vy++) {
      for (int vx = 0; vx <= vy; vx++) { // vx+vy < n
        float c = vx / float(n);
        float a = vy / float(n);

        glm::vec3 bary = glm::vec3((1-a), (a-c), c);

        subdivided.vertices.push_back(surfacePoint(f, bary));
      }
    }

    // add microfaces
    for (int fy = 0; fy < n; fy++) {
      for (int fx = 0; fx < n; fx++) {
        glm::ivec2 v0(fx, fy), v1(fx, fy + 1), v2(fx + 1, fy + 1);

        if (fx > fy) { // flip "red" triangle
          v0 = glm::ivec2(n, n) - v0;
          v1 = glm::ivec2(n, n) - v1;
          v2 = glm::ivec2(n, n) - v2;
        }

        subdivided.addFace(
          k + v0.y * (v0.y + 1) / 2 + v0.x,
          k + v1.y * (v1.y + 1) / 2 + v1.x,
          k + v2.y * (v2.y + 1) / 2 + v2.x
        );
      }
    }
  }

  return subdivided;
}

Mesh Mesh::micromeshSubdivide()
{
  Mesh subdivided = Mesh();
  qDebug() << "To implement";
  return subdivided;
}

Mesh Mesh::nSubdivide(int n)
{
  Mesh subdivided = Mesh();
  subdivided.vertices = this->vertices;

  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    glm::vec3 delta0 = (vertices.at(v1).pos - vertices.at(v0).pos) / float(n);
    glm::vec3 delta1 = (vertices.at(v1).pos - vertices.at(v2).pos) / float(n);
    glm::vec3 delta2 = (vertices.at(v2).pos - vertices.at(v0).pos) / float(n);

    int stored[2];

    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n - i; j++)
      {
        glm::vec3 t1 = vertices.at(v0).pos + (float(i) * delta0) + (float(j) * delta2);
        glm::vec3 t2 = t1 + delta2;
        glm::vec3 t3 = t2 + delta1;

        int m1 = subdivided.addVertex(t1);
        int m2 = subdivided.addVertex(t2);
        int m3 = subdivided.addVertex(t3);

        subdivided.addFace(m1, m3, m2);
        if (j > 0)
          subdivided.addFace(stored[0], stored[1], m3);

        stored[0] = m2;
        stored[1] = m3;
      }
    }
  }

  return subdivided;
}



std::map<int, int> Mesh::getDoubleAreaToSubdivisionLevelMap() const
{
  float avgFaceDoubleArea = getAvgFacesDoubleArea();
  std::vector<float> facesDeviations = getDeviationsFromAvgFaceDoubleArea();

  float facesStdDeviation = 0.0f;
  for (float deviations : facesDeviations)
    facesStdDeviation += deviations;
  facesStdDeviation /= facesDeviations.size();

  int subdivisionInterval = static_cast<int>(std::round(facesStdDeviation));

  float maxDisplacementFromAvg = *std::max_element(facesDeviations.begin(), facesDeviations.end());

  int minSubdivisionLevel = 0;
  int maxSubdivisionLevel = static_cast<int>(std::round(maxDisplacementFromAvg));
  int avgSubdivisionLevel = static_cast<int>(std::round(avgFaceDoubleArea));

  std::map<int, int> areaToSubdivisionMap;

  for (int level = avgSubdivisionLevel - subdivisionInterval; level >= minSubdivisionLevel; level--)
    areaToSubdivisionMap[level] = level + subdivisionInterval;

  areaToSubdivisionMap[avgSubdivisionLevel] = avgSubdivisionLevel + 1;

  for (int level = avgSubdivisionLevel + subdivisionInterval; level <= maxSubdivisionLevel; level++)
    areaToSubdivisionMap[level] = level + subdivisionInterval;

  return areaToSubdivisionMap;
}

std::map<int, int> Mesh::getEdgeLengthToSubdivisionLevelMap() const
{
  float avgEdgeLength = getAvgEdgeLength();
  std::vector<float> edgesAvgLengthDeviations = getDeviationsFromAvgEdge();

  float edgeAvgLengthStdDeviation = 0.0f;
  for (float deviations : edgesAvgLengthDeviations)
  {
    edgeAvgLengthStdDeviation += deviations;
  }

  edgeAvgLengthStdDeviation /= edgesAvgLengthDeviations.size();

  int subdivisionInterval = static_cast<int>(std::round(edgeAvgLengthStdDeviation));

  float maxDisplacementFromAvg = *std::max_element(edgesAvgLengthDeviations.begin(), edgesAvgLengthDeviations.end());

  int minSubdivisionLevel = 0;
  int maxSubdivisionLevel = static_cast<int>(std::round(maxDisplacementFromAvg)) * 3;
  int avgSubdivisionLevel = static_cast<int>(std::round(avgEdgeLength));

  std::map<int, int> edgeLengthToSubdivisionMap;

  for (int level = avgSubdivisionLevel - subdivisionInterval; level >= minSubdivisionLevel; level--)
    edgeLengthToSubdivisionMap[level] = level + subdivisionInterval;

  edgeLengthToSubdivisionMap[avgSubdivisionLevel] = avgSubdivisionLevel + 1;

  for (int level = avgSubdivisionLevel + subdivisionInterval; level <= maxSubdivisionLevel; level++)
    edgeLengthToSubdivisionMap[level] = level + subdivisionInterval;

  return edgeLengthToSubdivisionMap;
}

float Mesh::getAvgFacesDoubleArea() const
{
  float avgArea = 0.0f;

  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    glm::vec3 ab = (vertices.at(v1).pos - vertices.at(v0).pos);
    glm::vec3 bc = (vertices.at(v2).pos - vertices.at(v1).pos);

    float faceArea = glm::length(glm::cross(ab, bc));
    avgArea += faceArea;
  }

  avgArea /= faces.size();
  return avgArea;
}

float Mesh::getAvgEdgeLength() const
{
  float avgEdge = 0.0f;

  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    float l0 = glm::length(vertices.at(v1).pos - vertices.at(v0).pos);
    float l1 = glm::length(vertices.at(v2).pos - vertices.at(v1).pos);
    float l2 = glm::length(vertices.at(v0).pos - vertices.at(v2).pos);

    avgEdge += (l0 + l1 + l2);
  }

  avgEdge /= faces.size() * 3;
  return avgEdge;
}

std::vector<float> Mesh::getDeviationsFromAvgFaceDoubleArea() const
{
  float avgFaceDoubleArea = getAvgFacesDoubleArea();
  std::vector<float> deviations;
  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    glm::vec3 v01 = (vertices.at(v1).pos - vertices.at(v0).pos);
    glm::vec3 v12 = (vertices.at(v2).pos - vertices.at(v1).pos);
    float faceDoubleArea = glm::length(glm::cross(v01, v12));
    deviations.push_back(glm::abs(avgFaceDoubleArea - faceDoubleArea));
  }

  return deviations;
}

std::vector<float> Mesh::getDeviationsFromAvgEdge() const
{
  float avgEdgeLength = getAvgEdgeLength();
  std::vector<float> deviations;

  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    float l0 = glm::length(vertices.at(v1).pos - vertices.at(v0).pos);
    float l1 = glm::length(vertices.at(v2).pos - vertices.at(v1).pos);
    float l2 = glm::length(vertices.at(v0).pos - vertices.at(v2).pos);

    float avgFaceEdge = (l0 + l1 + l2) / 3;

    deviations.push_back(glm::abs(avgEdgeLength - avgFaceEdge));
  }

  return deviations;
}

void Mesh::exportOFF(const std::string &fileName) const
{
  std::ostringstream oss;
  std::string fileNameExt = fileName + ".off";
  std::ofstream fileStream(".\\mesh\\" + fileNameExt, std::ios::out);

  if (!fileStream.is_open())
  {
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

  if (!fileStream.is_open())
  {
    printf("Failed to open \'%s\'. File doesn't exist.", fileNameExt.c_str());
    return;
  }

  for (const Vertex &v : vertices)
  {
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

void Mesh::updateGL() {}

void Mesh::updateFaceNormals()
{
  for (Face &f : faces)
  {
    glm::vec3 v0 = vertices[f.index[0]].pos;
    glm::vec3 v1 = vertices[f.index[1]].pos;
    glm::vec3 v2 = vertices[f.index[2]].pos;
    f.norm = glm::normalize(glm::cross(v1 - v0, v2 - v0));
  }
}

void Mesh::updateVertexNormals()
{
  for (Vertex &v : vertices)
    v.norm = glm::vec3(0, 0, 0);

  for (Face &f : faces)
  {
    Vertex &v0 = vertices[f.index[0]];
    Vertex &v1 = vertices[f.index[1]];
    Vertex &v2 = vertices[f.index[2]];
    v0.norm += f.norm;
    v1.norm += f.norm;
    v2.norm += f.norm;
  }

  for (Vertex &v : vertices) v.norm = glm::normalize(v.norm);
}

void Mesh::updateEdges()
{
  struct AvailableEdge
  {
    unsigned int startVertexIndex;
    unsigned int endVertexIndex;

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
  };

  struct EdgeLocalLocation
  {
    unsigned int faceIndex;
    unsigned int edgeId;
  };

  std::map<AvailableEdge, EdgeLocalLocation> matingPool;

  unsigned int faceIndex = 0;

  for (Face &f : faces)
  {
    for (int w = 0; w < 3; w++)
    {
      AvailableEdge edgeToCheck = AvailableEdge();
      edgeToCheck.startVertexIndex = f.index[(w + 1) % 3];
      edgeToCheck.endVertexIndex = f.index[w];

      auto it = matingPool.find(edgeToCheck);

      if (it != matingPool.end())
      {
        EdgeLocalLocation matchingEdgeLocation = it->second;
        int edgeIndex = addEdge(faceIndex, matchingEdgeLocation.faceIndex, w, matchingEdgeLocation.edgeId);
        f.edges[w] = edgeIndex;
        matingPool.erase(it);
      }
      else
      {
        AvailableEdge availableEdge = AvailableEdge();
        availableEdge.startVertexIndex = f.index[w];
        availableEdge.endVertexIndex = f.index[(w + 1) % 3];

        EdgeLocalLocation edgeLocation = EdgeLocalLocation();
        edgeLocation.faceIndex = faceIndex;
        edgeLocation.edgeId = w;

        matingPool.insert({availableEdge, edgeLocation});
      }
    }
    faceIndex++;
  }

  for (auto &openEdge : matingPool)
  {
    EdgeLocalLocation openEdgeLocation = openEdge.second;
    int edgeIndex = addEdge(openEdgeLocation.faceIndex, -1, openEdgeLocation.edgeId, -1);
    faces.at(openEdgeLocation.faceIndex).edges[openEdgeLocation.edgeId] = edgeIndex;
  }
}

void Mesh::setInitialEdgeSubdivisions()
{
  for (const Face &f : faces)
  {
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    float l0 = glm::length(vertices.at(v1).pos - vertices.at(v0).pos);
    float l1 = glm::length(vertices.at(v2).pos - vertices.at(v1).pos);
    float l2 = glm::length(vertices.at(v0).pos - vertices.at(v2).pos);

    edges.at(f.edges[0]).subdivisions = nearestRoundPow2(l0);
    edges.at(f.edges[1]).subdivisions = nearestRoundPow2(l1);
    edges.at(f.edges[2]).subdivisions = nearestRoundPow2(l2);
  }
}

void Mesh::updateEdgesSubdivisions()
{
  setInitialEdgeSubdivisions();

  while (true) {
    bool changeAnything = false;

    for (Face &f : faces) {
      changeAnything |= enforceMicromesh(f);
    }

    if (!changeAnything) break;
  }
}

void Mesh::updateBoundingBox()
{
  bbox.init(vertices[0].pos);
  for (Vertex &v : vertices) bbox.includeAnotherPoint(v.pos);
}

void Mesh::displaceVertices(float k)
{
  for (Vertex &v : vertices)
    v.pos = v.pos + (k * v.norm);
}

void Mesh::displaceVertex(int index, float k)
{
  vertices[index].pos = vertices[index].pos + (k * vertices[index].norm);
}

void Mesh::displaceFace(float k)
{
  for (Face &f : faces)
  {
    Vertex &v0 = vertices[f.index[0]];
    Vertex &v2 = vertices[f.index[1]];
    Vertex &v1 = vertices[f.index[2]];

    v0.pos = v0.pos + (k * f.norm);
    v1.pos = v1.pos + (k * f.norm);
    v2.pos = v2.pos + (k * f.norm);
  }
}

bool Mesh::isMicromeshScheme() const
{
  int correctFaces = 0;
  for (const Face &f : faces) {
    int i = edges.at(f.edges[0]).subdivisions;
    int j = edges.at(f.edges[0]).subdivisions;
    int k = edges.at(f.edges[0]).subdivisions;

    int totalDelta = std::abs(i - j) + std::abs(j - k) + std::abs(i - k);

    if (totalDelta <= 2) {
      correctFaces++;
    }
  }

  return correctFaces == faces.size();
}

std::vector<std::tuple<int, float>> Mesh::displaceVerticesTowardsTargetMesh(const Mesh &targetMesh)
{
  int index = 0;
  std::vector<std::tuple<int, float>> displacements;

  for (const Vertex &v : vertices)
  {
    glm::vec3 rayOrigin = v.pos;
    glm::vec3 rayDirection = glm::normalize(v.norm);
    float tMin = FLT_MAX;

    for (const Face &f : targetMesh.faces)
    {
      // Triangle processing
      glm::vec3 v0 = targetMesh.vertices[f.index[0]].pos;
      glm::vec3 v1 = targetMesh.vertices[f.index[1]].pos;
      glm::vec3 v2 = targetMesh.vertices[f.index[2]].pos;
      glm::vec3 v01 = v0 - v1;
      glm::vec3 v02 = v0 - v2;
      glm::vec3 planeNormal = glm::normalize(glm::cross(v01, v02));

      // Use v0 for finding the 'd' parameter of the supporting plane
      float d = glm::dot(planeNormal, v0);
      // finding 't' parameter of ray
      float t = (glm::dot(planeNormal, rayOrigin) + d) / glm::dot(planeNormal, rayDirection);
      // finding the point on the ray
      glm::vec3 P = rayOrigin + rayDirection * t;
      // vector vertices acting as barycentric coordinates for highlighting the point
      float v0P = glm::dot(glm::cross(v1 - v0, P - v1), rayDirection);
      float v1P = glm::dot(glm::cross(v2 - v1, P - v1), rayDirection);
      float v2P = glm::dot(glm::cross(v0 - v2, P - v2), rayDirection);

      if (!(v0P < 0 && v1P < 0 && v2P < 0) && abs(tMin) > abs(t))
      {
        tMin = t;
      }
    }
    displacements.push_back(std::tuple<int, float>(index, tMin));
    index++;
  }

  for (const auto &e : displacements)
  {
    auto &[index, t] = e;
    displaceVertex(index, t);
  }

  return displacements;
}

std::vector<float> Mesh::getPositionsVector() const
{
  std::vector<float> pos;

  for (const Vertex &v : vertices)
  {
    pos.push_back(v.pos[0]);
    pos.push_back(v.pos[1]);
    pos.push_back(v.pos[2]);
  }

  return pos;
}

std::vector<unsigned int> Mesh::getFacesVector() const
{
  std::vector<unsigned int> indices;
  for (const Face &f : faces)
  {
    indices.push_back(f.index[0]);
    indices.push_back(f.index[1]);
    indices.push_back(f.index[2]);
  }

  return indices;
}

int Mesh::nearestRoundPow2(float edgeLength) const
{
  return round(abs(log2(edgeLength)));
}

int Mesh::nearestCeilPow2(float edgeLength) const
{
  return ceil(abs(log2(edgeLength)));
}

int Mesh::maxInt2(int a, int b) const
{
  if (a >= b) return a;
  else return b;
}

int Mesh::maxInt3(int a, int b, int c) const
{
  if (a >= b)
  {
    if (a >= c) return a;
    else return c;
  }
  else
  {
    if (b >= c) return b;
    else return c;
  }
}

int Mesh::maxIntIndex(int arr[]) const
{
  int maxIdx = 0;
  int max = arr[0];
  size_t length = sizeof(arr) / sizeof(arr[0]);

  for (int i = 0; i < length; i++)
  {
    if (arr[i] > max)
    {
      max = arr[i];
      maxIdx = i;
    }
  }

  return maxIdx;
}

bool Mesh::enforceMicromesh(const Face &f)
{
  bool changeAnything = false;
  unsigned int edgeSubdivision[3] =
  {
    edges.at(f.edges[0]).subdivisions,
    edges.at(f.edges[1]).subdivisions,
    edges.at(f.edges[2]).subdivisions
  };

  int max = maxInt3(edgeSubdivision[0], edgeSubdivision[1], edgeSubdivision[2]);

  if (max == 0) return changeAnything;

  for (unsigned int &eSub : edgeSubdivision)
  {
    if (eSub < max - 1) {
      eSub = max - 1;
      changeAnything = true;
    }
  }

  edges[f.edges[0]].subdivisions = edgeSubdivision[0];
  edges[f.edges[1]].subdivisions = edgeSubdivision[1];
  edges[f.edges[2]].subdivisions = edgeSubdivision[2];

  return changeAnything;
}

bool Mesh::enforceAnisoMicromesh(const Face &f)
{
  unsigned int edgeSubdivisions[3] =
  {
    edges.at(f.edges[0]).subdivisions,
    edges.at(f.edges[1]).subdivisions,
    edges.at(f.edges[2]).subdivisions
  };

  bool changeAnything = false;

  int max = maxInt3(edgeSubdivisions[0], edgeSubdivisions[1], edgeSubdivisions[2]);
  int maxCount = 0;

  int edgeSubdivisionsLowerThenMax[2] = {0, 0};
  int i = 0;

  for (unsigned int e : edgeSubdivisions)
  {
    if (e == max) maxCount++;
    else if (e < max) edgeSubdivisionsLowerThenMax[i++] = e;
  }

  if (maxCount > 1) return changeAnything;

  int maxEdgeMinor = maxInt2(edgeSubdivisionsLowerThenMax[0], edgeSubdivisionsLowerThenMax[1]);

  for (unsigned int &e : edgeSubdivisions) {
    if (e == maxEdgeMinor) {
      e = max;
      changeAnything = true;
      break;
    }
  }

  edges[f.edges[0]].subdivisions = edgeSubdivisions[0];
  edges[f.edges[1]].subdivisions = edgeSubdivisions[1];
  edges[f.edges[2]].subdivisions = edgeSubdivisions[2];

  return changeAnything;
}

void Mesh::draw(bool wireframe)
{
  initializeOpenGLFunctions();
  glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
  unsigned int numFaces = static_cast<unsigned int>(getFacesVector().size());
  glDrawElements(GL_TRIANGLES, numFaces, GL_UNSIGNED_INT, 0);
}

void Mesh::drawDirect()
{
  initializeOpenGLFunctions();
  glBegin(GL_TRIANGLES);

  for (const Face &f : faces)
  {
    glm::vec3 v0 = vertices[f.index[0]].pos;
    glm::vec3 v1 = vertices[f.index[1]].pos;
    glm::vec3 v2 = vertices[f.index[2]].pos;
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
  }
  glEnd();
}

Mesh Mesh::parseOFF(const std::string &rawOFF)
{
  if (rawOFF.empty())
  {
    Mesh empty = Mesh();
    return empty;
  }

  std::istringstream iss(rawOFF);
  std::string line;

  std::getline(iss, line);

  if (line != "OFF")
  {
    std::cerr << "Wrong format!" << std::endl;
    Mesh corr_mesh = Mesh();
    return Mesh();
  }

  int numVertices, numFaces, numEdges;
  iss >> numVertices >> numFaces >> numEdges;
  std::getline(iss, line);

  Mesh mesh = Mesh();
  int nTotal = numVertices + numFaces + 0; // todo: atm no edges

  for (int i = 0; i < nTotal; i++)
  {
    if (i < numVertices)
    {
      float x, y, z;
      iss >> x >> y >> z;
      mesh.addVertex(glm::vec3(x, y, z));
    }
    else if (i >= numVertices && i < numVertices + numFaces)
    {
      int n_vrtx, v1, v2, v3;
      iss >> n_vrtx >> v1 >> v2 >> v3;
      mesh.addFace(v1, v2, v3);
    }
    // else
    //{
    //	 TODO: no edge atm
    //	 std::vector<unsigned int> idx_edge = std::vector<unsigned int>(4);
    //	 iss >> idx_edge[0] >> idx_edge[1] >> idx_edge[2] >> idx_edge[3];
    //	 mesh.edges.push_back(idx_edge);
    // }
    std::getline(iss, line);
  }

  mesh.updateBoundingBox();
  mesh.updateFaceNormals();
  mesh.updateVertexNormals();
  mesh.updateEdges();
  mesh.updateEdgesSubdivisions();

  return mesh;
}

/*
 * Still missing some cases
 * - vp 0.310000 3.210000 2.100000 (Parameter space vertices)
 * - f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ... (Vertex normal indices)
 * - f v1//vn1 v2//vn2 v3//vn3 ... (Vertex normal indices without texture coordinate indices)
 * - l v1 v2 v3 v4 v5 v6 ... (Line elements)
 *
 * Materials are still not handled.
 */

Mesh Mesh::parseOBJ(const std::string &raw_obj)
{
  Mesh mesh = Mesh();

  if (raw_obj.empty())
  {
    std::cerr << "The .obj file is empty, a null mesh has been returned." << std::endl;
    return mesh;
  }

  std::istringstream in(raw_obj);
  std::string line;

  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> texels;
  std::vector<glm::vec3> normals;
  std::vector<glm::uvec3> faces;

  while (std::getline(in, line))
  {
    if (line.substr(0, 2) == "v ")
    {
      std::istringstream is(line.substr(2));
      glm::vec3 vpos;
      float x, y, z;
      is >> x;
      is >> y;
      is >> z;
      vpos = glm::vec3(x, y, z);
      positions.push_back(vpos);
    }

    if (line.substr(0, 3) == "vt ")
    {
      std::istringstream is(line.substr(3));
      glm::vec2 tex;
      float u, v;
      is >> u;
      is >> v;
      tex = glm::vec2(u, v);
      texels.push_back(tex);
    }

    if (line.substr(0, 3) == "vn ")
    {
      std::istringstream is(line.substr(3));
      glm::vec3 normal;
      float x, y, z;
      is >> x;
      is >> y;
      is >> z;
      normal = glm::vec3(x, y, z);
      normals.push_back(normal);
    }

    if (line.substr(0, 2) == "f ")
    {
      // TODO: atm not considering textures, vn indices, ...
      std::istringstream is(line.substr(2));
      std::string dump;
      int i1, i2, i3;
      is >> i1;
      is >> dump;
      is >> i2;
      is >> dump;
      is >> i3;
      faces.push_back(glm::uvec3(--i1, --i2, --i3));
    }
  }

  for (const glm::vec3 &pos : positions) mesh.addVertex(pos);

  int i = 0;
  for (Vertex &v : mesh.vertices)
  {
    v.norm = normals[i];
    i++;
  }

  for (const glm::vec3 &f : faces) mesh.addFace(f[0], f[1], f[2]);

  mesh.updateBoundingBox();
  mesh.updateFaceNormals();
  mesh.updateVertexNormals();
  mesh.updateEdges();
  mesh.updateEdgesSubdivisions();

  return mesh;
}
