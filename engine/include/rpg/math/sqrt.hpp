#pragma once
#include "abs.hpp"

namespace rpg::math {
    namespace alg {
        template <typename T>
        inline constexpr T heron(T x, T estimate, T precision) {
            while(true) {
                T next = (estimate + (x / estimate)) / static_cast<T>(2.0);
                if (abs(estimate - next) <= precision) return next;
                estimate = next;
            }
        }
        template <typename T>
        inline constexpr T heron(T x, T precision) {
            return heron(x, x / static_cast<T>(2.0), precision);
        }
    }

    template <typename T>
    inline constexpr T sqrt(T x) {
        return alg::heron(x, static_cast<T>(0.0));
    }
}