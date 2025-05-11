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
                                        float sx, float sy) {
  glm::vec3 right, up;

  if (glm::abs(N.y) == 1.0f) {
    right = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 0, 1);
  } else if (glm::abs(N.x) == 1.0f) {
    right = glm::vec3(0, 0, 1);
    up = glm::vec3(0, 1, 0);
  } else { // Z-facing wall (back wall)
    right = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 1, 0);
  }

  glm::vec3 T = right * sx;
  glm::vec3 B = up * sy;

  constexpr float tilesPerMeter = 1.0f;

  float fullW = sx * 2.0f * tilesPerMeter;
  float fullH = sy * 2.0f * tilesPerMeter;

  return {{{P - T - B, {0.0f, 0.0f}},
           {P + T - B, {fullW, 0.0f}},
           {P + T + B, {fullW, fullH}},

           {P - T - B, {0.0f, 0.0f}},
           {P + T + B, {fullW, fullH}},
           {P - T + B, {0.0f, fullH}}}};
}

TexturedQuad::TexturedQuad(Shader *sh, const glm::vec3 &P, const glm::vec3 &N,
                           float sx, float sy, std::shared_ptr<Texture2D> tex,
                           const glm::mat4 &M)
    : GLShape(sh), texture(std::move(tex)), modelMat(M) {
  auto v = buildQuad(P, glm::normalize(N), sx, sy);

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

  texture->bind(0); // now we are in the render thread
  pShader->setInt("tex0", 0);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
