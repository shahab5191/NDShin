#pragma once
#include "ray.h"

class Shape {
public:
  Shape() : color(0.7f, 0.3f, 0.3f) {}
  virtual ~Shape() = default;
  virtual bool intersect(const Ray &ray, float &t) const = 0;

  glm::vec3 getColor() const { return color; }
  void setColor(const glm::vec3 &color) { this->color = color; }

  virtual glm::vec3 getNormal(const glm::vec3 &point) const = 0;

private:
  glm::vec3 color;
};
