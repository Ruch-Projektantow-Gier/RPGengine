function(target_set_warning_options target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    )

    set_property(TARGET ${target}
        PROPERTY COMPILE_WARNING_AS_ERROR ON
    )
endfunction()