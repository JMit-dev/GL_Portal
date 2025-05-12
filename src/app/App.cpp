#include "app/App.h"

#include "app/Controls.h"
#include "app/DebugUI.h"
#include "render/Renderer.h"
#include "util/SceneManager.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

App &App::instance() {
  static App app;
  return app;
}

App::App() : Window(kWidth, kHeight, kTitle) {
  // Dear ImGui bootstrap
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  // Our subsystems
  renderer = std::make_unique<Renderer>(kWidth, kHeight);
  sceneMgr = std::make_unique<SceneManager>();
  controls = std::make_unique<Controls>(pWindow); // registers Input cb
  ui = std::make_unique<DebugUI>();

  // callbacks
  glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
}

App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
//  Main loop
// -----------------------------------------------------------------------------

void App::run() {
  double last = glfwGetTime();

  while (!glfwWindowShouldClose(pWindow)) {
    double now = glfwGetTime();
    float dt = static_cast<float>(now - last);
    last = now;

    Scene &scene = sceneMgr->currentSceneMutable();
    controls->update(dt, scene);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ui->draw(*renderer, *sceneMgr, *controls, dt);

    sceneMgr->update(static_cast<float>(now));

    renderFrame(dt);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }
}

// -----------------------------------------------------------------------------
//  Helpers
// -----------------------------------------------------------------------------
void App::renderFrame(float /*dt*/) {
  renderer->draw(sceneMgr->currentScene(), controls->camera());
}

void App::framebufferSizeCallback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
  instance().renderer->resize(w, h);
}
