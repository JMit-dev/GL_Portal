
#ifndef PORTAL_RENDERER_H
#define PORTAL_RENDERER_H

#include "app/Camera.h"
#include "portal/Portal.h"
#include "portal/Scene.h"
#include <GL/gl.h>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <vector>

struct PortalPass {
  GLuint fbo = 0;
  GLuint colorTex = 0;
  GLuint depthRb = 0;
  int width = 512; // render‐to‐tex resolution
  int height = 512;
};

namespace PortalUtils {
void checkPortalTeleport(Scene &scene, Camera &cam);
}

class PortalRenderer {
public:
  void init(int screenW, int screenH);
  void renderScene(const Scene &scene, const Camera &cam, int maxDepth);

  static Camera throughPortal(const Camera &camSrc, const glm::mat4 &srcToDst) {
    // Copy the source camera to keep projection settings
    Camera camDst = camSrc;

    // Construct full source view matrix
    glm::mat4 viewSrc = camSrc.GetViewMatrix();

    // Compute the destination view by transforming the source view through the
    // portal
    glm::mat4 viewDst = viewSrc * glm::inverse(srcToDst);

    // Invert to get the new camera world transform
    glm::mat4 worldDst = glm::inverse(viewDst);

    // Extract new position and direction from the destination matrix
    camDst.Position = glm::vec3(worldDst[3]);
    camDst.Front = glm::normalize(
        glm::vec3(-viewDst[0][2], -viewDst[1][2], -viewDst[2][2]));
    camDst.Right =
        glm::normalize(glm::vec3(viewDst[0][0], viewDst[1][0], viewDst[2][0]));
    camDst.Up =
        glm::normalize(glm::vec3(viewDst[0][1], viewDst[1][1], viewDst[2][1]));

    camDst.Yaw += 180.0f;

    return camDst;
  }

  static Camera throughPortalFixed(const Camera &camSrc,
                                   const glm::mat4 &srcToDst,
                                   const glm::vec3 &dstCenter,
                                   bool flipView = false) {
    Camera camDst = camSrc;

    // Extract *only* the rotation from srcToDst:
    glm::mat3 rot = glm::mat3(srcToDst);

    // 1) Anchor the camera at the portal center:
    camDst.Position = dstCenter;

    // 2) Rotate its basis vectors by rot:
    camDst.Front = glm::normalize(rot * camSrc.Front);
    camDst.Right = glm::normalize(rot * camSrc.Right);
    camDst.Up = glm::normalize(rot * camSrc.Up);

    if (flipView) {
      camDst.updateCameraVectors();

      camDst.Yaw += 180.0f;
    }

    // Preserve FOV, yaw/pitch, etc.
    return camDst;
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
