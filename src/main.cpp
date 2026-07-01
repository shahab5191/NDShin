#include "raytracer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  RayTracer ray_tracer(WIDTH, HEIGHT);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Allocate the texture storage once (nullptr = no initial data); the pixels
  // are (re)uploaded every frame with glTexSubImage2D below.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT,
  nullptr);

  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textureID, 0);

  double last_time = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    // Seconds elapsed since the previous frame.
    double now = glfwGetTime();
    float delta = static_cast<float>(now - last_time);
    last_time = now;

    // Animate b in [0,1] over time, re-render, and upload the new pixels
    // into the existing texture.
    float b = 0.5f * (std::sin(now) + 1.0f);
    ray_tracer.render(b);
    auto pixel_buffer = ray_tracer.getPixelBuffer();

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGB, GL_FLOAT,
                    pixel_buffer.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Query the actual framebuffer size (differs from WIDTH/HEIGHT on HiDPI
    // displays or after a resize) so the blit covers the whole window.
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, fb_width,
                      fb_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Swap buffers and poll window events (like clicking the close X)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &textureID);
  glDeleteFramebuffers(1, &fbo);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
