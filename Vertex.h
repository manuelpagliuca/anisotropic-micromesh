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
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(const Vertex &v) const
        {
            size_t hash = 0;
            hash = std::hash<int>()(v.pos.x) ^ std::hash<int>()(v.pos.x) ^ std::hash<int>()(v.pos.x);
            hash = std::hash<int>()(v.norm.x) ^ std::hash<int>()(v.norm.x) ^ std::hash<int>()(v.norm.x);
            hash = std::hash<int>()(v.tex.x) ^ std::hash<int>()(v.tex.x) ^ std::hash<int>()(v.tex.x);

            return hash;
        }
    };
}

#endif // VERTEX_H
