#include <rpg/runGame.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
#include <glm/gtx/quaternion.hpp>

#include <rpg/ren/mesh.hpp>
#include <rpg/ren/texture.hpp>
#include "ren/wgp/Backend.hpp"
#include "ren/wgp/constmeshbuffer.hpp"

namespace rpg {
    void runGame(ren::Scene&& Scene, void(*onUpdate)(ren::Scene&, float)) {
        static rpg::ren::wgp::Backend backend(1280, 720);
        backend.window.setFramebufferSizeCallback<[](
            glfw::Window&, int width, int height
        ){
            backend.onScreenResized(width, height);
        }>();

        static const auto texture = ren::Texture::make(backend.device,
            ren::Texture::singleColorData<ren::RGBA8{255, 255, 255, 255}, 16, 16>(),
            wgpu::TextureFormat::RGBA8Unorm,
            "Color Texture"
        );

        static const auto worldUniforms = backend.createUniforms<glm::mat4>(
            glm::perspective(
                70.0f, 128.0f / 72.0f, 0.01f, 20.0f
            ) * glm::lookAt(
                glm::vec3(5.0f),
                glm::vec3(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        );
        static const wgpu::BindGroup worldBindGroup = backend.makeWorldBindGroup(
            worldUniforms.offset,
            "World Bind Group"
        );

        static const wgpu::BindGroup bindGroup = backend.makeModelBindGroup(
            texture.view,
            "Object Bind Group"
        );

        static ren::Scene scene = std::forward<ren::Scene>(Scene);
        static void(*upd)(ren::Scene&, float) = onUpdate;

        static float oldTime = static_cast<float>(glfw::getTime());
        static const auto render = []() {
            float newTime = static_cast<float>(glfw::getTime());
            float deltaTime = newTime - oldTime;
            oldTime = newTime;

            upd(scene, deltaTime);

            std::vector<glm::mat4> M(scene.entries.size());
            for (size_t i = 0; i < scene.entries.size(); ++i) {
                auto& e = scene.entries[i];
                M[i] = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(e.position.x, e.position.y, e.position.z)
                );
                M[i] *= glm::toMat4(glm::quat(glm::vec3(e.rotation.x, e.rotation.y, e.rotation.z)));
                M[i] = glm::scale(M[i], glm::vec3(e.scale.x, e.scale.y, e.scale.z));
            }
            backend.queue.WriteBuffer(
                backend.litRenderer.instanceBuffer, 0,
                M.data(), M.size() * sizeof(glm::mat4)
            );

            backend.draw([](
                ren::wgp::LitRenderer::Pass pass
            ){
                using namespace ren::wgp;
                pass.setWorldBindGroup(worldBindGroup);
                pass.setObjectBindGroup(bindGroup);
                pass.draw(
                    constmeshbuffer::CubePointer,
                    0,
                    static_cast<uint32_t>(scene.entries.size())
                );
            });
        };
#if defined(__EMSCRIPTEN__)
        emscripten_set_main_loop(render, 0, false);
#else
        while (!backend.window.shouldClose()) {
            glfw::pollEvents();
            render();
            backend.surface.Present();
            backend.instance.ProcessEvents();
        }
#endif
    }
}