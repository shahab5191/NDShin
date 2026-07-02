#pragma once
// glad must be included before GLFW, and GLFW_INCLUDE_NONE stops glfw3.h from
// pulling in the system <GL/gl.h> (which would clash with glad's loader).
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();

  bool shouldClose() const { return glfwWindowShouldClose(window); }
  float beginFrame();
  void presentFrame(const std::vector<float> &buffer);

private:
  int width;
  int height;
  std::string title;
  GLFWwindow *window;
  GLuint textureID;
  GLuint fbo;
  double last_time;
};
