#include "sphere.h"

float Sphere::intersect(const Ray &ray, float &t) const {
  glm::vec3 oc = ray.origin - center;
  float a = glm::dot(ray.direction, ray.direction);
  float b = 2.0f * glm::dot(oc, ray.direction);
  float c = glm::dot(oc, oc) - radius * radius;
  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0f; // No intersection
  } else {
    float sqrt_discriminant = sqrt(discriminant);
    float t1 = (-b - sqrt_discriminant) / (2.0f * a);
    float t2 = (-b + sqrt_discriminant) / (2.0f * a);

    // Return the closest positive intersection
    if (t1 > 0 && t2 > 0) {
      t = std::min(t1, t2);
      return t;
    } else if (t1 > 0) {
      t = t1;
      return t;
    } else if (t2 > 0) {
      t = t2;
      return t;
    } else {
      return -1.0f; // Both intersections are behind the ray origin
    }
  }
}
