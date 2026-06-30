#pragma once
#include <webgpu/webgpu_cpp.h>
#include <rpg/ren/texture.hpp>

namespace rpg::ren::wgp {
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
        struct FormatTraits<ren::RGBA8> {
            inline static constexpr wgpu::TextureFormat WGPUFormat = wgpu::TextureFormat::RGBA8Unorm;
        };

        template <typename T>
        inline constexpr wgpu::TextureFormat formatOf = FormatTraits<T>::WGPUFormat;

        inline wgpu::Texture create(
            const wgpu::Device& device,
            const wgpu::TextureDescriptor& descriptor
        ) {
            return device.CreateTexture(&descriptor);
        }

        namespace view {
            inline wgpu::TextureView create(
                const wgpu::Texture& texture,
                const wgpu::TextureViewDescriptor& descriptor
            ) {
                return texture.CreateView(&descriptor);
            }
        }
    }
    struct Texture {
        wgpu::Texture texture;
        wgpu::TextureView view;

        Texture() = default;

        Texture(
            const wgpu::Texture& texture_,
            const wgpu::TextureView& view_
        ) : texture(texture_), view(view_) {}
        Texture(
            const wgpu::Texture& texture_,
            wgpu::TextureView&& view_
        ) : texture(texture_), view(std::move(view_)) {}
        Texture(
            wgpu::Texture&& texture_,
            const wgpu::TextureView& view_
        ) : texture(std::move(texture_)), view(view_) {}
        Texture(
            wgpu::Texture&& texture_,
            wgpu::TextureView&& view_
        ) : texture(std::move(texture_)), view(std::move(view_)) { }

        Texture(
            const wgpu::Device& device,
            uint32_t width, uint32_t height, uint32_t count,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) : texture(wgp::texture::create(device, {
            .label = label,
            .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
            .dimension = wgpu::TextureDimension::e2D,
            .size = { width, height, count },
            .format = format,
        })), view(wgp::texture::view::create(texture, {
            .label = label,
            .format = format,
            .dimension = wgpu::TextureViewDimension::e2DArray,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = count,
            .aspect = wgpu::TextureAspect::All,
        })) {}

        Texture(
            const wgpu::Device& device,
            uint32_t width, uint32_t height,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) : texture(wgp::texture::create(device, {
            .label = label,
            .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
            .dimension = wgpu::TextureDimension::e2D,
            .size = { width, height, 1 },
            .format = format,
        })), view(wgp::texture::view::create(texture, {
            .label = label,
            .format = format,
            .dimension = wgpu::TextureViewDimension::e2D,
            .baseMipLevel = 0,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 1,
            .aspect = wgpu::TextureAspect::All,
        })) { }

        Texture(
            const wgpu::Device& device,
            uint32_t width, uint32_t height, uint32_t count,
            const void* data,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) : Texture(device, width, height, count, format, label) {
            write(
                device.GetQueue(),
                width, height,
                texture::bytesPerPixel(format),
                data, 0, count
            );
        }

        void write(
            const wgpu::Queue& queue,
            uint32_t width, uint32_t height,
            uint32_t bytesPerPixel,
            const void* data,
            uint32_t offset = 0,
            uint32_t count = 1
        ) const {
            wgpu::TexelCopyTextureInfo destination {
                .texture = texture,
                .mipLevel = 0,
                .origin = { 0, 0, offset },
                .aspect = wgpu::TextureAspect::All,
            };
            wgpu::TexelCopyBufferLayout source {
                .offset = 0,
                .bytesPerRow = bytesPerPixel * width,
                .rowsPerImage = height,
            };

            wgpu::Extent3D size = { width, height, count };
            queue.WriteTexture(
                &destination,
                data,
                static_cast<size_t>(bytesPerPixel) * width * height * count,
                &source,
                &size
            );
        }
        void write(
            const wgpu::Queue& queue,
            const void* data,
            uint32_t offset = 0,
            uint32_t count = 1
        ) {
            assert(offset + count <= texture.GetDepthOrArrayLayers());
            write(
                queue,
                texture.GetWidth(),
                texture.GetHeight(),
                texture::bytesPerPixel(texture.GetFormat()),
                data, offset, count
            );
        }
    };
}
