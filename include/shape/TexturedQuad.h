#ifndef SHAPE_TEXTURED_QUAD_H
#define SHAPE_TEXTURED_QUAD_H
#include "shape/GLShape.h"
#include "shape/Texture.h"
#include <glm/glm.hpp>
#include <memory>

class TexturedQuad : public GLShape, public Renderable {
public:
  /// xyz = quad center,  nx ny nz = normal,  sx,sy = half‑size in local axes
  TexturedQuad(Shader *sh, const glm::vec3 &pos, const glm::vec3 &normal,
               float sx, float sy, std::shared_ptr<Texture2D> tex,
               const glm::mat4 &model = glm::mat4(1.f));

  void setViewProj(const glm::mat4 &v, const glm::mat4 &p) {
    view = v;
    proj = p;
  }

  void render() override;

private:
  std::shared_ptr<Texture2D> texture;

  glm::mat4 modelMat;
  glm::mat4 view{1.f}, proj{1.f};
};
#endif
