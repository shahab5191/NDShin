#include "scene.h"

void Scene::addShape(Shape *shape) { shapes.push_back(shape); }

const std::vector<Shape *> &Scene::getShapes() const { return shapes; }

void Scene::setCamera(Camera *cam) { camera = cam; }
const Camera &Scene::getCamera() const { return *camera; }
