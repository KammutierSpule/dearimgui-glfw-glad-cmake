/*     SPDX-License-Identifier: BSD-3-Clause     */

//        Copyright The Authors 2021.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include <cstdint>
#include <imgui.h>

bool RenderCallback(void) {
  // Draw the Application
  ImGui::ShowDemoWindow(nullptr);

  // Draw status bar (no docking)
  ImGui::SetNextWindowSize(ImVec2(320, 240), ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::Begin("statusbar", nullptr,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize);
  // Draw the common stuff
  ImGui::SameLine(20);
  ImGui::Text("FPS: %d", (uint32_t)(ImGui::GetIO().Framerate));
  ImGui::End();

  return true;
}