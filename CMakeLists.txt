cmake_minimum_required(VERSION 3.04)
project(322COM_raycaster)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(${PROJECT_NAME} ${SOURCES} glad/src/glad.c src/main.cpp src/shapes/shape.h src/shapes/sphere.cpp src/shapes/sphere.h src/shapes/plane.cpp src/shapes/plane.h src/ray.cpp src/ray.h src/image.cpp src/image.h src/shapes/triangle.cpp src/shapes/triangle.h src/shapes/shape.cpp src/light.cpp src/light.h src/boxLight.cpp src/boxLight.h src/shapes/mesh.cpp src/shapes/mesh.h)

# GLM
include_directories(${PROJECT_NAME} ./glm)

# GLFW
# Disable GLFW docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(./glfw)
target_link_libraries(${PROJECT_NAME} glfw)

# GLAD
include_directories(${PROJECT_NAME} glad/include)

# objloader
add_library(objloader STATIC objloader/objloader.cpp)
include_directories(${PROJECT_NAME} objloader)
target_link_libraries(${PROJECT_NAME} objloader)

# imgui
add_library(imgui STATIC imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_widgets.cpp)
include_directories(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} imgui)