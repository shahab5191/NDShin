#pragma once
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>

class Camera {
public:
  Camera();
  ~Camera();

  void setPosition(float x, float y, float z);
  void setRotation(float pitch, float yaw, float roll);
  void setFOV(float fov);
  void setAspectRatio(float aspectRatio);
  void setNearPlane(float nearPlane);
  void setFarPlane(float farPlane);

  glm::vec3 getPosition() const;
  glm::vec3 getRotation() const;
  float getFOV() const;
  float getAspectRatio() const;
  float getNearPlane() const;
  float getFarPlane() const;

private:
  glm::vec3 position;
  glm::vec3 rotation;
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;
};
