#include "camera3d.h"
#include <algorithm>
#include <cmath>
#include <glm/gtc/constants.hpp>

void Camera3D::orbit(float d_azimuth, float d_elevation) {
  azimuth += d_azimuth;
  elevation += d_elevation;
  // Stop just short of the poles to avoid the up-vector degenerating.
  const float lim = glm::radians(89.0f);
  elevation = std::clamp(elevation, -lim, lim);
  update();
}

void Camera3D::zoom(float d_distance) {
  distance = std::clamp(distance + d_distance, 1.5f, 20.0f);
  update();
}

void Camera3D::update() {
  const float ce = std::cos(elevation), se = std::sin(elevation);
  const float ca = std::cos(azimuth), sa = std::sin(azimuth);
  // Standard spherical orbit around the target, world up is +Y.
  position = target + distance * glm::vec3(ce * sa, se, ce * ca);
  forward = glm::normalize(target - position);
  right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
  up = glm::cross(right, forward);
}
