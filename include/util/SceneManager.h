#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "util/Shader.h"
#include "util/ShaderStore.h"
#include <memory>
#include <vector>

class SceneManager {
public:
  SceneManager() {
    scenes.reserve(6);

    scenes.emplace_back(makeTeapotScene()); // room 0 = teapot
    for (int i = 1; i < 6; ++i)             // 5 placeholders
      scenes.emplace_back(makeEmptyScene());
  }

  const Scene &currentScene() const { return *scenes[curIndex]; }
  void select(std::size_t i) { curIndex = i % scenes.size(); }

  static std::unique_ptr<Scene> makeEmptyScene() {
    auto s = std::make_unique<Scene>();
    s->createCell();
    s->setViewpoint(s->cells().front().get());
    return s;
  }

  static std::unique_ptr<Scene> makeTeapotScene() {
    auto s = std::make_unique<Scene>();
    Cell *cell = s->createCell();
    s->setViewpoint(cell);

    Shader *phong = ShaderStore::inst().phong();

    auto teapot = std::make_shared<ModelShape>(
        phong, "rsrc/models/teapot.obj",
        glm::scale(glm::mat4(1.f), glm::vec3(0.5f)));
    cell->getGeometry().push_back(teapot);
    return s;
  }

private:
  std::vector<std::unique_ptr<Scene>> scenes;
  std::size_t curIndex{0};
};

#endif
