#include "Sources/Mesh.h"

void Mesh::intersectTriangle(int fIndex, Line line, float &minDistance)
{
    vec3 v0 = vertices[faces.at(fIndex).index[0]].pos;
    vec3 v1 = vertices[faces.at(fIndex).index[1]].pos;
    vec3 v2 = vertices[faces.at(fIndex).index[2]].pos;

    float newDistance;
    bool intersect = line.intersectTriangle(v0, v1, v2, newDistance);

    if (intersect)
        minDistance = abs(minDistance) < abs(newDistance) ? minDistance : newDistance;
}

float Mesh::minimumDisplacement(const vec3 &origin, const vec3 &direction, Mesh &target)
{
    const float DIST_MAX = bbox.diagonal() * 0.01;
    float minDisp = DIST_MAX;

    Line line = Line(origin, direction);
    float posOrigin = origin[maxAxis];

    int i = -1;                       // first after posOrigin
    int j = int(target.faces.size()); // last before posOrigin

    while (j != i + 1)
    {
        int m = (i + j) / 2;

        if (target.faces.at(m).posMiddle > posOrigin) j = m;
        else i = m;
    }

    while (i >= 0 || j < target.faces.size())
    {
        if (i >= 0)
        {
            // early rejection test
            if (target.faces.at(i).posMiddle - posOrigin + target.R < -abs(minDisp))
            {
                // stop leftmost search
                i = -1;
            }
            else
            {
                target.intersectTriangle(i--, line, minDisp);
            }
        }

        if (j < target.faces.size())
        {
            // early rejection test
            if (target.faces.at(j).posMiddle - posOrigin - target.R > abs(minDisp))
            {
                // stop rightmost search
                j = int(target.faces.size());
            }
            else
            {
                target.intersectTriangle(j++, line, minDisp);
            }
        }
    }

    if (minDisp == DIST_MAX) minDisp = 0.0f;

    return minDisp;
}

float Mesh::minimumDistanceBruteForce(const vec3 &origin, const vec3 &direction, Mesh &target)
{
    float minDistance = INF;
    Line line = Line(origin, direction);

    for (int i = 0; i < target.faces.size(); i++)
        target.intersectTriangle(i, line, minDistance);

    return minDistance;
}

std::vector<float> Mesh::getDisplacements(Mesh &target)
{
    std::vector<float> displacements;

    for (const Vertex &v : vertices)
        displacements.push_back(minimumDisplacement(v.pos, v.norm, target));

    return displacements;
}

float Mesh::getFaceAreaVariance() const
{
    float facesMeanArea = getFacesMeanArea();

    float squaredDiffs = 0.f;

    for (const Face &f : faces)
    {
        vec3 v0 = vertices.at(f.index[0]).pos;
        vec3 v1 = vertices.at(f.index[1]).pos;
        vec3 v2 = vertices.at(f.index[2]).pos;

        vec3 ab = v1 - v0;
        vec3 bc = v2 - v1;

        float area = length(cross(ab, bc)) / 2.0f;
        float squaredDiff = (area - facesMeanArea) * (area - facesMeanArea);

        squaredDiffs += squaredDiff;
    }

    return squaredDiffs / float(faces.size());
}

float Mesh::getFaceAreaVariationCoefficient() const
{
    float facesMeanArea = getFacesMeanArea();
    float stdDeviation = sqrtf(getFaceAreaVariance());

    return (stdDeviation / facesMeanArea) * 100.0f;
}

void Mesh::removeDuplicatedVertices()
{
    std::unordered_set<Vertex> uniqueVertices;

    for (Vertex &v : vertices) uniqueVertices.insert(v);

    for (Face &f : faces)
    {
        for (int w = 0; w < 3; w++)
        {
            Vertex v = vertices.at(f.index[w]);
            auto it = uniqueVertices.find(v);
            f.index[w] = std::distance(uniqueVertices.begin(), it);
        }
    }

    vertices = std::vector(uniqueVertices.begin(), uniqueVertices.end());
}

void Mesh::removeDegenerateFaces()
{
    std::vector<Face> cleanFaces;

    for (const Face &f : faces)
        if (f.index[0] != f.index[1] &&
            f.index[0] != f.index[2] &&
            f.index[1] != f.index[2])
            cleanFaces.push_back(f);

    std::swap(faces, cleanFaces);
}

