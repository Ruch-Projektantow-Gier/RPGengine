#pragma once

namespace rpg::math {
    template <typename T> constexpr T pi;

    template <> inline constexpr float pi<float> = 3.1415926535f;

    template <> inline constexpr double pi<double> = 3.141592653589793238;
}