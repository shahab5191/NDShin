#include "raytracer.h"
#include <vector>

RayTracer::RayTracer(int width, int height) : width(width), height(height) {
  pixel_buffer.resize(width * height * 3, 0.0f);
}

void RayTracer::render(float b) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      float r = static_cast<float>(x) / static_cast<float>(width);
      float g = static_cast<float>(y) / static_cast<float>(height);
      int index = (y * width + x) * 3;
      pixel_buffer[index] = r;
      pixel_buffer[index + 1] = g;
      pixel_buffer[index + 2] = b;
    }
  }
}

std::vector<float> RayTracer::getPixelBuffer() const { return pixel_buffer; }
