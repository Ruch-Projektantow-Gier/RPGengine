#pragma once

#include <webgpu/webgpu_cpp.h>

namespace rpg::ren::wgp {
    struct MipMapCalculator {
        wgpu::BindGroupLayout bindGroupLayout;
        wgpu::ComputePipeline pipeline;

        MipMapCalculator(const wgpu::Device& device);
        void calculateMipMaps(
            const wgpu::Device& device,
            const wgpu::Queue& queue,
            const wgpu::Texture& texture
        ) const;
    };
}