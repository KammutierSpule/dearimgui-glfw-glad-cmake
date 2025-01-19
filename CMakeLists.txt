# ===-----------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause).
# SPDX-License-Identifier: BSD-3-Clause
# ===-----------------------------------------------------------------------===#
cmake_minimum_required(VERSION 3.22)

# ------------------------------------------------------------------------------
# Project Name
# ------------------------------------------------------------------------------
set(APP_NAME dearimgui-glfw-glad-cmake)

project(${APP_NAME})

# ------------------------------------------------------------------------------
# Include dependencies
# ------------------------------------------------------------------------------
include(cmake/dependencies/dearimgui_dependency.cmake)
include(cmake/dependencies/glfw_dependency.cmake)

find_package(glfw3 REQUIRED)

# ------------------------------------------------------------------------------
# Include sources
# ------------------------------------------------------------------------------
file(GLOB APP_SOURCES src/main.cpp
  src/render_example.cpp
  src/imgui_runner/imgui_runner.cpp
  third_party/glad/glad.c)

add_executable(${APP_NAME} ${APP_SOURCES})

target_include_directories(${APP_NAME} PRIVATE third_party)

target_link_libraries(${APP_NAME} PRIVATE dearimgui glfw)

