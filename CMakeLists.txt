cmake_minimum_required(VERSION 3.12)
project(322COM_raycaster)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(${PROJECT_NAME} ${SOURCES} glad/src/glad.c src/main.cpp src/shapes/Shape.h src/shapes/Sphere.cpp src/shapes/Sphere.h src/shapes/Plane.cpp src/shapes/Plane.h src/Ray.cpp src/Ray.h src/Image.cpp src/Image.h src/shapes/Triangle.cpp src/shapes/Triangle.h src/shapes/Shape.cpp src/light.cpp src/light.h src/mathelper.hpp)

# GLM
include_directories(${PROJECT_NAME} ./glm)

# GLFW
add_subdirectory(./glfw)
# Disable GLFW docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(${PROJECT_NAME} glfw)

# GLAD
include_directories(${PROJECT_NAME} glad/include)

# objloader
add_library(objloader STATIC objloader/objloader.cpp)
include_directories(${PROJECT_NAME} objloader)
target_link_libraries(${PROJECT_NAME} objloader)