#include <glad/glad.h>

#include "shape/Skybox.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Converts a direction vector to equirectangular UV coordinates
static glm::vec2 dirToEquirectUV(const glm::vec3 &dir) {
  glm::vec3 n = glm::normalize(dir);

  float u = 0.5f + atan2(n.z, n.x) / (2.0f * glm::pi<float>()); // z before x

  float v = 0.5f - asin(n.y) / glm::pi<float>();
  return {u, v};
}

// Builds a quad facing direction N with correct panoramic UVs
static std::shared_ptr<TexturedQuad>
buildSkyQuad(Shader *shader, const glm::vec3 &center, const glm::vec3 &normal,
             float size, std::shared_ptr<Texture2D> tex, int rotation = 0) {
  // Build basis
  glm::vec3 upHint =
      glm::abs(normal.y) > 0.999f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
  glm::vec3 R = glm::normalize(glm::cross(upHint, normal)); // right
  glm::vec3 U = glm::normalize(glm::cross(normal, R));      // up

  glm::vec3 T = R * size;
  glm::vec3 B = U * size;

  glm::vec3 c0 = center - T - B;
  glm::vec3 c1 = center + T - B;
  glm::vec3 c2 = center + T + B;
  glm::vec3 c3 = center - T + B;

  // Default UVs (counter-clockwise)
  glm::vec2 uv[4] = {
      {0.f, 0.f}, // bottom-left
      {1.f, 0.f}, // bottom-right
      {1.f, 1.f}, // top-right
      {0.f, 1.f}  // top-left
  };

  // Rotate UVs clockwise in 90° steps
  for (int i = 0; i < (rotation % 4); ++i) {
    std::swap(uv[0], uv[3]);
    std::swap(uv[0], uv[2]);
    std::swap(uv[0], uv[1]);
  }

  // Vertex data with rotated UVs
  struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
  };
  std::array<Vertex, 6> verts = {Vertex{c0, uv[0]}, Vertex{c1, uv[1]},
                                 Vertex{c2, uv[2]}, Vertex{c0, uv[0]},
                                 Vertex{c2, uv[2]}, Vertex{c3, uv[3]}};

  // Upload to GPU
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, uv));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  auto quad = std::make_shared<TexturedQuad>(shader, tex);
  quad->overrideVAO(vao);
  return quad;
}

Skybox::Skybox(Shader *sh,
               const std::array<std::shared_ptr<Texture2D>, 6> &tex) {
  const float S = 50.f;

  // px, nx, py, ny, pz, nz
  faces[0] = buildSkyQuad(sh, {+S, 0, 0}, {-1, 0, 0}, S, tex[0]); // +X = right
  faces[1] = buildSkyQuad(sh, {-S, 0, 0}, {+1, 0, 0}, S, tex[1]); // -X = left

  faces[2] = buildSkyQuad(sh, {0, +S, 0}, {0, -1, 0}, S, tex[2]); // +Y = up
  faces[3] =
      buildSkyQuad(sh, {0, -S, 0}, {0, +1, 0}, S, tex[3], 2); // -Y = down

  faces[4] = buildSkyQuad(sh, {0, 0, -S}, {0, 0, +1}, S, tex[4]); // -Z = front
  faces[5] = buildSkyQuad(sh, {0, 0, +S}, {0, 0, -1}, S, tex[5]); // +Z = back
}
void Skybox::render() {
  glDepthFunc(GL_LEQUAL);

  for (auto &f : faces)
    f->render();

  glDepthFunc(GL_LESS);
}
