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

    float aniso = Texture2D::anisotropy();
    const float max = Texture2D::maxAnisotropy();

    if (ImGui::SliderFloat("Anisotropy", &aniso,
                           1.0f, // min
                           max,  // max
                           "%.1f", ImGuiSliderFlags_NoRoundToFormat)) {
      Texture2D::setAnisotropy(aniso); // clamp inside
    }

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

void DebugUI::draw(Renderer &renderer, SceneManager &sm, Controls &c,
                   float dt) {
  if (!c.uiVisible())
    return;

  // move window once to top‑right
  ImGuiIO &io = ImGui::GetIO();
  ImGui::SetNextWindowPos({io.DisplaySize.x - 10.f, 10.f}, ImGuiCond_Once,
                          {1.f, 0.f});

  if (ImGui::Begin("Debug")) {
    ImGui::Text("FPS: %.1f", 1.0f / dt);

    // ------------------ Tab Bar -------------------------------------
    if (ImGui::BeginTabBar("Rooms")) {
      // one tab per room
      for (int i = 0; i < 6; ++i) {
        char label[16];
        sprintf(label, "Room %d", i);
        if (ImGui::BeginTabItem(label)) {
          if (i == 0) {
            ImGui::Text("Teapot demo room");
          } else {
            ImGui::Text("Room %d not implemented yet.", i);
          }
          if (ImGui::Button("Switch to this room"))
            sm.select(i);
          ImGui::EndTabItem();
        }
      }

      if (ImGui::BeginTabItem("Settings")) {
        DrawSettings(renderer, c);
        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }
  }
  ImGui::End();
}
