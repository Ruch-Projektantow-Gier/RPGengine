#include <rpg/runGame.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.hpp>
#endif
#include <glm/gtx/quaternion.hpp>

#include <rpg/ren/mesh.hpp>
#include <rpg/ren/texture.hpp>
#include "ren/wgp/texture.hpp"
#include "ren/wgp/Backend.hpp"
#include "ren/wgp/constmeshbuffer.hpp"

namespace rpg {
    struct State {
        static constexpr const std::array<ren::texture::Data<ren::texture::RGBA8, 16, 16>, 2> TextureData = {
            ren::texture::Data<ren::texture::RGBA8, 16, 16>({255, 255, 255, 255}),
            ren::texture::Data<ren::texture::RGBA8, 16, 16>({255, 0, 0, 255}),
        };

        ren::wgp::Backend backend;
        ren::Scene scene;
        ren::Texture texture;
        ren::wgp::StackUniformBuffer::Uniforms<glm::mat4> worldUniforms;
        wgpu::BindGroup worldBindGroup;
        wgpu::BindGroup objectBindGroup;
        void (*onUpdate)(ren::Scene& scene, float deltaTime);
        float oldTime;

        State(
            ren::Scene&& Scene,
            void (*OnUpdate)(ren::Scene& scene, float deltaTime)
        ) : backend(1280, 720),
            scene(std::forward<ren::Scene>(Scene)),
            texture(ren::Texture(
                backend.device, 16, 16, 2, TextureData.data(),
                wgpu::TextureFormat::RGBA8Unorm, "Color Texture"
            )), worldUniforms(backend.createUniforms<glm::mat4>(
                glm::perspective(
                    70.0f, 128.0f / 72.0f, 0.01f, 20.0f
                ) * glm::lookAt(
                    glm::vec3(5.0f),
                    glm::vec3(0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                )
            )), worldBindGroup(backend.makeWorldBindGroup(
                worldUniforms.offset, "World Bind Group"
            )), objectBindGroup(backend.makeModelBindGroup(
                texture.view, "Object Bind Group"
            )), onUpdate(OnUpdate), oldTime(static_cast<float>(glfw::getTime()))
        {
            backend.window.setUserPointer(&backend);
            backend.window.setFramebufferSizeCallback<[](
                glfw::Window& window, int width, int height
            ){
                window.getUserPointer<ren::wgp::Backend>()->onScreenResized(width, height);
            }>();
        }

        void update() {
            float newTime = static_cast<float>(glfw::getTime());
            float deltaTime = newTime - oldTime;
            oldTime = newTime;

            onUpdate(scene, deltaTime);

            struct InstanceData {
                glm::mat4 M;
                uint32_t textureId;
            };
            std::vector<InstanceData> M(scene.entries.size());
            for (size_t i = 0; i < scene.entries.size(); ++i) {
                auto& e = scene.entries[i];
                M[i].M = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3(e.position.x, e.position.y, e.position.z)
                );
                M[i].M *= glm::toMat4(glm::quat(glm::vec3(e.rotation.x, e.rotation.y, e.rotation.z)));
                M[i].M = glm::scale(M[i].M, glm::vec3(e.scale.x, e.scale.y, e.scale.z));
                M[i].textureId = e.materialId;
            }
            backend.queue.WriteBuffer(
                backend.litRenderer.instanceBuffer, 0,
                M.data(), M.size() * sizeof(InstanceData)
            );

            backend.draw([this](
                ren::wgp::LitRenderer::Pass pass
            ){
                using namespace ren::wgp;
                pass.setWorldBindGroup(worldBindGroup);
                pass.setObjectBindGroup(objectBindGroup);
                pass.draw(
                    constmeshbuffer::CylinderPointer,
                    0,
                    static_cast<uint32_t>(scene.entries.size())
                );
            });
        }
    };

    void runGame(ren::Scene&& Scene, void(*onUpdate)(ren::Scene&, float)) {
        State state(std::forward<ren::Scene>(Scene), onUpdate);
#if defined(__EMSCRIPTEN__)
        emscripten::set_main_loop([s = std::move(state)]() mutable{ s.update(); }, 0, false);
#else
        while (!state.backend.window.shouldClose()) {
            glfw::pollEvents();
            state.update();
            state.backend.surface.Present();
            state.backend.instance.ProcessEvents();
        }
#endif
    }
}