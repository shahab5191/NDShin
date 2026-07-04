#include "camera.h"
#include "ellipsoid.h"
#include "hyperbox.h"
#include "hyperplane.h"
#include "light.h"
#include "raytracer.h"
#include "scene.h"
#include "sphere.h"
#include "window.hpp"
#include <iostream>

// On-screen window size.
const int WIDTH = 800;
const int HEIGHT = 600;

// 3D voxel sensor resolution. The renderer casts SENSOR_WIDTH * SENSOR_HEIGHT *
// SENSOR_DEPTH rays per frame, so keep the depth modest. The composited image
// is SENSOR_WIDTH x SENSOR_HEIGHT and gets upscaled to the window.
const int SENSOR_WIDTH = 200;
const int SENSOR_HEIGHT = 200;
const int SENSOR_DEPTH = 200;

int main() {

  Window window(WIDTH, HEIGHT, "4D Ray Tracer", SENSOR_WIDTH, SENSOR_HEIGHT);

  // Initial camera pose: pulled back along +w, looking at the scene centre.
  // From here the free-fly controls take over (see the loop below).
  Camera camera;
  camera.setPosition({0.0f, 0.0f, 0.0f, 12.0f});
  camera.setLookAt({0.0f, 0.0f, 0.0f, 0.0f});
  camera.setUp({0.0f, 1.0f, 0.0f, 0.0f});
  camera.setOver({0.0f, 0.0f, 1.0f, 0.0f});
  camera.setFOV(45.0f);

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

  RayTracer ray_tracer(&scene, SENSOR_WIDTH, SENSOR_HEIGHT, SENSOR_DEPTH);

  while (!window.shouldClose()) {
    float delta_time = window.beginFrame();
    if (window.isKeyDown(GLFW_KEY_ESCAPE)) {
      break;
    }

    // Free-fly camera. Translation is scaled by delta_time so movement speed is
    // frame-rate independent. The four move axes span all of 4D; at the start
    // forward is the w axis, so W/S is what glides through the 4th dimension.
    //   Move:  W/S forward/back (w axis), A/D left/right, Q/E down/up,
    //          R/F along the sensor depth.
    //   Look:  arrows yaw/pitch, Z/X swing the view toward the sensor-depth
    //          axis. A 90-degree turn brings a different axis into view, which
    //          is how you reorient which 3D slice of 4D space you see.
    const float move = 6.0f * delta_time;
    const float turn = 1.5f * delta_time;

    if (window.isKeyDown(GLFW_KEY_W)) camera.moveForward(move);
    if (window.isKeyDown(GLFW_KEY_S)) camera.moveForward(-move);
    if (window.isKeyDown(GLFW_KEY_D)) camera.moveRight(move);
    if (window.isKeyDown(GLFW_KEY_A)) camera.moveRight(-move);
    if (window.isKeyDown(GLFW_KEY_E)) camera.moveUp(move);
    if (window.isKeyDown(GLFW_KEY_Q)) camera.moveUp(-move);
    if (window.isKeyDown(GLFW_KEY_R)) camera.moveOver(move);
    if (window.isKeyDown(GLFW_KEY_F)) camera.moveOver(-move);

    if (window.isKeyDown(GLFW_KEY_RIGHT)) camera.yaw(-turn);
    if (window.isKeyDown(GLFW_KEY_LEFT)) camera.yaw(turn);
    if (window.isKeyDown(GLFW_KEY_UP)) camera.pitch(turn);
    if (window.isKeyDown(GLFW_KEY_DOWN)) camera.pitch(-turn);
    if (window.isKeyDown(GLFW_KEY_Z)) camera.wLook(turn);
    if (window.isKeyDown(GLFW_KEY_X)) camera.wLook(-turn);

    // Shed floating-point drift so the basis stays orthonormal over time.
    camera.reorthonormalize();

    std::cout << "FPS: " << 1.0f / delta_time << '\n';

    ray_tracer.render();
    window.presentFrame(ray_tracer.getPixelBuffer());
  }
  return 0;
}
