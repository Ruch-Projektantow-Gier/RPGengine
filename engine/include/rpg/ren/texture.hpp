#pragma once

#include <span>
#include <variant>

#include "RGBA8.hpp"

namespace rpg::ren {
    namespace texture {
        namespace datasource {
            struct EncodedData {
                const void* data;
            };
            struct RawData {
                uint32_t width;
                uint32_t height;
                const void* data;
            };
            struct SolidColor {
                uint32_t width;
                uint32_t height;
                ren::RGBA8 color;
            };
        };
        using DataSource = std::variant<datasource::RawData, datasource::SolidColor>;
    }
    namespace texturearray {
        namespace texture {
            namespace datasource {
                struct EncodedData { const void* data; };
                struct RawData { const void* data; };
                struct SolidColor { ren::RGBA8 color; };
            };
            using DataSource = std::variant<datasource::RawData, datasource::SolidColor>;
        }
        struct CreateInfo {
            uint32_t width;
            uint32_t height;
            std::span<const texture::DataSource> sources;
        };
    }
}