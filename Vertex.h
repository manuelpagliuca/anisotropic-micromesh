#ifndef VERTEX_H
#define VERTEX_H

#include <QtCore/QString>
#include <GLM/glm.hpp>

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 tex;

  bool operator==(const Vertex &v) const {
    return v.pos == pos && v.norm == norm && v.tex == tex;
  }
};

#endif // VERTEX_H
