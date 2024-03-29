#include "Sources/Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh()
{
    vertices.clear();
    faces.clear();
    edges.clear();
}

void Mesh::updatePosMiddleAndR()
{
    R = -INF;

    for (Face &f : faces)
    {
        float pos0 = vertices.at(f.index[0]).pos[maxAxis];
        float pos1 = vertices.at(f.index[1]).pos[maxAxis];
        float pos2 = vertices.at(f.index[2]).pos[maxAxis];

        float posMax = maxFloat3(pos0, pos1, pos2);
        float posMin = minFloat3(pos0, pos1, pos2);
        float halfExt = (posMax - posMin) / 2.f;

        f.posMiddle = (posMax + posMin) / 2.f;

        if (R < halfExt) R = halfExt;
    }
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

    if (faceIndex0 != -1) faces.at(faceIndex0).edges[side0] = edgeIndex;
    if (faceIndex1 != -1) faces.at(faceIndex1).edges[side1] = edgeIndex;

    return edgeIndex;
}

void Mesh::displaceVertex(int index, float k)
{
    vertices[index].pos = vertices[index].pos + (k * vertices[index].norm);
}

void Mesh::displaceVertices(float k)
{
    for (Vertex &v : vertices)
        v.pos = v.pos + (k * v.norm);
}

void Mesh::displaceFace(int index, float k)
{
    Vertex &v0 = vertices[faces.at(index).index[0]];
    Vertex &v2 = vertices[faces.at(index).index[1]];
    Vertex &v1 = vertices[faces.at(index).index[2]];

    v0.pos = v0.pos + (k * faces.at(index).norm);
    v1.pos = v1.pos + (k * faces.at(index).norm);
    v2.pos = v2.pos + (k * faces.at(index).norm);
}

