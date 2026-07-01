#pragma once
#include "ray.h"

class Shape {
public:
  virtual ~Shape() = default;
  virtual float intersect(const Ray &ray, float &t) const = 0;
};
