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

  // For each display pixel, march the sensor's depth axis front to back and
  // alpha-composite every voxel whose ray hits a shape. Each hit is only
  // partially opaque (see density), so voxels behind it still show through --
  // giving a cloud-like look that reveals the interior of the 3D sensor volume.
  void render();
  const std::vector<float> &getPixelBuffer() const;

  // Opacity deposited by a single hit voxel, in [0, 1]. Lower = wispier / more
  // see-through; higher = denser / closer to solid surfaces.
  void setDensity(float d) { density = d; }

private:
  int width;
  int height;
  int depth;
  float density = 0.15f;
  std::vector<float> pixel_buffer; // RGB for display
  Scene *scene;

  Shape *findClosestIntersection(const Ray &ray, float &closest_t);
  glm::vec3 computeLighting(const glm::vec4 &point, const glm::vec4 &normal);
  bool isInShadow(const glm::vec4 &point, const glm::vec4 &light_dir,
                  const Light &light);
};
