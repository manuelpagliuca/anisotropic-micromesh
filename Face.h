#ifndef FACE_H
#define FACE_H

#include <QtCore/QString>
#include <GLM/glm.hpp>

struct Face
{
    uint index[3];
    uint edgesIndices[3]; // 0, 1 and 2
    glm::vec3 norm;
    float posMiddle;

    bool operator<(const Face &other) const
    {
        return posMiddle < other.posMiddle;
    }

    bool operator==(const Face &f) const
    {
        return f.index[0] == index[0] && f.index[1] == index[1] &&f.index[2] == index[2];
    }

};

namespace std
{
template <>
struct hash<Face>
{
    size_t operator()(const Face &f) const
    {
        size_t hash = 0;

        hash = std::hash<int>()(f.norm.x) ^ std::hash<int>()(f.norm.x) ^ std::hash<int>()(f.norm.x);
        hash = std::hash<int>()(f.posMiddle) ^ std::hash<int>()(f.posMiddle) ^ std::hash<int>()(f.posMiddle);
        hash = std::hash<int>()(f.index[0]) ^ std::hash<int>()(f.index[0]) ^ std::hash<int>()(f.index[0]);
        hash = std::hash<int>()(f.index[1]) ^ std::hash<int>()(f.index[1]) ^ std::hash<int>()(f.index[1]);
        hash = std::hash<int>()(f.index[2]) ^ std::hash<int>()(f.index[2]) ^ std::hash<int>()(f.index[2]);

        return hash;
    }
};
}

#endif // FACE_H
