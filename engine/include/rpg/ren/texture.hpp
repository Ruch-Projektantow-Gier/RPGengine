#pragma once

#include <cstdint>
#include <array>

namespace rpg::ren::texture {
    struct RGBA8 {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;
    };

    template <typename T, size_t Width, size_t Height>
    struct Data {
        std::array<T, Width * Height> _data;

        explicit constexpr Data(const T& color) : _data{} {
            std::fill_n(_data.data(), Width * Height, color);
        }

        constexpr size_t width() const { return Width; }
        constexpr size_t height() const { return Height; }
        constexpr T* data() { return _data.data(); }
        constexpr const T* data() const { return _data.data(); }
    };
}