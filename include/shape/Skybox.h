#pragma once
#include "shape/Renderable.h"
#include "shape/Texture.h"
#include "shape/TexturedQuad.h"
#include "util/Shader.h"
#include <array>
#include <memory>

class Skybox : public Renderable {
public:
  Skybox(Shader *sh,
         const std::array<std::shared_ptr<Texture2D>, 6> &faceTextures);

  void setViewProj(const glm::mat4 &v, const glm::mat4 &p) {
    for (auto &face : faces)
      face->setViewProj(v, p);
  }

  void render() override;

  const std::array<std::shared_ptr<TexturedQuad>, 6> &getFaces() const {
    return faces;
  }
  std::array<std::shared_ptr<TexturedQuad>, 6> &getFaces() { return faces; }

private:
  std::array<std::shared_ptr<TexturedQuad>, 6> faces;
};
