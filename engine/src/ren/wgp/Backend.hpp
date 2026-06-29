#pragma once

#include <glfw.hpp>
#include <webgpu/webgpu_cpp.h>

#include "LitRenderer.hpp"
#include "constmeshbuffer.hpp"
#include "Resources.hpp"
#include "Scene.hpp"

namespace rpg::ren::wgp {
    inline constexpr wgpu::TextureFormat depthFormat = wgpu::TextureFormat::Depth24Plus;
    inline constexpr uint32_t sampleCount = 4;
    inline constexpr size_t indexBufferSize = 1000;
    inline constexpr size_t vertexBufferSize = 1000;
    inline constexpr size_t maxBufferSize = 1000;

    inline void setGlfwWindowHints() {
        glfw::Window::hint(glfw::ClientApi::NoApi);
    }

    wgpu::Instance makeInstance();

    wgpu::Adapter makeAdapter(const wgpu::Instance& instance);

    wgpu::Device makeDevice(
        const wgpu::Instance& instance,
        const wgpu::Adapter& adapter
    );

	void configureSurface(
        const wgpu::Surface& surface,
        const wgpu::SurfaceConfiguration& config
    );
	void configureSurface(
        const wgpu::Surface& surface,
        const wgpu::Device& device,
        wgpu::TextureFormat colorFormat,
        uint32_t width, uint32_t height
    );

    wgpu::TextureFormat getColorFormat(
        const wgpu::Surface& surface,
        const wgpu::Adapter& adapter
    );

	wgpu::Texture makeDepthTexture(
        const wgpu::Device& device,
        uint32_t width, uint32_t height
    );
    wgpu::TextureView makeDepthTextureView(
        const wgpu::Texture& depthTexture
    );

	wgpu::Texture makeMultisampleTexture(
        const wgpu::Device& device,
        wgpu::TextureFormat colorFormat,
        uint32_t width, uint32_t height
    );

    struct Backend {
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Device device;
        wgpu::Queue queue;
        wgpu::Surface surface;
        wgpu::TextureFormat colorFormat;
        wgpu::Texture depthTexture;
        wgpu::TextureView depthTextureView;
        wgpu::Texture multipleTexture;
        LitRenderer litRenderer;
        Resources resources;
        ren::wgp::buffer::Pointer<glm::mat4> worldUniforms;
        wgpu::BindGroup worldBindGroup;
        wgpu::BindGroup objectBindGroup;

        Backend(
            glfw::Window& window,
            uint32_t width, uint32_t height,
            const Resources::Descriptor& resourcesDescriptor
        );
        Backend(
            glfw::Window& window,
            const Resources::Descriptor& resourcesDescriptor
        );
        Backend(const Backend&) = delete;
        Backend& operator=(const Backend&) = delete;
        Backend(Backend&&) noexcept = default;
        Backend& operator=(Backend&&) noexcept = default;

        wgpu::BindGroup makeWorldBindGroup(
            size_t uniformBufferOffset = 0,
            std::string_view label = std::string_view()
        ) const;
        wgpu::BindGroup makeModelBindGroup(
            const wgpu::TextureView& textureView,
            std::string_view label = std::string_view()
        ) const;

        template <typename T>
        buffer::Pointer<T> createUniforms() {
            return resources.uniformBuffer.allocate<T>();
        }
        template <typename T>
        buffer::Pointer<T> createUniforms(const T& data) {
            buffer::Pointer<T> ptr = resources.uniformBuffer.allocate<T>();
            ptr.write(queue, data);
            return ptr;
        }

        void onScreenResized(uint32_t width, uint32_t height);

        struct RenderPass {
            wgpu::RenderPassEncoder pass;
            wgpu::Buffer buffer;

            void setWorldBindGroup(
                const wgpu::BindGroup& bindGroup
            ) {
                pass.SetBindGroup(0, bindGroup);
            }
            void setBindGroup(
                const wgpu::BindGroup& bindGroup
            ) {
                pass.SetBindGroup(1, bindGroup);
            }
            void setVertexBuffer(constmeshbuffer::Pointer p, uint32_t slot = 0) {
                pass.SetVertexBuffer(slot, buffer,
                    p.vertexOffset(),
                    p.vertexDataSize()
                );
            }
            void drawIndexed(constmeshbuffer::Pointer p, uint32_t instanceCount = 1) {
                pass.DrawIndexed(
                    p.indexCount,
                    instanceCount,
                    p.firstIndex()
                );
            }
        };

        void draw(const Scene& scene) const;
    };
}