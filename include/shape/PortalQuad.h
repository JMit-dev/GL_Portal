#ifndef SHAPE_PORTAL_QUAD_H
#define SHAPE_PORTAL_QUAD_H

#include "shape/GLShape.h"
#include "shape/Renderable.h"
#include <glm/glm.hpp>

class PortalQuad : public GLShape, public Renderable {
public:
  PortalQuad(Shader *sh, const glm::vec3 &center, const glm::vec3 &normal,
             float sx, float sy);
  ~PortalQuad() = default;

  void setViewProj(const glm::mat4 &v, const glm::mat4 &p) {
    view = v;
    proj = p;
  }

  void setPortalVP(const glm::mat4 &m) { portalVP = m; }

  void render() override;

  // World-space plane
  glm::vec3 normal() const;
  float planeD() const;
  glm::vec3 c() const;

  const glm::mat4 &model() const { return modelMat; }
  void setModel(const glm::mat4 &m) { modelMat = m; }

  float width() const { return halfW * 2.0f; }
  float height() const { return halfH * 2.0f; }
  float halfWidth() const { return halfW; }
  float halfHeight() const { return halfH; }

private:
  glm::mat4 modelMat;
  glm::mat4 view{1.f}, proj{1.f};
  glm::mat4 portalVP{1.f};

  float halfW{}, halfH{};
};

#endif
