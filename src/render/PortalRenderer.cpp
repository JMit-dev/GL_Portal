#define GLM_ENABLE_EXPERIMENTAL

#include "render/PortalRenderer.h"
#include "app/Camera.h"
#include "portal/Cell.h"
#include "portal/Portal.h"
#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "shape/PortalQuad.h"
#include "shape/Skybox.h"
#include "shape/TexturedBox.h"
#include "shape/TexturedQuad.h"
#include <cassert>
#include <cstdio> // debug prints
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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

inline float sgn(float x) { return (x > 0.0f ? 1.f : (x < 0.0f ? -1.f : 0.f)); }

glm::mat4 makeObliqueProj(glm::mat4 P, const glm::vec4 &clipPlaneCam) {
  // 1) find the clip‐space corner opposite the plane
  glm::vec4 q;
  q.x = (sgn(clipPlaneCam.x) + P[0][2]) / P[0][0];
  q.y = (sgn(clipPlaneCam.y) + P[1][2]) / P[1][1];
  q.z = -1.0f;
  q.w = (1.0f + P[2][2]) / P[2][3];

  // 2) scale the plane into projection space
  glm::vec4 c = clipPlaneCam * (2.0f / glm::dot(clipPlaneCam, q));

  // 3) replace the 3rd row of P
  P[2][0] = c.x;
  P[2][1] = c.y;
  P[2][2] = c.z + 1.0f;
  P[2][3] = c.w;
  return P;
}

// global state toggle -------------------------------------------------
static void pushClip() { glEnable(GL_CLIP_DISTANCE0); }
static void popClip() { glDisable(GL_CLIP_DISTANCE0); }

// send <A,B,C,D> to *the shader that is bound right now*

static inline void setClipPlane(GLint loc, const glm::vec4 &eq) {
  if (loc >= 0) // shader actually has the uniform
    glUniform4fv(loc, 1, &eq.x);
}

//------------------------------------------------------------------------------
// PortalRenderer::renderPortal
// Renders the “view‐through” for one portal by
//  • building an oblique‐clipped camera,
//  • drawing the destination cell into an offscreen FBO,
//  • then texturing that FBO back onto the source quad.
//------------------------------------------------------------------------------
void PortalRenderer::renderPortal(Portal &portal, const Camera &camSrc,
                                  int depth) {
  if (depth <= 0)
    return;

  // 1) source portal quad
  auto &srcQuad = static_cast<PortalQuad &>(portal.getSurface());

  // 2) back‐face cull: only if we’re looking at its front
  {
    glm::vec3 N = normalize(srcQuad.normal());
    glm::vec3 toCam = normalize(srcQuad.c() - camSrc.Position);
    if (dot(N, toCam) > 0.0f)
      return;
  }

  // 3) destination portal
  Portal *dstP = portal.getDestinationPortal();
  if (!dstP)
    return;
  auto &dstQuad = static_cast<PortalQuad &>(dstP->getSurface());

  // 4) pick next FBO
  static int passIdx = 0;
  PortalPass &pp = portalPasses[passIdx++];
  if (passIdx >= portalPasses.size())
    passIdx = 0;

  // 5) build the “through‐portal” camera
  glm::vec3 dstCenter = dstQuad.c();
  bool flip = portal.getFlipView();
  Camera camDst =
      throughPortalFixed(camSrc, portal.transform(), dstCenter, flip);
  glm::mat4 Vdst = camDst.GetViewMatrix();

  // 6) build the clip‐plane in camera‐space
  glm::vec3 clipN = normalize(dstQuad.normal());
  glm::vec3 clipP = dstQuad.c();
  glm::vec4 planeW(clipN, -dot(clipN, clipP));
  glm::vec4 clipPlaneCam = transpose(inverse(Vdst)) * planeW;

  // 7) build an oblique projection
  float aspect = float(pp.width) / pp.height;
  glm::mat4 P =
      glm::perspective(glm::radians(camDst.Zoom), aspect, 0.1f, 100.f);
  glm::mat4 Pdst = makeObliqueProj(P, clipPlaneCam);

  // 8) render destination cell into offscreen FBO
  glBindFramebuffer(GL_FRAMEBUFFER, pp.fbo);
  GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBufs);
  glViewport(0, 0, pp.width, pp.height);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ** now use the oblique projection & clip‐plane to cull everything behind **
  //    (no extra gl_ClipDistance needed—oblique proj does it for you)
  renderCell(*portal.destination(), camDst, depth - 1, &portal);

  // 9) restore default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, screenW, screenH);

  // 10) draw the source quad with the rendered texture
  srcQuad.shader()->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, pp.colorTex);
  srcQuad.shader()->setInt("portalTex", 0);

  // feed it the original view & projection so the frame of the portal itself
  // lines up with the scene
  glm::mat4 Vsrc = camSrc.GetViewMatrix();
  glm::mat4 Psrc = glm::perspective(glm::radians(camSrc.Zoom),
                                    float(screenW) / screenH, 0.1f, 100.f);
  srcQuad.shader()->setMat4("uModel", srcQuad.model());
  srcQuad.shader()->setMat4("uView", Vsrc);
  srcQuad.shader()->setMat4("uProj", Psrc);

  // and hand it the portal‐camera’s VP so it can do the 2D uv =
  // clipPos.xy/clipPos.w*0.5+0.5
  srcQuad.shader()->setMat4("uPortalVP", Pdst * Vdst);

  srcQuad.setViewProj(camSrc.GetViewMatrix(),
                      glm::perspective(glm::radians(camSrc.Zoom),
                                       float(screenW) / screenH, 0.1f, 100.f));

  srcQuad.render();
}

