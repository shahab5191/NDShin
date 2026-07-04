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
  // Hoist per-frame invariants out of the per-pixel loop. The sensor is a 3D
  // box spanned by right/up/over, sitting one focal unit forward.
  const Camera &camera = scene->getCamera();
  const glm::vec4 origin = camera.getPosition();
  const glm::vec4 right = camera.getRight();
  const glm::vec4 up = camera.getUp();
  const glm::vec4 over = camera.getOver();
  const glm::vec4 forward = camera.getForward();
  const float scale = std::tan(glm::radians(camera.getFOV() / 2.0f));
  const float inv_depth = 2.0f * scale / depth;
  const float pz0 = (1.0f / depth - 1.0f) * scale; // pz at z == 0

  // One display pixel per (x, y). The columns are independent, so they split
  // across threads; each thread writes disjoint pixel_buffer indices.
#pragma omp parallel for schedule(dynamic) collapse(2)
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      const float px = (2.0f * (x + 0.5f) / width - 1.0f) * scale;
      const float py = (1.0f - 2.0f * (y + 0.5f) / height) * scale;
      // Ray direction (pre-normalization) = base + pz * over. Only the pz term
      // changes as we march the depth axis, so hoist the constant part.
      const glm::vec4 base = px * right + py * up + forward;

      glm::vec3 out(0.0f);
      // March the sensor depth (over axis) front to back. Hits are opaque, so
      // the first one occludes everything behind it -- stop there. Lighting is
      // therefore evaluated at most once per pixel.
      for (int z = 0; z < depth; ++z) {
        const float pz = pz0 + z * inv_depth;

        Ray ray;
        ray.origin = origin;
        ray.direction = glm::normalize(base + pz * over);

        float closest_t;
        Shape *closest_shape = findClosestIntersection(ray, closest_t);
        if (closest_shape) {
          glm::vec4 point = ray.at(closest_t);
          glm::vec4 normal = closest_shape->getNormal(point);
          out = closest_shape->getColor() * computeLighting(point, normal);
          break;
        }
      }

      const int i = (y * width + x) * 3;
      pixel_buffer[i + 0] = out.r;
      pixel_buffer[i + 1] = out.g;
      pixel_buffer[i + 2] = out.b;
    }
  }
}

const std::vector<float> &RayTracer::getPixelBuffer() const {
  return pixel_buffer;
}
