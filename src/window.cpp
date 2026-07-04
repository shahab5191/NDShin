#include "window.hpp"
#include <iostream>
#include <string>

Window::Window(int width, int height, const std::string &title,
               int renderWidth, int renderHeight)
    : width(width), height(height), renderWidth(renderWidth),
      renderHeight(renderHeight), title(title), last_time(0.0) {
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
  // Disable vsync: we throttle nothing and report our own FPS, so let frames
  // present as fast as they render instead of blocking on the compositor.
  glfwSwapInterval(0);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    throw std::runtime_error("Failed to initialize GLAD");
  }
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Allocate the texture storage once at the render resolution (nullptr = no
  // initial data); the pixels are (re)uploaded every frame with glTexSubImage2D
  // below and upscaled to the window on blit.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderWidth, renderHeight, 0, GL_RGB,
               GL_FLOAT, nullptr);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureID, 0);

  // Seed the frame clock so the first beginFrame() delta is ~0 instead of
  // (now - uninitialized). GLFW's timer starts at glfwInit() above, so this is
  // already a small value.
  last_time = glfwGetTime();
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
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, renderWidth, renderHeight, GL_RGB,
                  GL_FLOAT, buffer.data());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Query the actual framebuffer size (differs from WIDTH/HEIGHT on HiDPI
  // displays or after a resize) so the blit covers the whole window.
  int fb_width, fb_height;
  glfwGetFramebufferSize(window, &fb_width, &fb_height);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, renderWidth, renderHeight, 0, 0, fb_width, fb_height,
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);

  // Swap buffers and poll window events (like clicking the close X)
  glfwSwapBuffers(window);
  glfwPollEvents();
}
