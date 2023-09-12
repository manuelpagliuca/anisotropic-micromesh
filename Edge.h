#ifndef EDGE_H
#define EDGE_H

#include <QtCore/QString>

struct Edge
{
    uint faces[2];
    uint side[2]; // 0 and 1 (l and r)
    uint subdivisions = 0;
};

struct AvailableEdge
{
    uint startVertexIndex;
    uint endVertexIndex;

    AvailableEdge(uint a, uint b) : startVertexIndex(a), endVertexIndex(b) {}

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

    AvailableEdge flip() const { return AvailableEdge(endVertexIndex, startVertexIndex); }
};

struct EdgeLocation
{
    uint faceIndex;
    uint sideIndex;

    EdgeLocation(uint a, uint b) : faceIndex(a), sideIndex(b) {}
};

#endif // EDGE_H
