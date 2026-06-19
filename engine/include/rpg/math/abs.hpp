#pragma once

namespace rpg::math {
    template <typename T>
    inline constexpr T abs(T x) {
        return x >= static_cast<T>(0) ? x : -x;
    }
}