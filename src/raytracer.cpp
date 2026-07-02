#include "raytracer.h"
#include "shape.h"
#include <vector>

glm::vec3 getRayDirection(const Camera &camera, int x, int y, int width,
                          int height) {
  float aspectRatio = camera.getAspectRatio();
  float fov = camera.getFOV();
  auto px = float((2.0f * (x + 0.5f) / width - 1.0f) * aspectRatio *
                  tan(glm::radians(fov / 2.0f))) *
            camera.getRight();
  auto py = float((1.0f - 2.0f * (y + 0.5f) / height) *
                  tan(glm::radians(fov / 2.0f))) *
            camera.getUp();
  return glm::normalize(px + py + camera.getForward());
}

Shape *RayTracer::findClosestIntersection(const Ray &ray, float &closest_t) {
  Shape *closest_shape = nullptr;
  closest_t = std::numeric_limits<float>::max();
  for (const auto &shape : scene->getShapes()) {
    float t;
    if (shape->intersect(ray, t) && t < closest_t) {
      closest_t = t;
      closest_shape = shape;
    }
  }
  return closest_shape;
}

void RayTracer::render() {
  auto camera = scene->getCamera();
  auto shapes = scene->getShapes();
  Ray ray;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      ray.origin = camera.getPosition();
      ray.direction = getRayDirection(camera, x, y, width, height);
      float closest_t;
      auto closest_shape = findClosestIntersection(ray, closest_t);
      int pixelbuffer_index = (y * width + x) * 3;
      pixel_buffer[pixelbuffer_index] =
          closest_shape ? closest_shape->getColor().r : 0.0f;
      pixel_buffer[pixelbuffer_index + 1] =
          closest_shape ? closest_shape->getColor().g : 0.0f;
      pixel_buffer[pixelbuffer_index + 2] =
          closest_shape ? closest_shape->getColor().b : 0.0f;
    }
  }
}

std::vector<float> RayTracer::getPixelBuffer() const { return pixel_buffer; }
