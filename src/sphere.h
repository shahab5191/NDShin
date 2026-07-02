#pragma once
#include "glm/ext/vector_float3.hpp"
#include "ray.h"
#include "shape.h"

class Sphere : public Shape {
public:
  Sphere(const glm::vec3 &center, float radius)
      : Shape(), center(center), radius(radius) {}
  bool intersect(const Ray &ray, float &t) const override;

  glm::vec3 getCenter() const { return center; }
  float getRadius() const { return radius; }

  glm::vec3 getNormal(const glm::vec3 &point) const override {
    return glm::normalize(point - center);
  }

private:
  glm::vec3 center;
  float radius;
};
