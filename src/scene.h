#pragma once
#include "shape.h"
#include <vector>

class Scene {
public:
  Scene() = default;
  Scene(const Scene &) = delete;
  void addShape(Shape *shape);
  const std::vector<Shape *> &getShapes() const;

private:
  std::vector<Shape *> shapes;
};
