#ifndef ADAPTIVE_FRAMEBUFFER_H
#define ADAPTIVE_FRAMEBUFFER_H
#include <glad/glad.h>

class AdaptiveFramebuffer {
public:
  AdaptiveFramebuffer() = default;
  void allocate(int w, int h);
  void bind();
  void unbind();
  GLuint colorTex() const { return color; }

  // choose next size based on screen-coverage fraction (0-1)
  void setTargetFraction(float frac);

private:
  GLuint fbo{0}, color{0}, depth{0};
  int w{0}, h{0};
  float targetFraction{1.f};
};

#endif
