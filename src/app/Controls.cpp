
#include "app/Controls.h"
#include "shape/TexturedQuad.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>

static std::optional<Camera_Movement> keyToMove(int key) {
  switch (key) {
  case GLFW_KEY_W:
    return FORWARD;
  case GLFW_KEY_S:
    return BACKWARD;
  case GLFW_KEY_A:
    return LEFT;
  case GLFW_KEY_D:
    return RIGHT;
  case GLFW_KEY_SPACE:
    return UP;
  case GLFW_KEY_LEFT_CONTROL:
    return DOWN;
  default:
    return std::nullopt;
  }
}

Controls::Controls(GLFWwindow *win) : window(win) {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, scrollCB);

  // store pointer for static cb if needed later
  glfwSetWindowUserPointer(window, this);
}

bool Controls::keyPressed(GLFWwindow *w, int key) {
  return glfwGetKey(w, key) == GLFW_PRESS;
}

void Controls::update(float dt) {
  // -------------------------------------------------
  // Key toggles  (1 = capture cursor, 2 = UI)
  // -------------------------------------------------
  bool one = keyPressed(window, GLFW_KEY_1);
  if (one && !onePrev) {
    cursorCaptured = !cursorCaptured;
    glfwSetInputMode(window, GLFW_CURSOR,
                     cursorCaptured ? GLFW_CURSOR_DISABLED
                                    : GLFW_CURSOR_NORMAL);
    firstMouse = true; // reset mouse delta
  }
  onePrev = one;

  bool two = keyPressed(window, GLFW_KEY_2);
  if (two && !twoPrev)
    showUI = !showUI;
  twoPrev = two;

  // -------------------------------------------------
  // Keyboard movement (WASD + Space / Ctrl)
  // -------------------------------------------------
  float speedMul = keyPressed(window, GLFW_KEY_LEFT_SHIFT) ? 3.f : 1.f;

  for (int key : {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,
                  GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL}) {
    if (keyPressed(window, key)) {
      if (auto dir = keyToMove(key)) {
        // temporarily scale speed
        float original = cam.MovementSpeed;
        cam.MovementSpeed = original * speedMul;
        cam.ProcessKeyboard(*dir, dt);
        cam.MovementSpeed = original;
      }
    }
  }

  // -------------------------------------------------
  // Mouse look
  // -------------------------------------------------
  if (cursorCaptured) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if (firstMouse) {
      lastX = x;
      lastY = y;
      firstMouse = false;
    }

    float xoff = static_cast<float>(x - lastX);
    float yoff = static_cast<float>(lastY - y); // y inverted
    lastX = x;
    lastY = y;

    cam.ProcessMouseMovement(xoff, yoff);
  }

  // -------------------------------------------------
  // Scroll zoom (accumulated in callback)
  // -------------------------------------------------
  if (scrollOffset != 0.f) {
    cam.ProcessMouseScroll(scrollOffset);
    scrollOffset = 0.f;
  }
}

void Controls::scrollCB(GLFWwindow *win, double /*x*/, double y) {
  auto *self = static_cast<Controls *>(glfwGetWindowUserPointer(win));
  if (self)
    self->scrollOffset += static_cast<float>(y);
}
