#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

namespace emscripten {
    using callback_func = em_callback_func;
    using arg_callback_func = em_arg_callback_func;
    using str_callback_func = em_str_callback_func;

    inline void run_script(const char* script) {
        emscripten_run_script(script);
    }

    namespace main_loop_arg {
        using deleter_type = void(*)();

        inline void* pointer = nullptr;
        inline deleter_type deleter = nullptr;

        template <typename T>
        inline const deleter_type default_deleter = [](){
            delete static_cast<T*>(pointer);
        };

        inline void* get() { return pointer; }

        template <typename T>
        inline T* get() { return static_cast<T*>(pointer); }

        template <typename T>
        inline void set(T* arg, deleter_type Deleter = default_deleter<T>) {
            if (pointer != nullptr) deleter();
            pointer = arg;
            deleter = Deleter;
        }

        inline void reset() {
            if (pointer != nullptr) deleter();
            pointer = nullptr;
            deleter = nullptr;
        }
    }

    inline void set_main_loop(
        callback_func func, int fps = 0, bool simulate_infinite_loop = false
    ) {
        main_loop_arg::reset();
        emscripten_set_main_loop(func, fps, simulate_infinite_loop);
    }

    inline void set_main_loop_arg(
        arg_callback_func func, void* arg, int fps = 0, bool simulate_infinite_loop = false
    ) {
        main_loop_arg::reset();
        emscripten_set_main_loop_arg(func, arg, fps, simulate_infinite_loop);
    }

    template <typename Callable>
    inline void set_main_loop(
        Callable&& func, int fps = 0, bool simulate_infinite_loop = false
    ) {
        using T = std::remove_reference_t<Callable>;
        main_loop_arg::set(new T(std::forward<Callable>(func)));
        emscripten_set_main_loop([](){
            assert(main_loop_arg::get() != nullptr);
            std::invoke(*main_loop_arg::get<T>());
        }, fps, simulate_infinite_loop);
    }

    inline void pause_main_loop() {
        emscripten_pause_main_loop();
    }

    inline void resume_main_loop() {
        emscripten_resume_main_loop();
    }

    inline void cancel_main_loop() {
        emscripten_cancel_main_loop();
    }
}
#endif