#define GLM_ENABLE_EXPERIMENTAL

#include "shape/PortalQuad.h"
#include "util/Shader.h"
#include <glad/glad.h>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/quaternion.hpp>

// — Constructor —//
PortalQuad::PortalQuad(Shader *sh, const glm::vec3 &P, const glm::vec3 &N_,
                       float sx, float sy)
    : GLShape(sh), halfW(sx), halfH(sy) {
  // 1) build a simple quad in +XY plane, centered at origin:
  float verts[6 * 3] = {
      -sx, -sy, 0.0f, sx, -sy, 0.0f, sx,  sy, 0.0f,

      -sx, -sy, 0.0f, sx, sy,  0.0f, -sx, sy, 0.0f,
  };

  // 2) build a stable orthonormal basis: R (right), U (up), N (normal)
  glm::vec3 N = glm::normalize(N_);
  glm::vec3 upHint =
      (glm::abs(N.y) > 0.999f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
  glm::vec3 R = glm::normalize(glm::cross(upHint, N)); // local +X
  glm::vec3 U = glm::cross(N, R);                      // local +Y

  // 3) pack into a 4×4 rotation matrix whose columns are (R, U, N)
  glm::mat4 basis(1.0f);
  basis[0] = glm::vec4(R, 0.0f);
  basis[1] = glm::vec4(U, 0.0f);
  basis[2] = glm::vec4(N, 0.0f);

  // 4) translate to P and apply the basis
  modelMat = glm::translate(glm::mat4(1.0f), P) * basis;

  // 5) upload the vertex data
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

// — Render the portal quad (with debug‐coloring) —//
void PortalQuad::render() {
  pShader->use();

  pShader->setMat4("uModel", modelMat);
  pShader->setMat4("uView", view);
  pShader->setMat4("uProj", proj);

  pShader->setMat4("uPortalVP", portalVP);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

// — World‐space center point —//
glm::vec3 PortalQuad::c() const {
  return glm::vec3(modelMat * glm::vec4(0, 0, 0, 1));
}

// — World‐space surface normal —//
glm::vec3 PortalQuad::normal() const {
  return glm::normalize(glm::mat3(modelMat) * glm::vec3(0, 0, 1));
}

// — Plane “D” term in world‐space —//
float PortalQuad::planeD() const { return -glm::dot(normal(), c()); }
