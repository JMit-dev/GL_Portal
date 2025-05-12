#ifndef SHAPE_MODEL_SHAPE_H
#define SHAPE_MODEL_SHAPE_H

#include "shape/Mesh.h"
#include "util/Shader.h"
#include <assimp/scene.h>
#include <memory>
#include <string>

class ModelShape : public Renderable {
public:
  ModelShape(Shader *shader, const std::string &path,
             const glm::mat4 &model = glm::mat4(1.f));

  void render() override;

  // camera matrices pushed each frame by the renderer
  void setViewProj(const glm::mat4 &v, const glm::mat4 &p,
                   const glm::vec3 &eye);

  void setModel(const glm::mat4 &m) { modelMat = m; }

private:
  void loadNode(const aiNode *, const aiScene *, Shader *);

  std::vector<std::unique_ptr<Mesh>> meshes;
  glm::mat4 modelMat;

  // per‑frame
  glm::mat4 view{1.f}, proj{1.f};
  glm::vec3 eye{0.f};

  Shader *shader{nullptr};

public:
  Shader *getShader() const // read‑only accessor
  {
    return shader;
  }
};
#endif
