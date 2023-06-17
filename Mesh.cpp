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

  faces.at(faceIndex0).edges[side0] = edgeIndex;
  faces.at(faceIndex1).edges[side1] = edgeIndex;

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
  subdivided.edges = edges;

  auto toIndex = [&](int vx, int vy) { return vy * (vy + 1) / 2 + vx; };
  auto toIndexV = [&](ivec2 v) { return v.y * (v.y + 1) / 2 + v.x; };

  for (const Face& f: faces) {
    int e0 = edges[f.edges[0]].subdivisions;
    int e1 = edges[f.edges[1]].subdivisions;
    int e2 = edges[f.edges[2]].subdivisions;

    int eMax = maxInt3(e0, e1, e2);
    int n = pow(2, eMax);
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

    if (e0 < eMax) {
      for (int vy = 1; vy < n; vy += 2) {
        int delta = (vy < n/2) ? -1 : +1;
        subdivided.vertices[k + toIndex(0, vy)] = subdivided.vertices[k + toIndex(0, vy + delta)];
      }
    }

    if (e1 < eMax) {
      for (int vx = 1; vx < n; vx += 2) {
        int delta = (vx < n/2) ? -1 : +1;
        subdivided.vertices[k + toIndex(vx, n)] = subdivided.vertices[k + toIndex(vx + delta, n)];
      }
    }

    if (e2 < eMax) {
      for (int vxy = 1; vxy < n; vxy += 2) {
        int delta = (vxy < n/2) ? -1 : +1;
        subdivided.vertices[k + toIndex(vxy, vxy)] = subdivided.vertices[k + toIndex(vxy + delta, vxy + delta)];
      }
    }    
  }

  subdivided.fixEdges(); // to implement - still not working
  subdivided.updateBoundingBox();
  subdivided.updateFaceNormals();
  subdivided.updateVertexNormals();

  return subdivided;
}

Mesh Mesh::anisotropicMicromeshSubdivide()
{
  Mesh subdivided = Mesh();
  // to implement
  return subdivided;
}

// This function fixes the edges which are downscaled but should be higher scale
void Mesh::fixEdges()
{
  for (Edge &e: edges) {
    Face f0 = faces.at(e.faces[0]);
    Face f1 = faces.at(e.faces[1]);

    int e00 = edges.at(f0.edges[0]).subdivisions;
    int e01 = edges.at(f0.edges[1]).subdivisions;
    int e02 = edges.at(f0.edges[2]).subdivisions;

    int e10 = edges.at(f1.edges[0]).subdivisions;
    int e11 = edges.at(f1.edges[1]).subdivisions;
    int e12 = edges.at(f1.edges[2]).subdivisions;

    int eMax0 = maxInt3(e00, e01, e02);
    int eMax1 = maxInt3(e10, e11, e12);

    if (eMax0 == eMax1 && e.subdivisions < eMax0) e.subdivisions = eMax0;
  }
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
    faces.at(openEdgeLocation.faceIndex).edges[openEdgeLocation.sideIndex] = edgeIndex;
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

    edges.at(f.edges[0]).subdivisions = nearestRoundPow2(l0);
    edges.at(f.edges[1]).subdivisions = nearestRoundPow2(l1);
    edges.at(f.edges[2]).subdivisions = nearestRoundPow2(l2);
  }
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
      assert(faces[faceIndex].edges[faceSide] == edgeIndex);
    }
  }

  for (int faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
    Face f = faces[faceIndex];

    for (int faceEdge = 0; faceEdge < 3; faceEdge++) {
      int edgeIndex = f.edges[faceEdge];
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
    edges.at(f.edges[0]).subdivisions,
    edges.at(f.edges[1]).subdivisions,
    edges.at(f.edges[2]).subdivisions
  };

  uint max = uint(maxInt3(edgeSubdivision[0], edgeSubdivision[1], edgeSubdivision[2]));

  for (uint &eSub : edgeSubdivision) {
    if (eSub < (max - 1) && (max > 0)) {
      eSub = max - 1;
      changeAnything = true;
    }
  }

  edges[f.edges[0]].subdivisions = edgeSubdivision[0];
  edges[f.edges[1]].subdivisions = edgeSubdivision[1];
  edges[f.edges[2]].subdivisions = edgeSubdivision[2];

  return changeAnything;
}

