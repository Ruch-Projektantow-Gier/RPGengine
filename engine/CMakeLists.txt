add_library(Engine
    ./src/runGame.cpp
    ./src/ren/wgp/Backend.cpp
)

# Add namespaced alias to Engine target
add_library(RPGengine::Engine ALIAS Engine)

# Set warning level and warnings as errors
target_set_warning_options(Engine)

# C++ standard
target_compile_features(Engine PUBLIC cxx_std_20)

# Public headers
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# GLFW HPP
target_link_libraries(Engine PRIVATE glfw_hpp)

# GLM
target_link_libraries(Engine PUBLIC glm)
target_compile_definitions(
    Engine PUBLIC
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_LEFT_HANDED
)

# Dawn WebGPU
if(EMSCRIPTEN)
    target_link_libraries(Engine PRIVATE emdawnwebgpu_cpp webgpu_glfw)
    target_link_options(Engine PUBLIC "-sASYNCIFY=1" "-sUSE_GLFW=3")
else()
    target_link_libraries(Engine PRIVATE webgpu_dawn webgpu_glfw glfw)
endif()