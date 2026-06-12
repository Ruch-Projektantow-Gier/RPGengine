cmake_minimum_required(VERSION 3.25)

project(rpgengine LANGUAGES C CXX)

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn/ EXCLUDE_FROM_ALL)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE third_party/glm)

add_library(glfw_hpp INTERFACE)
target_include_directories(glfw_hpp INTERFACE third_party/glfw.hpp/include)

add_subdirectory(runtime)

# Examples
option(BUILD_EXAMPLES "Build engine examples" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()