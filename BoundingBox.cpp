#include "BoundingBox.h"

BoundingBox::BoundingBox(glm::vec3 point)
{
  init(point);
}

glm::vec3 BoundingBox::center() const
{
  return glm::mix(minPoint, maxPoint, 0.5f);
}

float BoundingBox::diagonal() const
{
  return glm::length(minPoint - maxPoint);
}

float BoundingBox::radius() const
{
  return diagonal() / 2.0f;
}

void BoundingBox::init(const glm::vec3 point)
{
  minPoint = maxPoint = point;
}

void BoundingBox::includeAnotherPoint(const glm::vec3 point)
{
  minPoint = glm::min(minPoint, point);
  maxPoint = glm::max(maxPoint, point);
}

glm::mat4 BoundingBox::centering() const
{
  return glm::scale(glm::vec3(1.0f / radius())) * glm::translate(-center());
}

void BoundingBox::printf() const
{
  std::cout << "min " << glm::to_string(minPoint) << std::endl;
  std::cout << "max " << glm::to_string(minPoint) << std::endl;
  std::cout << "center " << glm::to_string(center()) << std::endl;
  std::cout << "radius " << radius() << std::endl;
}
