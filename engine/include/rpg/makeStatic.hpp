#pragma once

namespace rpg {
    // forces compile time evaluation of a constexpr expression
    template <auto value> inline constexpr decltype(value) makeStatic = value;
}