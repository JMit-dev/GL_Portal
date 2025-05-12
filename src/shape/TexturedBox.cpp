#include "glad/glad.h"

#include "shape/TexturedBox.h"
#include "shape/TexturedQuad.h"

TexturedBox::TexturedBox(Shader *sh, const glm::vec3 &C, float W, float H,
                         float D, std::shared_ptr<Texture2D> tex, bool tile) {
  faces[0] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(+W / 2, 0, 0),
                                            glm::vec3(-1, 0, 0), D / 2, H / 2,
                                            tex, glm::mat4(1.f), tile);

  faces[1] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(-W / 2, 0, 0),
                                            glm::vec3(+1, 0, 0), D / 2, H / 2,
                                            tex, glm::mat4(1.f), tile);

  faces[2] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(0, +H / 2, 0),
                                            glm::vec3(0, -1, 0), W / 2, D / 2,
                                            tex, glm::mat4(1.f), tile);

  faces[3] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(0, -H / 2, 0),
                                            glm::vec3(0, +1, 0), W / 2, D / 2,
                                            tex, glm::mat4(1.f), tile);

  faces[4] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(0, 0, +D / 2),
                                            glm::vec3(0, 0, -1), W / 2, H / 2,
                                            tex, glm::mat4(1.f), tile);

  faces[5] = std::make_unique<TexturedQuad>(sh, C + glm::vec3(0, 0, -D / 2),
                                            glm::vec3(0, 0, +1), W / 2, H / 2,
                                            tex, glm::mat4(1.f), tile);
}
void TexturedBox::render() {
  for (auto &f : faces) {
    // Inherit view/proj from the box
    if (auto *q = dynamic_cast<TexturedQuad *>(f.get())) {
      q->setViewProj(view, proj);
    }
    f->render();
  }
}
