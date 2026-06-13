#pragma once
#include <webgpu/webgpu_cpp.h>

namespace rpg::ren {
	struct RGBA8 { uint8_t r, g, b, a; };
    struct Texture {
        wgpu::Texture texture;
        wgpu::TextureView view;

        template <typename T, size_t Width, size_t Height>
        struct StaticData {
            std::array<T, Width * Height> _data;

            constexpr size_t width() const { return Width; }
            constexpr size_t height() const { return Height; }
            constexpr T* data() { return _data.data(); }
            constexpr const T* data() const { return _data.data(); }
        };

        template <auto color, size_t Width, size_t Height>
        inline constexpr static auto singleColorData() {
            StaticData<std::decay_t<decltype(color)>, Width, Height> res {};
            std::fill_n(res.data(), Width * Height, color);
            return res;
        }

        inline static std::vector<uint8_t> textureGradient(uint32_t width, uint32_t height) {
            std::vector<uint8_t> pixels(4 * width * height);
            for (uint32_t i = 0; i < width; ++i) {
                for (uint32_t j = 0; j < height; ++j) {
                    uint8_t *p = &pixels[4 * (j * width + i)];
                    p[0] = (uint8_t)i; // r
                    p[1] = (uint8_t)j; // g
                    p[2] = 128; // b
                    p[3] = 255; // a
                }
            }
            return pixels;
        }

        inline static Texture make(
            const wgpu::Device& device,
            uint32_t width, uint32_t height, const void* data,
            wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm,
            std::string_view label = std::string_view()
        ) {
            Texture res;
            uint32_t bytesPerPixel = 4;
            if (format == wgpu::TextureFormat::R8Unorm) bytesPerPixel = 1;
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
