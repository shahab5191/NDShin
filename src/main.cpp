#include "camera.h"
#include "camera3d.h"
#include "ellipsoid.h"
#include "hyperbox.h"
#include "hyperplane.h"
#include "light.h"
#include "raytracer.h"
#include "scene.h"
#include "sphere.h"
#include "window.hpp"
#include <algorithm>
#include <iostream>

// On-screen window size.
const int WIDTH = 800;
const int HEIGHT = 600;

// 3D voxel sensor resolution -- the size of the captured voxel cloud. The 4D
// camera casts VOXEL_W * VOXEL_H * VOXEL_D rays per capture.
const int VOXEL_W = 96;
const int VOXEL_H = 96;
const int VOXEL_D = 96;

// Resolution the 3D view camera renders the cloud at (upscaled to the window).
const int VIEW_W = 400;
const int VIEW_H = 300;

int main() {

  Window window(WIDTH, HEIGHT, "4D Ray Tracer", VIEW_W, VIEW_H);

  // The 4D camera: it captures the voxel cloud. Move it in 4D space (WASD/QE/RF,
  // Z/X) to re-photograph the world from a different 4D viewpoint.
  Camera camera;
  camera.setPosition({0.0f, 0.0f, 0.0f, 12.0f});
  camera.setLookAt({0.0f, 0.0f, 0.0f, 0.0f});
  camera.setUp({0.0f, 1.0f, 0.0f, 0.0f});
  camera.setOver({0.0f, 0.0f, 1.0f, 0.0f});
  camera.setFOV(45.0f);

  // The 3D view camera: it orbits the captured voxel cloud so we can look at the
  // 3D snapshot from any angle (arrow keys to orbit, -/= to zoom).
  Camera3D view;
  view.setTarget({0.0f, 0.0f, 0.0f});
  view.setAspect(static_cast<float>(VIEW_W) / VIEW_H);
  view.setFOV(45.0f);

  Light light({-5.0f, 5.0f, 0.0f, 2.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
  Light light_2({10.0f, -5.0f, -3.0f, 0.0f}, {1.0f, 0.0f, 0.3f}, 2.0f);
  Light light_3({0.0f, 0.0f, 5.0f, 3.0f}, {0.0f, 1.0f, 0.3f}, 2.0f);

  // Two hyperspheres (glomes) placed in front of the camera along -w.
  Sphere sphere_1({-2.0f, 0.0f, 0.0f, -5.0f}, 1.2f);
  sphere_1.setColor({0.6f, 0.3f, 0.3f});

  Sphere sphere_2({-2.0f, 0.0f, 0.0f, 5.0f}, 1.2f);
  sphere_2.setColor({0.1f, 0.5f, 0.5f});

  // A tesseract (all four half-extents equal) off to the +x side.
  Hyperbox tesseract({3.0f, 0.0f, 0.0f, 0.0f}, glm::vec4(1.2f));
  tesseract.setColor({0.3f, 0.5f, 0.8f});

  // A 4D ellipsoid: stretched along x and w.
  Ellipsoid ellipsoid({0.0f, 3.0f, 0.0f, 0.0f}, {2.2f, 1.0f, 1.0f, 2.2f});
  ellipsoid.setColor({0.8f, 0.6f, 0.2f});

  // An infinite floor: the 3-plane y = -3.
  Hyperplane floor({0.0f, -3.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f});
  floor.setColor({0.4f, 0.4f, 0.4f});

  Scene scene;
  scene.setCamera(&camera);
  scene.addLight(light);
  scene.addLight(light_2);
  scene.addLight(light_3);
  scene.addShape(&sphere_1);
  scene.addShape(&sphere_2);
  scene.addShape(&tesseract);
  scene.addShape(&ellipsoid);
  scene.addShape(&floor);

  RayTracer ray_tracer(&scene, VOXEL_W, VOXEL_H, VOXEL_D, VIEW_W, VIEW_H);

  float density = 0.15f; // cloud opacity per voxel; tuned live with [ and ]
  ray_tracer.capture();  // initial snapshot of the 4D world

  while (!window.shouldClose()) {
    float delta_time = window.beginFrame();
    if (window.isKeyDown(GLFW_KEY_ESCAPE)) {
      break;
    }

    // --- 4D camera: move it through 4D to re-capture the voxel cloud --------
    // The four translation axes span all of 4D (forward starts as the w axis).
    //   Move:  W/S forward/back (w axis), A/D left/right, Q/E down/up,
    //          R/F along the sensor depth.   Rotate into 4th dim: Z/X.
    const float move = 6.0f * delta_time;
    const float turn = 1.5f * delta_time;
    bool moved_4d = false;
    auto move4d = [&](int key, auto fn, float amt) {
      if (window.isKeyDown(key)) {
        fn(amt);
        moved_4d = true;
      }
    };
    move4d(GLFW_KEY_W, [&](float a) { camera.moveForward(a); }, move);
    move4d(GLFW_KEY_S, [&](float a) { camera.moveForward(a); }, -move);
    move4d(GLFW_KEY_D, [&](float a) { camera.moveRight(a); }, move);
    move4d(GLFW_KEY_A, [&](float a) { camera.moveRight(a); }, -move);
    move4d(GLFW_KEY_E, [&](float a) { camera.moveUp(a); }, move);
    move4d(GLFW_KEY_Q, [&](float a) { camera.moveUp(a); }, -move);
    move4d(GLFW_KEY_R, [&](float a) { camera.moveOver(a); }, move);
    move4d(GLFW_KEY_F, [&](float a) { camera.moveOver(a); }, -move);
    move4d(GLFW_KEY_Z, [&](float a) { camera.wLook(a); }, turn);
    move4d(GLFW_KEY_X, [&](float a) { camera.wLook(a); }, -turn);
    if (moved_4d) {
      camera.reorthonormalize();
      ray_tracer.capture(); // re-photograph the world from the new 4D pose
    }

    // --- 3D view camera: orbit the captured cloud (does not re-capture) -----
    const float orbit = 1.2f * delta_time;
    const float zoom = 4.0f * delta_time;
    if (window.isKeyDown(GLFW_KEY_LEFT)) view.orbit(-orbit, 0.0f);
    if (window.isKeyDown(GLFW_KEY_RIGHT)) view.orbit(orbit, 0.0f);
    if (window.isKeyDown(GLFW_KEY_UP)) view.orbit(0.0f, orbit);
    if (window.isKeyDown(GLFW_KEY_DOWN)) view.orbit(0.0f, -orbit);
    if (window.isKeyDown(GLFW_KEY_MINUS)) view.zoom(zoom);
    if (window.isKeyDown(GLFW_KEY_EQUAL)) view.zoom(-zoom);

    // --- Cloud density: [ wispier, ] denser --------------------------------
    if (window.isKeyDown(GLFW_KEY_LEFT_BRACKET)) density -= 0.3f * delta_time;
    if (window.isKeyDown(GLFW_KEY_RIGHT_BRACKET)) density += 0.3f * delta_time;
    density = std::clamp(density, 0.01f, 1.0f);
    ray_tracer.setDensity(density);

    std::cout << "FPS: " << 1.0f / delta_time << '\n';

    ray_tracer.renderView(view); // stage 2, every frame
    window.presentFrame(ray_tracer.getPixelBuffer());
  }
  return 0;
}
