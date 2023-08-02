#include "Utility.h"

int nearestRoundPow2(float edgeLength)
{
  return int(round(abs(log2(edgeLength))));
}

int nearestCeilPow2(float edgeLength)
{
  return int(ceil(abs(log2(edgeLength))));
}

int maxInt2(int a, int b)
{
  return (a >= b) ? a : b;
}

int maxInt3(int a, int b, int c)
{
  return maxInt2(maxInt2(a, b), c);
}

int maxIntIndex(int arr[])
{
  int maxIdx = 0;
  int max = arr[0];
  size_t length = sizeof(arr) / sizeof(arr[0]);

  for (size_t i = 0; i < length; i++) {
    if (arr[i] > max) {
      max = arr[i];
      maxIdx = int(i);
    }
  }

  return maxIdx;
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
