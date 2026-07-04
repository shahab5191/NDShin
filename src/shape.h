#pragma once
#include "ray.h"
#include <glm/glm.hpp>

class Shape {
public:
  Shape() : color(0.7f, 0.3f, 0.3f) {}
  virtual ~Shape() = default;
  virtual bool intersect(const Ray &ray, float &t) const = 0;

  glm::vec3 getColor() const { return color; }
  void setColor(const glm::vec3 &color) { this->color = color; }

  // Surface normal at a point, in 4D. Colour stays RGB (vec3): the extra
  // dimension is geometric, the material is still just a colour.
  virtual glm::vec4 getNormal(const glm::vec4 &point) const = 0;

private:
  glm::vec3 color;
};
