#define GLM_ENABLE_EXPERIMENTAL

#include "shape/TexturedQuad.h"
#include "util/Shader.h"
#include <array>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/orthonormalize.hpp>

struct TVertex {
  glm::vec3 pos;
  glm::vec2 uv;
};

static std::array<TVertex, 6> buildQuad(const glm::vec3 &P, const glm::vec3 &N,
                                        float sx, float sy, bool tile) {
  glm::vec3 upHint =
      glm::abs(N.y) > 0.999f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);

  glm::vec3 R = glm::normalize(glm::cross(upHint, N));
  glm::vec3 U = glm::normalize(glm::cross(N, R));

  glm::vec3 T = R * sx;
  glm::vec3 B = U * sy;

  glm::vec3 c0 = P - T - B;
  glm::vec3 c1 = P + T - B;
  glm::vec3 c2 = P + T + B;
  glm::vec3 c3 = P - T + B;

  if (glm::dot(glm::cross(c1 - c0, c2 - c0), N) < 0.0f)
    std::swap(c1, c3);

  float u1 = tile ? 2.f * sx : 1.f;
  float v1 = tile ? 2.f * sy : 1.f;

  return {{{c0, {0, 0}},
           {c1, {u1, 0}},
           {c2, {u1, v1}},
           {c0, {0, 0}},
           {c2, {u1, v1}},
           {c3, {0, v1}}}};
}

TexturedQuad::TexturedQuad(Shader *sh, const glm::vec3 &P, const glm::vec3 &N_,
                           float sx, float sy, std::shared_ptr<Texture2D> tex,
                           const glm::mat4 &M, bool tile)
    : GLShape(sh), texture(std::move(tex)), modelMat(M), centre(P),
      N(glm::normalize(N_)) {
  auto v = buildQuad(P, N, sx, sy, tile);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(v), v.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex),
                        (void *)offsetof(TVertex, uv));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
}

void TexturedQuad::render() {
  pShader->use();
  pShader->setMat4("model", modelMat);
  pShader->setMat4("view", view);
  pShader->setMat4("proj", proj);

  if (texture) {
    texture->bind(0);
    pShader->setInt("tex0", 0);
  } else {
    // optional: set solid color fallback
    pShader->setInt("tex0", -1);
  }

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

TexturedQuad::TexturedQuad(Shader *sh, std::shared_ptr<Texture2D> tex,
                           const glm::mat4 &M)
    : GLShape(sh), texture(std::move(tex)), modelMat(M) {
  // nothing: user will override VAO and bind their own vertex data
}
