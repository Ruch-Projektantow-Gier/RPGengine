#include "Resources.hpp"
#include <stb_image.h>
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
            .compare = wgpu::CompareFunction::Undefined,
            .maxAnisotropy = 2,
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
                if constexpr (std::is_same_v<T, File>) {
                    assert(arg.filename != nullptr);
                    int width, height, channels;
                    stbi_uc* data = stbi_load(
                        arg.filename,
                        &width,
                        &height,
                        &channels,
                        STBI_rgb_alpha
                    );
                    assert(width >= 0);
                    assert(height >= 0);
                    assert(static_cast<uint32_t>(width) == descriptor.textureData.width);
                    assert(static_cast<uint32_t>(height) == descriptor.textureData.height);
                    textureArray.write(queue, data, static_cast<uint32_t>(i));
                    stbi_image_free(data);
                } else if constexpr (std::is_same_v<T, EncodedData>) {
                    assert(arg.data != nullptr);
                    assert(arg.size > 0);
                    int width, height, channels;
                    stbi_uc* data = stbi_load_from_memory(
                        arg.data,
                        static_cast<int>(arg.size),
                        &width,
                        &height,
                        &channels,
                        STBI_rgb_alpha
                    );
                    assert(width >= 0);
                    assert(height >= 0);
                    assert(static_cast<uint32_t>(width) == descriptor.textureData.width);
                    assert(static_cast<uint32_t>(height) == descriptor.textureData.height);
                    textureArray.write(queue, data, static_cast<uint32_t>(i));
                    stbi_image_free(data);
                } else if constexpr (std::is_same_v<T, RawData>) {
                    assert(arg.data != nullptr);
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