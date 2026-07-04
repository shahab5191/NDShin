#include "raytracer.h"
#include "camera.h"
#include "light.h"
#include "shape.h"
#include <cmath>
#include <limits>
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

void RayTracer::render() {
  // Hoist per-frame invariants out of the per-voxel loop. The sensor is a 3D
  // box spanned by right/up/over, sitting one focal unit forward.
  const Camera &camera = scene->getCamera();
  const glm::vec4 origin = camera.getPosition();
  const glm::vec4 right = camera.getRight();
  const glm::vec4 up = camera.getUp();
  const glm::vec4 over = camera.getOver();
  const glm::vec4 forward = camera.getForward();
  const float scale = std::tan(glm::radians(camera.getFOV() / 2.0f));

  // One ray per voxel. The voxels are independent, so the (z, y) planes can be
  // split across threads; writes target disjoint voxel_buffer indices.
#pragma omp parallel for schedule(dynamic) collapse(2)
  for (int z = 0; z < depth; ++z) {
    for (int y = 0; y < height; ++y) {
      // The sensor region is a cube [-scale, scale]^3, sampled at the voxel
      // resolution -- a coarser depth axis just samples the same cube sparser,
      // so hyperspheres stay round rather than getting stretched.
      const float pz = (2.0f * (z + 0.5f) / depth - 1.0f) * scale;
      const float py = (1.0f - 2.0f * (y + 0.5f) / height) * scale;
      const glm::vec4 pyz = py * up + pz * over + forward;
      for (int x = 0; x < width; ++x) {
        const float px = (2.0f * (x + 0.5f) / width - 1.0f) * scale;

        Ray ray;
        ray.origin = origin;
        ray.direction = glm::normalize(px * right + pyz);

        float closest_t;
        Shape *closest_shape = findClosestIntersection(ray, closest_t);

        const int vi = (z * height + y) * width + x;
        if (closest_shape) {
          glm::vec4 point = ray.at(closest_t);
          glm::vec4 normal = closest_shape->getNormal(point);
          glm::vec3 lighting = computeLighting(point, normal);
          glm::vec3 color = closest_shape->getColor() * lighting;
          voxel_buffer[vi] = glm::vec4(color, 1.0f); // opaque hit
        } else {
          voxel_buffer[vi] = glm::vec4(0.0f); // transparent miss
        }
      }
    }
  }

  compositeVolume();
}

// Flatten the RGBA voxel volume into the 2D display image by alpha-compositing
// along the depth (over) axis, front to back. Missed voxels are transparent so
// they contribute nothing; opaque hits stop the accumulation, so this shows the
// nearest lit surface along the sensor's depth.
void RayTracer::compositeVolume() {
#pragma omp parallel for schedule(dynamic)
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      glm::vec3 accum(0.0f);
      float transmittance = 1.0f; // how much light still gets through
      for (int z = 0; z < depth; ++z) {
        const glm::vec4 v = voxel_buffer[(z * height + y) * width + x];
        accum += transmittance * v.a * glm::vec3(v);
        transmittance *= (1.0f - v.a);
        if (transmittance <= 0.001f) {
          break; // fully opaque -- nothing behind this voxel is visible
        }
      }
      const int i = (y * width + x) * 3;
      pixel_buffer[i + 0] = accum.r;
      pixel_buffer[i + 1] = accum.g;
      pixel_buffer[i + 2] = accum.b;
    }
  }
}

const std::vector<float> &RayTracer::getPixelBuffer() const {
  return pixel_buffer;
}
