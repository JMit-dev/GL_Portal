#ifndef SHAPE_TEXTURED_QUAD_H
#define SHAPE_TEXTURED_QUAD_H
#include "shape/GLShape.h"
#include "shape/Texture.h"
#include <GL/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <memory>

class TexturedQuad : public GLShape, public Renderable {
public:
  /// xyz = quad center,  nx ny nz = normal,  sx,sy = half‑size in local axes
  TexturedQuad(Shader *sh, const glm::vec3 &pos, const glm::vec3 &normal,
               float sx, float sy, std::shared_ptr<Texture2D> tex,
               const glm::mat4 &model = glm::mat4(1.f));

  TexturedQuad(Shader *sh, const glm::vec3 &P, const glm::vec3 &N_, float sx,
               float sy, std::shared_ptr<Texture2D> tex, const glm::mat4 &M,
               bool tile);

  TexturedQuad(Shader *sh, std::shared_ptr<Texture2D> tex,
               const glm::mat4 &model = glm::mat4(1.f));

  void setViewProj(const glm::mat4 &v, const glm::mat4 &p) {
    view = v;
    proj = p;
  }

  void render() override;
  glm::vec3 normal() const { return N; }
  float planeD() const { return -glm::dot(N, centre); }
  const glm::mat4 &model() const { return modelMat; } // world transform
  const glm::vec3 &c() const { return centre; }       // world position
  void overrideVAO(GLuint customVao) { vao = customVao; }
  void setModel(const glm::mat4 &m) { modelMat = m; }

private:
  std::shared_ptr<Texture2D> texture;

  glm::mat4 modelMat;
  glm::mat4 view{1.f}, proj{1.f};

  glm::vec3 centre{};
  glm::vec3 N{};
};
#endif
