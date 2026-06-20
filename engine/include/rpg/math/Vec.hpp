#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <cassert>

#include "sqrt.hpp"

namespace rpg::math {
    template <typename T, size_t Dim>
    struct Vec {
    private:
        T _[Dim];
    public:
        constexpr Vec() = default;
        explicit constexpr Vec(T scalar) { std::fill_n(_, Dim, scalar); }
        explicit constexpr Vec(T (&other)[Dim]) : _(other) { }
        constexpr Vec(std::initializer_list<T> values) {
            assert(values.size() == Dim);
            T* d = begin();
            for (const T& v : values) *(d++) = v;
        }

        constexpr T& operator[](size_t i) { return _[i]; }
        constexpr const T& operator[](size_t i) const { return _[i]; }

        constexpr Vec& operator+=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] += rhs[i]; return *this; }
        constexpr Vec& operator-=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] -= rhs[i]; return *this; }
        constexpr Vec& operator*=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] *= rhs[i]; return *this; }
        constexpr Vec& operator/=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] /= rhs[i]; return *this; }

        constexpr Vec& operator*=(T rhs) { for (T& v : *this) v *= rhs; return *this; }
        constexpr Vec& operator/=(T rhs) { for (T& v : *this) v /= rhs; return *this; }

        friend constexpr Vec operator+(Vec lhs, const Vec& rhs) { lhs += rhs; return lhs; }
        friend constexpr Vec operator-(Vec lhs, const Vec& rhs) { lhs -= rhs; return lhs; }
        friend constexpr Vec operator*(Vec lhs, const Vec& rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator/(Vec lhs, const Vec& rhs) { lhs /= rhs; return lhs; }

        friend constexpr Vec operator*(Vec lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator*(T lhs, Vec rhs) { return rhs * lhs; }
        friend constexpr Vec operator/(Vec lhs, T rhs) { lhs /= rhs; return lhs; }
        constexpr Vec operator-() const {
            Vec res;
            for (size_t i = 0; i < Dim; ++i) res[i] = -_[i];
            return res;
        }

        friend constexpr bool operator==(Vec lhs, const Vec& rhs) {
            for (size_t i = 0; i < Dim; ++i) if (lhs[i] != rhs[i]) return false;
            return true;
        }
        friend constexpr bool operator!=(Vec lhs, const Vec& rhs) { return !(lhs == rhs); }

        constexpr T& x() { static_assert(Dim >= 1); return _[0]; }
        constexpr const T& x() const { static_assert(Dim >= 1); return _[0]; }

        constexpr T& y() { static_assert(Dim >= 2); return _[1]; }
        constexpr const T& y() const { static_assert(Dim >= 2); return _[1]; }

        constexpr T& z() { static_assert(Dim >= 3); return _[2]; }
        constexpr const T& z() const { static_assert(Dim >= 3); return _[2]; }

        constexpr T* begin() { return _; }
        constexpr const T* begin() const { return _; }
        constexpr T* end() { return _ + Dim; }
        constexpr const T* end() const { return _ + Dim; }

        constexpr T* data() { return _; }
        constexpr const T* data() const { return _; }

        constexpr T dot(const Vec& lhs) const {
            T res = static_cast<T>(0);
            for (size_t i = 0; i < Dim; ++i) res += _[i] * lhs[i];
            return res;
        }
        constexpr T length() const {
            return sqrt(dot(*this));
        }
        constexpr Vec normalized() const {
            return *this / length();
        }
    };

    template <typename T, size_t Dim>
    constexpr T dot(const Vec<T, Dim>& rhs, const Vec<T, Dim>& lhs) {
        return rhs.dot(lhs);
    }


    template<typename T> using Vec1 = Vec<T, 1>;

    template<typename T> using Vec4 = Vec<T, 4>;
    using Vec4f32 = Vec4<float>;
    using Vec4f64 = Vec4<double>;
    using Vec4i32 = Vec4<int32_t>;
    using Vec4u32 = Vec4<uint32_t>;

    using Vec4f = Vec4f32;
    using Vec4d = Vec4f64;
    using Vec4i = Vec4i32;
    using Vec4u = Vec4u32;
}

#include "Vec2.hpp"
#include "Vec3.hpp"