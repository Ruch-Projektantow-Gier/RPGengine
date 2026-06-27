#pragma once

#include <glfw.hpp>
#include <webgpu/webgpu_cpp.h>

#include <rpg/ren/wgp/StackBuffer.hpp>
#include <rpg/ren/wgp/StackUniformBuffer.hpp>
#include "LitRenderer.hpp"
#include "constmeshbuffer.hpp"

namespace rpg::ren::wgp {
    inline constexpr wgpu::TextureFormat depthFormat = wgpu::TextureFormat::Depth24Plus;
    inline constexpr uint32_t sampleCount = 4;
    inline constexpr size_t indexBufferSize = 1000;
    inline constexpr size_t vertexBufferSize = 1000;
    inline constexpr size_t maxBufferSize = 1000;

    glfw::Window& makeWindow(int width, int height);
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
        glfw::Window& window;
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
        wgpu::Sampler sampler;
        wgpu::Buffer meshBuffer;
        StackUniformBuffer uniforms;

        Backend(uint32_t width, uint32_t height);
        Backend(const Backend&) = delete;
        Backend& operator=(const Backend&) = delete;
        Backend(Backend&&) = default;
        ~Backend();

        wgpu::BindGroup makeWorldBindGroup(
            size_t uniformBufferOffset = 0,
            std::string_view label = std::string_view()
        ) const;
        wgpu::BindGroup makeModelBindGroup(
            const wgpu::TextureView& textureView,
            std::string_view label = std::string_view()
        ) const;

        template <typename T = Uniform>
        StackUniformBuffer::Uniforms<T> createUniforms() {
            return uniforms.push<T>(queue);
        }
        template <typename T = Uniform>
        StackUniformBuffer::Uniforms<T> createUniforms(const T& data) {
            return uniforms.push<T>(queue, data);
        }

        void writeUniform(size_t offset, const auto& data) const {
            queue.WriteBuffer(
                uniforms._buffer,
                offset,
                &data,
                sizeof(data)
            );
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

        void draw(auto fillRenderPass) {
            wgpu::SurfaceTexture surfaceTexture;
            surface.GetCurrentTexture(&surfaceTexture);
            wgpu::RenderPassColorAttachment colorAttachment{
                .view = multipleTexture.CreateView(),
                .resolveTarget = surfaceTexture.texture.CreateView(),
                .loadOp = wgpu::LoadOp::Clear,
                .storeOp = wgpu::StoreOp::Store,
                .clearValue = wgpu::Color{ 0.0, 0.0, 0.0, 1.0 }
            };	
            wgpu::RenderPassDepthStencilAttachment depthStencilAttachment {
                .view = depthTextureView,
                .depthLoadOp = wgpu::LoadOp::Clear,
                .depthStoreOp = wgpu::StoreOp::Store,
                .depthClearValue = 1.0f,
                .depthReadOnly = false,
                .stencilLoadOp = wgpu::LoadOp::Undefined,
                .stencilStoreOp = wgpu::StoreOp::Undefined,
                .stencilClearValue = 0,
                .stencilReadOnly = true,
            };
            wgpu::RenderPassDescriptor renderpass{
                .colorAttachmentCount = 1,
                .colorAttachments = &colorAttachment,
                .depthStencilAttachment = &depthStencilAttachment,
            };
            wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
            pass.SetPipeline(litRenderer.pipeline);
            pass.SetIndexBuffer(meshBuffer, wgpu::IndexFormat::Uint32);
            pass.SetVertexBuffer(LitRenderer::InstanceBufferSlot, litRenderer.instanceBuffer);
            fillRenderPass(LitRenderer::Pass { .pass = pass, .vertexBuffer = meshBuffer });
            pass.End();
            wgpu::CommandBuffer commands = encoder.Finish();
            queue.Submit(1, &commands);
        }
    };
}