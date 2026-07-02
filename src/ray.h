#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

class Ray {
public:
  Ray() {}
  Ray(const glm::vec3 &origin, const glm::vec3 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}

  glm::vec3 at(float t) const { return origin + t * direction; }

  glm::vec3 origin;
  glm::vec3 direction;
};
