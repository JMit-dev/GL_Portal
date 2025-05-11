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
  // clear framebuffer each frame
  glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::mat4 V = cam.GetViewMatrix();
  const glm::mat4 P =
      glm::perspective(glm::radians(cam.Zoom), float(w) / h, 0.1f, 100.f);
  const glm::vec3 eye = cam.Position;

  // single‑cell scene for now
  const auto &cell = *scene.viewpointCell();
  for (const auto &geo : cell.getGeometry()) {
    if (auto *ms = dynamic_cast<ModelShape *>(geo.get()))
      ms->setViewProj(V, P, eye);

    if (auto *tq = dynamic_cast<TexturedQuad *>(geo.get()))
      tq->setViewProj(V, P);

    geo->render();
  }
}

void Renderer::resize(int newW, int newH) {
  w = newW;
  h = newH;
  portalRenderer->init(w, h); // re-allocate depth/stencil buffers
}
