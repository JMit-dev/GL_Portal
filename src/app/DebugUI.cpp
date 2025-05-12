#include "app/DebugUI.h"
#include "app/Controls.h"
#include "render/Renderer.h"
#include "shape/Texture.h"
#include "util/SceneManager.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

static void DrawSettings(Renderer &renderer, Controls &c) {
  // --------------------------------------------------------------------
  // Graphics ------------------------------------------------------------
  // --------------------------------------------------------------------
  if (ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen)) {
    static bool msaa = true;
    static bool wire = false;
    static bool vsync = true;

    if (ImGui::Checkbox("MSAA", &msaa))
      msaa ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);

    float aniso = Texture2D::anisotropy(); // current
    float maxAniso = Texture2D::maxAnisotropy();
    if (ImGui::SliderFloat("Anisotropy", &aniso, 1.0f, maxAniso, "%.1f"))
      Texture2D::setAnisotropy(aniso);

    if (ImGui::Checkbox("Wireframe", &wire))
      glPolygonMode(GL_FRONT_AND_BACK, wire ? GL_LINE : GL_FILL);

    if (ImGui::Checkbox("V-Sync (swap interval)", &vsync))
      glfwSwapInterval(vsync ? 1 : 0);

    ImGui::SliderInt("Recursion depth", &renderer.recursionDepth, 0, 10);
  }

  // --------------------------------------------------------------------
  // Camera --------------------------------------------------------------
  // --------------------------------------------------------------------
  if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SliderFloat("Mouse sensitivity", &c.camera().MouseSensitivity, 0.01f,
                       1.0f);

    ImGui::SliderFloat("Move speed", &c.camera().MovementSpeed, 0.5f, 10.0f,
                       "%.2f");
  }

  // --------------------------------------------------------------------
  // Help / README -------------------------------------------------------
  // --------------------------------------------------------------------
  if (ImGui::CollapsingHeader("Controls help"),
      ImGuiTreeNodeFlags_DefaultOpen) {
    ImGui::BulletText("Mouse  : Look around");
    ImGui::BulletText("W A S D: Move");
    ImGui::BulletText("Space  : Move up");
    ImGui::BulletText("Ctrl   : Move down");
    ImGui::BulletText("Shift  : Speed boost (x3)");
    ImGui::Separator();
    ImGui::BulletText("1 : Capture / release cursor");
    ImGui::BulletText("2 : Toggle this UI");
  }
}

void DebugUI::draw(Renderer &renderer, SceneManager &, Controls &c, float dt) {
  if (!c.uiVisible())
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImGui::SetNextWindowPos({io.DisplaySize.x - 10.f, 10.f}, ImGuiCond_Once,
                          {1.f, 0.f});

  if (ImGui::Begin("Settings")) {
    ImGui::Text("FPS: %.1f", 1.0f / dt);
    ImGui::Separator();

    DrawSettings(renderer, c);
  }
  ImGui::End();
}
