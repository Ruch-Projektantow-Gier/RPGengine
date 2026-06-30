#pragma once
#include <webgpu/webgpu_cpp.h>

#include <rpg/ren/texture.hpp>
#include "texture.hpp"
#include "mem/AlignedBumpAllocator.hpp"

namespace rpg::ren::wgp {
    struct Resources {
        struct Descriptor {
            size_t instanceBufferSize;
            struct {
                size_t size;
                size_t maxCount;
            } uniform;
            texturearray::CreateInfo textureData;
        };

        wgpu::Buffer meshBuffer;
        wgpu::Buffer instanceBuffer;
        wgpu::Sampler sampler;
        Texture textureArray;
        mem::AlignedBumpAllocatedBuffer uniformBuffer;

        Resources(
            const wgpu::Device& device,
            const wgpu::Queue& queue,
            const Descriptor& desc
        );
    };
}