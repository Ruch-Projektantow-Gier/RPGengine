#pragma once
#include <webgpu/webgpu_cpp.h>

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
            struct {
                uint32_t width;
                uint32_t height;
                uint32_t count;
                const void* data;
            } textureData;
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