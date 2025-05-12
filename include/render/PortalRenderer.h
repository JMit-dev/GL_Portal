
#ifndef PORTAL_RENDERER_H
#define PORTAL_RENDERER_H

#include "app/Camera.h"
#include "portal/Portal.h"
#include <GL/gl.h>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <vector>
class Scene;

struct PortalPass {
  GLuint fbo = 0;
  GLuint colorTex = 0;
  GLuint depthRb = 0;
  int width = 512; // can tweak resolution
  int height = 512;
};

class PortalRenderer {
public:
  void init(int screenW, int screenH);
  void renderScene(const Scene &scene, const Camera &cam, int maxDepth);

  static Camera throughPortal(const Camera &src, const glm::mat4 &toDst) {
    Camera out = src;

    // move position
    glm::vec4 P = toDst * glm::vec4(src.Position, 1.0f);
    out.Position = glm::vec3(P);

    // rotate direction‑vectors
    glm::vec4 F = toDst * glm::vec4(src.Front, 0.0f);
    glm::vec4 U = toDst * glm::vec4(src.Up, 0.0f);
    out.Front = glm::normalize(glm::vec3(F));
    out.Up = glm::normalize(glm::vec3(U));

    return out;
  }

private:
  void renderCell(const class Cell &, const Camera &, int depth,
                  const class Portal *entryPortal);
  void renderPortal(class Portal &, const Camera &, int depth);
  // stencil depth tracking
  int stencilDepth{0};
  glm::vec4 clipEq{0, 0, 0, 0};
  std::vector<PortalPass> portalPasses;
  int screenW, screenH;
};

#endif
