#ifndef APP_H
#define APP_H

#include "app/Window.h"
#include <memory>

class Renderer;
class Scene;
class Camera;

/**
 * Top-level singleton that owns the window, ImGui context,
 * the scene graph and the high-level renderer.
 *
 * Usage:
 *     int main() { App::getInstance().run(); }
 */
class App : private Window {
public:
  static App &getInstance();
  void run(); ///< main loop (blocks until window close)

private:
  static constexpr int kWidth = 1280;
  static constexpr int kHeight = 720;
  static constexpr char kTitle[] = "Portal Demo";

  App();            ///< sets up GLFW window + ImGui
  ~App() = default; ///< default – GLFW/ImGui cleaned up by base/Imgui

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void initScene();           ///< create cells, portals, geometry
  void renderFrame(float dt); ///< per-frame draw
  static void framebufferSizeCallback(GLFWwindow *, int, int);

  // TO DO
  std::unique_ptr<Scene> scene;
  std::unique_ptr<Renderer> renderer;
  std::unique_ptr<Camera> camera;
};

#endif
