#pragma once
#include "Vec.hpp"
#include "hendedness.hpp"
#include "trigonometric.hpp"

namespace rpg::math {
    template <typename T>
    struct Vec<T, 3> {
        static constexpr size_t Dim = 3;

        T x, y, z;

        constexpr Vec() = default;
        explicit constexpr Vec(T scalar) : x(scalar), y(scalar), z(scalar) {}
        constexpr Vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
        constexpr Vec(std::initializer_list<T> values) :
        x(*values.begin()), y(*(values.begin() + 1)), z(*(values.begin() + 2)) {
            assert(values.size() == 3);
        }

        constexpr T& operator[](size_t i) {
            switch(i) {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: assert(false);
            };
        }
        constexpr const T& operator[](size_t i) const {
            switch(i) {
                case 0: return x;
                case 1: return y;
                case 2: return z;
                default: assert(false);
            };
        }

        constexpr Vec& operator+=(const Vec& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        constexpr Vec& operator-=(const Vec& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        constexpr Vec& operator*=(const Vec& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
        constexpr Vec& operator/=(const Vec& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

        constexpr Vec& operator*=(T rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
        constexpr Vec& operator/=(T rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; }

        friend constexpr Vec operator+(Vec lhs, const Vec& rhs) { lhs += rhs; return lhs; }
        friend constexpr Vec operator-(Vec lhs, const Vec& rhs) { lhs -= rhs; return lhs; }
        friend constexpr Vec operator*(Vec lhs, const Vec& rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator/(Vec lhs, const Vec& rhs) { lhs /= rhs; return lhs; }

        friend constexpr Vec operator*(Vec lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator*(T lhs, Vec rhs) { return rhs * lhs; }
        friend constexpr Vec operator/(Vec lhs, T rhs) { lhs /= rhs; return lhs; }
        constexpr Vec operator-() const { return Vec(-x, -y, -z); }

        friend constexpr bool operator==(Vec lhs, const Vec& rhs) {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
        }
        friend constexpr bool operator!=(Vec lhs, const Vec& rhs) { return !(lhs == rhs); }

        constexpr size_t dim() const { return Dim; }

        constexpr T dot(const Vec& rhs) const {
            return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
        }
        constexpr Vec crossRH(const Vec& rhs) const {
            return Vec(
                (y * rhs.z) - (z * rhs.y),
                (z * rhs.x) - (x * rhs.z),
                (x * rhs.y) - (y * rhs.x)
            );
        }
        constexpr Vec crossLH(const Vec& rhs) const {
            return Vec(
                (z * rhs.y) - (y * rhs.z),
                (x * rhs.z) - (z * rhs.x),
                (y * rhs.x) - (x * rhs.y)
            );
        }
        template <Handedness H = defaultHandedness>
        constexpr Vec cross(const Vec& rhs) const {
            if constexpr (H == Handedness::Right) return crossRH(rhs);
            else return crossLH(rhs);
        }
        constexpr T length() const {
            return sqrt(dot(*this));
        }
        constexpr Vec& normalizeAssign() {
            return *this /= length();
        }
        constexpr Vec normalize() const {
            return *this / length();
        }
    };

    template <typename T>
    constexpr Vec<T, 3> crossRH(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) {
        return lhs.crossRH(rhs);
    }
    template <typename T>
    constexpr Vec<T, 3> crossLH(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) {
        return lhs.crossLH(rhs);
    }
    template <Handedness H = defaultHandedness, typename T>
    constexpr Vec<T, 3> cross(const Vec<T, 3>& lhs, const Vec<T, 3>& rhs) {
        return lhs.template cross<H>(rhs);
    }

    template <Handedness H = defaultHandedness, typename T>
    constexpr Vec<T, 3> rotate(const Vec<T, 3>& v, const Vec<T, 3>& axis, float angle) {
        Vec<T, 3> cr = cross<H>(axis, v);
        return
            v + (sin(angle) * cr) +
            (
                (static_cast<T>(1.0) - cos(angle)) *
                cross<H>(axis, cr)
            );
    }
    template <typename T>
    constexpr Vec<T, 3> rotateRH(const Vec<T, 3>& v, const Vec<T, 3>& axis, float angle) {
        return rotate<Handedness::Right>(v, axis, angle);
    }
    template <typename T>
    constexpr Vec<T, 3> rotateLH(const Vec<T, 3>& v, const Vec<T, 3>& axis, float angle) {
        return rotate<Handedness::Left>(v, axis, angle);
    }

    template<typename T> using Vec3 = Vec<T, 3>;
    using Vec3f32 = Vec3<float>;
    using Vec3f64 = Vec3<double>;
    using Vec3i32 = Vec3<int32_t>;
    using Vec3u32 = Vec3<uint32_t>;

    using Vec3f = Vec3f32;
    using Vec3d = Vec3f64;
    using Vec3i = Vec3i32;
    using Vec3u = Vec3u32;

    static_assert(
        cross(
            Vec3f(2.0f, 0.0f, 0.0f),
            Vec3f(0.0f, 2.0f, 0.0f)
        ) == Vec3f(0.0f, 0.0f, 4.0f)
    );
    static_assert(
        cross(
            Vec3f(0.0f, 2.0f, 0.0f),
            Vec3f(2.0f, 0.0f, 0.0f)
        ) == Vec3f(0.0f, 0.0f, -4.0f)
    );

    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            Vec3f(0.0f, 1.0f, 0.0f),
            pi<float>
        ) == Vec3f(-1.0f, 0.0f, 0.0f)
    );
}