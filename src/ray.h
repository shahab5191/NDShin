#pragma once

#include <glm/glm.hpp>
#include <glm/geometric.hpp>

class Ray {
public:
  Ray() {}
  Ray(const glm::vec4 &origin, const glm::vec4 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}

  glm::vec4 at(float t) const { return origin + t * direction; }

  glm::vec4 origin;
  glm::vec4 direction;
};
