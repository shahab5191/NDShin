#include "camera.h"
#include "math4.h"
#include <cmath>

Camera::Camera()
    : position(0.0f, 0.0f, 0.0f, 0.0f), target(0.0f, 0.0f, 0.0f, -1.0f),
      up_ref(0.0f, 1.0f, 0.0f, 0.0f), over_ref(0.0f, 0.0f, 1.0f, 0.0f),
      fov(45.0f) {
  updateCameraVectors();
}

Camera::~Camera() {}

void Camera::setPosition(const glm::vec4 &position) {
  this->position = position;
  updateCameraVectors();
}

void Camera::setLookAt(const glm::vec4 &target) {
  this->target = target;
  updateCameraVectors();
}

void Camera::setUp(const glm::vec4 &up) {
  this->up_ref = up;
  updateCameraVectors();
}

void Camera::setOver(const glm::vec4 &over) {
  this->over_ref = over;
  updateCameraVectors();
}

void Camera::setFOV(float fov) { this->fov = fov; }

// Build an orthonormal 4D frame from the position/target and the two reference
// up vectors, using the 4D (ternary) cross product. This is the standard
// Hollasch 4D-camera construction: each axis is the vector orthogonal to the
// three already fixed.
void Camera::updateCameraVectors() {
  forward = glm::normalize(target - position);
  right = glm::normalize(cross4(up_ref, over_ref, forward));
  up = glm::normalize(cross4(over_ref, forward, right));
  over = cross4(forward, right, up); // already unit length
}

// Rotate a in the a-b plane by `angle`, and b to stay orthogonal to it. Both
// stay unit length, so an orthonormal basis remains orthonormal.
void Camera::rotatePlane(glm::vec4 &a, glm::vec4 &b, float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  const glm::vec4 na = c * a + s * b;
  const glm::vec4 nb = -s * a + c * b;
  a = na;
  b = nb;
}

void Camera::reorthonormalize() {
  forward = glm::normalize(forward);
  right = glm::normalize(right - glm::dot(right, forward) * forward);
  up = glm::normalize(up - glm::dot(up, forward) * forward -
                      glm::dot(up, right) * right);
  over = cross4(forward, right, up);
}
