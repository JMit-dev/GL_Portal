#include <glad/glad.h>

#include "app/Window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(int width, int height, const char *title) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // Use OpenGL 3.3 core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create the window
  pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!pWindow) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  // Make this window's OpenGL context current
  glfwMakeContextCurrent(pWindow);

  // Initialize GLAD
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }

  // By default, set the initial viewport
  glViewport(0, 0, width, height);
}

Window::~Window() noexcept {
  if (pWindow) {
    glfwDestroyWindow(pWindow);
    pWindow = nullptr;
  }
  glfwTerminate();
}
