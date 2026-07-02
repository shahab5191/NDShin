#include "camera.h"
#include "raytracer.h"
#include "scene.h"
#include "sphere.h"
#include "window.hpp"
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {

  Window window(WIDTH, HEIGHT, "Ray Tracer");

  Camera camera;
  camera.setPosition({0.0f, 0.0f, 5.0f});
  camera.setLookAt({0.0f, 0.0f, -5.0f});
  camera.setUp({0.0f, 1.0f, 0.0f});
  camera.setFOV(45.0f);
  camera.setAspectRatio(static_cast<float>(WIDTH) / HEIGHT);

  Sphere sphere_1({-1.0f, 0.0f, -5.0f}, 1.0f);
  sphere_1.setColor({1.0f, 0.0f, 0.0f});

  Sphere sphere_2({1.0f, 0.0f, -5.0f}, 1.0f);
  sphere_2.setColor({0.0f, 1.0f, 0.0f});

  Scene scene;
  scene.setCamera(&camera);
  scene.addShape(&sphere_1);
  scene.addShape(&sphere_2);

  RayTracer ray_tracer(&scene, WIDTH, HEIGHT);

  float t = 0.0f;
  while (!window.shouldClose()) {
    float delta_time = window.beginFrame();
    glm::vec3 target = camera.getLookAt();
    float radius = 5.0f;
    camera.setPosition({target.x + radius * sin(t), target.y,
                        target.z + radius * cos(t)});
    std::cout << "Camera Position: (" << camera.getPosition().x << ", "
              << camera.getPosition().y << ", " << camera.getPosition().z
              << ")" << std::endl;
    std::cout << "FPS: " << 1.0f / delta_time << std::endl;
    t += delta_time;
    ray_tracer.render();
    window.presentFrame(ray_tracer.getPixelBuffer());
  }
  return 0;
}
