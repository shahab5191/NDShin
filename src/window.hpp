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
  // The render resolution (the composited image we upload) is decoupled from
  // the on-screen window size: the 3D voxel sensor is small (W*H*D rays), so we
  // render a small image and let the GPU upscale it to the window on blit.
  Window(int width, int height, const std::string &title, int renderWidth,
         int renderHeight);
  ~Window();

  bool shouldClose() const { return glfwWindowShouldClose(window); }
  float beginFrame();
  void presentFrame(const std::vector<float> &buffer);

  // Poll a key's held state (use GLFW_KEY_* constants). Reflects events from the
  // most recent glfwPollEvents() in presentFrame().
  bool isKeyDown(int key) const { return glfwGetKey(window, key) == GLFW_PRESS; }

private:
  int width;
  int height;
  int renderWidth;
  int renderHeight;
  std::string title;
  GLFWwindow *window;
  GLuint textureID;
  GLuint fbo;
  double last_time;
};
