#include <glad/glad.h>

#include "app/Camera.h"
#include "portal/Scene.h"
#include "render/PortalRenderer.h"
#include "render/Renderer.h"

Renderer::Renderer(int screenW, int screenH) : w(screenW), h(screenH) {
  portalRenderer = std::make_unique<PortalRenderer>();
  portalRenderer->init(w, h);
}

void Renderer::draw(const Scene &scene, const Camera &cam) {
  portalRenderer->renderScene(scene, cam, recursionDepth);
}

void Renderer::resize(int newW, int newH) {
  w = newW;
  h = newH;
  portalRenderer->init(w, h); // re-allocate depth/stencil buffers
}
