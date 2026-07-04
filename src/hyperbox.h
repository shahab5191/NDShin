#pragma once
#include "ray.h"
#include "shape.h"
#include <glm/glm.hpp>

// An axis-aligned 4-box (hyperrectangle). A tesseract is the special case where
// all four half-extents are equal. Intersection uses the slab method, which
// generalises to any number of dimensions -- here we just clip against 4 pairs
// of parallel bounding hyperplanes instead of 3.
class Hyperbox : public Shape {
public:
  Hyperbox(const glm::vec4 &center, const glm::vec4 &half_extents)
      : Shape(), min_bound(center - half_extents),
        max_bound(center + half_extents) {}

  bool intersect(const Ray &ray, float &t) const override;
  glm::vec4 getNormal(const glm::vec4 &point) const override;

  glm::vec4 getCenter() const { return 0.5f * (min_bound + max_bound); }

private:
  glm::vec4 min_bound;
  glm::vec4 max_bound;
};
