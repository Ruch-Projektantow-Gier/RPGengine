#pragma once

#include "pi.hpp"

namespace rpg::math {
    template <typename T>
    constexpr T radians(T x) { return x * pi<T> / static_cast<T>(180); }

    template <typename T>
    constexpr T degrees(T x) { return x * static_cast<T>(180) / pi<T>; }
}