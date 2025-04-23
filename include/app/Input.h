#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

struct InputState {
  double mouseX{}, mouseY{};
  bool LMB{false}, RMB{false};
  float scrollOffset{};
};

class Input {
public:
  static void init(GLFWwindow *w);
  static const InputState &state();

private:
  static void cursorCB(GLFWwindow *, double, double);
  static void buttonCB(GLFWwindow *, int, int, int);
  static void scrollCB(GLFWwindow *, double, double);

  inline static InputState s;
};

#endif
