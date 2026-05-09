cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RPGengine VERSION 0.1.0)
add_library(RPGengine SHARED
    src/RPGengine.cpp
)
target_include_directories(RPGengine PRIVATE ../include/)
target_compile_definitions(RPGengine PRIVATE RPGENGINE_EXPORT)