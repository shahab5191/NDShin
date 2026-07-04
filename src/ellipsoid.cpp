#include "ellipsoid.h"
#include <cmath>

bool Ellipsoid::intersect(const Ray &ray, float &t) const {
  // Warp into unit-sphere space (divide out the per-axis radii). The warped
  // direction is no longer unit length, so we keep the full quadratic (can't
  // drop `a` the way the unit-radius sphere does).
  const glm::vec4 o = (ray.origin - center) / radii;
  const glm::vec4 d = ray.direction / radii;

  const float a = glm::dot(d, d);
  const float half_b = glm::dot(o, d);
  const float c = glm::dot(o, o) - 1.0f;
  const float discriminant = half_b * half_b - a * c;

  if (discriminant < 0.0f) {
    return false;
  }

  const float sqrt_discriminant = std::sqrt(discriminant);
  const float t1 = (-half_b - sqrt_discriminant) / a;
  const float t2 = (-half_b + sqrt_discriminant) / a;

  if (t1 > 1e-4f) {
    t = t1;
    return true;
  }
  if (t2 > 1e-4f) {
    t = t2;
    return true;
  }
  return false;
}
