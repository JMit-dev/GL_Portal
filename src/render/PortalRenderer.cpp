// ──────────────────────────────────────────────────────────────────────────────
//  PortalRenderer.cpp   – mini‑portal pass + run‑time probes
// ──────────────────────────────────────────────────────────────────────────────
#include "render/PortalRenderer.h"
#include "app/Camera.h"
#include "portal/Cell.h"
#include "portal/Portal.h"
#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "shape/Skybox.h"
#include "shape/TexturedBox.h"
#include "shape/TexturedQuad.h"
#include <cassert>
#include <cstdio> // debug prints
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#if !defined(GL_DEBUG_SOURCE_APPLICATION)
#define GL_DEBUG_SOURCE_APPLICATION 0x824A // GL < 4.3 headers
#endif

//------------------------------------------------------------------------------
// tiny helpers
//------------------------------------------------------------------------------
static inline void glDbg(const char *label) {
#ifndef NDEBUG
  glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, label);
#endif
}

static inline void glDbgPop() {
#ifndef NDEBUG
  glPopDebugGroup();
#endif
}

// global state toggle -------------------------------------------------
static void pushClip() { glEnable(GL_CLIP_DISTANCE0); }
static void popClip() { glDisable(GL_CLIP_DISTANCE0); }

// send <A,B,C,D> to *the shader that is bound right now*

static inline void setClipPlane(GLint loc, const glm::vec4 &eq) {
  if (loc >= 0) // shader actually has the uniform
    glUniform4fv(loc, 1, &eq.x);
}

void PortalRenderer::renderPortal(Portal &portal, const Camera &camSrc,
                                  int depth) {
  if (depth <= 0)
    return;

  // 0) pick a pass slot for this portal
  static int passIdx = 0;
  PortalPass &pp = portalPasses[passIdx++];
  if (passIdx >= portalPasses.size())
    passIdx = 0;

  // 1) compute the portal‐camera
  Camera camDst = throughPortal(camSrc, portal.transform());
  glm::mat4 Vdst = camDst.GetViewMatrix();
  glm::mat4 Pdst = glm::perspective(glm::radians(camDst.Zoom),
                                    float(pp.width) / pp.height, 0.1f, 100.f);

  // 2) render INTO the FBO
  glBindFramebuffer(GL_FRAMEBUFFER, pp.fbo);
  glViewport(0, 0, pp.width, pp.height);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the **entire** scene from camDst
  // we pass nullptr for entryPortal so we don't skip the quad (!)
  // and decrement depth so you get recursion:
  renderCell(*portal.destination(), camDst, depth - 1, nullptr);

  // restore main framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screenW, screenH);

  // 3) draw the portal’s source‐quad, sampling pp.colorTex with
  //    a projector shader that does exactly the article’s clip→NDC→UV.

  auto &srcQuad = static_cast<TexturedQuad &>(portal.getSurface());

  // 3) draw the portal’s source-quad, sampling pp.colorTex with our
  //    projector shader
  srcQuad.shader()->use();

  // bind the offscreen color texture into unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pp.colorTex);
  srcQuad.shader()->setInt("portalTex", 0);

  // set your uniforms:
  srcQuad.shader()->setMat4("uModel", srcQuad.model());
  srcQuad.shader()->setMat4("uView", camSrc.GetViewMatrix());
  srcQuad.shader()->setMat4("uProj", glm::perspective(glm::radians(camSrc.Zoom),
                                                      float(screenW) / screenH,
                                                      0.1f, 100.f));
  srcQuad.shader()->setMat4("uPortalVP", Pdst * Vdst);

  // now draw the quad
  srcQuad.render();
}

void PortalRenderer::init(int w, int h) {
  screenW = w;
  screenH = h;
  // assume 2 portals for now
  portalPasses.resize(2);
  for (int i = 0; i < portalPasses.size(); ++i) {
    auto &pp = portalPasses[i];
    pp.width = 1024;
    pp.height = 1024; // render‐to‐tex size
    // create color texture
    glGenTextures(1, &pp.colorTex);
    glBindTexture(GL_TEXTURE_2D, pp.colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pp.width, pp.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth‐renderbuffer
    glGenRenderbuffers(1, &pp.depthRb);
    glBindRenderbuffer(GL_RENDERBUFFER, pp.depthRb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pp.width,
                          pp.height);
    // create FBO
    glGenFramebuffers(1, &pp.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pp.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pp.colorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, pp.depthRb);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PortalRenderer::renderScene(const Scene &scene, const Camera &cam,
                                 int maxDepth) {
  if (!scene.viewpointCell())
    return;

  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);

  stencilDepth = 0;
  renderCell(*scene.viewpointCell(), cam, maxDepth, nullptr);
}

// ── generic cell draw: first its portals, then its geometry ───────
void PortalRenderer::renderCell(const Cell &cell, const Camera &cam, int depth,
                                const Portal *cameFrom) {
  // 1. recurse into every visible portal first
  if (depth > 0) {
    for (auto &p : cell.getPortals())
      if (p.get() != cameFrom) // avoid immediate back‑edge
        renderPortal(*p, cam, depth - 1);
  }

  // 2. now draw **this** cell’s own geometry with the current eye
  glm::mat4 V = cam.GetViewMatrix();
  glm::mat4 P = glm::perspective(glm::radians(cam.Zoom),
                                 float(screenW) / screenH, 0.1f, 100.f);
  glm::vec3 eye = cam.Position;

  for (auto &g : cell.getGeometry()) {
    if (auto *m = dynamic_cast<ModelShape *>(g.get())) {
      m->setViewProj(V, P, eye);
    } else if (auto *q = dynamic_cast<TexturedQuad *>(g.get())) {
      q->setViewProj(V, P);
    } else if (auto *sky = dynamic_cast<Skybox *>(g.get())) {
      sky->setViewProj(V, P);
    } else if (auto *box = dynamic_cast<TexturedBox *>(g.get())) {
      box->setViewProj(V, P);
    }

    g->render();
  }
}
