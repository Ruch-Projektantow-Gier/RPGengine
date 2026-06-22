#pragma once

#include "constmeshbuffer.hpp"
#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    struct LitRenderer {
        static constexpr size_t WorldBindingSize = 16 * sizeof(float);

        static constexpr uint32_t VertexBufferSlot = 0;
        static constexpr uint32_t InstanceBufferSlot = 1;

        static constexpr uint32_t WorldBindGroupIndex = 0;
        static constexpr uint32_t ObjectBindGroupIndex = 1;
        static constexpr uint32_t BindGroups = 2;

        static constexpr size_t MaxObjects = 10;

        wgpu::Device device;
        struct {
            wgpu::BindGroupLayout world;
            wgpu::BindGroupLayout object;
            const wgpu::BindGroupLayout* data() const { return &world; }
        } bindGroupLayouts;
        wgpu::RenderPipeline pipeline;
        wgpu::Buffer instanceBuffer;

        LitRenderer(const wgpu::Device& Device, wgpu::TextureFormat colorFormat);

        wgpu::BindGroup createWorldBindGroup(
            const wgpu::Buffer& uniformBuffer,
            size_t uniformBufferOffset,
            std::string_view label = std::string_view()
        ) const;
        wgpu::BindGroup createModelBindGroup(
            const wgpu::TextureView textureView,
            const wgpu::Sampler sampler,
            std::string_view label = std::string_view()
        ) const;

        struct Pass {
            wgpu::RenderPassEncoder pass;
            wgpu::Buffer vertexBuffer;

            void setWorldBindGroup(const wgpu::BindGroup& bindGroup) {
                pass.SetBindGroup(WorldBindGroupIndex, bindGroup);
            }
            void setObjectBindGroup(const wgpu::BindGroup& bindGroup) {
                pass.SetBindGroup(ObjectBindGroupIndex, bindGroup);
            }
            void setVertexBuffer(constmeshbuffer::Pointer p) {
                pass.SetVertexBuffer(0, vertexBuffer,
                    p.vertexOffset(),
                    p.vertexDataSize()
                );
            }
            void drawIndexed(
                constmeshbuffer::Pointer p,
                uint32_t firstInstance = 0,
                uint32_t instanceCount = 1
            ) {
                pass.DrawIndexed(
                    p.indexCount,
                    instanceCount,
                    p.firstIndex(),
                    0,
                    firstInstance
                );
            }
            void draw(
                constmeshbuffer::Pointer p,
                uint32_t firstInstance = 0,
                uint32_t instanceCount = 1
            ) {
                setVertexBuffer(p);
                drawIndexed(p, firstInstance, instanceCount);
            }
        };
    };
}