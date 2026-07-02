#include "window.hpp"
#include <iostream>
#include <string>

Window::Window(int width, int height, const std::string &title)
    : width(width), height(height), title(title) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    throw std::runtime_error("Failed to initialize GLFW");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    throw std::runtime_error("Failed to initialize GLAD");
  }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Allocate the texture storage once (nullptr = no initial data); the pixels
  // are (re)uploaded every frame with glTexSubImage2D below.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT,
               nullptr);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureID, 0);
}

Window::~Window() {
  glDeleteTextures(1, &textureID);
  glDeleteFramebuffers(1, &fbo);
  glfwDestroyWindow(window);
  glfwTerminate();
}

float Window::beginFrame() {
  double now = glfwGetTime();
  float delta = static_cast<float>(now - last_time);
  last_time = now;
  return delta;
}

void Window::presentFrame(const std::vector<float> &buffer) {
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT,
                  buffer.data());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Query the actual framebuffer size (differs from WIDTH/HEIGHT on HiDPI
  // displays or after a resize) so the blit covers the whole window.
  int fb_width, fb_height;
  glfwGetFramebufferSize(window, &fb_width, &fb_height);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, width, height, 0, 0, fb_width, fb_height,
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);

  // Swap buffers and poll window events (like clicking the close X)
  glfwSwapBuffers(window);
  glfwPollEvents();
}
