cmake_minimum_required(VERSION 3.12)
project(322COM_raycaster)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# GLM
include_directories(322COM_raycaster ./glm)

# GLFW
add_subdirectory(./glfw)
# Disable GLFW docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLAD
include_directories(GLTFViewer glad/include)
list(APPEND SOURCES glad/src/glad.c)

add_executable(322COM_raycaster ${SOURCES} src/main.cpp)

target_link_libraries(322COM_raycaster glfw)