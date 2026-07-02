#include "camera.h"

Camera::Camera()
    : position(0.0f, 0.0f, 0.0f), target(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0), fov(45.0f), aspectRatio(1.77778f), nearPlane(0.1f),
      farPlane(100.0f) {}

Camera::~Camera() {}

void Camera::setPosition(glm::vec3 position) {
  this->position = position;
  updateCameraVectors();
}

void Camera::setFOV(float fov) { this->fov = fov; }

void Camera::setAspectRatio(float aspectRatio) {
  this->aspectRatio = aspectRatio;
}

void Camera::setNearPlane(float nearPlane) { this->nearPlane = nearPlane; }

void Camera::setFarPlane(float farPlane) { this->farPlane = farPlane; }

glm::vec3 Camera::getPosition() const { return position; }

glm::vec3 Camera::getLookAt() const { return target; }

float Camera::getFOV() const { return fov; }

float Camera::getAspectRatio() const { return aspectRatio; }

float Camera::getNearPlane() const { return nearPlane; }

float Camera::getFarPlane() const { return farPlane; }

void Camera::setLookAt(const glm::vec3 &target) {
  this->target = target;
  updateCameraVectors();
}

void Camera::setUp(const glm::vec3 &up) {
  this->up = up;
  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  forward = glm::normalize(target - position);
  right = glm::normalize(glm::cross(forward, up));
}

glm::vec3 Camera::getForward() const { return forward; }
glm::vec3 Camera::getRight() const { return right; }
glm::vec3 Camera::getUp() const { return up; }
