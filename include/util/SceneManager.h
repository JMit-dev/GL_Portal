#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "portal/Portal.h"
#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "shape/Skybox.h"
#include "shape/TexturedBox.h"
#include "shape/TexturedQuad.h"
#include "util/ResourceCache.h"
#include "util/Shader.h"
#include "util/ShaderStore.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

class SceneManager {
public:
  SceneManager() {
    SceneBuild result = makePortalDemoScene();
    scene = std::move(result.scene);
    animatedTeapot = std::move(result.animatedTeapot);
    fallingCubes = std::move(result.fallingCubes);
  }

  const Scene &currentScene() const { return *scene; }

  Scene &currentSceneMutable() { return *scene; }

  void update(float time) {
    // Animate teapot
    if (animatedTeapot) {
      float angle = glm::radians(time * 30.f);
      float bob = 0.05f * sinf(time * 2.f);
      glm::mat4 model =
          glm::translate(glm::mat4(1.f), glm::vec3(-1.5f, 0.15f + bob, -1.5f));
      model = glm::rotate(model, angle, glm::vec3(0.f, 1.f, 0.f));
      model = glm::scale(model, glm::vec3(0.07f));
      animatedTeapot->setModel(model);
    }

    // Animate falling cubes
    float fallSpeed = 5.f;
    float rotateSpeed = 0.1f;

    for (auto &fc : fallingCubes) {
      fc.y -= fc.fallSpeed * 0.016f;
      fc.angle += fc.rotationSpeed * 0.016f;

      if (fc.y < -50.f) {
        fc.y = 50.f;
        fc.x = randomXZ() + 0.f;
        fc.z = randomXZ() + 120.f;

        // Avoid platform area again on respawn
        if (std::abs(fc.x - 0.f) < 3.f && std::abs(fc.z - 120.f) < 3.f) {
          fc.x += (fc.x > 0.f ? 1.f : -1.f) * 4.f;
          fc.z += (fc.z > 120.f ? 1.f : -1.f) * 4.f;
        }
      }

      glm::mat4 model =
          glm::translate(glm::mat4(1.f), glm::vec3(fc.x, fc.y, fc.z));
      model = glm::rotate(model, fc.angle, fc.rotationAxis);
      model = glm::scale(model, glm::vec3(2.0f));

      for (auto &quad : fc.box->getFaces())
        quad->setModel(model);
    }
  }

private:
  std::unique_ptr<Scene> scene;
  std::shared_ptr<ModelShape> animatedTeapot;

  struct FallingCube {
    std::shared_ptr<TexturedBox> box;
    float x, y, z;
    float angle;
    float fallSpeed;
    glm::vec3 rotationAxis;
    float rotationSpeed;
  };

  std::vector<FallingCube> fallingCubes;

  struct SceneBuild {
    std::unique_ptr<Scene> scene;
    std::shared_ptr<ModelShape> animatedTeapot;
    std::vector<FallingCube> fallingCubes;
  };

