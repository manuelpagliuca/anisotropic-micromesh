#ifndef RAY_H
#define RAY_H

#include <GLM/glm.hpp>

class Ray
{
public:
    Ray();
    Ray(const glm::vec3 &o, const glm::vec3 &d) : origin(o), direction(d) {}

    bool intersectTriangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, float &t);

private:
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif // RAY_H
