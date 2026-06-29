#include <rpg/runGame.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.hpp>
#endif
#include <glm/gtx/quaternion.hpp>

#include <rpg/ren/mesh.hpp>
#include <rpg/ren/texture.hpp>
#include "ren/wgp/Backend.hpp"

namespace rpg {
    struct State {
        glfw::Window& window;
        ren::wgp::Backend backend;
        ren::Scene scene;
        void (*onUpdate)(ren::Scene& scene, float deltaTime);
        float oldTime;

        template <typename SceneT>
        State(
            TextureDataView TextureData,
            SceneT&& Scene,
            void (*OnUpdate)(ren::Scene& scene, float deltaTime)
        ) : window([]() -> glfw::Window& {
                glfw::Window::hint(glfw::ClientApi::NoApi);
                return glfw::createWindow(1280, 720);
            }()), backend(window, 1280, 720, {
                .instanceBufferSize = 10 * ren::wgp::LitRenderer::InstanceSize,
                .uniform = {
                    .size = ren::wgp::LitRenderer::WorldBindingSize,
                    .maxCount = 1
                },
                .textureData = {
                    .width = TextureData.width,
                    .height = TextureData.height,
                    .count = TextureData.count,
                    .data = TextureData.data
                }
            }),
            scene(std::forward<SceneT>(Scene)),
            onUpdate(OnUpdate), oldTime(static_cast<float>(glfw::getTime()))
        {
            window.setUserPointer(this);
            window.setFramebufferSizeCallback<[](
                glfw::Window& window, int width, int height
            ){
                window.getUserPointer<State>()->backend.onScreenResized(width, height);
            }>();
        }

        void update() {
            float newTime = static_cast<float>(glfw::getTime());
            float deltaTime = newTime - oldTime;
            oldTime = newTime;

            onUpdate(scene, deltaTime);

            backend.draw(ren::wgp::Scene(scene));
        }
        static void run(State&& state) {
        #if defined(__EMSCRIPTEN__)
            emscripten::set_main_loop([s = std::move(state)]() mutable{ s.update(); }, 0, false);
        #else
            while (!state.window.shouldClose()) {
                glfw::pollEvents();
                state.update();
                state.backend.surface.Present();
                state.backend.instance.ProcessEvents();
            }
        #endif
        }
    };

    void runGame(
        TextureDataView TextureData,
        const ren::Scene& Scene,
        void(*onUpdate)(ren::Scene&, float)
    ) {
        State::run(State(
            TextureData, Scene, onUpdate
        ));
    }
    void runGame(
        TextureDataView TextureData,
        ren::Scene&& Scene,
        void(*onUpdate)(ren::Scene&, float)
    ) {
        State::run(State(
            TextureData, std::move(Scene), onUpdate
        ));
    }
}