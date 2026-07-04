#pragma once
#include "ray.h"
#include "shape.h"
#include <glm/glm.hpp>

// A glome with a different radius along each axis (a 4D ellipsoid). We solve it
// by warping the ray into the space where the ellipsoid is a unit sphere,
// intersecting there, then mapping back.
class Ellipsoid : public Shape {
public:
  Ellipsoid(const glm::vec4 &center, const glm::vec4 &radii)
      : Shape(), center(center), radii(radii) {}

  bool intersect(const Ray &ray, float &t) const override;

  glm::vec4 getNormal(const glm::vec4 &point) const override {
    // Gradient of ((p - c) / r)^2: points outward, scaled by 1/r^2 per axis.
    return glm::normalize((point - center) / (radii * radii));
  }

  glm::vec4 getCenter() const { return center; }

private:
  glm::vec4 center;
  glm::vec4 radii;
};
