#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "shape/TexturedQuad.h"
#include "util/ResourceCache.h"
#include "util/Shader.h"
#include "util/ShaderStore.h"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <memory>
#include <vector>

class SceneManager {
public:
  SceneManager() {
    scenes.resize(6); // allocate empty slots only
                      // nothing heavy happens here
  }

  // -----------------------------------------------------------------
  // accessor
  // -----------------------------------------------------------------
  const Scene &currentScene() {
    // build on demand
    if (!scenes[curIndex])
      buildScene(curIndex);
    return *scenes[curIndex];
  }

  void select(std::size_t i) { curIndex = i % scenes.size(); }

private:
  void buildScene(std::size_t idx) {
    switch (idx) {
    case 0:
      scenes[idx] = makeMirrorHallway();
      break;
    case 1:
      scenes[idx] = makeTeapotScene();
      break;
    default:
      scenes[idx] = makeEmptyScene();
      break;
    }
  }

  static inline glm::vec3 inward(glm::vec3 n, glm::vec3 camDir = {0, 0, 1}) {
    // If the normal points roughly away from the corridor centre (‑Z),
    // flip it so the quad faces the inside.
    return (glm::dot(n, camDir) < 0.f) ? -n : n;
  }

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

  static std::unique_ptr<Scene> makeMirrorHallway() {
    auto scene = std::make_unique<Scene>();
    Cell *cell = scene->createCell();
    scene->setViewpoint(cell);

    Shader *texSh = ShaderStore::inst().textured();
    auto check = ResourceCache::inst().texture("rsrc/textures/checker.png");

    /*  inside extents ( full sizes )  */
    const float W = 4.0f; // width   (X)
    const float L = 8.0f; // length  (Z)
    const float H = 2.5f; // height  (Y)

    /*  helper: centre/normal + *half*‑sizes  */
    auto addQuad = [&](glm::vec3 centre, glm::vec3 normal, float fullSX,
                       float fullSY) {
      cell->getGeometry().push_back(
          std::make_shared<TexturedQuad>(texSh, centre,
                                         normal,        // already inward
                                         fullSX * 0.5f, // half‑sizes only once!
                                         fullSY * 0.5f, check));
    };

    /* coordinate system: +Y up, +Z forward, +X right
       we centre the corridor on the origin so every half‑size is ± value */
    const float hx = W * 0.5f;
    const float hy = H * 0.5f;
    const float hz = L * 0.5f;

    /* ─────────── six faces ─────────── */
    addQuad({0, -hy, 0}, {0, 1, 0}, W, L); // floor
    addQuad({0, hy, 0}, {0, -1, 0}, W, L); // ceiling

    addQuad({-hx, 0, 0}, {1, 0, 0}, L, H); // left wall  (+X normal)
    addQuad({hx, 0, 0}, {-1, 0, 0}, L, H); // right wall (–X normal)

    /* leave the front (+Z) empty for a portal; put a solid back wall at –Z */
    addQuad({0, 0, -hz}, {0, 0, 1}, W, H); // back wall (+Z normal)

    return scene;
  }

  std::vector<std::unique_ptr<Scene>> scenes;
  std::size_t curIndex{0};
};

#endif
