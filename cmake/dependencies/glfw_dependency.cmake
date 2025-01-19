message(STATUS "Downloading and setup GLFW - A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input")

include(FetchContent)

FetchContent_Declare(
  glfw_external
  GIT_REPOSITORY  https://github.com/glfw/glfw.git
  GIT_TAG         3.4
  GIT_SHALLOW     TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw_external)