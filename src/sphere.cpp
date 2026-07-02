#include "sphere.h"

bool Sphere::intersect(const Ray &ray, float &t) const {
  glm::vec3 oc = ray.origin - center;
  // ray.direction is normalized, so a = dot(dir, dir) = 1. Using the reduced
  // quadratic (half_b = dot(oc, dir)) drops the 2s and 4s and a division.
  float half_b = glm::dot(oc, ray.direction);
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = half_b * half_b - c;

  if (discriminant < 0) {
    return false; // No intersection
  }

  float sqrt_discriminant = sqrt(discriminant);
  // t1 <= t2 always, since sqrt_discriminant >= 0
  float t1 = -half_b - sqrt_discriminant;
  float t2 = -half_b + sqrt_discriminant;

  // Return the closest intersection in front of the ray origin
  if (t1 > 0) {
    t = t1;
    return true;
  }
  if (t2 > 0) {
    t = t2;
    return true;
  }
  return false; // Both intersections are behind the ray origin
}
