#include "camera.h"

Camera::Camera()
    : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), fov(45.0f),
      aspectRatio(1.77778f), nearPlane(0.1f), farPlane(100.0f) {}

Camera::~Camera() {}

void Camera::setPosition(float x, float y, float z) {
  position = glm::vec3(x, y, z);
}

void Camera::setRotation(float pitch, float yaw, float roll) {
  rotation = glm::vec3(pitch, yaw, roll);
}

void Camera::setFOV(float fov) { this->fov = fov; }

void Camera::setAspectRatio(float aspectRatio) {
  this->aspectRatio = aspectRatio;
}

void Camera::setNearPlane(float nearPlane) { this->nearPlane = nearPlane; }

void Camera::setFarPlane(float farPlane) { this->farPlane = farPlane; }

glm::vec3 Camera::getPosition() const { return position; }

glm::vec3 Camera::getRotation() const { return rotation; }

float Camera::getFOV() const { return fov; }

float Camera::getAspectRatio() const { return aspectRatio; }

float Camera::getNearPlane() const { return nearPlane; }

float Camera::getFarPlane() const { return farPlane; }
