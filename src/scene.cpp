#include "scene.h"

void Scene::addShape(Shape *shape) { shapes.push_back(shape); }

const std::vector<Shape *> &Scene::getShapes() const { return shapes; }