  static SceneBuild makePortalDemoScene() {
    SceneBuild out;

    out.scene = std::make_unique<Scene>();
    Shader *skySh = ShaderStore::inst().textured();
    Shader *texSh = ShaderStore::inst().textured();
    Shader *phong = ShaderStore::inst().phong();
    auto chk = ResourceCache::inst().texture("rsrc/textures/checker.png");
    std::vector<std::string> texturePaths = {
        "rsrc/textures/adachi.png", "rsrc/textures/awesomeface.png",
        "rsrc/textures/box.jpg",    "rsrc/textures/cobblestone.png",
        "rsrc/textures/dirt.png",   "rsrc/textures/metal.jpg"};

    std::vector<std::shared_ptr<Texture2D>> cubeTextures;
    for (const auto &path : texturePaths) {
      cubeTextures.push_back(
          ResourceCache::inst().texture(path, true)); // clamp = true
    }

    float PW = 4.f, PH = 0.1f, PD = 4.f;

    // ── Scene A (origin) ─────────────────────────────
    Cell *cell = out.scene->createCell();
    out.scene->setViewpoint(cell);

    std::array<std::shared_ptr<Texture2D>, 6> skyFaces = {
        ResourceCache::inst().texture("rsrc/textures/px.png", true),
        ResourceCache::inst().texture("rsrc/textures/nx.png", true),
        ResourceCache::inst().texture("rsrc/textures/ny.png", true),
        ResourceCache::inst().texture("rsrc/textures/py.png", true),
        ResourceCache::inst().texture("rsrc/textures/pz.png", true),
        ResourceCache::inst().texture("rsrc/textures/nz.png", true)};

    auto skyA = std::make_shared<Skybox>(skySh, skyFaces);
    cell->getGeometry().push_back(skyA);

    auto platformA = std::make_shared<TexturedBox>(
        texSh, glm::vec3(0.f, PH / 2.f, 0.f), PW, PH, PD, chk, true);
    cell->getGeometry().push_back(platformA);

    auto suzanne = std::make_shared<ModelShape>(
        phong, "rsrc/models/suzanne.obj",
        glm::translate(glm::mat4(1.f), glm::vec3(0.f, PH + 0.2f, 0.f)) *
            glm::scale(glm::mat4(1.f), glm::vec3(0.2f)));
    cell->getGeometry().push_back(suzanne);

    out.animatedTeapot = std::make_shared<ModelShape>(
        phong, "rsrc/models/teapot.obj", glm::mat4(1.f));
    cell->getGeometry().push_back(out.animatedTeapot);

    // ── Scene B (offset) ─────────────────────────────
    const glm::vec3 offset(0.f, 0.f, 120.f);
    const glm::vec3 platformCenterB = glm::vec3(0.f, PH / 2.f, 0.f) + offset;

    std::array<std::shared_ptr<Texture2D>, 6> skyFaces1 = {
        ResourceCache::inst().texture("rsrc/textures/px1.png", true),
        ResourceCache::inst().texture("rsrc/textures/nx1.png", true),
        ResourceCache::inst().texture("rsrc/textures/ny1.png", true),
        ResourceCache::inst().texture("rsrc/textures/py1.png", true),
        ResourceCache::inst().texture("rsrc/textures/pz1.png", true),
        ResourceCache::inst().texture("rsrc/textures/nz1.png", true)};

    auto skyB = std::make_shared<Skybox>(skySh, skyFaces1);
    for (auto &face : skyB->getFaces()) {
      glm::mat4 m = face->model();
      m = glm::translate(glm::mat4(1.f), offset) * m;
      face->setModel(m);
      cell->getGeometry().push_back(face);
    }

    auto platformB = std::make_shared<TexturedBox>(texSh, platformCenterB, PW,
                                                   PH, PD, chk, true);
    cell->getGeometry().push_back(platformB);

    // ── Falling cubes ────────────────────────────────
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> distXZ(-45.f, 45.f);
    std::uniform_real_distribution<float> distY(40.f, 50.f);
    std::uniform_real_distribution<float> distSpeed(5.0f, 10.0f);
    std::uniform_real_distribution<float> distSpin(2.0f, 6.0f);
    std::uniform_real_distribution<float> distAxis(-1.f, 1.f);

    for (int i = 0; i < 1000; ++i) {
      float x = distXZ(rng) + offset.x;
      float z = distXZ(rng) + offset.z;
      float y = distY(rng) + offset.y;

      // Avoid platform area at Scene B
      if (std::abs(x - platformCenterB.x) < 3.f &&
          std::abs(z - platformCenterB.z) < 3.f) {
        x += (x > platformCenterB.x ? 1.f : -1.f) * 4.f;
        z += (z > platformCenterB.z ? 1.f : -1.f) * 4.f;
      }

      float cubeSize = 0.5f;
      auto texture = cubeTextures[rng() % cubeTextures.size()];
      auto box = std::make_shared<TexturedBox>(texSh, glm::vec3(0.f), cubeSize,
                                               cubeSize, cubeSize, texture);

      glm::vec3 axis = glm::normalize(
          glm::vec3(distAxis(rng), distAxis(rng), distAxis(rng)));
      float initialAngle = 0.f;

      glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z));
      model = glm::rotate(model, initialAngle, axis);
      model = glm::scale(model, glm::vec3(2.0f));

      for (auto &quad : box->getFaces())
        quad->setModel(model);

      cell->getGeometry().push_back(box);

      out.fallingCubes.push_back(FallingCube{
          box, x, y, z, initialAngle, distSpeed(rng), axis, distSpin(rng)});
    }

    return out;
  }

  static float randomXZ() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(-45.f, 45.f);
    return dist(rng);
  }
};

#endif // SCENE_MANAGER_H
