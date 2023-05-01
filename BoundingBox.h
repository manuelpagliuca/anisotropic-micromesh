#pragma once

#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <iostream>

// Axis-Aligned Bounding Box
class BoundingBox
{
public:
  BoundingBox() {}
  BoundingBox(glm::vec3 point);
  glm::vec3 center() const;
  float diagonal() const;
  float radius() const;

  void init(const glm::vec3 point);
  void includeAnotherPoint(const glm::vec3 point);
  glm::mat4 centering() const;

  void printf() const;

private:
  glm::vec3 minPoint;
  glm::vec3 maxPoint;
};
