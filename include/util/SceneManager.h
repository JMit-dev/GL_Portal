#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "portal/Portal.h"
#include "portal/Scene.h"
#include "shape/ModelShape.h"
#include "shape/PortalQuad.h"
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
    auto result = makePortalDemoScene();
    scene = std::move(result.scene);
    animatedTeapot = std::move(result.animatedTeapot);
    fallingCubes = std::move(result.fallingCubes);

    const float PW = 4, PH = 0.1f;
    glm::vec3 smallSize{1, 1.5f, 0.1f};

    glm::vec3 leftC{-PW * 0.5f + smallSize.z * 0.5f, PH + smallSize.y * 0.5f,
                    0};
    glm::vec3 rightC{+PW * 0.5f - smallSize.z * 0.5f, PH + smallSize.y * 0.5f,
                     0};

    portalA = leftC;
    portalB = rightC;

    shootProjectile(portalA + glm::vec3(0, 0.5f, 0), glm::vec3(1, 0, 0),
                    scene->viewpointCell());
  }

  const Scene &currentScene() const { return *scene; }

  Scene &currentSceneMutable() { return *scene; }

  void update(float time) {
    float dt = 0.016f;

    // move & wrap projectile
    for (auto &pr : projectiles) {
      pr.pos += pr.vel * dt;

      // if we've gone past portalB.x, reset to portalA:
      if (pr.pos.x > portalB.x + resetMargin) {
        pr.pos = portalA + glm::vec3(0, 0.5f, 0);
      }

      // update its ModelShape
      glm::mat4 M = glm::translate(glm::mat4(1.0f), pr.pos) *
                    glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
      pr.model->setModel(M);
    }
    if (animatedTeapot) {
      float angle = glm::radians(time * 30.f);
      float bob = 0.05f * sinf(time * 2.f);

      // PH = 0.1, clearance = 0.1
      const float baseY = 0.1f + 0.1f; // floor height + teapot model offset
      const float baseZ =
          4.0f * 0.5f - 0.1f; // +PW/2 - clearance = +2 - 0.1 = +1.9

      glm::mat4 model =
          glm::translate(glm::mat4(1.f), glm::vec3(0.f, baseY + bob, baseZ));
      model = glm::rotate(model, angle, glm::vec3(0.f, 1.f, 0.f));
      model = glm::scale(model, glm::vec3(0.07f));

      animatedTeapot->setModel(model);
    }

    for (auto &fc : fallingCubes) {
      fc.y -= fc.fallSpeed * 0.016f;
      fc.angle += fc.rotationSpeed * 0.016f;

      if (fc.y < -50.f) {
        fc.y = 50.f;
        fc.x = randomXZ();
        fc.z = randomXZ() + 120.f;

        if (std::abs(fc.x) < 3.f && std::abs(fc.z - 120.f) < 3.f) {
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

  void shootProjectile(const glm::vec3 &start, const glm::vec3 &dir,
                       Cell *cell) {
    Projectile p;
    p.pos = start;
    p.vel = glm::normalize(dir) * projectileSpeed;
    p.model = std::make_shared<ModelShape>(
        ShaderStore::inst().phong(), "rsrc/models/sphere.obj",
        glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
    cell->getGeometry().push_back(p.model);
    projectiles.push_back(std::move(p));
  }

private:
  std::unique_ptr<Scene> scene;
  std::shared_ptr<ModelShape> animatedTeapot;

  glm::vec3 portalA, portalB;
  float resetMargin{0.1f};

  struct Projectile {
    glm::vec3 pos, vel;
    std::shared_ptr<ModelShape> model;
  };
  std::vector<Projectile> projectiles;
  float projectileSpeed{8.0f};

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
    // shaders & textures
    Shader *skySh = ShaderStore::inst().textured();
    Shader *texSh = ShaderStore::inst().textured();
    Shader *phong = ShaderStore::inst().phong();
    Shader *portalSh = ShaderStore::inst().portal_quad();
    auto chk = ResourceCache::inst().texture("rsrc/textures/checker.png");
    auto metal = ResourceCache::inst().texture("rsrc/textures/metal.jpg", true);

    // platform dims
    const float PW = 4.f, PH = 0.1f, PD = 4.f;

    // main volumetric portal
    glm::vec3 bigSize(1.5f, 2.5f, 0.1f);
    glm::vec3 bigA(0, PH + bigSize.y * 0.5f, -PW * 0.5f + 0.25f);
    glm::vec3 hall(0, 0, 120.f);
    glm::vec3 bigB = bigA + hall + glm::vec3(0, 0, bigSize.z * 0.5f);

    Cell *cell = out.scene->createCell();
    out.scene->setViewpoint(cell);

    // sky + floor
    auto skyA = std::make_shared<Skybox>(
        skySh,
        std::array{
            ResourceCache::inst().texture("rsrc/textures/px.png", true),
            ResourceCache::inst().texture("rsrc/textures/nx.png", true),
            ResourceCache::inst().texture("rsrc/textures/ny.png", true),
            ResourceCache::inst().texture("rsrc/textures/py.png", true),
            ResourceCache::inst().texture("rsrc/textures/pz.png", true),
            ResourceCache::inst().texture("rsrc/textures/nz.png", true)});
    cell->getGeometry().push_back(skyA);
    cell->getGeometry().push_back(std::make_shared<TexturedBox>(
        texSh, glm::vec3(0, PH * 0.5f, 0), PW, PH, PD, chk, true));

    // Suzanne + teapot
    cell->getGeometry().push_back(std::make_shared<ModelShape>(
        phong, "rsrc/models/suzanne.obj",
        glm::translate(glm::mat4(1), glm::vec3(0, PH + 0.2f, 0)) *
            glm::scale(glm::mat4(1), glm::vec3(0.2f))));
    out.animatedTeapot = std::make_shared<ModelShape>(
        phong, "rsrc/models/teapot.obj", glm::mat4(1));
    cell->getGeometry().push_back(out.animatedTeapot);

    // second sky+floor
    auto skyB = std::make_shared<Skybox>(
        skySh,
        std::array{
            ResourceCache::inst().texture("rsrc/textures/px1.png", true),
            ResourceCache::inst().texture("rsrc/textures/nx1.png", true),
            ResourceCache::inst().texture("rsrc/textures/ny1.png", true),
            ResourceCache::inst().texture("rsrc/textures/py1.png", true),
            ResourceCache::inst().texture("rsrc/textures/pz1.png", true),
            ResourceCache::inst().texture("rsrc/textures/nz1.png", true)});
    for (auto &f : skyB->getFaces()) {
      f->setModel(glm::translate(glm::mat4(1), hall) * f->model());
      cell->getGeometry().push_back(f);
    }
    cell->getGeometry().push_back(std::make_shared<TexturedBox>(
        texSh, glm::vec3(0, PH * 0.5f, 0) + hall, PW, PH, PD, chk, true));

    // add main volumetric portal
    addVolumetricPortal(cell, portalSh, bigA, bigB, bigSize);

    // add two smaller side portals
    // ─── two small side‐portals whose LARGE faces face each other ────

    // ─── Two small side-portals whose LARGE faces face each other ────
    glm::vec3 smallSize(1.0f, 1.5f, 0.1f);

    // Left portal sits just inside the –X edge
    glm::vec3 leftC(-PW * 0.5f + smallSize.z * 0.5f, // x
                    PH + smallSize.y * 0.5f,         // y
                    0.0f);                           // z

    // Right portal sits just inside the +X edge
    glm::vec3 rightC(PW * 0.5f - smallSize.z * 0.5f, // x
                     PH + smallSize.y * 0.5f,        // y
                     0.0f);                          // z

    // Build their world→world transform (rotate 180° around Y then translate)
    auto quadL = std::make_shared<PortalQuad>(
        portalSh,
        glm::vec3(0.0f),    // center passed in via setModel below
        glm::vec3(0, 0, 1), // initial normal (we’ll rotate it)
        smallSize.x * 0.5f, // half-width of quad
        smallSize.y * 0.5f  // half-height of quad
    );
    auto quadR = std::make_shared<PortalQuad>(
        portalSh, glm::vec3(0.0f), glm::vec3(0, 0, 1), smallSize.x * 0.5f,
        smallSize.y * 0.5f);

    // Now position and *rotate* them in place so their “front” (+Z) points
    // inward:

    // Left portal sits at leftC, we want its +Z to point toward +X → rotate
    // -90° about Y
    glm::mat4 modelL =
        glm::translate(glm::mat4(1.0f), leftC) *
        glm::rotate(glm::mat4(1.0f),
                    glm::radians(+90.0f), // <— +90 so quadL’s +Z points +X
                    glm::vec3(0, 1, 0));
    quadL->setModel(modelL);

    glm::mat4 modelR =
        glm::translate(glm::mat4(1.0f), rightC) *
        glm::rotate(glm::mat4(1.0f),
                    glm::radians(-90.0f), // <— -90 so quadR’s +Z points -X
                    glm::vec3(0, 1, 0));
    quadR->setModel(modelR);

    cell->getGeometry().push_back(quadL);
    cell->getGeometry().push_back(quadR);

    // 2) Build ML/MR *exactly* the same way:
    glm::mat4 ML =
        glm::translate(glm::mat4(1.0f), leftC) *
        glm::rotate(glm::mat4(1.0f), glm::radians(+90.0f), glm::vec3(0, 1, 0));

    glm::mat4 MR =
        glm::translate(glm::mat4(1.0f), rightC) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));

    glm::mat4 L2R = MR * glm::inverse(ML);
    glm::mat4 R2L = glm::inverse(L2R);

    auto pLR = std::make_shared<Portal>(quadL, cell, L2R);
    auto pRL = std::make_shared<Portal>(quadR, cell, R2L);
    pLR->setFlipView(true);
    pRL->setFlipView(true);
    pLR->setDestinationPortal(pRL.get());
    pRL->setDestinationPortal(pLR.get());
    cell->addPortal(pLR);
    cell->addPortal(pRL);
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> dXZ(-45.f, 45.f), dY(40.f, 50.f),
        dSp(5.f, 10.f), dRot(2.f, 6.f), dA(-1.f, 1.f);
    std::vector<std::shared_ptr<Texture2D>> cubeTexs;
    for (auto &p : std::vector<std::string>{
             "rsrc/textures/adachi.png", "rsrc/textures/awesomeface.png",
             "rsrc/textures/box.jpg", "rsrc/textures/cobblestone.png",
             "rsrc/textures/dirt.png", "rsrc/textures/metal.jpg"})
      cubeTexs.push_back(ResourceCache::inst().texture(p, true));

    for (int i = 0; i < 100; ++i) {
      FallingCube fc;
      fc.x = dXZ(rng) + hall.x;
      fc.z = dXZ(rng) + hall.z;
      fc.y = dY(rng) + hall.y;
      fc.fallSpeed = dSp(rng);
      fc.rotationSpeed = dRot(rng);
      fc.rotationAxis = glm::normalize(glm::vec3(dA(rng), dA(rng), dA(rng)));
      fc.angle = 0.f;
      fc.box =
          std::make_shared<TexturedBox>(texSh, glm::vec3(0), 0.5f, 0.5f, 0.5f,
                                        cubeTexs[rng() % cubeTexs.size()]);
      glm::mat4 m = glm::translate(glm::mat4(1), glm::vec3(fc.x, fc.y, fc.z));
      m = glm::rotate(m, fc.angle, fc.rotationAxis);
      m = glm::scale(m, glm::vec3(2.f));
      for (auto &q : fc.box->getFaces())
        q->setModel(m);
      cell->getGeometry().push_back(fc.box);
      out.fallingCubes.push_back(fc);
    }

    /*// non euclid*/
    /**/
    /*// Non-Euclidean geometry fixtures*/
    /**/
    /*float corridorLen = 1.5f;*/
    /*float corridorH = 2.0f;*/
    /*float wallT = 0.1f;*/
    /*glm::vec2 doorSz{1.0f, 2.0f};*/
    /**/
    /*glm::vec3 base = hall;*/
    /**/
    /*// —— Door‐frame beams ——*/
    /*// —— Door‐frame beams ——*/
    /*float beamW = wallT;    // thickness*/
    /*float beamH = doorSz.y; // full door height*/
    /*float beamD = wallT;    // depth*/
    /**/
    /*// base transform: position at left edge, rotate so local +Z faces world
     * +Z*/
    /*glm::mat4 doorFrameM =*/
    /*    glm::translate(glm::mat4(1.f), base + glm::vec3(-PW * 0.5f + 0.6f,*/
    /*                                                    0.5f * doorSz.y, 0.f))
     * **/
    /*    glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0, 1,
     * 0));*/
    /**/
    /*auto makeBeam = [&](glm::vec3 offs, glm::vec3 dims) {*/
    /*  auto b = std::make_shared<TexturedBox>(texSh, glm::vec3(0), dims.x,*/
    /*                                         dims.y, dims.z, metal, true);*/
    /*  glm::mat4 local =*/
    /*      glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0, 1, 0))
     * **/
    /*      glm::translate(glm::mat4(1.f), offs);*/
    /**/
    /*  glm::mat4 M = doorFrameM * local;*/
    /*  for (auto &q : b->getFaces())*/
    /*    q->setModel(M * q->model());*/
    /*  cell->getGeometry().push_back(b);*/
    /*};*/
    /**/
    /*// left post*/
    /*makeBeam(glm::vec3(-doorSz.x * 0.5f, 0.f, 0.f),*/
    /*         glm::vec3(beamW, beamH, beamD));*/
    /*// right post*/
    /*makeBeam(glm::vec3(+doorSz.x * 0.5f, 0.f, 0.f),*/
    /*         glm::vec3(beamW, beamH, beamD));*/
    /*// top beam (as before)*/
    /*makeBeam(*/
    /*    glm::vec3(0.f, doorSz.y * 0.5f + beamW * 0.5f, 0.f),*/
    /*    glm::vec3(*/
    /*        doorSz.x + beamW * 2, beamW,*/
    /*        beamD)); // === CORRIDOR on RIGHT side (+X), rotated to face +Z
     * ===*/
    /*glm::vec3 corridorStart = base + glm::vec3(+PW * 0.5f - 0.6f, 0.f, 0.f);*/
    /**/
    /*glm::mat4 corridorBase =*/
    /*    glm::translate(glm::mat4(1.f), corridorStart) **/
    /*    glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0, 1,
     * 0));*/
    /**/
    /*// Floor*/
    /*glm::mat4 floorM =*/
    /*    corridorBase **/
    /*    glm::translate(glm::mat4(1.f),*/
    /*                   glm::vec3(0.5f * corridorLen, PH * 0.5f, 0));*/
    /*auto floor = std::make_shared<TexturedBox>(texSh, glm::vec3(0),
     * corridorLen,*/
    /*                                           PH, wallT, chk, true);*/
    /*for (auto &q : floor->getFaces())*/
    /*  q->setModel(floorM * q->model());*/
    /*cell->getGeometry().push_back(floor);*/
    /**/
    /*// Ceiling*/
    /*glm::mat4 ceilM =*/
    /*    corridorBase **/
    /*    glm::translate(glm::mat4(1.f),*/
    /*                   glm::vec3(0.5f * corridorLen, corridorH - PH * 0.5f,
     * 0));*/
    /*auto ceil = std::make_shared<TexturedBox>(texSh, glm::vec3(0),
     * corridorLen,*/
    /*                                          PH, doorSz.x, chk, true);*/
    /*for (auto &q : ceil->getFaces())*/
    /*  q->setModel(ceilM * q->model());*/
    /*cell->getGeometry().push_back(ceil);*/
    /**/
    /*// Left wall*/
    /*glm::mat4 wallLM =*/
    /*    corridorBase **/
    /*    glm::translate(*/
    /*        glm::mat4(1.f),*/
    /*        glm::vec3(0.5f * corridorLen, corridorH * 0.5f, -doorSz.x *
     * 0.5f));*/
    /*auto wallL = std::make_shared<TexturedBox>(texSh, glm::vec3(0),
     * corridorLen,*/
    /*                                           corridorH, wallT, metal,
     * true);*/
    /*for (auto &q : wallL->getFaces())*/
    /*  q->setModel(wallLM * q->model());*/
    /*cell->getGeometry().push_back(wallL);*/
    /**/
    /*// Right wall*/
    /*glm::mat4 wallRM =*/
    /*    wallLM * glm::translate(glm::mat4(1.f), glm::vec3(0, 0, doorSz.x));*/
    /*auto wallR = std::make_shared<TexturedBox>(texSh, glm::vec3(0),
     * corridorLen,*/
    /*                                           corridorH, wallT, metal,
     * true);*/
    /*for (auto &q : wallR->getFaces())*/
    /*  q->setModel(wallRM * q->model());*/
    /*cell->getGeometry().push_back(wallR);*/
    /**/
    /*// carve two portal-quads in this same cell*/
    /**/
    /*// Portal 1: door frame front face*/
    /*glm::mat4 doorFrontM =*/
    /*    glm::translate(glm::mat4(1.f), base + glm::vec3(-PW * 0.5f + 0.6f,*/
    /*                                                    doorSz.y * 0.5f,
     * 0.f));*/
    /*auto frameA = std::make_shared<PortalQuad>(*/
    /*    portalSh,*/
    /*    glm::vec3(0),       // center is overridden by setModel*/
    /*    glm::vec3(0, 0, 1), // local +Z (rotated ⇒ world +X)*/
    /*    doorSz.x * 0.5f, doorSz.y * 0.5f);*/
    /*frameA->setModel(doorFrontM);*/
    /*cell->getGeometry().push_back(frameA);*/
    /**/
    /*// Portal 2: corridor front face*/
    /*glm::mat4 corridorFrontM =*/
    /*    glm::translate(glm::mat4(1.f), base + glm::vec3(+PW * 0.5f - 0.6f,*/
    /*                                                    doorSz.y * 0.5f, 0.f))
     * **/
    /*    glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0, 1,
     * 0));*/
    /*auto frameB = std::make_shared<PortalQuad>(*/
    /*    portalSh, glm::vec3(0),*/
    /*    glm::vec3(0, 0, -1), // **flipped** local Z ⇒ world -X*/
    /*    doorSz.x * 0.5f, doorSz.y * 0.5f);*/
    /*frameB->setModel(corridorFrontM);*/
    /*cell->getGeometry().push_back(frameB);*/
    /**/
    /*{*/
    /**/
    /*  glm::mat4 AtoB = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),*/
    /*                               glm::vec3(0, 1, 0)) **/
    /*                   corridorFrontM * glm::inverse(doorFrontM);*/
    /**/
    /*  glm::mat4 BtoA = glm::inverse(AtoB);*/
    /*  auto pAB = std::make_shared<Portal>(frameA, cell, AtoB);*/
    /*  auto pBA = std::make_shared<Portal>(frameB, cell, BtoA);*/
    /*  pAB->setDestinationPortal(pBA.get());*/
    /*  pBA->setDestinationPortal(pAB.get());*/
    /*  cell->addPortal(pAB);*/
    /*  cell->addPortal(pBA);*/
    /*}*/
    /**/
    /*// back faces*/
    /**/
    /*// door back face ⇒ must face world -X, so use local -Z again*/
    /*glm::mat4 doorBackM = glm::translate(*/
    /*    glm::mat4(1.f),*/
    /*    base + glm::vec3(-PW * 0.5f + 0.6f, doorSz.y * 0.5f, +0.001f));*/
    /*auto frameA2 = std::make_shared<PortalQuad>(*/
    /*    portalSh, glm::vec3(0), glm::vec3(0, 0, -1), // local -Z ⇒ world -X*/
    /*    doorSz.x * 0.5f, doorSz.y * 0.5f);*/
    /*frameA2->setModel(doorBackM);*/
    /*cell->getGeometry().push_back(frameA2);*/
    /**/
    /*// corridor back face ⇒ must face world +X, so use local +Z*/
    /*glm::mat4 corridorBackM = glm::translate(*/
    /*    glm::mat4(1.f),*/
    /*    base + glm::vec3(+PW * 0.5f - 0.6f, doorSz.y * 0.5f, +0.001f));*/
    /*auto frameB2 = std::make_shared<PortalQuad>(*/
    /*    portalSh, glm::vec3(0),*/
    /*    glm::vec3(0, 0, 1), // **flipped** local +Z ⇒ world +X*/
    /*    doorSz.x * 0.5f, doorSz.y * 0.5f);*/
    /*frameB2->setModel(corridorBackM);*/
    /*cell->getGeometry().push_back(frameB2);*/
    /**/
    /*{*/
    /*  glm::mat4 A2B2 = corridorBackM * glm::inverse(doorBackM);*/
    /*  glm::mat4 B2A2 = glm::inverse(A2B2);*/
    /*  auto pAB2 = std::make_shared<Portal>(frameA2, cell, A2B2);*/
    /*  auto pBA2 = std::make_shared<Portal>(frameB2, cell, B2A2);*/
    /*  pAB2->setDestinationPortal(pBA2.get());*/
    /*  pBA2->setDestinationPortal(pAB2.get());*/
    /*  cell->addPortal(pAB2);*/
    /*  cell->addPortal(pBA2);*/
    /*}*/
    return out;
  }

  // —— Volumetric portal builder — swap only the ±X faces ————————
  static void addVolumetricPortal(Cell *cell, Shader *portalSh,
                                  glm::vec3 centerA, glm::vec3 centerB,
                                  glm::vec3 size) {
    // half-extents
    glm::vec3 h = size * 0.5f;

    // A→B uses **rotate-180° about Y** then translate
    glm::mat4 M_A = glm::translate(glm::mat4(1), centerA);
    glm::mat4 M_B =
        glm::translate(glm::mat4(1), centerB) *
        glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
    glm::mat4 A2B = M_B * glm::inverse(M_A);

    struct Face {
      glm::vec3 offset, normal;
      glm::vec2 dims;
    };
    std::array<Face, 6> faces = {{
        {{-h.x, 0, 0}, {-1, 0, 0}, {size.z, size.y}}, // -X (thin)
        {{+h.x, 0, 0}, {+1, 0, 0}, {size.z, size.y}}, // +X (thin)
        {{0, +h.y, 0}, {0, +1, 0}, {size.x, size.z}}, // +Y
        {{0, -h.y, 0}, {0, -1, 0}, {size.x, size.z}}, // -Y
        {{0, 0, +h.z}, {0, 0, +1}, {size.x, size.y}}, // +Z
        {{0, 0, -h.z}, {0, 0, -1}, {size.x, size.y}}  // -Z
    }};

    for (auto &f : faces) {
      bool isThin = std::abs(f.normal.x) > 0.5f;

      // A-side quad
      auto surfA =
          std::make_shared<PortalQuad>(portalSh, centerA + f.offset, f.normal,
                                       f.dims.x * 0.5f, f.dims.y * 0.5f);

      // B-side quad: if “thin,” mirror across X; otherwise same side
      glm::vec3 offB = centerB + (isThin ? -f.offset : f.offset);
      glm::vec3 normB = (isThin ? -f.normal : f.normal);

      auto surfB = std::make_shared<PortalQuad>(
          portalSh, offB, normB, f.dims.x * 0.5f, f.dims.y * 0.5f);

      cell->getGeometry().push_back(surfA);
      cell->getGeometry().push_back(surfB);

      // link them with the same A2B transform
      auto pA = std::make_shared<Portal>(surfA, cell, A2B);
      auto pB = std::make_shared<Portal>(surfB, cell, glm::inverse(A2B));
      pA->setDestinationPortal(pB.get());
      pB->setDestinationPortal(pA.get());
      cell->addPortal(pA);
      cell->addPortal(pB);
    }
  }
  static float randomXZ() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(-45.f, 45.f);
    return dist(rng);
  }
};

#endif // SCENE_MANAGER_H
