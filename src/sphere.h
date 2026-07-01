#pragma once
#include "glm/ext/vector_float3.hpp"
#include "ray.h"
#include "shape.h"

class Sphere : public Shape {
public:
  Sphere(const glm::vec3 &center, float radius)
      : center(center), radius(radius) {}
  float intersect(const Ray &ray, float &t) const override;

private:
  glm::vec3 center;
  float radius;
};
