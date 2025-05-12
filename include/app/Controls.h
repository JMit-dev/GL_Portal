#ifndef CONTROLS_H
#define CONTROLS_H

#include "app/Camera.h"
#include "portal/Scene.h"
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>

class Controls {
public:
  explicit Controls(GLFWwindow *win);

  void update(float dt); ///< call once per frame

  Camera &camera() { return cam; }
  const Camera &camera() const { return cam; }
  bool uiVisible() const { return showUI; }

private:
  static bool keyPressed(GLFWwindow *w, int key);

  // GLFW callbacks (static → trampoline)
  static void scrollCB(GLFWwindow *win, double /*x*/, double y);

  GLFWwindow *window;
  Camera cam{glm::vec3(0.f, 1.0f, 0.f)};

  // mouse interaction
  bool cursorCaptured{true};
  bool firstMouse{true};
  double lastX{0.0}, lastY{0.0};
  float scrollOffset{0.f};

  // UI toggle
  bool showUI{true};

  // edge detection
  bool onePrev{false}, twoPrev{false};
  glm::vec3 prevCamPos{0.0f};
};

#endif
