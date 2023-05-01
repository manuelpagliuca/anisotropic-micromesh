#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <cmath>

#include <QVector3D>
#include <QDebug>

#include <GLM/vec3.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <GLM/gtx/transform.hpp>

class TrackBall
{
public:
  TrackBall();
  void setFistClick(QPoint point);
  void trackMousePositions(QPoint currPos, int openGLWWidth, int openGLWHeight);
  void trackWheelIncrement(float angleDelta);
  glm::mat4 getRotationMatrix();
  glm::mat4 getScalingMatrix();

  void printf() const;

private:
  glm::vec3 getTrackBallVector(QPoint pos, int openGLWWidth, int openGLWHeight);
  glm::vec3 op1, op2;
  QPoint prevPos;
  glm::vec3 rotAxis;
  float rotAngle = 0.0f;
  float sensitivity = 0.05f;
  bool rotation = false;

  float scalingFactor = 1.0f;
};

#endif // TRACKBALL_H
