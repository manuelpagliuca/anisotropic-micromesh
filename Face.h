#ifndef FACE_H
#define FACE_H

#include <QtCore/QString>
#include <GLM/glm.hpp>

struct Face
{
  uint index[3];
  uint edgesIndices[3]; // 0, 1 and 2
  glm::vec3 norm;
  float xMiddle;

  bool operator<(const Face& other) const {
    return xMiddle < other.xMiddle;
  }
};

#endif // FACE_H
