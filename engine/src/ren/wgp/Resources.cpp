#include "Resources.hpp"
#include <stb_image.h>
#include "constmeshbuffer.hpp"

namespace rpg::ren::wgp {
    Resources::Resources(
        const wgpu::Device& device,
        const wgpu::Queue& queue,
        const Descriptor& descriptor
    ) : Resources([&device, &queue, &descriptor](){
        wgpu::BufferDescriptor instanceBufferDescriptor {
            .label = "Instance Buffer",
            .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
            .size = descriptor.instanceBufferSize,
            .mappedAtCreation = false
        };
        wgpu::BufferDescriptor materialBufferDescriptor {
            .label = "Material Buffer",
            .usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
            .size = descriptor.materialBufferSize,
            .mappedAtCreation = false
        };
        wgpu::SamplerDescriptor samplerDescriptor {
            .addressModeU = wgpu::AddressMode::Repeat,
            .addressModeV = wgpu::AddressMode::Repeat,
            .addressModeW = wgpu::AddressMode::Repeat,
            .magFilter = wgpu::FilterMode::Linear,
            .minFilter = wgpu::FilterMode::Linear,
            .mipmapFilter = wgpu::MipmapFilterMode::Linear,
            .compare = wgpu::CompareFunction::Undefined,
            .maxAnisotropy = 2,
        };
        struct Data {
            uint32_t width;
            uint32_t height;
            std::variant<stbi_uc*, const void*, RGBA8> data;
        };
        struct TextureArrayInfo {
            uint32_t width;
            uint32_t height;
            std::vector<std::variant<stbi_uc*, const void*, RGBA8>> data;
        };
        std::vector<TextureArrayInfo> textureArrayInfos;
        std::vector<TexturePointer> textureMapping;
        textureMapping.reserve(descriptor.textureData.size());
        // group texture sources by texture dimentions
        for (size_t i = 0; i < descriptor.textureData.size(); ++i) {
            Data d = std::visit([](auto&& arg){
                using namespace ren::texture::datasource;
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, File>) {
                    assert(arg.filename != nullptr);
                    int width, height, channels;
                    stbi_uc* data = stbi_load(
                        arg.filename,
                        &width, &height,
                        &channels,
                        STBI_rgb_alpha
                    );
                    assert(width > 0);
                    assert(height > 0);
                    return Data {
                        .width = static_cast<uint32_t>(width),
                        .height = static_cast<uint32_t>(height),
                        .data = data
                    };
                } else if constexpr (std::is_same_v<T, EncodedData>) {
                    assert(arg.data != nullptr);
                    int width, height, channels;
                    stbi_uc* data = stbi_load_from_memory(
                        arg.data,
                        static_cast<int>(arg.size),
                        &width, &height,
                        &channels,
                        STBI_rgb_alpha
                    );
                    assert(width > 0);
                    assert(height > 0);
                    return Data {
                        .width = static_cast<uint32_t>(width),
                        .height = static_cast<uint32_t>(height),
                        .data = data
                    };
                } else if constexpr (std::is_same_v<T, RawData>) {
                    return Data {
                        .width = arg.width,
                        .height = arg.height,
                        .data = arg.data
                    };
                } else if constexpr (std::is_same_v<T, SolidColor>) {
                    return Data {
                        .width = arg.width,
                        .height = arg.height,
                        .data = arg.color
                    };
                }
            }, descriptor.textureData[i]);
            auto it = std::find_if(
                textureArrayInfos.begin(),
                textureArrayInfos.end(),
                [&d](const TextureArrayInfo& a) {
                    return ((a.width == d.width) && (a.height == d.height));
                }
            );
            if (it != textureArrayInfos.end()) {
                textureMapping.push_back({
                    .textureArray = static_cast<uint32_t>(std::distance(
                        textureArrayInfos.begin(), it
                    )),
                    .layer = static_cast<uint32_t>(it->data.size())
                });
                it->data.push_back(std::move(d.data));
            } else {
                textureMapping.push_back({
                    .textureArray = static_cast<uint32_t>(textureArrayInfos.size()),
                    .layer = 0
                });
                textureArrayInfos.push_back({
                    .width = d.width,
                    .height = d.height,
                    .data = { std::move(d.data) }
                });
            }
        }
        // create actual textures
        std::vector<Texture> textures;
        textures.reserve(textureArrayInfos.size());
        for (auto& tai : textureArrayInfos) {
            // create texture
            textures.push_back(Texture(
                device, tai.width, tai.height,
                static_cast<uint32_t>(tai.data.size()),
                wgpu::TextureFormat::RGBA8Unorm,
                "Texture Array"
            ));
            // write texture
            for (size_t layerId = 0; layerId < tai.data.size(); ++layerId) {
                uint32_t offset = static_cast<uint32_t>(layerId);
                std::visit([
                    &textures, &queue, offset,
                    size = tai.width * tai.height
                ](auto&& arg){
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, stbi_uc*>) {
                        textures.back().write(queue, arg, offset);
                        stbi_image_free(arg);
                    } else if constexpr (std::is_same_v<T, const void*>) {
                        textures.back().write(queue, arg, offset);
                    } else if constexpr (std::is_same_v<T, RGBA8>) {
                        std::vector<RGBA8> data(size, arg);
                        textures.back().write(queue, data.data(), offset);
                    }
                }, tai.data[layerId]);
            }
        }
        return Resources(
            constmeshbuffer::create(device, queue, "Const Mesh Buffer"),
            device.CreateBuffer(&instanceBufferDescriptor),
            device.CreateBuffer(&materialBufferDescriptor),
            device.CreateSampler(&samplerDescriptor),
            std::move(textures),
            std::move(textureMapping),
            mem::bumpAllocatedUniformBuffer(device,
                mem::ceilToNextMultiple(
                    descriptor.uniform.size,
                    getLimits(device).minUniformBufferOffsetAlignment
                ) * descriptor.uniform.maxCount
            )
        );
    }()) {}
}