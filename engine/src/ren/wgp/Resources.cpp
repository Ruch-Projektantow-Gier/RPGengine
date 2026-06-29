#include "Resources.hpp"
#include "constmeshbuffer.hpp"

namespace rpg::ren::wgp {
    Resources::Resources(
        const wgpu::Device& device,
        const wgpu::Queue& queue,
        const Descriptor& descriptor
    ) : meshBuffer(constmeshbuffer::create(
        device, queue, "Const Mesh Buffer"
    )), instanceBuffer([device, descriptor](){
        wgpu::BufferDescriptor desc {
            .label = "Instance Buffer",
            .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
            .size = descriptor.instanceBufferSize,
            .mappedAtCreation = false
        };
        return device.CreateBuffer(&desc);
    }()), sampler([device]() {
        wgpu::SamplerDescriptor desc {
            .addressModeU = wgpu::AddressMode::Repeat,
            .addressModeV = wgpu::AddressMode::Repeat,
            .addressModeW = wgpu::AddressMode::Repeat,
            .magFilter = wgpu::FilterMode::Linear,
            .minFilter = wgpu::FilterMode::Linear,
            .mipmapFilter = wgpu::MipmapFilterMode::Linear,
            .lodMinClamp = 0.0f,
            .lodMaxClamp = 1.0f,
            .compare = wgpu::CompareFunction::Undefined,
            .maxAnisotropy = 1,
        };
        return device.CreateSampler(&desc);
    }()), textureArray(ren::wgp::Texture(
        device,
        descriptor.textureData.width,
        descriptor.textureData.height,
        descriptor.textureData.count,
        descriptor.textureData.data,
        wgpu::TextureFormat::RGBA8Unorm,
        "Color Texture Array"
    )), uniformBuffer(mem::bumpAllocatedUniformBuffer(device,
        mem::ceilToNextMultiple(
            descriptor.uniform.size,
            getLimits(device).minUniformBufferOffsetAlignment
        ) * descriptor.uniform.maxCount
    )) {}
}