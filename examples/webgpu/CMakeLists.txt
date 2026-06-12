add_executable(WebgpuExample main.cpp)
target_link_libraries(WebgpuExample PRIVATE RPGEngine::Runtime)

if(EMSCRIPTEN)
    set_target_properties(WebgpuExample PROPERTIES SUFFIX ".html")
    target_link_options(WebgpuExample PRIVATE
        "--shell-file=${CMAKE_SOURCE_DIR}/web/index.html"
    )
endif()