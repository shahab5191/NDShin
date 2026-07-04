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
    voxel_buffer.resize(width * height * depth, glm::vec4(0.0f));
    pixel_buffer.resize(width * height * 3, 0.0f);
  }
  RayTracer(const RayTracer &) = delete;

  // Cast one ray per voxel of the 3D sensor, then flatten the RGBA volume into
  // the 2D pixel buffer that the window displays.
  void render();
  const std::vector<float> &getPixelBuffer() const;

private:
  int width;
  int height;
  int depth;
  std::vector<glm::vec4> voxel_buffer; // RGBA per voxel (alpha 0 = ray missed)
  std::vector<float> pixel_buffer;     // RGB, composited for display
  Scene *scene;

  Shape *findClosestIntersection(const Ray &ray, float &closest_t);
  glm::vec3 computeLighting(const glm::vec4 &point, const glm::vec4 &normal);
  bool isInShadow(const glm::vec4 &point, const glm::vec4 &light_dir,
                  const Light &light);
  void compositeVolume();
};
