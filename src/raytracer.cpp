#include "raytracer.h"
#include "camera.h"
#include "light.h"
#include "shape.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

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

bool RayTracer::isInShadow(const glm::vec4 &point, const glm::vec4 &light_dir,
                           const Light &light) {
  const std::vector<Shape *> &shapes = scene->getShapes();
  float light_distance = glm::length(light.getPosition() - point);
  Ray shadow_ray(point + 0.001f * light_dir, light_dir);

  for (const auto &shape : shapes) {
    float t;
    if (shape->intersect(shadow_ray, t) && t < light_distance) {
      return true; // Point is in shadow
    }
  }
  return false; // Point is not in shadow
}

glm::vec3 RayTracer::computeLighting(const glm::vec4 &point,
                                     const glm::vec4 &normal) {
  glm::vec3 color(0.0f);
  for (const auto &light : scene->getLights()) {
    glm::vec4 light_dir = glm::normalize(light.getPosition() - point);
    if (this->isInShadow(point, light_dir, light)) {
      continue; // Skip this light if the point is in shadow
    }
    // The Lambert term is just a dot product, which is dimension-agnostic:
    // the normal and light direction are now vec4, everything else is the same.
    float diff = std::max(glm::dot(normal, light_dir), 0.0f);
    color += diff * light.getColor() * light.getIntensity();
  }
  return color;
}

// Stage 1: the 4D camera casts one ray per voxel of its 3D sensor into the 4D
// world and stores the lit result. The output is a 3D RGBA volume -- a snapshot
// of the 4D scene from the current 4D viewpoint.
void RayTracer::capture() {
  const Camera &camera = scene->getCamera();
  const glm::vec4 origin = camera.getPosition();
  const glm::vec4 right = camera.getRight();
  const glm::vec4 up = camera.getUp();
  const glm::vec4 over = camera.getOver();
  const glm::vec4 forward = camera.getForward();
  const float scale = std::tan(glm::radians(camera.getFOV() / 2.0f));

  // The sensor is a cube [-scale, scale]^3 sampled at the voxel resolution;
  // a coarser axis just samples the same cube more sparsely.
#pragma omp parallel for schedule(dynamic) collapse(2)
  for (int z = 0; z < voxel_d; ++z) {
    for (int y = 0; y < voxel_h; ++y) {
      const float pz = (2.0f * (z + 0.5f) / voxel_d - 1.0f) * scale;
      const float py = (1.0f - 2.0f * (y + 0.5f) / voxel_h) * scale;
      const glm::vec4 pyz = py * up + pz * over + forward;
      for (int x = 0; x < voxel_w; ++x) {
        const float px = (2.0f * (x + 0.5f) / voxel_w - 1.0f) * scale;

        Ray ray;
        ray.origin = origin;
        ray.direction = glm::normalize(px * right + pyz);

        float closest_t;
        Shape *closest_shape = findClosestIntersection(ray, closest_t);

        const int vi = (z * voxel_h + y) * voxel_w + x;
        if (closest_shape) {
          const glm::vec4 point = ray.at(closest_t);
          const glm::vec4 normal = closest_shape->getNormal(point);
          const glm::vec3 color =
              closest_shape->getColor() * computeLighting(point, normal);
          voxel_buffer[vi] = glm::vec4(color, 1.0f); // occupied
        } else {
          voxel_buffer[vi] = glm::vec4(0.0f); // empty
        }
      }
    }
  }
}

// Stage 2: a conventional 3D camera orbits the voxel cloud (mapped to the box
// [-1, 1]^3) and ray-marches it, alpha-compositing occupied voxels front to
// back. This is the 2D image the window shows.
void RayTracer::renderView(const Camera3D &view) {
  const glm::vec3 origin = view.getPosition();
  const glm::vec3 right = view.getRight();
  const glm::vec3 up = view.getUp();
  const glm::vec3 forward = view.getForward();
  const float aspect = view.getAspect();
  const float scale = std::tan(glm::radians(view.getFOV() / 2.0f));

  // Step a bit finer than one voxel so we don't skip any. We composite only on
  // entering a new voxel, so the step count doesn't affect the density.
  const int max_dim = std::max({voxel_w, voxel_h, voxel_d});
  const float dt = 1.0f / static_cast<float>(max_dim);

#pragma omp parallel for schedule(dynamic) collapse(2)
  for (int y = 0; y < view_h; ++y) {
    for (int x = 0; x < view_w; ++x) {
      const float sx = (2.0f * (x + 0.5f) / view_w - 1.0f) * aspect * scale;
      const float sy = (1.0f - 2.0f * (y + 0.5f) / view_h) * scale;
      const glm::vec3 dir = glm::normalize(sx * right + sy * up + forward);

      // Clip the ray to the [-1, 1]^3 cloud box (3D slab method).
      float t0 = 0.0f;
      float t1 = std::numeric_limits<float>::max();
      bool miss = false;
      for (int a = 0; a < 3; ++a) {
        if (std::fabs(dir[a]) < 1e-8f) {
          if (origin[a] < -1.0f || origin[a] > 1.0f) {
            miss = true;
            break;
          }
          continue;
        }
        const float inv = 1.0f / dir[a];
        float ta = (-1.0f - origin[a]) * inv;
        float tb = (1.0f - origin[a]) * inv;
        if (ta > tb) {
          std::swap(ta, tb);
        }
        t0 = std::max(t0, ta);
        t1 = std::min(t1, tb);
        if (t0 > t1) {
          miss = true;
          break;
        }
      }

      glm::vec3 accum(0.0f);
      if (!miss) {
        float transmittance = 1.0f;
        int last_idx = -1;
        for (float t = t0; t <= t1; t += dt) {
          const glm::vec3 p = origin + t * dir; // inside [-1, 1]^3
          int ix = static_cast<int>((p.x * 0.5f + 0.5f) * voxel_w);
          int iy = static_cast<int>((p.y * 0.5f + 0.5f) * voxel_h);
          int iz = static_cast<int>((p.z * 0.5f + 0.5f) * voxel_d);
          ix = std::clamp(ix, 0, voxel_w - 1);
          iy = std::clamp(iy, 0, voxel_h - 1);
          iz = std::clamp(iz, 0, voxel_d - 1);
          const int idx = (iz * voxel_h + iy) * voxel_w + ix;
          if (idx == last_idx) {
            continue; // still in the same voxel -> count it once
          }
          last_idx = idx;

          const glm::vec4 v = voxel_buffer[idx];
          if (v.a > 0.0f) {
            accum += transmittance * density * glm::vec3(v);
            transmittance *= (1.0f - density);
            if (transmittance <= 0.01f) {
              break; // cloud is opaque from here on
            }
          }
        }
      }

      const int i = (y * view_w + x) * 3;
      pixel_buffer[i + 0] = accum.r;
      pixel_buffer[i + 1] = accum.g;
      pixel_buffer[i + 2] = accum.b;
    }
  }
}

const std::vector<float> &RayTracer::getPixelBuffer() const {
  return pixel_buffer;
}
