#pragma once
#include "glm/ext/vector_float3.hpp"
#include "scene.h"
#include "shape.h"
#include <vector>

class RayTracer {
public:
  RayTracer(Scene *scene, int width, int height)
      : scene(scene), width(width), height(height) {
    pixel_buffer.resize(width * height * 3, 0.0f);
  }
  RayTracer(const RayTracer &) = delete;
  void render();
  const std::vector<float> &getPixelBuffer() const;

private:
  int width;
  int height;
  std::vector<float> pixel_buffer;
  Scene *scene;
  Shape *findClosestIntersection(const Ray &ray, float &closest_t);

  glm::vec3 computeLighting(const glm::vec3 &point, const glm::vec3 &normal);
  bool isInShadow(const glm::vec3 &point, glm::vec3 &light_dir,
                  const Light &light);
};
