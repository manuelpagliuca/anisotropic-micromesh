#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

// Axis-Aligned Bounding Box
class BoundingBox
{
public:
  BoundingBox() {}
  BoundingBox(vec3 point);
  vec3 center() const;
  float diagonal() const;
  float radius() const;

  void init(const vec3 point);
  void includeAnotherPoint(const vec3 point);
  mat4 centering() const;

  void printf() const;

private:
  vec3 minPoint;
  vec3 maxPoint;
};

#endif // BOUNDING_BOX_H
