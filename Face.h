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

  bool operator<(const Face& other) const {
    return posMiddle < other.posMiddle;
  }
};

#endif // FACE_H
