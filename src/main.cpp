#include "raytracer.h"
#include "window.hpp"
#include <cmath>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {


  RayTracer ray_tracer(WIDTH, HEIGHT);
  Window window(WIDTH, HEIGHT, "Ray Tracer");

  float t = 0.0f;
  while (!window.shouldClose()) {
    float delta_time = window.beginFrame();
    t += delta_time;
    ray_tracer.render(std::sin(t) + 1.0f);
    window.presentFrame(ray_tracer.getPixelBuffer());
  }
  return 0;
}
