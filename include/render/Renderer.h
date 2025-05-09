#ifndef RENDERER_H
#define RENDERER_H
#include <memory>

class Scene;
class Camera;
class ModelShape;

#include "render/PortalRenderer.h"

class Renderer {
public:
  Renderer(int screenW, int screenH);
  ~Renderer() = default;

  void draw(const Scene &scene, const Camera &cam);
  void resize(int w, int h);

  int recursionDepth{3}; // editable from ImGui

private:
  int w, h;
  std::unique_ptr<PortalRenderer> portalRenderer;
};

#endif
