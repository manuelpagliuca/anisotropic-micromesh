#include "Mesh.h"

Mesh::Mesh() {}

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

void Mesh::print() const
{
    std::cout << vertices.size() << " " << faces.size() << "\n";// << " " << edges.size();

    for (auto& v : vertices)
        std::cout << glm::to_string(v.pos) << "\n";

    std::cout << std::endl;

    for (auto& f : faces)
        std::cout << f.index[0] << f.index[1] << f.index[2] << "\n";
}

Mesh Mesh::subdivide()
{
    Mesh subdivided = Mesh();
    subdivided.vertices = this->vertices;

    for (const Face& f : faces) {
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

void Mesh::exportOFF(const std::string& fileName) const
{
    std::ostringstream oss;
    std::string fileNameExt = fileName + ".off";
    std::ofstream fileStream(".\\mesh\\" + fileNameExt, std::ios::out);

    if (!fileStream.is_open()) {
        printf("Failed to open \'%s\'. File doesn't exist.", fileNameExt.c_str());
        return;
    }

    fileStream
            << "OFF" << "\n"
            << vertices.size() << " "
            << faces.size() << " "
            << 0 << std::endl; // todo: atm the # of edges is 0

    for (const auto& v : vertices)
        fileStream
                << v.pos.x << " "
                << v.pos.y << " "
                << v.pos.z << std::endl;

    for (const auto& f : faces)
        fileStream
                << 3
                << " " << f.index[0]
                << " " << f.index[1]
                << " " << f.index[2] << std::endl;

    fileStream.close();
}

void Mesh::exportOBJ(const std::string& f_name) const
{
    std::ostringstream oss;
    std::string file_name_ext = f_name + ".obj";
    std::ofstream file_stream(".\\mesh\\" + file_name_ext, std::ios::out);

    if (!file_stream.is_open()) {
        printf("Failed to open \'%s\'. File doesn't exist.", file_name_ext.c_str());
        return;
    }

    for (const Vertex& v : vertices) {
        file_stream << std::setprecision(6) << std::fixed
                    << "vn " << v.norm.x
                    << " " << v.norm.y
                    << " " << v.norm.z << std::endl;
        file_stream << std::setprecision(6) << std::fixed
                    << "v " << v.pos.x
                    << " " << v.pos.y
                    << " " << v.pos.z << std::endl;
    }

    for (const Face& f : faces)
        file_stream << "f "
                    << f.index[0] + 1 << "//" << f.index[0] + 1 << " "
                    << f.index[1] + 1 << "//" << f.index[1] + 1 << " "
                    << f.index[2] + 1 << "//" << f.index[2] + 1 << std::endl;

    file_stream.close();
}

void Mesh::updateGL()
{
}

void Mesh::updateFaceNormals()
{
    for (Face& f : faces) {
        glm::vec3 v0 = vertices[f.index[0]].pos;
        glm::vec3 v1 = vertices[f.index[1]].pos;
        glm::vec3 v2 = vertices[f.index[2]].pos;
        f.norm = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }
}

void Mesh::updateVertexNormals()
{
    for (Vertex& v : vertices)
        v.norm = glm::vec3(0, 0, 0);

    for (Face& f : faces) {
        Vertex& v0 = vertices[f.index[0]];
        Vertex& v1 = vertices[f.index[1]];
        Vertex& v2 = vertices[f.index[2]];
        v0.norm += f.norm;
        v1.norm += f.norm;
        v2.norm += f.norm;
    }

    for (Vertex& v : vertices)
        v.norm = glm::normalize(v.norm);
}

void Mesh::updateBoundingBox()
{
    bbox.init(vertices[0].pos);
    for (Vertex &v: vertices)
        bbox.includeAnotherPoint(v.pos);
}

void Mesh::displaceVertices(float k)
{
    for (Vertex& v : vertices)
        v.pos = v.pos + (k * v.norm);
}

void Mesh::displaceVertex(int index, float k)
{
    vertices[index].pos = vertices[index].pos + (k * vertices[index].norm);
}

void Mesh::displaceFace(float k)
{
    for (Face& f : faces) {
        Vertex& v0 = vertices[f.index[0]];
        Vertex& v2 = vertices[f.index[1]];
        Vertex& v1 = vertices[f.index[2]];

        v0.pos = v0.pos + (k * f.norm);
        v1.pos = v1.pos + (k * f.norm);
        v2.pos = v2.pos + (k * f.norm);
    }
}

std::vector<std::tuple<int, float>> Mesh::displaceVerticesTowardsTargetMesh(const Mesh &targetMesh)
{
    int index = 0;
    std::vector<std::tuple<int, float>> displacements;

    for (auto &v: vertices) {
        glm::vec3 rayOrigin = v.pos;
        glm::vec3 rayDirection = glm::normalize(v.norm);
        float tMin = FLT_MAX;

        for (auto &f: targetMesh.faces) {
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

            if (!(v0P < 0 && v1P < 0 && v2P < 0) && abs(tMin) > abs(t)) {
                tMin = t;
            }
        }
        displacements.push_back(std::tuple<int, float>(index, tMin));
        index++;
    }

//    for (const auto &e: displacements) {
//        auto & [index, t] = e;
//        displaceVertex(index, t);
//    }

    return displacements;
}

std::vector<float> Mesh::getPositionsVector() const
{
    std::vector<float> pos;
    for (auto& v : vertices) {
        pos.push_back(v.pos[0]);
        pos.push_back(v.pos[1]);
        pos.push_back(v.pos[2]);
    }
    return pos;
}

std::vector<int> Mesh::getFacesVector() const
{
    std::vector<int> indices;
    for (auto& f : faces) {
        indices.push_back(f.index[0]);
        indices.push_back(f.index[1]);
        indices.push_back(f.index[2]);
    }
    return indices;
}

void Mesh::draw(bool wireframe)
{
    initializeOpenGLFunctions();
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    int num_faces = static_cast<int>(getFacesVector().size());
    glDrawElements(GL_TRIANGLES, num_faces, GL_UNSIGNED_INT, 0);
}

void Mesh::drawDirect()
{
    initializeOpenGLFunctions();
    glBegin(GL_TRIANGLES);

    for (const auto& f : faces) {
        glm::vec3 v0 = vertices[f.index[0]].pos;
        glm::vec3 v1 = vertices[f.index[1]].pos;
        glm::vec3 v2 = vertices[f.index[2]].pos;
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();
}

Mesh Mesh::parseOFF(const std::string& rawOFF)
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
    int n_total = numVertices + numFaces + 0; // todo: atm no edges

    for (int i = 0; i < n_total; i++) {
        if (i < numVertices) {
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
        //else
        //{
        //	 TODO: no edge atm
        //	 std::vector<unsigned int> idx_edge = std::vector<unsigned int>(4);
        //	 iss >> idx_edge[0] >> idx_edge[1] >> idx_edge[2] >> idx_edge[3];
        //	 mesh.edges.push_back(idx_edge);
        //}
        std::getline(iss, line);
    }

    mesh.updateBoundingBox();
    mesh.updateFaceNormals();
    mesh.updateVertexNormals();

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

Mesh Mesh::parseOBJ(const std::string& raw_obj)
{
    Mesh mesh = Mesh();

    if (raw_obj.empty()) {
        std::cerr << "The .obj file is empty, a null mesh has been returned." << std::endl;
        return mesh;
    }

    std::istringstream in(raw_obj);
    std::string line;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texels;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec3> faces;

    while (std::getline(in, line))
    {
        if (line.substr(0, 2) == "v ") {
            std::istringstream is(line.substr(2));
            glm::vec3 vpos;
            float x, y, z;
            is >> x;
            is >> y;
            is >> z;
            vpos = glm::vec3(x, y, z);
            positions.push_back(vpos);
        }

        if (line.substr(0, 3) == "vt ") {
            std::istringstream is(line.substr(3));
            glm::vec2 tex;
            float u, v;
            is >> u;
            is >> v;
            tex = glm::vec2(u, v);
            texels.push_back(tex);
        }

        if (line.substr(0, 3) == "vn ") {
            std::istringstream is(line.substr(3));
            glm::vec3 normal;
            float x, y, z;
            is >> x;
            is >> y;
            is >> z;
            normal = glm::vec3(x, y, z);
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
            faces.push_back(glm::ivec3(--i1, --i2, --i3));
        }
    }

    for (const auto& pos : positions)
        mesh.addVertex(pos);

    int i = 0;
    for (auto& v : mesh.vertices) {
        v.norm = normals[i];
        i++;
    }

    for (const auto& f : faces)
        mesh.addFace(f[0], f[1], f[2]);

    mesh.updateBoundingBox();
    mesh.updateFaceNormals();
    mesh.updateVertexNormals();
    return mesh;
}
