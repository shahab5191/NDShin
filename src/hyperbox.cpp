#include "hyperbox.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

bool Hyperbox::intersect(const Ray &ray, float &t) const {
  // Slab method: for each of the 4 axes the box is bounded by two parallel
  // hyperplanes; the ray is inside the box only where all 4 slab intervals
  // overlap. t_near is the last entry, t_far is the first exit.
  float t_near = -std::numeric_limits<float>::max();
  float t_far = std::numeric_limits<float>::max();

  for (int a = 0; a < 4; ++a) {
    const float o = ray.origin[a];
    const float dir = ray.direction[a];
    if (std::fabs(dir) < 1e-8f) {
      // Ray parallel to this slab: miss unless the origin is already between
      // the two bounding planes.
      if (o < min_bound[a] || o > max_bound[a]) {
        return false;
      }
      continue;
    }
    const float inv = 1.0f / dir;
    float t1 = (min_bound[a] - o) * inv;
    float t2 = (max_bound[a] - o) * inv;
    if (t1 > t2) {
      std::swap(t1, t2);
    }
    t_near = std::max(t_near, t1);
    t_far = std::min(t_far, t2);
    if (t_near > t_far) {
      return false; // slab intervals stopped overlapping -> no hit
    }
  }

  // Prefer the entry point; if the origin is inside the box, use the exit.
  if (t_near > 1e-4f) {
    t = t_near;
    return true;
  }
  if (t_far > 1e-4f) {
    t = t_far;
    return true;
  }
  return false; // box is entirely behind the ray
}

glm::vec4 Hyperbox::getNormal(const glm::vec4 &point) const {
  // The hit face is the one the point is closest to in units of half-extent:
  // pick the axis where |offset from centre| / half-extent is largest, then
  // the normal is that axis, signed toward the point.
  const glm::vec4 center = 0.5f * (min_bound + max_bound);
  const glm::vec4 half = 0.5f * (max_bound - min_bound);
  const glm::vec4 d = point - center;

  int axis = 0;
  float best = -1.0f;
  for (int a = 0; a < 4; ++a) {
    const float ratio = std::fabs(d[a]) / half[a];
    if (ratio > best) {
      best = ratio;
      axis = a;
    }
  }

  glm::vec4 n(0.0f);
  n[axis] = (d[axis] < 0.0f) ? -1.0f : 1.0f;
  return n;
}
