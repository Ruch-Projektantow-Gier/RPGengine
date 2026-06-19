#pragma once

#include "Vec.hpp"
#include <algorithm>

namespace rpg::math {
    template <typename T, size_t X, size_t Y>
    struct Mat {
        using value_type = T;

        Vec<T, Y> _[X];

        explicit constexpr Mat() = default;
        explicit constexpr Mat(const T& scalar) { std::fill_n(_, X, Vec<T, Y>(scalar)); }
        constexpr Mat(std::initializer_list<Vec<T, Y>> values) {
            assert(values.size() == X);
            for (size_t i = 0; i < X; ++i) _[i] = values[i];
        }
        constexpr Mat(std::initializer_list<T> values) {
            assert(values.size() == X * Y);
            Vec<T, Y>* d = begin();
            for (auto s = values.begin(); s != values.end(); s += Y) {
                T* d1 = (d++)->begin();
                for (auto s1 = s; s1 < s + Y; ++s1)
                    *(d1++) = *s1;
            }
        }
        constexpr Mat(std::initializer_list<std::initializer_list<T>> values) {
            assert(values.size() == X);
            Vec<T, Y>* d = begin();
            for (auto v : values) *(d++) = v;
        }

        constexpr Vec<T, Y>& operator[](size_t i) { return _[i]; }
        constexpr const Vec<T, Y>& operator[](size_t i) const { return _[i]; }

        constexpr Mat& operator+=(const Mat& rhs) { for (size_t i = 0; i < X; ++i) _[i] += rhs[i]; }
        constexpr Mat& operator-=(const Mat& rhs) { for (size_t i = 0; i < X; ++i) _[i] -= rhs[i]; }

        friend constexpr Vec<T, X> operator*(Mat lhs, const Vec<T, Y>& rhs) {
            Vec<T, X> res;
            for (size_t i = 0; i < X; ++i) res[i] = dot(lhs[i], rhs);
            return res;
        }

        constexpr Vec<T, Y>* begin() { return _; }
        constexpr const Vec<T, Y>* begin() const { return _; }
        constexpr Vec<T, Y>* end() { return _ + X; }
        constexpr const Vec<T, Y>* end() const { return _ + X; }

        constexpr T* data() { return _[0].data(); }
        constexpr const T* data() const { return _[0].data(); }

        static constexpr Mat identity() { }
    };
    namespace mat {
        template <typename T>
        constexpr Mat<T, 4, 4> translation(
            const T& x, const T& y, const T& z
        ) {
            return Mat<T, 4, 4> {
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
            };
        }
        template <typename T>
        constexpr Mat<T, 4, 4> translation(const Vec<T, 3>& p) {
            return translation(p.x, p.y, p.z);
        }
        static_assert(translation(1.0f, 1.0f, 1.0f)[0][0] == 1.0f);
        static_assert(
            translation(1.0f, 1.0f, 1.0f) *
            Vec4f({0.0f, 0.0f, 0.0f, 1.0f}) == 
            Vec4f({1.0f, 1.0f, 1.0f, 1.0f})
        );
    }
}