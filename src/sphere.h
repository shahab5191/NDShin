#pragma once
#include "ray.h"
#include "shape.h"
#include <glm/glm.hpp>

// A "sphere" in 4D is a glome (3-sphere): the set of points at a fixed distance
// from a 4D centre. The intersection maths is dimension-agnostic, so this is
// just the 3D sphere with vec4 in place of vec3.
class Sphere : public Shape {
public:
  Sphere(const glm::vec4 &center, float radius)
      : Shape(), center(center), radius(radius) {}
  bool intersect(const Ray &ray, float &t) const override;

  glm::vec4 getCenter() const { return center; }
  void setCenter(const glm::vec4 &center) { this->center = center; }
  float getRadius() const { return radius; }

  glm::vec4 getNormal(const glm::vec4 &point) const override {
    return glm::normalize(point - center);
  }

private:
  glm::vec4 center;
  float radius;
};
