#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace rpg::math {
    template <typename T, size_t Dim>
    struct Vec {
    private:
        T _[Dim];
    public:
        constexpr Vec() = default;
        explicit constexpr Vec(T scalar) { std::fill_n(_, Dim, scalar); }
        explicit constexpr Vec(T (&other)[Dim]) : _(other) { }

        constexpr T& operator[](size_t i) { return _[i]; }
        constexpr const T& operator[](size_t i) const { return _[i]; }

        constexpr Vec& operator+=(const Vec& lhs) { for (size_t i = 0; i < Dim; ++i) _[i] += lhs[i]; }
        constexpr Vec& operator-=(const Vec& lhs) { for (size_t i = 0; i < Dim; ++i) _[i] -= lhs[i]; }
        constexpr Vec& operator*=(const Vec& lhs) { for (size_t i = 0; i < Dim; ++i) _[i] *= lhs[i]; }
        constexpr Vec& operator/=(const Vec& lhs) { for (size_t i = 0; i < Dim; ++i) _[i] /= lhs[i]; }

        constexpr Vec& operator*=(T lhs) { for (size_t i = 0; i < Dim; ++i) _[i] *= lhs; }
        constexpr Vec& operator/=(T lhs) { for (size_t i = 0; i < Dim; ++i) _[i] /= lhs; }

        friend constexpr Vec operator+(Vec rhs, const Vec& lhs) { rhs += lhs; return rhs; }
        friend constexpr Vec operator-(Vec rhs, const Vec& lhs) { rhs -= lhs; return rhs; }
        friend constexpr Vec operator*(Vec rhs, const Vec& lhs) { rhs *= lhs; return rhs; }
        friend constexpr Vec operator/(Vec rhs, const Vec& lhs) { rhs /= lhs; return rhs; }

        friend constexpr Vec operator*(Vec rhs, T lhs) { rhs *= lhs; return rhs; }
        friend constexpr Vec operator*(T rhs, Vec lhs) { return lhs * rhs; }
        friend constexpr Vec operator/(Vec rhs, T lhs) { rhs /= lhs; return rhs; }

        constexpr T* begin() { return _; }
        constexpr const T* begin() const { return _; }
        constexpr T* end() { return _ + Dim; }
        constexpr const T* end() const { return _ + Dim; }

        constexpr T dot(Vec lhs) const {
            T res = static_cast<T>(0);
            for (size_t i = 0; i < Dim; ++i) res += _[i] * lhs[i];
            return res;
        }
    };

    template <typename T, size_t Dim>
    constexpr T dot(const Vec<T, Dim>& rhs, const Vec<T, Dim>& lhs) {
        return rhs.dot(lhs);
    }

    template <typename T>
    struct Vec3 {
        static constexpr size_t Dim = 3;

        T x, y, z;

        constexpr Vec3() = default;
        explicit constexpr Vec3(T scalar) : x(scalar), y(scalar), z(scalar) {}
        constexpr Vec3(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
        constexpr Vec3(Vec<T, Dim> other) : x(other[0]), y(other[1]), z(other[2]) {}

        constexpr T& operator[](size_t i) { return *(&x + i); }
        constexpr const T& operator[](size_t i) const { return *(&x + i); }

        constexpr Vec3& operator+=(Vec3 lhs) {
            x += lhs.x;
            y += lhs.y;
            z += lhs.z;
        }

        constexpr size_t dim() const { return Dim; }

        constexpr T* begin() { return &x; }
        constexpr const T* begin() const { return &x; }
        constexpr T* end() { return &x + Dim; }
        constexpr const T* end() const { return &x + Dim; }
    };

    using Vec3f32 = Vec3<float>;
    using Vec3f64 = Vec3<double>;
    using Vec3i32 = Vec3<int32_t>;
    using Vec3u32 = Vec3<uint32_t>;

    using Vec3f = Vec3f32;
    using Vec3d = Vec3f64;
    using Vec3i = Vec3i32;
    using Vec3u = Vec3u32;
}