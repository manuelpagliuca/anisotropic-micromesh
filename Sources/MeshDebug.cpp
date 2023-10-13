#include "Sources/Mesh.h"
#include "Sources/Utility.h"

void Mesh::print() const
{
    std::cout << vertices.size() << " " << faces.size() << "\n";
    for (const Vertex &v : vertices)
        PRINT_VECTOR(v.pos);

    std::cout << std::endl;
    for (const Face &f : faces)
        std::cout << f.index[0] << f.index[1] << f.index[2] << "\n";

    std::cout << std::endl;
    for (const Edge &e : edges)
        std::cout << e.faces[0] << e.faces[1] << "\n";
}

void Mesh::printEdgeSubdivisions() const
{
    for (const Face &f : faces)
    {
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
    for (const Edge &e : edges)
    {
        if (e.side[1] == -1)
        {
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
