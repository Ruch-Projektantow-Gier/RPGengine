#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <cassert>

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

        constexpr Vec& operator+=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] += rhs[i]; }
        constexpr Vec& operator-=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] -= rhs[i]; }
        constexpr Vec& operator*=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] *= rhs[i]; }
        constexpr Vec& operator/=(const Vec& rhs) { for (size_t i = 0; i < Dim; ++i) _[i] /= rhs[i]; }

        constexpr Vec& operator*=(T rhs) { for (size_t i = 0; i < Dim; ++i) _[i] *= rhs; }
        constexpr Vec& operator/=(T rhs) { for (size_t i = 0; i < Dim; ++i) _[i] /= rhs; }

        friend constexpr Vec operator+(Vec lhs, const Vec& rhs) { lhs += rhs; return lhs; }
        friend constexpr Vec operator-(Vec lhs, const Vec& rhs) { lhs -= rhs; return lhs; }
        friend constexpr Vec operator*(Vec lhs, const Vec& rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator/(Vec lhs, const Vec& rhs) { lhs /= rhs; return lhs; }

        friend constexpr Vec operator*(Vec lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator*(T lhs, Vec rhs) { return rhs * lhs; }
        friend constexpr Vec operator/(Vec lhs, T rhs) { lhs /= rhs; return lhs; }

        friend constexpr bool operator==(Vec lhs, const Vec& rhs) {
            for (size_t i = 0; i < Dim; ++i) if (lhs[i] != rhs[i]) return false;
            return true;
        }
        friend constexpr bool operator!=(Vec lhs, const Vec& rhs) { return !(lhs == rhs); }

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

        constexpr Vec3& operator+=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
        constexpr Vec3& operator-=(const Vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
        constexpr Vec3& operator*=(const Vec3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
        constexpr Vec3& operator/=(const Vec3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; }

        constexpr Vec3& operator*=(T rhs) { x *= rhs; y *= rhs; z *= rhs; }
        constexpr Vec3& operator/=(T rhs) { x /= rhs; y /= rhs; z /= rhs; }

        friend constexpr Vec3 operator+(Vec3 lhs, const Vec3& rhs) { lhs += rhs; return lhs; }
        friend constexpr Vec3 operator-(Vec3 lhs, const Vec3& rhs) { lhs -= rhs; return lhs; }
        friend constexpr Vec3 operator*(Vec3 lhs, const Vec3& rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec3 operator/(Vec3 lhs, const Vec3& rhs) { lhs /= rhs; return lhs; }

        friend constexpr Vec3 operator*(Vec3 lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec3 operator*(T lhs, const Vec3& rhs) { return rhs * lhs; }
        friend constexpr Vec3 operator/(Vec3 lhs, T rhs) { lhs /= rhs; return lhs; }

        constexpr size_t dim() const { return Dim; }

        constexpr T* begin() { return &x; }
        constexpr const T* begin() const { return &x; }
        constexpr T* end() { return &x + Dim; }
        constexpr const T* end() const { return &x + Dim; }

        constexpr T dot(const Vec3<T>& rhs) const {
            return (x * rhs.x) + (y * rhs.y) * (z * rhs.z);
        }
    };

    template <typename T>
    constexpr T dot(const Vec3<T>& rhs, const Vec3<T>& lhs) {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) * (lhs.z * rhs.z);
    }

    using Vec3f32 = Vec3<float>;
    using Vec3f64 = Vec3<double>;
    using Vec3i32 = Vec3<int32_t>;
    using Vec3u32 = Vec3<uint32_t>;

    using Vec3f = Vec3f32;
    using Vec3d = Vec3f64;
    using Vec3i = Vec3i32;
    using Vec3u = Vec3u32;
}