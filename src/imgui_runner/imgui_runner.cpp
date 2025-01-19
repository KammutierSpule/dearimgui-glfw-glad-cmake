/*     SPDX-License-Identifier: BSD-3-Clause     */

//        Copyright The Authors 2021.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include "imgui_runner.h"

// clang-format off
// Include order is important
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
// clang-format on

#include <assert.h>
#include <csignal> // for signal handling
#include <iostream>

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "GLFW Error " << error << " " << description << "\n";
}

static volatile std::sig_atomic_t gSignalInterrupt_;

static void SignalHandler(int signal) {
  gSignalInterrupt_ = signal;
}

ImGuiRunner::ImGuiRunner()
    : samples_(GLFW_DONT_CARE), vsync_(true), windowed_(false),
      full_screen_(false), window_(nullptr) {
}

void ImGuiRunner::InitGraphics() {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() == 0) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // Decide GL version
#if __APPLE__
  // GL 3.2
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#elif _WIN32
  // GL 3.0 (as per ImGui demo)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#else
  // GL 3.2
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only
#endif
}

void ImGuiRunner::SetupContext() {
  assert(window_ != nullptr);

  glfwMakeContextCurrent(window_);

  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
    throw std::runtime_error("failed to initialize OpenGL context");
  }
}

void ImGuiRunner::InitImGui() {
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  // io.IniFilename =

  //
  // Various flags controlling ImGui IO behavior
  //

  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable Gamepad Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // Enable Multi-Viewport
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  // NOTE: Platform Windows
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

  ImGui_ImplGlfw_InitForOpenGL(window_, true);

  // Decide GLSL version
#if __APPLE__
  // GLSL 150
  const char *glsl_version = "#version 150";
#else
  // GLSL 130
  const char *glsl_version = "#version 130";
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGuiRunner::Windowed(int width, int height, const std::string &title) {
  if (windowed_ && !full_screen_) {
    SaveWindowPosition();
  }

  windowed_ = true;
  full_screen_ = false;

  if (window_ == nullptr) {
    glfwWindowHint(GLFW_SAMPLES, samples_); // multisampling

    window_ = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

    if (window_ == nullptr) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    SetupContext();

    glfwSwapInterval(vsync_ ? 1 : 0); // Enable vsync

    InitImGui();
  } else {
    // Restore the window position when you switch to windowed mode
    glfwSetWindowMonitor(window_, nullptr, saved_position_[0],
        saved_position_[1], width, height, GLFW_DONT_CARE);
    glfwSetWindowTitle(window_, title.data());
  }
}

static GLFWmonitor *GetMonitorByNumber(int monitor) {
  GLFWmonitor *the_monitor = nullptr;

  if (monitor == 0) {
    the_monitor = glfwGetPrimaryMonitor();
  } else {
    int count = 0;

    GLFWmonitor **glfw_monitors = glfwGetMonitors(&count);

    if (monitor >= 0 && monitor < count) {
      the_monitor = glfw_monitors[monitor];
    } else {
      the_monitor = glfw_monitors[0];
    }
  }

  return the_monitor;
}

void ImGuiRunner::FullScreenWindowed(const std::string &title, int monitor) {
  windowed_ = true;
  full_screen_ = true;

  GLFWmonitor *the_monitor = GetMonitorByNumber(monitor);
  const GLFWvidmode *mode = glfwGetVideoMode(the_monitor);

  if (window_ == nullptr) {
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_SAMPLES, samples_);

    window_ = glfwCreateWindow(
        mode->width, mode->height, title.data(), the_monitor, nullptr);

    if (window_ == nullptr) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    SetupContext();

    glfwSwapInterval(vsync_ ? 1 : 0); // Enable vsync

    InitImGui();
  } else {
    SaveWindowPosition();

    glfwSetWindowMonitor(window_, the_monitor, 0, 0, mode->width, mode->height,
        mode->refreshRate);
    glfwSetWindowTitle(window_, title.data());
  }
}

void ImGuiRunner::FullScreen(int width, int height, const std::string &title,
    int monitor, int refresh_rate) {
  windowed_ = false;
  full_screen_ = true;

  GLFWmonitor *the_monitor = GetMonitorByNumber(monitor);

  if (window_ == nullptr) {
    glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);
    glfwWindowHint(GLFW_SAMPLES, samples_); // multisampling
    window_ =
        glfwCreateWindow(width, height, title.data(), the_monitor, nullptr);
    if (window_ == nullptr) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    SetupContext();
    glfwSwapInterval(vsync_ ? 1 : 0); // Enable vsync
    InitImGui();
  } else {
    SaveWindowPosition();

    glfwSetWindowMonitor(
        window_, the_monitor, 0, 0, width, height, refresh_rate);
    glfwSetWindowTitle(window_, title.data());
  }
}

void ImGuiRunner::CleanUp() {
  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void ImGuiRunner::Run(RenderFn render_function) {
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  std::signal(SIGINT, SignalHandler);
  std::signal(SIGTERM, SignalHandler);

  bool application_wants_to_run = true;

  // Main loop
  while ((glfwWindowShouldClose(window_) == 0) && (gSignalInterrupt_ == 0) &&
         (application_wants_to_run)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data
    // to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all
    // inputs to dear imgui, and hide them from your application based on
    // those two flags.
    glfwPollEvents();

    if (glfwGetWindowAttrib(window_, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Skip frame rendering if the window width or heigh is 0
    // Not doing so will cause the docking system to lose its mind
    int wsize_x;
    int wsize_y;
    glfwGetWindowSize(window_, &wsize_x, &wsize_y);

    if (wsize_x == 0 || wsize_y == 0) {
      continue;
    }

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (render_function != nullptr) {
      application_wants_to_run = (*render_function)();
    }

    // Rendering
    ImGui::Render();
    int display_w = 0;
    int display_h = 0;
    glfwMakeContextCurrent(window_);
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#if 0
    // Update and Render additional Platform Windows
    if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
    }
#endif
    glfwMakeContextCurrent(window_);
    glfwSwapBuffers(window_);
  }

  CleanUp();
}

void ImGuiRunner::EnableVsync(bool state) {
  glfwSwapInterval(state ? 1 : 0);
  vsync_ = state;
}

void ImGuiRunner::MultiSample(int samples) {
  if (samples < 0 || samples > 4) {
    samples = GLFW_DONT_CARE;
  }
  samples_ = samples;
  glfwWindowHint(GLFW_SAMPLES, samples_);
}

void ImGuiRunner::SaveWindowPosition() {
  assert(window_);
  glfwGetWindowPos(window_, &saved_position_[0], &saved_position_[1]);
}
