#ifndef VERTEX_H
#define VERTEX_H

#include <QtCore/QString>
#include <GLM/glm.hpp>
#include <cstdlib>
#include <functional>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 tex;

    bool operator==(const Vertex &v) const
    {
        return v.pos == pos && v.norm == norm && v.tex == tex;
    }

    bool operator<(const Vertex &v) const
    {
        if (pos.x < v.pos.x) return true;
        if (v.pos.x < pos.x) return false;
        if (pos.y < v.pos.y) return true;
        if (v.pos.y < pos.y) return false;
        return pos.z < v.pos.z;
    }
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(const Vertex &v) const
        {
            size_t hash = 0;

            hash = std::hash<int>()(v.pos.x) ^ std::hash<int>()(v.pos.y) ^ std::hash<int>()(v.pos.z);

            return hash;
        }
    };
}

#endif // VERTEX_H
