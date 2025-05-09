#ifndef DEBUG_UI_H
#define DEBUG_UI_H
class Renderer;
class SceneManager;
class Controls;

class DebugUI {
public:
  void draw(Renderer &r, SceneManager &sm, Controls &c, float dt);
};
#endif
