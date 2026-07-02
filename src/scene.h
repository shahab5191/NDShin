#pragma once
#include "camera.h"
#include "shape.h"
#include <vector>

class Scene {
public:
  Scene() = default;
  Scene(const Scene &) = delete;
  void addShape(Shape *shape);
  const std::vector<Shape *> &getShapes() const;
  void setCamera(Camera *cam);
  const Camera &getCamera() const;

private:
  std::vector<Shape *> shapes;
  Camera *camera = nullptr;
};
