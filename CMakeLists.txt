cmake_minimum_required(VERSION 3.6)

project(diligent-sample)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(glfw)
add_subdirectory(DiligentCore)

include_directories("glfw/include")
include_directories(DiligentCore)
include_directories(src)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only")

file(GLOB SOURCE_FILES src/*.cpp src/core/*.cpp)

add_executable(diligent-sample ${SOURCE_FILES})
target_link_libraries(diligent-sample
  glfw
  Diligent-GraphicsEngine
)

if (D3D11_SUPPORTED)
  target_link_libraries(diligent-sample Diligent-GraphicsEngineD3D11-static)
endif()

if (D3D12_SUPPORTED)
  target_link_libraries(diligent-sample Diligent-GraphicsEngineD3D12-static)
endif()

if (GL_SUPPORTED)
  target_link_libraries(diligent-sample Diligent-GraphicsEngineOpenGL-static)
endif()

if (VULKAN_SUPPORTED)
  target_link_libraries(diligent-sample Diligent-GraphicsEngineVk-static)
endif()

if (METAL_SUPPORTED)
  target_link_libraries(diligent-sample Diligent-GraphicsEngineMetal-static)
endif()