void Mesh::displaceFaces(float k)
{
    for (int i = 0; i < faces.size(); i++)
        displaceFace(i, k);
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

std::vector<uint> Mesh::getFacesVector() const
{
    std::vector<uint> indices;

    for (const Face &f : faces)
    {
        indices.push_back(f.index[0]);
        indices.push_back(f.index[1]);
        indices.push_back(f.index[2]);
    }

    return indices;
}

int Mesh::getFaceSubdivisionIndex(int index) const
{
    int e0SubdivisionLvl = edges.at(faces.at(index).edges[0]).subdivisions;
    int e1SubdivisionLvl = edges.at(faces.at(index).edges[1]).subdivisions;
    int e2SubdivisionLvl = edges.at(faces.at(index).edges[2]).subdivisions;

    return maxInt3(e0SubdivisionLvl, e1SubdivisionLvl, e2SubdivisionLvl);
}

float Mesh::getFacesMeanDoubleArea() const
{
    float avgArea = 0.0f;

    for (const Face &f : faces)
    {
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

float Mesh::getFacesMeanArea() const
{
    float avgArea = 0.0f;

    for (const Face &f : faces)
    {
        vec3 v0 = vertices.at(f.index[0]).pos;
        vec3 v1 = vertices.at(f.index[1]).pos;
        vec3 v2 = vertices.at(f.index[2]).pos;

        vec3 ab = v1 - v0;
        vec3 bc = v2 - v1;

        float faceArea = length(cross(ab, bc)) / 2.0f;

        avgArea += faceArea;
    }

    avgArea /= float(faces.size());

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

        float l0 = length(vertices.at(v1).pos - vertices.at(v0).pos);
        float l1 = length(vertices.at(v2).pos - vertices.at(v1).pos);
        float l2 = length(vertices.at(v0).pos - vertices.at(v2).pos);

        avgEdge += (l0 + l1 + l2);
    }

    avgEdge /= float(faces.size() * 3.f);

    return avgEdge;
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

Mesh Mesh::subdivideNtimes(int n)
{
    Mesh subdivided = Mesh();
    subdivided.vertices = this->vertices;

    for (const Face &f : faces)
    {
        int v0 = f.index[0];
        int v1 = f.index[1];
        int v2 = f.index[2];

        vec3 delta0 = (vertices.at(v1).pos - vertices.at(v0).pos) / float(n);
        vec3 delta1 = (vertices.at(v1).pos - vertices.at(v2).pos) / float(n);
        vec3 delta2 = (vertices.at(v2).pos - vertices.at(v0).pos) / float(n);

        int stored[2];

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n - i; j++)
            {
                vec3 t1 = vertices.at(v0).pos + (float(i) * delta0) + (float(j) * delta2);
                vec3 t2 = t1 + delta2;
                vec3 t3 = t2 + delta1;

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

Mesh Mesh::micromeshSubdivide()
{
    Mesh subdivided = Mesh();

    auto toIndex = [&](int vx, int vy) { return vy * (vy + 1) / 2 + vx; };
    auto toIndexV = [&](ivec2 v) { return v.y * (v.y + 1) / 2 + v.x; };

    for (const Face &f : faces)
    {
        int subLvlEdge0 = edges[f.edges[0]].subdivisions;
        int subLvlEdge1 = edges[f.edges[1]].subdivisions;
        int subLvlEdge2 = edges[f.edges[2]].subdivisions;

        int subLvlMax = maxInt3(subLvlEdge0, subLvlEdge1, subLvlEdge2);
        int n = 1 << subLvlMax;
        int k = int(subdivided.vertices.size());

        // add microvertices
        for (int vy = 0; vy <= n; vy++)
        {
            for (int vx = 0; vx <= vy; vx++)
            {
                float c = vx / float(n);
                float a = vy / float(n);
                vec3 bary = vec3((1 - a), (a - c), c);

                subdivided.vertices.push_back(getSurfaceVertex(f, bary));
            }
        }

        // add microfaces
        for (int fy = 0; fy < n; fy++)
        {
            for (int fx = 0; fx < n; fx++)
            {
                ivec2 v0(fx, fy), v1(fx, fy + 1), v2(fx + 1, fy + 1);

                if (fx > fy)
                { // flip "red" triangle
                    v0 = ivec2(n, n) - v0;
                    v1 = ivec2(n, n) - v1;
                    v2 = ivec2(n, n) - v2;
                }

                subdivided.addFace(k + toIndexV(v0), k + toIndexV(v1), k + toIndexV(v2));
            }
        }

        if (subLvlEdge0 < subLvlMax)
        {
            for (int vy = 1; vy < n; vy += 2)
            {
                int delta = (vy < n / 2) ? -1 : +1;
                subdivided.vertices[k + toIndex(0, vy)] = subdivided.vertices[k + toIndex(0, vy + delta)];
            }
        }

        if (subLvlEdge1 < subLvlMax)
        {
            for (int vx = 1; vx < n; vx += 2)
            {
                int delta = (vx < n / 2) ? -1 : +1;
                subdivided.vertices[k + toIndex(vx, n)] = subdivided.vertices[k + toIndex(vx + delta, n)];
            }
        }

        if (subLvlEdge2 < subLvlMax)
        {
            for (int vxy = 1; vxy < n; vxy += 2)
            {
                int delta = (vxy < n / 2) ? +1 : -1;
                subdivided.vertices[k + toIndex(vxy, vxy)] = subdivided.vertices[k + toIndex(vxy + delta, vxy + delta)];
            }
        }
    }

    subdivided.updateBoundingBox();
    subdivided.updateEdges();

    return subdivided;
}

Mesh Mesh::anisotropicMicromeshSubdivide()
{
    Mesh subdivided = Mesh();

    for (const Face &f : faces)
    {
        // w2 is the shortest edge
        int w0, w1, w2;

        // rule for setting w2 always as the shortest edge
        if (edges[f.edges[2]].subdivisions > edges[f.edges[1]].subdivisions)
        {
            // f.edgeIndices[1] is the smallest edge
            w0 = 2;
            w1 = 0;
            w2 = 1;
        }
        else if (edges[f.edges[2]].subdivisions > edges[f.edges[0]].subdivisions)
        {
            // f.edgeIndices[0] is the smallest edge
            w0 = 1;
            w1 = 2;
            w2 = 0;
        }
        else
            w0 = 0, w1 = 1, w2 = 2;

        int subLvlEdge0 = edges[f.edges[w0]].subdivisions;
        int subLvlEdge2 = edges[f.edges[w2]].subdivisions;

        int n = 1 << subLvlEdge0;
        int m = 1 << subLvlEdge2;
        int aniso = 1 << (subLvlEdge0 - subLvlEdge2);
        int k = int(subdivided.vertices.size());

        // add microvertices
        for (int vy = 0; vy <= m; vy++)
        {
            int lastVx = vy * aniso + aniso - 1;

            if (vy == m) lastVx -= aniso - 1;

            for (int vx = 0; vx <= lastVx; vx++)
            {
                // Number of vertical segments -> m - vx / aniso;
                // Number of vertical segments below the microvertex -> m - vy;
                float c = vx / float(n);
                float a;
                if (vx == n)
                    a = 0.f;
                else
                    a = (1 - c) * float(m - vy) / float(m - vx / aniso);
                float b = 1 - a - c;

                vec3 bary = vec3();
                bary[w0] = b;
                bary[w1] = c;
                bary[w2] = a;

                subdivided.vertices.push_back(getSurfaceVertex(f, bary));
            }
        }

        // add microfaces
        auto toVertexIndex = [&](ivec2 v) { return v.y * aniso * (v.y + 1) / 2 + v.x; };

        for (int fy = 0; fy < m; fy++)
        {
            for (int fx = 0; fx < (n + aniso - 1); fx++)
            {
                ivec2 v0(fx, fy), v1(fx, fy + 1), v2(fx + 1, fy + 1);

                // flip "red" triangle
                if (fx > (fy + 1) * aniso - 1)
                {
                    ivec2 rotate = ivec2(n + aniso - 1, m);
                    v0 = rotate - v0;
                    v1 = rotate - v1;
                    v2 = rotate - v2;
                }

                subdivided.addFace(k + toVertexIndex(v0), k + toVertexIndex(v1), k + toVertexIndex(v2));
            }
        }
    }

    subdivided.updateBoundingBox();
    subdivided.updateEdges();
\
    return subdivided;
}

int Mesh::micromeshPredictFaces() const
{
    int count = 0;

    for (const Face &f : faces)
    {
        int subLvlEdge0 = edges[f.edges[0]].subdivisions;
        int subLvlEdge1 = edges[f.edges[1]].subdivisions;
        int subLvlEdge2 = edges[f.edges[2]].subdivisions;

        int maxSubLvl = maxInt3(subLvlEdge0, subLvlEdge1, subLvlEdge2);

        count += (1 << maxSubLvl) * (1 << maxSubLvl);
    }

    return count;
}

int Mesh::anisotropicMicroMeshPredictFaces() const
{
    int count = 0;

    for (const Face &f : faces)
    {
        int subLvlEdge0 = edges[f.edges[0]].subdivisions;
        int subLvlEdge1 = edges[f.edges[1]].subdivisions;
        int subLvlEdge2 = edges[f.edges[2]].subdivisions;

        int maxSubLvl = maxInt3(subLvlEdge0, subLvlEdge1, subLvlEdge2);
        int minSubLvl = minInt3(subLvlEdge0, subLvlEdge1, subLvlEdge2);

        int k = 1 << maxSubLvl;
        int h = 1 << minSubLvl;
        int aniso = 1 << (maxSubLvl - minSubLvl);

        count += h * (k + aniso - 1);
    }

    return count;
}

// This function fixes the edges which are downscaled but should be higher scale
void Mesh::fixEdgesSubdivisionIndicesMicromesh()
{
    for (Edge &e : edges)
    {
        if (e.faces[0] == -1 || e.faces[1] == -1) continue;

        uint eMax0 = getFaceSubdivisionIndex(e.faces[0]);
        uint eMax1 = getFaceSubdivisionIndex(e.faces[1]);

        // if the edge is between two triangles N + 1 and it is N
        if (eMax0 == eMax1 && e.subdivisions + 1 == eMax0)
            e.subdivisions = eMax0;
    }
}

void Mesh::updateFaceNormals()
{
    for (int i = 0; i < faces.size(); i++)
    {
        Face &f = faces[i];
        vec3 v0 = vertices[f.index[0]].pos;
        vec3 v1 = vertices[f.index[1]].pos;
        vec3 v2 = vertices[f.index[2]].pos;

        f.norm = normalize(cross(v1 - v0, v2 - v0));
    }
}

void Mesh::updateVertexNormals()
{
    for (int i = 0; i < vertices.size(); i++)
        vertices[i].norm = vec3(0, 0, 0);

    for (int i = 0; i < faces.size(); i++)
    {
        Face &f = faces[i];
        Vertex &v0 = vertices[f.index[0]];
        Vertex &v1 = vertices[f.index[1]];
        Vertex &v2 = vertices[f.index[2]];
        vec3 faceNormal = f.norm;

        v0.norm += faceNormal;
        v1.norm += faceNormal;
        v2.norm += faceNormal;
    }

    for (int i = 0; i < vertices.size(); i++)
        vertices[i].norm = normalize(vertices[i].norm);
}

void Mesh::updateEdges()
{
    edges.clear();

    std::map<AvailableEdge, EdgeLocation> matingPool;

    for (int fIdx = 0; fIdx < faces.size(); fIdx++)
    {
        Face &f = faces[fIdx];

        for (int w = 0; w < 3; w++)
        {
            AvailableEdge edgeToCheck = AvailableEdge(f.index[w], f.index[(w + 1) % 3]);

            auto it = matingPool.find(edgeToCheck);

            // if the edge is already present
            if (it != matingPool.end())
            {
                EdgeLocation matchingEdgeLocation = it->second;
                addEdge(fIdx, matchingEdgeLocation.faceIndex, w, matchingEdgeLocation.sideIndex);
                matingPool.erase(it);
            }
            else
            {
                EdgeLocation newLocation = EdgeLocation(fIdx, w);
                matingPool.insert({ edgeToCheck.flip(), newLocation });
            }
        }
    }

    // adding open edges
    for (auto &openEdge : matingPool)
    {
        EdgeLocation openEdgeLocation = openEdge.second;
        int edgeIndex = addEdge(openEdgeLocation.faceIndex, -1, openEdgeLocation.sideIndex, -1);
        faces.at(openEdgeLocation.faceIndex).edges[openEdgeLocation.sideIndex] = edgeIndex;
    }
}

void Mesh::setInitialEdgeSubdivisionLevels(float targetEdgeLength)
{
    for (int i = 0; i < faces.size(); i++)
    {
        const Face &f = faces[i];

        int v0 = f.index[0];
        int v1 = f.index[1];
        int v2 = f.index[2];

        float l0 = length(vertices.at(v1).pos - vertices.at(v0).pos) / targetEdgeLength;
        float l1 = length(vertices.at(v2).pos - vertices.at(v1).pos) / targetEdgeLength;
        float l2 = length(vertices.at(v0).pos - vertices.at(v2).pos) / targetEdgeLength;

        edges.at(f.edges[0]).subdivisions = nearestRoundPow2(l0);
        edges.at(f.edges[1]).subdivisions = nearestRoundPow2(l1);
        edges.at(f.edges[2]).subdivisions = nearestRoundPow2(l2);
    }
}

void Mesh::updateEdgesSubdivisionLevelsMicromesh(double targetEdgeLength)
{
    setInitialEdgeSubdivisionLevels(targetEdgeLength);

    int count = 0;

    while (true)
    {
        bool changeAnything = false;

        for (Face &f : faces) changeAnything |= enforceMicromesh(f);

        count++;

        if (!changeAnything) break;
    }

    qDebug() << "Micromesh scheme enforced: " << count << " times.";

    fixEdgesSubdivisionIndicesMicromesh();
}

void Mesh::updateEdgesSubdivisionLevelsAniso(double targetEdgeLength)
{
    setInitialEdgeSubdivisionLevels(targetEdgeLength);

    int count = 0;

//    while (true)
//    {
//        bool changeAnything = false;

//        for (Face &f : faces) changeAnything |= enforceAnisotropicMicromesh(f);

//        count++;

//        if (!changeAnything) break;
//    }

    qDebug() << "Anisotropic scheme enforced: " << count << " times.";
}

void Mesh::updateBoundingBox()
{
    bbox.init(vertices[0].pos);
    for (Vertex &v : vertices) bbox.includeAnotherPoint(v.pos);
    maxAxis = bbox.maxAxis();
}

bool Mesh::enforceMicromesh(const Face &f)
{
    bool changeAnything = false;

    int edgeSubdivision[3] = {
        int(edges.at(f.edges[0]).subdivisions),
        int(edges.at(f.edges[1]).subdivisions),
        int(edges.at(f.edges[2]).subdivisions)};

    int max = maxInt3(edgeSubdivision[0], edgeSubdivision[1], edgeSubdivision[2]);

    for (int i = 0; i < 3; i++)
    {
        int &eSub = edgeSubdivision[i];

        if (eSub < (max - 1) && (max > 0))
        {
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
        edges.at(f.edges[2]).subdivisions};

    bool changeAnything = false;

    uint max = uint(maxInt3(edgeSubdivisions[0], edgeSubdivisions[1], edgeSubdivisions[2]));
    int maxCount = 0;

    int edgeSubdivisionsLowerThenMax[2] = {0, 0};
    int i = 0;

    for (uint e : edgeSubdivisions)
    {
        if (e == max) maxCount++;
        else if (e < max) edgeSubdivisionsLowerThenMax[i++] = e;
    }

    if (maxCount > 1) return changeAnything;

    int maxEdgeMinor = maxInt2(edgeSubdivisionsLowerThenMax[0], edgeSubdivisionsLowerThenMax[1]);

    for (int j = 0; j < 3; j++)
    {
        uint &e = edgeSubdivisions[j];

        if (e == maxEdgeMinor)
        {
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

Vertex Mesh::getSurfaceVertex(const Face &f, vec3 bary) const
{
    int v0 = f.index[0];
    int v1 = f.index[1];
    int v2 = f.index[2];

    Vertex v;
    v.pos = vertices.at(v0).pos * bary[0] +
            vertices.at(v1).pos * bary[1] +
            vertices.at(v2).pos * bary[2];

    v.norm = vertices.at(v0).norm * bary[0] +
             vertices.at(v1).norm * bary[1] +
             vertices.at(v2).norm * bary[2];

    return v;
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

    for (const Face &f : faces)
    {
        vec3 v0 = vertices[f.index[0]].pos;
        vec3 v1 = vertices[f.index[1]].pos;
        vec3 v2 = vertices[f.index[2]].pos;
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }

    glEnd();
}
