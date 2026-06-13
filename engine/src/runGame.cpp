#include <rpg/runGame.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <rpg/ren/mesh.hpp>
#include <rpg/ren/texture.hpp>
#include "ren/wgp/Backend.hpp"
#include "ren/wgp/constmeshbuffer.hpp"

namespace rpg {
    void runGame() {
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

        static const auto uniforms = backend.createUniforms<glm::mat4>(glm::mat4(1.0f));
        static const wgpu::BindGroup bindGroup = backend.makeModelBindGroup(
            uniforms.offset,
            texture.view,
            "Object Bind Group"
        );

        static const auto render = []() {
            float time = static_cast<float>(glfw::getTime());
            uniforms.write(glm::rotate(
                glm::mat4(1.0f),
                time,
                glm::vec3(0.0f, 1.0f, 0.0f)
            ));

            backend.draw([](
                ren::wgp::Backend::RenderPass pass
            ){
                using namespace ren::wgp;
                pass.setVertexBuffer(constmeshbuffer::CubePointer);
                pass.setWorldBindGroup(worldBindGroup);
                pass.setBindGroup(bindGroup);
                pass.drawIndexed(constmeshbuffer::CubePointer);
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