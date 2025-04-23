#include <glad/glad.h>

#include "app/App.h"

#include "app/Camera.h"
#include "portal/Cell.h"
#include "portal/Portal.h"
#include "portal/Scene.h"
#include "render/Renderer.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

App &App::getInstance() {
  static App inst;
  return inst;
}

void App::run() {
  double last = glfwGetTime();

  while (!glfwWindowShouldClose(pWindow)) {
    double now = glfwGetTime();
    float dt = static_cast<float>(now - last);
    last = now;

    // -- ImGui frame bootstrap --
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Temp Debug HUD
    {
      ImGui::Begin("Debug");
      ImGui::SliderInt("Recursion depth", &renderer->recursionDepth, 0, 10);
      ImGui::Text("FPS: %.1f", 1.0f / dt);
      ImGui::End();
    }

    renderFrame(dt);

    // -- ImGui render --
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // present
    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }
}

App::App() : Window(kWidth, kHeight, kTitle) {
  // ---------------- ImGui bootstrap ----------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
  ImGui_ImplOpenGL3_Init("#version 410");

  // ---------------- Engine objects -----------------
  renderer = std::make_unique<Renderer>(kWidth, kHeight);
  camera = std::make_unique<Camera>(60.0f, static_cast<float>(kWidth) / kHeight,
                                    0.1f, 100.0f);

  initScene();

  // ---------------- GLFW callbacks -----------------
  glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
}

void App::initScene() {
  // TO DO
}

void App::renderFrame(float dt) {
  camera->update(dt);
  renderer->draw(*scene, *camera);
}

void App::framebufferSizeCallback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
  App::getInstance().renderer->resize(w, h);
}
