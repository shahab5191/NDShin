#pragma once
#include "camera.h"
#include "light.h"
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

  void addLight(const Light &light) { lights.push_back(light); }
  const std::vector<Light> &getLights() const { return lights; }

private:
  std::vector<Shape *> shapes;
  Camera *camera = nullptr;
  std::vector<Light> lights;
};
