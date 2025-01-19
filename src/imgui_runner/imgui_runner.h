/*     SPDX-License-Identifier: BSD-3-Clause     */

//        Copyright The Authors 2021.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

//
// https://glad.dav1d.de/

#pragma once

#include <string>

struct GLFWwindow;
struct GLFWmonitor;

// Render function
// returns true if application is running, false is don't want to run anymore
typedef bool (*RenderFn)(void);

class ImGuiRunner {
public:
  ImGuiRunner();
  ~ImGuiRunner() = default;

  ImGuiRunner(const ImGuiRunner &) = delete;
  ImGuiRunner(const ImGuiRunner &&) = delete;

  auto operator=(const ImGuiRunner &) -> ImGuiRunner & = delete;
  auto operator=(const ImGuiRunner &&) -> ImGuiRunner & = delete;

  void Windowed(int width, int height, const std::string &title);
  void FullScreenWindowed(const std::string &title, int monitor);
  void FullScreen(int width, int height, const std::string &title, int monitor,
      int refresh_rate);

  void EnableVsync(bool state = true);
  void MultiSample(int samples);

  void Run(RenderFn render_function);
  void InitGraphics();

private:
  void SetupContext();
  void InitImGui();
  void CleanUp();
  void SaveWindowPosition();

  GLFWwindow *window_;

  bool full_screen_;
  bool windowed_;

  bool vsync_;
  int samples_;

  int saved_position_[2];
};
