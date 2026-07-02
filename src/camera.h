#pragma once
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>

class Camera {
public:
  Camera();
  ~Camera();

  void setPosition(glm::vec3 position);
  void setUp(const glm::vec3 &up);
  void setLookAt(const glm::vec3 &target);
  void setFOV(float fov);
  void setAspectRatio(float aspectRatio);
  void setNearPlane(float nearPlane);
  void setFarPlane(float farPlane);

  glm::vec3 getPosition() const;
  glm::vec3 getUp() const;
  glm::vec3 getRight() const;
  glm::vec3 getForward() const;
  glm::vec3 getLookAt() const;
  float getFOV() const;
  float getAspectRatio() const;
  float getNearPlane() const;
  float getFarPlane() const;

private:
  glm::vec3 position;
  glm::vec3 target;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 forward;
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;

private:
  void updateCameraVectors();
};
