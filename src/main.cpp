/*     SPDX-License-Identifier: BSD-3-Clause     */

//        Copyright The Authors 2021.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include "imgui_runner/imgui_runner.h"
#include <imgui.h>
#include <iostream>

static ImGuiRunner s_imguirunner;

extern bool RenderCallback(void);

int main(int argc, char *argv[]) {
  try {
    s_imguirunner.InitGraphics();
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return -1;
  }

  s_imguirunner.Windowed(1024, 768, "dearimgui-glfw-glad-cmake");
  // imguirunner.FullScreenWindowed(1024, 768, "dearimgui-glfw-glad-cmake", 0);
  // imguirunner.FullScreenWindowed("dearimgui-glfw-glad-cmake", 0);

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  s_imguirunner.Run(&RenderCallback);

  return 0;
}
