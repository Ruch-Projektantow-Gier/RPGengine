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
        static_cast<uint32_t>(descriptor.textureData.sources.size()),
        wgpu::TextureFormat::RGBA8Unorm,
        "Color Texture Array"
    )), uniformBuffer(mem::bumpAllocatedUniformBuffer(device,
        mem::ceilToNextMultiple(
            descriptor.uniform.size,
            getLimits(device).minUniformBufferOffsetAlignment
        ) * descriptor.uniform.maxCount
    )) {
        for (size_t i = 0; i < descriptor.textureData.sources.size(); ++i) {
            std::visit([this, &queue, &descriptor, i](auto&& arg){
                using namespace texturearray::texture::datasource;
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, RawData>) {
                    textureArray.write(queue, arg.data, static_cast<uint32_t>(i));
                } else if constexpr (std::is_same_v<T, SolidColor>) {
                    std::vector<ren::RGBA8> data(
                        descriptor.textureData.width * descriptor.textureData.height,
                        arg.color
                    );
                    textureArray.write(
                        queue, data.data(),
                        static_cast<uint32_t>(i)
                    );
                }

            }, descriptor.textureData.sources[i]);
        }
    }
}