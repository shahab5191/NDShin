#include "raytracer.h"
#include "light.h"
#include "shape.h"
#include <cmath>
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

bool RayTracer::isInShadow(const glm::vec3 &point, glm::vec3 &light_dir,
                           const Light &light) {
  std::vector<Shape *> shapes = scene->getShapes();
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

glm::vec3 RayTracer::computeLighting(const glm::vec3 &point, const glm::vec3 &normal) {
  glm::vec3 color(0.0f);
  for (const auto &light : scene->getLights()) {
    glm::vec3 light_dir = glm::normalize(light.getPosition() - point);
    if (this->isInShadow(point, light_dir, light)) {
      continue; // Skip this light if the point is in shadow
    }
    float diff = std::max(glm::dot(normal, light_dir), 0.0f);
    color += diff * light.getColor() * light.getIntensity();
  }
  return color;
}

void RayTracer::render() {
  // Hoist per-frame invariants out of the per-pixel loop: the camera basis and
  // fov scale are constant for the whole frame, so compute them once here
  // instead of ~480k times inside the loop.
  const Camera &camera = scene->getCamera();
  const glm::vec3 origin = camera.getPosition();
  const glm::vec3 right = camera.getRight();
  const glm::vec3 up = camera.getUp();
  const glm::vec3 forward = camera.getForward();
  const float aspect = camera.getAspectRatio();
  const float scale = std::tan(glm::radians(camera.getFOV() / 2.0f));
  const std::vector<Light> &lights = scene->getLights();

  // Each pixel is independent, so the rows can be split across threads. Writes
  // target disjoint pixel_buffer indices, so no synchronization is needed.
#pragma omp parallel for schedule(dynamic)
  for (int y = 0; y < height; ++y) {
    // py depends only on the row, so compute it once per row.
    const float py = (1.0f - 2.0f * (y + 0.5f) / height) * scale;
    const glm::vec3 py_up = py * up;
    for (int x = 0; x < width; ++x) {
      const float px = (2.0f * (x + 0.5f) / width - 1.0f) * aspect * scale;

      Ray ray;
      ray.origin = origin;
      ray.direction = glm::normalize(px * right + py_up + forward);

      float closest_t;
      Shape *closest_shape = findClosestIntersection(ray, closest_t);

      const int i = (y * width + x) * 3;

      if (closest_shape) {
        glm::vec3 intersection_point = ray.at(closest_t);
        glm::vec3 normal = closest_shape->getNormal(intersection_point);
        glm::vec3 lighting =
            computeLighting(intersection_point, normal);
        glm::vec3 color = closest_shape->getColor();
        color *= lighting;

        pixel_buffer[i + 0] = color.r;
        pixel_buffer[i + 1] = color.g;
        pixel_buffer[i + 2] = color.b;
      } else {
        pixel_buffer[i + 0] = 0.0f;
        pixel_buffer[i + 1] = 0.0f;
        pixel_buffer[i + 2] = 0.0f;
      }
    }
  }
}

const std::vector<float> &RayTracer::getPixelBuffer() const {
  return pixel_buffer;
}
