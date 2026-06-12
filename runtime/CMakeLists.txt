add_library(Runtime
    ./src/Runtime.cpp
    ./src/ren/wgp/Backend.cpp
)
add_library(RPGEngine::Runtime ALIAS Runtime)

target_compile_features(
    Runtime PUBLIC cxx_std_20
)

target_include_directories(
    Runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# GLFW HPP
target_link_libraries(Runtime PRIVATE glfw_hpp)

# GLM
target_link_libraries(Runtime PUBLIC glm)
target_compile_definitions(
    Runtime PUBLIC
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_LEFT_HANDED
)

if(EMSCRIPTEN)
    target_link_libraries(Runtime PRIVATE emdawnwebgpu_cpp webgpu_glfw)
    target_link_options(Runtime PUBLIC "-sASYNCIFY=1" "-sUSE_GLFW=3")
else()
    target_link_libraries(Runtime PRIVATE webgpu_dawn webgpu_glfw glfw)
endif()