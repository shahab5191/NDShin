#pragma once
#include "ray.h"
#include "shape.h"
#include <cmath>
#include <glm/glm.hpp>

// An infinite flat 3-plane in 4D (the analogue of a plane in 3D), defined by a
// point on it and a unit normal. Handy as a floor or backdrop.
class Hyperplane : public Shape {
public:
  Hyperplane(const glm::vec4 &point, const glm::vec4 &normal)
      : Shape(), normal(glm::normalize(normal)),
        d(glm::dot(glm::normalize(normal), point)) {}

  bool intersect(const Ray &ray, float &t) const override {
    const float denom = glm::dot(normal, ray.direction);
    if (std::fabs(denom) < 1e-8f) {
      return false; // ray runs parallel to the plane
    }
    const float tt = (d - glm::dot(normal, ray.origin)) / denom;
    if (tt > 1e-4f) {
      t = tt;
      return true;
    }
    return false; // plane is behind the ray
  }

  // The normal is constant everywhere on the plane.
  glm::vec4 getNormal(const glm::vec4 &) const override { return normal; }

private:
  glm::vec4 normal; // unit normal
  float d;          // plane offset: dot(normal, x) == d for points on the plane
};
