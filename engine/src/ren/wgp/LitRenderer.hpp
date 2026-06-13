#pragma once

#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    struct LitRenderer {
        static constexpr size_t WorldBindingSize = 16 * sizeof(float);
        static constexpr size_t ObjectBindingSize = 16 * sizeof(float);

        wgpu::Device device;
        struct {
            wgpu::BindGroupLayout world;
            wgpu::BindGroupLayout object;
            const wgpu::BindGroupLayout* data() const { return &world; }
        } bindGroupLayouts;
        wgpu::RenderPipeline pipeline;

        LitRenderer(const wgpu::Device& Device, wgpu::TextureFormat colorFormat);

        wgpu::BindGroup createWorldBindGroup(
            const wgpu::Buffer& uniformBuffer,
            size_t uniformBufferOffset,
            std::string_view label = std::string_view()
        ) const;
        wgpu::BindGroup createModelBindGroup(
            const wgpu::Buffer& uniformBuffer,
            size_t uniformBufferOffset,
            const wgpu::TextureView textureView,
            const wgpu::Sampler sampler,
            std::string_view label = std::string_view()
        ) const;
    };
}