#include "app/Input.h"

const InputState &Input::state() { return s; }

void Input::init(GLFWwindow *w) {
  glfwSetCursorPosCallback(w, cursorCB);
  glfwSetMouseButtonCallback(w, buttonCB);
  glfwSetScrollCallback(w, scrollCB);
}

void Input::cursorCB(GLFWwindow *, double x, double y) {
  s.mouseX = x;
  s.mouseY = y;
}
void Input::buttonCB(GLFWwindow *, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    s.LMB = action == GLFW_PRESS;
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    s.RMB = action == GLFW_PRESS;
}
void Input::scrollCB(GLFWwindow *, double /*xoff*/, double yoff) {
  s.scrollOffset = static_cast<float>(yoff);
}
