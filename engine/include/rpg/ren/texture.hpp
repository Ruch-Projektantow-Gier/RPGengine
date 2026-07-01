#pragma once

#include <span>
#include <variant>

#include "RGBA8.hpp"

namespace rpg::ren {
    namespace texture {
        namespace datasource {
            struct File {
                const char* filename;
            };
            struct EncodedData {
                const uint8_t* data;
                size_t size;
            };
            struct RawData {
                uint32_t width;
                uint32_t height;
                const void* data;
            };
            struct SolidColor {
                uint32_t width = 1;
                uint32_t height = 1;
                ren::RGBA8 color;
            };
        };
        using DataSource = std::variant<
            datasource::File,
            datasource::EncodedData,
            datasource::RawData,
            datasource::SolidColor
        >;
    }
    namespace texturearray {
        namespace texture {
            namespace datasource {
                struct File { const char* filename; };
                struct EncodedData { const uint8_t* data; size_t size; };
                struct RawData { const void* data; };
                struct SolidColor { ren::RGBA8 color; };
            };
            using DataSource = std::variant<
                datasource::File,
                datasource::EncodedData,
                datasource::RawData,
                datasource::SolidColor
            >;
        }
        struct CreateInfo {
            uint32_t width;
            uint32_t height;
            std::span<const texture::DataSource> sources;
        };
    }
}