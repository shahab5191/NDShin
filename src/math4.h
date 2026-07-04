#pragma once
#include <glm/glm.hpp>

// The 3D cross product takes two vectors and returns one orthogonal to both.
// Its 4D analogue takes *three* vectors and returns the vector orthogonal to
// all of them -- exactly what we need to build the camera's 3D sensor basis.
// Formula from Steven Hollasch, "Four-Space Visualization of 4D Objects".
inline glm::vec4 cross4(const glm::vec4 &u, const glm::vec4 &v,
                        const glm::vec4 &w) {
  const float A = v.x * w.y - v.y * w.x;
  const float B = v.x * w.z - v.z * w.x;
  const float C = v.x * w.w - v.w * w.x;
  const float D = v.y * w.z - v.z * w.y;
  const float E = v.y * w.w - v.w * w.y;
  const float F = v.z * w.w - v.w * w.z;

  return glm::vec4(u.y * F - u.z * E + u.w * D,   //
                   -u.x * F + u.z * C - u.w * B,  //
                   u.x * E - u.y * C + u.w * A,   //
                   -u.x * D + u.y * B - u.z * A);
}
