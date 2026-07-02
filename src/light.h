#pragma once

#include "glm/ext/vector_float3.hpp"
class Light {
public:
  Light() : position(0.0f), color(1.0f), intensity(1.0f) {}
  Light(const glm::vec3 &position, const glm::vec3 &color, float intensity)
      : position(position), color(color), intensity(intensity) {}
  ~Light() = default;

  void setPosition(const glm::vec3 &position) { this->position = position; }
  void setColor(const glm::vec3 &color) { this->color = color; }
  void setIntensity(float intensity) { this->intensity = intensity; }

  glm::vec3 getPosition() const { return position; }
  glm::vec3 getColor() const { return color; }
  float getIntensity() const { return intensity; }

private:
  glm::vec3 position;
  glm::vec3 color;
  float intensity;
};
