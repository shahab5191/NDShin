#pragma once
#include <glm/glm.hpp>

// A 4D camera. The "sensor" is a 3D box of voxels (spanned by right/up/over)
// sitting one focal unit forward along the 4th viewing axis. Rays are cast
// from the camera position through each voxel, exactly as the 3D camera casts
// through each pixel -- one dimension higher.
class Camera {
public:
  Camera();
  ~Camera();

  void setPosition(const glm::vec4 &position);
  void setLookAt(const glm::vec4 &target);
  // Two reference "up" vectors are needed to pin down a 3D sensor orientation
  // in 4D (one is not enough -- there is a whole plane of directions orthogonal
  // to forward + up). They are re-orthogonalised into the sensor basis.
  void setUp(const glm::vec4 &up);
  void setOver(const glm::vec4 &over);
  void setFOV(float fov);

  // --- Free-fly controls -------------------------------------------------
  // Translate along the camera's own four axes (local space). All four together
  // span the full 4D space; with the default pose `forward` is the w axis, so
  // moveForward is what actually walks through the 4th dimension.
  void moveForward(float d) { position += d * forward; }
  void moveRight(float d) { position += d * right; }
  void moveUp(float d) { position += d * up; }
  void moveOver(float d) { position += d * over; }

  // Rotate the orthonormal view basis in a plane (Givens rotation, radians),
  // staying well-conditioned no matter how far you turn. yaw/pitch/wLook each
  // swing the view direction against one sensor axis -- a full turn there swaps
  // which axis you look along, so you can bring the w axis (or any other) into
  // view. roll spins the image within the sensor.
  void yaw(float a) { rotatePlane(forward, right, a); }
  void pitch(float a) { rotatePlane(forward, up, a); }
  void wLook(float a) { rotatePlane(forward, over, a); }
  void roll(float a) { rotatePlane(up, right, a); }

  // Gram-Schmidt the basis back to orthonormal to shed accumulated float drift.
  // Call periodically (e.g. once per frame) after applying rotations.
  void reorthonormalize();

  glm::vec4 getPosition() const { return position; }
  glm::vec4 getLookAt() const { return target; }
  // Orthonormal sensor basis: right/up/over span the 3D sensor, forward is the
  // viewing direction (the "w-ish" axis the sensor sits in front of).
  glm::vec4 getRight() const { return right; }
  glm::vec4 getUp() const { return up; }
  glm::vec4 getOver() const { return over; }
  glm::vec4 getForward() const { return forward; }
  float getFOV() const { return fov; }

private:
  glm::vec4 position;
  glm::vec4 target;
  glm::vec4 up_ref;   // user-supplied reference up
  glm::vec4 over_ref; // user-supplied reference over

  // Computed orthonormal basis.
  glm::vec4 right;
  glm::vec4 up;
  glm::vec4 over;
  glm::vec4 forward;

  float fov;

  void updateCameraVectors();
  // Givens rotation of two orthonormal basis vectors within their plane; keeps
  // the pair (and therefore the whole basis) orthonormal.
  static void rotatePlane(glm::vec4 &a, glm::vec4 &b, float angle);
};