void PortalRenderer::init(int w, int h) {
  screenW = w;
  screenH = h;

  portalPasses.resize(2);
  for (auto &pp : portalPasses) {
    pp.width = screenW; // Match screen dimensions
    pp.height = screenH;
    // 1) color texture
    glGenTextures(1, &pp.colorTex);
    glBindTexture(GL_TEXTURE_2D, pp.colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pp.width, pp.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 2) depth renderbuffer
    glGenRenderbuffers(1, &pp.depthRb);
    glBindRenderbuffer(GL_RENDERBUFFER, pp.depthRb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pp.width,
                          pp.height);

    // 3) create & bind FBO
    glGenFramebuffers(1, &pp.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pp.fbo);

    // 4) attach your textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pp.colorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, pp.depthRb);

    // 5) *here* is the magic bit*
    GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  }

  // back to default
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
  // 1. recurse into portals as you already do…
  if (depth > 0) {
    for (auto &p : cell.getPortals())
      if (p.get() != cameFrom)
        renderPortal(*p, cam, depth - 1);
  }

  // 2. draw this cell’s own geometry
  glm::mat4 V = cam.GetViewMatrix();
  glm::mat4 P = glm::perspective(glm::radians(cam.Zoom),
                                 float(screenW) / screenH, 0.1f, 100.f);
  glm::vec3 eye = cam.Position;

  for (auto &g : cell.getGeometry()) {
    if (dynamic_cast<PortalQuad *>(g.get()))
      continue;

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

void PortalUtils::checkPortalTeleport(Scene &scene, Camera &cam) {
  static glm::vec3 prevPos = cam.Position;

  Cell *cur = scene.viewpointCell();
  if (!cur)
    return;

  for (auto &pPtr : cur->getPortals()) {
    Portal *p = pPtr.get();
    auto &quad = static_cast<const PortalQuad &>(p->getSurface());

    glm::vec3 n = quad.normal();
    float d = quad.planeD();

    bool wasFront = glm::dot(n, prevPos) + d > 0.0f;
    bool isBack = glm::dot(n, cam.Position) + d < 0.0f;

    if (wasFront && isBack) {
      // boundary check…
      glm::vec3 upHint =
          (std::abs(n.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
      glm::vec3 R = glm::normalize(glm::cross(upHint, n));
      glm::vec3 U = glm::normalize(glm::cross(n, R));

      glm::vec3 rel = cam.Position - quad.c();
      float x = glm::dot(rel, R), y = glm::dot(rel, U);

      if (std::abs(x) <= quad.halfWidth() && std::abs(y) <= quad.halfHeight()) {
        scene.setViewpoint(p->destination());

        // *** use the original throughPortal here ***
        cam = PortalRenderer::throughPortal(cam, p->transform());
        break;
      }
    }
  }

  prevPos = cam.Position;
}