void Mesh::removeHighlyOttuseIsoFaces()
{
//    for (Face &f : faces)
//    {
//        Vertex v0 = vertices.at(f.index[0]);
//        Vertex v1 = vertices.at(f.index[1]);
//        Vertex v2 = vertices.at(f.index[2]);

//        glm::vec3 side0 = v2.pos - v0.pos;
//        glm::vec3 side1 = v2.pos - v1.pos;
//        glm::vec3 side2 = v0.pos - v1.pos;

//        float edgeL0 = glm::length(side0);
//        float edgeL1 = glm::length(side1);
//        float edgeL2 = glm::length(side2);
//        float angle = -1.0f;

//        if (edgeL0 == edgeL1)      angle = glm::angle(side0, side1);
//        else if (edgeL0 == edgeL2) angle = glm::angle(side0, side2);
//        else if (edgeL2 == edgeL1) angle = glm::angle(side2, side1);

//        // the triangle is isosceles and highly obtuse
//        if (angle >= 150.f && angle <= 180.f)
//        {

//        }
//    }
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
            mesh.addVertex(vec3(x, y, z));
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
        //	 std::vector<uint> idx_edge = std::vector<uint>(4);
        //	 iss >> idx_edge[0] >> idx_edge[1] >> idx_edge[2] >> idx_edge[3];
        //	 mesh.edges.push_back(idx_edge);
        // }
        std::getline(iss, line);
    }

    mesh.updateBoundingBox();
    mesh.updateFaceNormals();
    mesh.updateVertexNormals();
    mesh.updatePosMiddleAndR();
    std::sort(mesh.faces.begin(), mesh.faces.end());
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

    if (raw_obj.empty())
    {
        std::cerr << "The .obj file is empty, a null mesh has been returned." << std::endl;
        return mesh;
    }

    std::istringstream in(raw_obj);
    std::string line;

    std::vector<vec3> positions;
    std::vector<vec2> texels;
    std::vector<vec3> normals;
    std::vector<uvec3> faces;

    while (std::getline(in, line))
    {
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

        if (line.substr(0, 3) == "vt ")
        {
            std::istringstream is(line.substr(3));
            vec2 tex;
            float u, v;
            is >> u;
            is >> v;
            tex = vec2(u, v);
            texels.push_back(tex);
        }

        if (line.substr(0, 3) == "vn ")
        {
            std::istringstream is(line.substr(3));
            vec3 normal;
            float x, y, z;
            is >> x;
            is >> y;
            is >> z;
            normal = vec3(x, y, z);
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
            faces.push_back(uvec3(--i1, --i2, --i3));
        }
    }

    for (const vec3 &pos : positions)
        mesh.addVertex(pos);

    int i = 0;

    for (Vertex &v : mesh.vertices)
    {
        v.norm = normals[i];
        i++;
    }

    for (const vec3 &f : faces)
        mesh.addFace(f[0], f[1], f[2]);

    mesh.updateBoundingBox();
    mesh.updateFaceNormals();
    mesh.updateVertexNormals();
    mesh.updatePosMiddleAndR();
    std::sort(mesh.faces.begin(), mesh.faces.end());
    mesh.updateEdges();

    return mesh;
}

void Mesh::exportOFF(const std::string &fileName, QString filePath) const
{
    std::ostringstream oss;
    std::string fileNameExt = fileName + ".off";

    if (filePath.isEmpty())
    {
        filePath = QString::fromStdString("./Output/" + fileNameExt);
    }

    std::ofstream fileStream(filePath.toStdString().c_str(), std::ios::out);

    if (!fileStream.is_open())
    {
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

    if (filePath.isEmpty())
    {
        filePath = QString::fromStdString("./Output");
    }

    filePath += QString::fromStdString(fileNameExt.c_str());

    std::ofstream fileStream(filePath.toStdString(), std::ios::out);

    if (!fileStream.is_open())
    {
        printf("Failed to open \'%s\'. File doesn't exist.", filePath.toStdString().c_str());
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

    qDebug() << fileNameExt.c_str() << "has been exported.";
}

void Mesh::exportVariationCoefficient(float mfsFactor, QString filePath) const
{
    std::string fileNameExt = "variation_coefficients.txt";

    if (filePath.isEmpty()) filePath = QString::fromStdString("./Output");

    filePath += QString::fromStdString(fileNameExt.c_str());

    // Apri il file in modalità append invece che write
    std::ofstream fileStream(filePath.toStdString(), std::ios::out | std::ios::app);

    if (!fileStream.is_open())
    {
        printf("Failed to open \'%s\'. File doesn't exist.", filePath.toStdString().c_str());
        return;
    }

    fileStream << mfsFactor << " " << getFaceAreaVariationCoefficient() << "\n";
    fileStream.close();

    qDebug() << fileNameExt.c_str() << "has been exported.";
}

