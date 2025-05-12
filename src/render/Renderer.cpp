#include <glad/glad.h>

#include "app/Camera.h"
#include "portal/Scene.h"
#include "render/PortalRenderer.h"
#include "render/Renderer.h"
#include "shape/ModelShape.h"
#include "shape/TexturedQuad.h"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(int screenW, int screenH) : w(screenW), h(screenH) {
  portalRenderer = std::make_unique<PortalRenderer>();
  portalRenderer->init(w, h);
}

void Renderer::draw(const Scene &scene, const Camera &cam) {
  glClearColor(0.05f, 0.05f, 0.08f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  portalRenderer->renderScene(scene, cam, recursionDepth);
}

void Renderer::resize(int newW, int newH) {
  w = newW;
  h = newH;
  portalRenderer->init(w, h); // re-allocate depth/stencil buffers
}
