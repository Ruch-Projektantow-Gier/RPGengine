#pragma once

#include <cstdint>

namespace rpg::math {
    enum struct Handedness : uint8_t { Right, Left };
    inline constexpr Handedness defaultHandedness = Handedness::Right;
}