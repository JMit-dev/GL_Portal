#include <glad/glad.h>

#include "portal/Cell.h"
#include "portal/Portal.h"
#include "render/PortalRenderer.h"
#include "shape/Renderable.h"
#include "util/GLUtils.h"

void PortalRenderer::init(int w, int h) {
  // TO DO
}

void PortalRenderer::renderScene(const Scene &scene, const Camera &cam,
                                 int maxDepth) {
  // TO DO
}

//--- private -------------------------------------------------------------

void PortalRenderer::renderCell(const Cell &cell, const Camera &cam,
                                int depth) {

  // TODO
}

void PortalRenderer::renderPortal(const Portal &portal, const Camera &cam,
                                  int depth) {

  // TODO
}
