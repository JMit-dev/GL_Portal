#ifndef APP_H
#define APP_H

#include "app/Window.h"
#include <memory>

class Renderer;
class SceneManager;
class Controls;
class DebugUI;

class App : private Window {
public:
  static App &instance();
  void run(); ///< main loop (blocks until window close)

private:
  static constexpr int kWidth = 1280;
  static constexpr int kHeight = 720;
  static constexpr char kTitle[] = "Portal Demo";

  App();  ///< sets up window + subsystems
  ~App(); ///< shuts down ImGui & GLFW

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  // helpers
  void renderFrame(float dt);
  static void framebufferSizeCallback(GLFWwindow *, int, int);

  // members
  std::unique_ptr<Renderer> renderer;
  std::unique_ptr<SceneManager> sceneMgr;
  std::unique_ptr<Controls> controls;
  std::unique_ptr<DebugUI> ui;
};

#endif
