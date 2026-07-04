#pragma once
#include "camera3d.h"
#include "light.h"
#include "scene.h"
#include "shape.h"
#include <glm/glm.hpp>
#include <vector>

// Two-stage 4D renderer:
//   1. capture()     -- the 4D camera casts one ray per voxel and stores a lit
//                       RGBA volume: a 3D snapshot of the 4D world.
//   2. renderView()  -- a separate 3D camera orbits that voxel cloud and
//                       ray-marches it to the 2D display image.
// The stages are decoupled: re-capture only when the 4D camera moves, but
// re-render the view every frame so orbiting stays cheap.
class RayTracer {
public:
  RayTracer(Scene *scene, int voxel_w, int voxel_h, int voxel_d, int view_w,
            int view_h)
      : voxel_w(voxel_w), voxel_h(voxel_h), voxel_d(voxel_d), view_w(view_w),
        view_h(view_h), scene(scene) {
    voxel_buffer.resize(voxel_w * voxel_h * voxel_d, glm::vec4(0.0f));
    pixel_buffer.resize(view_w * view_h * 3, 0.0f);
  }
  RayTracer(const RayTracer &) = delete;

  void capture();                        // stage 1: fill the voxel volume
  void renderView(const Camera3D &view); // stage 2: ray-march it to the screen

  // Opacity deposited per occupied voxel while marching the view, in [0, 1].
  void setDensity(float d) { density = d; }
  const std::vector<float> &getPixelBuffer() const;

private:
  int voxel_w, voxel_h, voxel_d;
  int view_w, view_h;
  float density = 0.15f;
  std::vector<glm::vec4> voxel_buffer; // RGBA per voxel (alpha 0 = empty)
  std::vector<float> pixel_buffer;     // RGB for display
  Scene *scene;

  Shape *findClosestIntersection(const Ray &ray, float &closest_t);
  glm::vec3 computeLighting(const glm::vec4 &point, const glm::vec4 &normal);
  bool isInShadow(const glm::vec4 &point, const glm::vec4 &light_dir,
                  const Light &light);
};
