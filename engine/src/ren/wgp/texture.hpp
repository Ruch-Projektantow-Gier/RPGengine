#pragma once
#include <webgpu/webgpu_cpp.h>
#include <rpg/ren/texture.hpp>

namespace rpg::ren {
    namespace texture {
        constexpr uint8_t bytesPerPixel(wgpu::TextureFormat format) {
            switch (format) {
                case wgpu::TextureFormat::R8Unorm:
                case wgpu::TextureFormat::R8Snorm:
                case wgpu::TextureFormat::R8Uint:
                case wgpu::TextureFormat::R8Sint:
                return 1;
                case wgpu::TextureFormat::R16Uint:
                case wgpu::TextureFormat::R16Sint:
                case wgpu::TextureFormat::R16Unorm:
                case wgpu::TextureFormat::R16Snorm:
                case wgpu::TextureFormat::R16Float:
                case wgpu::TextureFormat::RG8Unorm:
                case wgpu::TextureFormat::RG8Snorm:
                case wgpu::TextureFormat::RG8Uint:
                case wgpu::TextureFormat::RG8Sint:
                return 2;
                case wgpu::TextureFormat::R32Uint:
                case wgpu::TextureFormat::R32Sint:
                case wgpu::TextureFormat::R32Float:
                case wgpu::TextureFormat::RG16Uint:
                case wgpu::TextureFormat::RG16Sint:
                case wgpu::TextureFormat::RG16Unorm:
                case wgpu::TextureFormat::RG16Snorm:
                case wgpu::TextureFormat::RG16Float:
                case wgpu::TextureFormat::RGBA8Unorm:
                case wgpu::TextureFormat::RGBA8UnormSrgb:
                case wgpu::TextureFormat::RGBA8Snorm:
                case wgpu::TextureFormat::RGBA8Uint:
                case wgpu::TextureFormat::RGBA8Sint:
                case wgpu::TextureFormat::BGRA8Unorm:
                case wgpu::TextureFormat::BGRA8UnormSrgb:
                case wgpu::TextureFormat::RGB10A2Uint:
                case wgpu::TextureFormat::RGB10A2Unorm:
                case wgpu::TextureFormat::RG11B10Ufloat:
                case wgpu::TextureFormat::RGB9E5Ufloat:
                return 4;
                case wgpu::TextureFormat::RG32Float:
                case wgpu::TextureFormat::RG32Uint:
                case wgpu::TextureFormat::RG32Sint:
                case wgpu::TextureFormat::RGBA16Uint:
                case wgpu::TextureFormat::RGBA16Sint:
                case wgpu::TextureFormat::RGBA16Unorm:
                case wgpu::TextureFormat::RGBA16Snorm:
                case wgpu::TextureFormat::RGBA16Float:
                return 8;
                case wgpu::TextureFormat::RGBA32Float:
                case wgpu::TextureFormat::RGBA32Uint:
                case wgpu::TextureFormat::RGBA32Sint:
                return 16;
                default:
                assert(false); // remaining formats not handled yet
                return 0;
            }
        }

        template <wgpu::TextureFormat format>
        inline constexpr uint8_t BytesPerPixel = bytesPerPixel(format);

        template <typename T>
        struct FormatTraits;

        template <>
        struct FormatTraits<RGBA8> {
            inline static constexpr wgpu::TextureFormat WGPUFormat = wgpu::TextureFormat::RGBA8Unorm;
        };

        template <typename T>
        inline constexpr wgpu::TextureFormat formatOf = FormatTraits<T>::WGPUFormat;
    }
    struct Texture {
        wgpu::Texture texture;
        wgpu::TextureView view;

        Texture() = default;
        Texture(
            const wgpu::Device& device,
            uint32_t width, uint32_t height, uint32_t layers,
            const void* data,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) : texture([&device, width, height, layers, format, label](){
            wgpu::TextureDescriptor desc {
                .label = label,
                .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
                .dimension = wgpu::TextureDimension::e2D,
                .size = { width, height, layers },
                .format = format,
            };
            return device.CreateTexture(&desc);

        }()), view([this, layers, format, label](){
            wgpu::TextureViewDescriptor desc {
                .label = label,
                .format = format,
                .dimension = wgpu::TextureViewDimension::e2DArray,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
                .baseArrayLayer = 0,
                .arrayLayerCount = layers,
                .aspect = wgpu::TextureAspect::All,
            };
            return texture.CreateView(&desc);
        }()) {
            uint32_t bytesPerPixel = texture::bytesPerPixel(format);
            wgpu::TexelCopyTextureInfo destination {
                .texture = texture,
                .mipLevel = 0,
                .origin = { 0, 0, 0 },
                .aspect = wgpu::TextureAspect::All,
            };
            wgpu::TexelCopyBufferLayout source {
                .offset = 0,
                .bytesPerRow = bytesPerPixel * width,
                .rowsPerImage = height,
            };

            wgpu::Extent3D size = { width, height, layers };
            device.GetQueue().WriteTexture(
                &destination,
                data,
                static_cast<size_t>(bytesPerPixel) * width * height * layers,
                &source,
                &size
            );
        }

        inline static Texture make(
            const wgpu::Device& device,
            uint32_t width, uint32_t height, const void* data,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) {
            Texture res;
            uint32_t bytesPerPixel = texture::bytesPerPixel(format);
            wgpu::TextureDescriptor desc {
                .label = label,
                .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
                .dimension = wgpu::TextureDimension::e2D,
                .size = { width, height, 1 },
                .format = format,
            };
            res.texture = device.CreateTexture(&desc);

            wgpu::TexelCopyTextureInfo destination {
                .texture = res.texture,
                .mipLevel = 0,
                .origin = { 0, 0, 0 },
                .aspect = wgpu::TextureAspect::All,
            };
            wgpu::TexelCopyBufferLayout source {
                .offset = 0,
                .bytesPerRow = bytesPerPixel * width,
                .rowsPerImage = height,
            };

            device.GetQueue().WriteTexture(
                &destination,
                data,
                static_cast<size_t>(bytesPerPixel) * width * height,
                &source,
                &desc.size
            );

            wgpu::TextureViewDescriptor vdesc {
                .label = label,
                .format = format,
                .dimension = wgpu::TextureViewDimension::e2D,
                .baseMipLevel = 0,
                .mipLevelCount = desc.mipLevelCount,
                .baseArrayLayer = 0,
                .arrayLayerCount = 1,
                .aspect = wgpu::TextureAspect::All,
            };
            res.view = res.texture.CreateView(&vdesc);
            return res;
        }

        inline static Texture make(
            const wgpu::Device& device,
            const auto& data,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) {
            return make(
                device,
                static_cast<uint32_t>(data.width()),
                static_cast<uint32_t>(data.height()),
                data.data(),
                format,
                label
            );
        }
    };
}
