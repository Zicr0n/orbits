cmake_minimum_required(VERSION 3.16)

project(OrbitSim)

set(CMAKE_CXX_STANDARD 20)

add_executable(OrbitSim
    src/main.cpp
    external/glad/src/glad.c
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_glfw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(OrbitSim PRIVATE
    external/glad/include
    external/assimp/include
    external/imgui/include
    include
)

find_package(glfw3 REQUIRED)

target_link_libraries(OrbitSim PRIVATE
    glfw
    dl
    assimp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)