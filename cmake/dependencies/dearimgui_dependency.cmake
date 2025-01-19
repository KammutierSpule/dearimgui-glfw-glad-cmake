message(STATUS "Downloading and setup Dear ImGui: Bloat-free Graphical User interface")

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  dearimgui_external
  GIT_REPOSITORY  https://github.com/ocornut/imgui.git
  GIT_TAG         v1.91.6
  GIT_SHALLOW     TRUE
)

FetchContent_GetProperties(dearimgui_external)
if(NOT dearimgui_external_POPULATED)
    FetchContent_Populate(dearimgui_external)
endif()

add_library(dearimgui
  ${dearimgui_external_SOURCE_DIR}/imgui.cpp
  ${dearimgui_external_SOURCE_DIR}/imgui_draw.cpp
  ${dearimgui_external_SOURCE_DIR}/imgui_tables.cpp
  ${dearimgui_external_SOURCE_DIR}/imgui_widgets.cpp
  ${dearimgui_external_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${dearimgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${dearimgui_external_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(dearimgui PUBLIC ${dearimgui_external_SOURCE_DIR})