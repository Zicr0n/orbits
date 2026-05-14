cmake_minimum_required(VERSION 3.16)

project(OrbitSim)

set(CMAKE_CXX_STANDARD 20)

add_executable(OrbitSim
    src/main.cpp
    external/glad/src/glad.c
)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(OrbitSim PRIVATE
    external/glad/include
    external/assimp/include
    include
)

find_package(glfw3 REQUIRED)

target_link_libraries(OrbitSim PRIVATE
    glfw
    dl
    assimp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)