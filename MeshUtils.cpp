#include "Mesh.h"

float Mesh::minimumDisplacement(const vec3 &origin, const vec3 &direction, const Mesh &target)
{
  float minDisp = INF;

  for (const Face &f : target.faces) {
    vec3 v0 = target.vertices[f.index[0]].pos;
    vec3 v1 = target.vertices[f.index[1]].pos;
    vec3 v2 = target.vertices[f.index[2]].pos;

    Line line = Line(origin, direction);

    float disp;

    bool intersect = line.intersectTriangle(v0, v1, v2, disp);

    if (intersect)
      minDisp = (abs(minDisp) < abs(disp)) ? minDisp : disp;
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

void Mesh::removeDuplicatedVertices()
{
  std::vector<Vertex> newVertices;

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
    if (line.substr(0, 2) == "v ") {
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

  for (const vec3 &pos : positions)
    mesh.addVertex(pos);

  int i = 0;
  for (Vertex &v : mesh.vertices) {
    v.norm = normals[i];
    i++;
  }

  for (const vec3 &f : faces)
    mesh.addFace(f[0], f[1], f[2]);

  mesh.updateBoundingBox();
  mesh.updateFaceNormals();
  mesh.updateVertexNormals();
  mesh.updateEdges();

  return mesh;
}

void Mesh::exportOFF(const std::string &fileName, QString filePath) const
{
  std::ostringstream oss;
  std::string fileNameExt = fileName + ".off";

  if (filePath.isEmpty()) {
    filePath = QString::fromStdString("./Output/" + fileNameExt);
  }

  std::ofstream fileStream(filePath.toStdString().c_str(), std::ios::out);

  if (!fileStream.is_open()) {
    printf("Failed to open \'%s\'. File doesn't exist.", filePath.toStdString().c_str());
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
  qDebug() << fileNameExt.c_str() << "has been exported.";
}

void Mesh::exportOBJ(const std::string &fName, QString filePath) const
{
  std::ostringstream oss;
  std::string fileNameExt = fName + ".obj";

  if (filePath.isEmpty()) {
    filePath = QString::fromStdString("./Output/" + fileNameExt);
  }

  std::ofstream fileStream(filePath.toStdString().c_str(), std::ios::out);

  if (!fileStream.is_open()) {
    printf("Failed to open \'%s\'. File doesn't exist.", filePath.toStdString().c_str());
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

  qDebug() << fileNameExt.c_str() << "has been exported.";
}
