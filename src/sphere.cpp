#include "sphere.h"

bool Sphere::intersect(const Ray &ray, float &t) const {
  glm::vec3 oc = ray.origin - center;
  float a = glm::dot(ray.direction, ray.direction);
  float b = 2.0f * glm::dot(oc, ray.direction);
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return false; // No intersection
  }

  float sqrt_discriminant = sqrt(discriminant);
  // t1 <= t2 always, since sqrt_discriminant >= 0
  float t1 = (-b - sqrt_discriminant) / (2.0f * a);
  float t2 = (-b + sqrt_discriminant) / (2.0f * a);

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
