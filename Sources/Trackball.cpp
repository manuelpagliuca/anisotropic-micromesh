#include "Headers/Trackball.h"
#include <iostream>

TrackBall::TrackBall() {}

void TrackBall::trackMousePositions(QPoint currPos, int openGLWWidth, int openGLWHeight)
{
  op1 = getTrackBallVector(prevPos, openGLWWidth, openGLWHeight);
  op2 = getTrackBallVector(currPos, openGLWWidth, openGLWHeight);

  if (op1 != op2) {
    rotAngle = std::acosf(std::min(1.0f, glm::dot(op1, op2))) * sensitivity;
    rotAxis = glm::cross(op1, op2);
    prevPos = currPos;
    rotation = true;
  }
}

void TrackBall::setFistClick(QPoint initialPos)
{
  prevPos = initialPos;
}

glm::mat4 TrackBall::getRotationMatrix()
{
  if (rotation) {
    rotation = false;
    return glm::rotate(glm::degrees(rotAngle), rotAxis);
  }
  else return glm::mat4(1.f);
}

void TrackBall::trackWheelIncrement(float angleDelta)
{
  if (angleDelta > 0.f) scalingFactor *= 1.1f;
  else if (angleDelta < 0.f) scalingFactor /= 1.1f;
  else scalingFactor = 1.f;
}

glm::mat4 TrackBall::getScalingMatrix()
{
  float scale = 1.f;

  if (scalingFactor != 1.f) {
    scale = scalingFactor;
    scalingFactor = 1.f;
  }

  return glm::scale(glm::vec3(scale));
}

void TrackBall::printf() const
{
  qDebug() << "Rotation angle: " << rotAngle << Qt::endl
           << "Rotation axis: " << glm::to_string(rotAxis).c_str();
}

glm::vec3 TrackBall::getTrackBallVector(QPoint clickedPoint, int openGLWWidth, int openGLWHeight)
{
  float xUnit = (1.f * clickedPoint.x() / openGLWWidth * 2.f) - 1.f;
  float yUnit = (1.f * clickedPoint.y() / openGLWHeight * 2.f) - 1.f;
  glm::vec3 originPoint = glm::vec3(-xUnit, yUnit, 0.f);

  float xySquared = std::powf(originPoint.x, 2.f) + std::powf(originPoint.y, 2.f);

  if (xySquared <= 1.f * 1.f)
    originPoint.z = std::sqrtf(1.f * 1.f - xySquared);
  else
    originPoint = glm::normalize(originPoint);

  return originPoint;
}
