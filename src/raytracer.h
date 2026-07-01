#pragma once
#include <vector>

class RayTracer {
public:
  RayTracer(int width, int height);
  RayTracer(const RayTracer &) = delete;
  void render(float b);
  std::vector<float> getPixelBuffer() const;

private:
  int width;
  int height;
  std::vector<float> pixel_buffer;
};
