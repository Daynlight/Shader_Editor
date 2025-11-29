cmake_minimum_required(VERSION 3.15)

project(ShaderEditor LANGUAGES CXX C)

set(src "Main.cpp")
set(headers "Shaders.h")

add_executable(ShaderEditor ${src})
target_link_libraries(ShaderEditor CWindow)