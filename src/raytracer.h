#pragma once
#include "light.h"
#include "scene.h"
#include "shape.h"
#include <glm/glm.hpp>
#include <vector>

class RayTracer {
public:
  RayTracer(Scene *scene, int width, int height, int depth)
      : width(width), height(height), depth(depth), scene(scene) {
    pixel_buffer.resize(width * height * 3, 0.0f);
  }
  RayTracer(const RayTracer &) = delete;

  // For each display pixel, march the sensor's depth axis and keep the nearest
  // opaque hit (misses are transparent). This fuses the old volume-render +
  // composite passes so lighting is computed only for the visible surface and
  // occluded voxels behind a hit are never traced.
  void render();
  const std::vector<float> &getPixelBuffer() const;

private:
  int width;
  int height;
  int depth;
  std::vector<float> pixel_buffer; // RGB for display
  Scene *scene;

  Shape *findClosestIntersection(const Ray &ray, float &closest_t);
  glm::vec3 computeLighting(const glm::vec4 &point, const glm::vec4 &normal);
  bool isInShadow(const glm::vec4 &point, const glm::vec4 &light_dir,
                  const Light &light);
};
