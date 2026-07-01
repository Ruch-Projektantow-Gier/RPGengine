#pragma once
#include <vector>
#include <webgpu/webgpu_cpp.h>

#include <rpg/ren/texture.hpp>
#include <rpg/ren/Material.hpp>
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
            std::span<const ren::texture::DataSource> textureData;
            std::span<const ren::Material> materials;
        };
        struct TexturePointer {
            uint32_t textureArray;
            uint32_t layer;
        };
        using TextureMapping = std::vector<TexturePointer>;

        wgpu::Buffer meshBuffer;
        wgpu::Buffer instanceBuffer;
        wgpu::Sampler sampler;
        std::vector<Texture> textureArrays;
        std::vector<TexturePointer> textureMapping;
        mem::AlignedBumpAllocatedBuffer uniformBuffer;

        Resources(
            wgpu::Buffer&& meshBuffer,
            wgpu::Buffer&& instanceBuffer,
            wgpu::Sampler&& sampler,
            std::vector<Texture> textureArrays,
            std::vector<TexturePointer> textureMapping,
            mem::AlignedBumpAllocatedBuffer uniformBuffer
        ) : meshBuffer(std::move(meshBuffer)),
            instanceBuffer(std::move(instanceBuffer)),
            sampler(std::move(sampler)),
            textureArrays(std::move(textureArrays)),
            textureMapping(std::move(textureMapping)),
            uniformBuffer(std::move(uniformBuffer))
        {}
        Resources(
            const wgpu::Device& device,
            const wgpu::Queue& queue,
            const Descriptor& desc
        );
    };
}