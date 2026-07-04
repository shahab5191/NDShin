#pragma once

#include <glm/glm.hpp>
class Light {
public:
  Light() : position(0.0f), color(1.0f), intensity(1.0f) {}
  Light(const glm::vec4 &position, const glm::vec3 &color, float intensity)
      : position(position), color(color), intensity(intensity) {}
  ~Light() = default;

  void setPosition(const glm::vec4 &position) { this->position = position; }
  void setColor(const glm::vec3 &color) { this->color = color; }
  void setIntensity(float intensity) { this->intensity = intensity; }

  glm::vec4 getPosition() const { return position; }
  glm::vec3 getColor() const { return color; }
  float getIntensity() const { return intensity; }

private:
  glm::vec4 position;
  glm::vec3 color;
  float intensity;
};
