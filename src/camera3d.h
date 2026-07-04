#pragma once
#include <glm/glm.hpp>

// A conventional 3D camera that orbits a fixed target. It is used to *view* the
// 3D voxel cloud captured by the 4D camera: rotate it around the cloud to see
// the captured 4D world from any 3D angle. This is entirely separate from the
// 4D Camera -- moving one does not affect the other.
class Camera3D {
public:
  Camera3D() { update(); }

  void setTarget(const glm::vec3 &t) { target = t; update(); }
  void setAspect(float a) { aspect = a; }
  void setFOV(float f) { fov = f; }

  // Spin around the target (radians) / dolly in and out.
  void orbit(float d_azimuth, float d_elevation);
  void zoom(float d_distance);

  glm::vec3 getPosition() const { return position; }
  glm::vec3 getRight() const { return right; }
  glm::vec3 getUp() const { return up; }
  glm::vec3 getForward() const { return forward; }
  float getFOV() const { return fov; }
  float getAspect() const { return aspect; }

private:
  glm::vec3 target{0.0f};
  float azimuth = 0.6f;   // radians around the world-up (Y) axis
  float elevation = 0.4f; // radians above the horizon
  float distance = 4.0f;  // distance from target
  float fov = 45.0f;
  float aspect = 1.0f;

  glm::vec3 position{0.0f};
  glm::vec3 right{1, 0, 0};
  glm::vec3 up{0, 1, 0};
  glm::vec3 forward{0, 0, -1};

  void update();
};
