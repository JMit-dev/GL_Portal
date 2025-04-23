
#ifndef PORTAL_RENDERER_H
#define PORTAL_RENDERER_H

#include <glm/glm.hpp>
class Scene;
class Camera;

class PortalRenderer {
public:
  void init(int screenW, int screenH);
  void renderScene(const Scene &scene, const Camera &cam, int maxDepth);

private:
  void renderCell(const class Cell &, const Camera &, int depth);
  void renderPortal(const class Portal &, const Camera &, int depth);

  // stencil depth tracking
  int stencilDepth{0};
};

#endif