bool Mesh::enforceAnisotropicMicromesh(const Face &f)
{
  uint edgeSubdivisions[3] = {
    edges.at(f.edges[0]).subdivisions,
    edges.at(f.edges[1]).subdivisions,
    edges.at(f.edges[2]).subdivisions
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

  edges[f.edges[0]].subdivisions = edgeSubdivisions[0];
  edges[f.edges[1]].subdivisions = edgeSubdivisions[1];
  edges[f.edges[2]].subdivisions = edgeSubdivisions[2];

  return changeAnything;
}

bool Mesh::isMicromeshScheme() const
{
  int correctFaces = 0;

  for (const Face &f : faces) {
    int i = edges.at(f.edges[0]).subdivisions;
    int j = edges.at(f.edges[0]).subdivisions;
    int k = edges.at(f.edges[0]).subdivisions;
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

std::vector<std::tuple<int, float>> Mesh::displaceVerticesTowardsTargetMesh(const Mesh &targetMesh)
{
  int index = 0;
  std::vector<std::tuple<int, float>> displacements;

  for (const Vertex &v : vertices) {
    vec3 rayOrigin = v.pos;
    vec3 rayDirection = normalize(v.norm);
    float tMin = FLT_MAX;

    for (const Face &f : targetMesh.faces) {
      // Triangle processing
      vec3 v0 = targetMesh.vertices[f.index[0]].pos;
      vec3 v1 = targetMesh.vertices[f.index[1]].pos;
      vec3 v2 = targetMesh.vertices[f.index[2]].pos;
      vec3 v01 = v0 - v1;
      vec3 v02 = v0 - v2;
      vec3 planeNormal = normalize(cross(v01, v02));

      // Use v0 for finding the 'd' parameter of the supporting plane
      float d = dot(planeNormal, v0);
      // finding 't' parameter of ray
      float t = (dot(planeNormal, rayOrigin) + d) / dot(planeNormal, rayDirection);
      // finding the point on the ray
      vec3 P = rayOrigin + rayDirection * t;
      // vector vertices acting as barycentric coordinates for highlighting the point
      float v0P = dot(cross(v1 - v0, P - v1), rayDirection);
      float v1P = dot(cross(v2 - v1, P - v1), rayDirection);
      float v2P = dot(cross(v0 - v2, P - v2), rayDirection);

      if (!(v0P < 0 && v1P < 0 && v2P < 0) && abs(tMin) > abs(t)) tMin = t;
    }
    displacements.push_back(std::tuple<int, float>(index, tMin));
    index++;
  }

  for (const auto &e : displacements) {
    auto &[index, t] = e;
    displaceVertex(index, t);
  }

  return displacements;
}
Mesh Mesh::parseOFF(const std::string &rawOFF)
{
  if (rawOFF.empty()) {
    Mesh empty = Mesh();
    return empty;
  }

  std::istringstream iss(rawOFF);
  std::string line;

  std::getline(iss, line);

  if (line != "OFF") {
    std::cerr << "Wrong format!" << std::endl;
    Mesh corr_mesh = Mesh();
    return Mesh();
  }

  int numVertices, numFaces, numEdges;
  iss >> numVertices >> numFaces >> numEdges;
  std::getline(iss, line);

  Mesh mesh = Mesh();
  int nTotal = numVertices + numFaces + 0; // todo: atm no edges

  for (int i = 0; i < nTotal; i++) {
    if (i < numVertices) {
      float x, y, z;
      iss >> x >> y >> z;
      mesh.addVertex(vec3(x, y, z));
    } else if (i >= numVertices && i < numVertices + numFaces) {
      int n_vrtx, v1, v2, v3;
      iss >> n_vrtx >> v1 >> v2 >> v3;
      mesh.addFace(v1, v2, v3);
    }
    // else
    //{
    //	 TODO: no edge atm
    //	 std::vector<uint> idx_edge = std::vector<uint>(4);
    //	 iss >> idx_edge[0] >> idx_edge[1] >> idx_edge[2] >> idx_edge[3];
    //	 mesh.edges.push_back(idx_edge);
    // }
    std::getline(iss, line);
  }

  mesh.updateBoundingBox();
  mesh.updateFaceNormals();
  mesh.updateVertexNormals();
  mesh.updateEdges();
  mesh.updateEdgesSubdivisionLevels();

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

  if (raw_obj.empty()) {
    std::cerr << "The .obj file is empty, a null mesh has been returned." << std::endl;
    return mesh;
  }

  std::istringstream in(raw_obj);
  std::string line;

  std::vector<vec3> positions;
  std::vector<vec2> texels;
  std::vector<vec3> normals;
  std::vector<uvec3> faces;

  while (std::getline(in, line)) {
    if (line.substr(0, 2) == "v ")
    {
      std::istringstream is(line.substr(2));
      vec3 vpos;
      float x, y, z;
      is >> x;
      is >> y;
      is >> z;
      vpos = vec3(x, y, z);
      positions.push_back(vpos);
    }

    if (line.substr(0, 3) == "vt ") {
      std::istringstream is(line.substr(3));
      vec2 tex;
      float u, v;
      is >> u;
      is >> v;
      tex = vec2(u, v);
      texels.push_back(tex);
    }

    if (line.substr(0, 3) == "vn ") {
      std::istringstream is(line.substr(3));
      vec3 normal;
      float x, y, z;
      is >> x;
      is >> y;
      is >> z;
      normal = vec3(x, y, z);
      normals.push_back(normal);
    }

    if (line.substr(0, 2) == "f ") {
      // TODO: atm not considering textures, vn indices, ...
      std::istringstream is(line.substr(2));
      std::string dump;
      int i1, i2, i3;
      is >> i1;
      is >> dump;
      is >> i2;
      is >> dump;
      is >> i3;
      faces.push_back(uvec3(--i1, --i2, --i3));
    }
  }

  for (const vec3 &pos : positions) mesh.addVertex(pos);

  int i = 0;
  for (Vertex &v : mesh.vertices) {
    v.norm = normals[i];
    i++;
  }

  for (const vec3 &f : faces) mesh.addFace(f[0], f[1], f[2]);

  mesh.updateBoundingBox();
  mesh.updateFaceNormals();
  mesh.updateVertexNormals();
  mesh.updateEdges();
  mesh.updateEdgesSubdivisionLevels();

  return mesh;
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
