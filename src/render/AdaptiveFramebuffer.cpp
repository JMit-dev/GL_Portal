#include "render/FramebufferUtils.h"
#include "render/AdaptiveFramebuffer.h"

void AdaptiveFramebuffer::allocate(int W, int H) {
  w = W;
  h = H;

  if (!fbo)
    glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // (re)create textures
  if (color)
    glDeleteTextures(1, &color);
  if (depth)
    glDeleteTextures(1, &depth);

  color = fb::makeColorTex(w, h);
  depth = fb::makeDepthTex(w, h);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AdaptiveFramebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
void AdaptiveFramebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void AdaptiveFramebuffer::setTargetFraction(float frac) {
  targetFraction = frac;
  // TO DO
}
