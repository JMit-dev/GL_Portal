#pragma once
#include "shape/Renderable.h"
#include "shape/Texture.h"
#include "shape/TexturedQuad.h"
#include "util/Shader.h"
#include <array>
#include <glm/glm.hpp>
#include <memory>

class TexturedBox : public Renderable {
public:
  TexturedBox(Shader *sh, const glm::vec3 &C, float W, float H, float D,
              std::shared_ptr<Texture2D> tex, bool tile = false);
  void render() override;
  void setViewProj(const glm::mat4 &v, const glm::mat4 &p) {
    view = v;
    proj = p;
  }

  const std::array<std::unique_ptr<TexturedQuad>, 6> &getFaces() const {
    return faces;
  }

private:
  std::array<std::unique_ptr<class TexturedQuad>, 6> faces;
  glm::mat4 view{1.f}, proj{1.f};
};
