#pragma once

#include "Vec3.hpp"

namespace rpg::math {
    template <typename T = float, Handedness H = defaultHandedness>
    struct Quat {
        T s;
        Vec3<T> v;

        constexpr Quat() = default;
        constexpr Quat(T w, T x, T y, T z) : s(w), v(x, y, z) {}
        constexpr Quat(T scalar, Vec3<T> vector) : s(scalar), v(vector) {}
        static constexpr Quat axisAngle(Vec3<T> axis, T angle) {
            T half_angle = angle / static_cast<T>(2.0);
            return Quat(cos(half_angle), sin(half_angle) * axis);
        }
        static constexpr Quat zero() {
            T zero = static_cast<T>(0);
            return Quat(zero, Vec3<T>(zero));
        }

        constexpr Quat& operator+=(Quat rhs) { s += rhs.s; v += rhs.v; }
        constexpr Quat& operator-=(Quat rhs) { s -= rhs.s; v -= rhs.v; }
        constexpr Quat& operator*=(Quat rhs) {
            s = (s * rhs.s) - math::dot(v, rhs.v);
            v = (s * rhs.v) + (rhs.s * v) + cross<H>(v, rhs.v);
            return *this;
        }
        constexpr Quat& operator*=(T rhs) { s *= rhs; v *= rhs; return *this; }
        constexpr Quat& operator/=(T rhs) { s /= rhs; v /= rhs; return *this; }

        friend constexpr Quat operator+(Quat lhs, Quat rhs) { lhs += rhs; return lhs; }
        friend constexpr Quat operator-(Quat lhs, Quat rhs) { lhs -= rhs; return lhs; }
        friend constexpr Quat operator*(Quat lhs, Quat rhs) { lhs *= rhs; return lhs; }
        friend constexpr Quat operator*(Quat lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Quat operator/(Quat lhs, T rhs) { lhs /= rhs; return lhs; }

        friend constexpr Quat operator*(Quat lhs, Vec3<T> rhs) { lhs *= rhs; return lhs; }

        friend constexpr bool operator==(Quat lhs, Quat rhs) {
            return (lhs.s == rhs.s) && (lhs.v == rhs.v);
        }

        constexpr Quat conjugate() const { return Quat(s, -v); }

        // norm squared
        constexpr T norm2() const { return (s * s) + math::dot(v, v); }

        // norm / magnitude / length
        constexpr T length() const { return sqrt(norm2()); }

        constexpr Quat normalized() const { return *this / length(); }

        constexpr Quat inverse() const { return conjugate() / norm2(); }

        constexpr T dot(Quat rhs) const {
            return (s * rhs.s) + math::dot(v, rhs.v);
        }
    };

    template <typename T, Handedness H>
    constexpr T dot(Quat<T, H> lhs, Quat<T, H> rhs) {
        return lhs.dot(rhs);
    }

    template <typename T = float>
    using QuatRH = Quat<T, Handedness::Right>;

    template <typename T = float>
    using QuatLH = Quat<T, Handedness::Left>;

    template <typename T, Handedness H>
    constexpr Vec3<T> rotate(Vec3<T> p, Quat<T, H> q) {
        if (q == Quat<T, H>::zero()) return p;
        Quat pp = Quat(
            -dot(q.v, p),
            (q.s * p) + cross(q.v, p)
        ) * q.inverse();
        assert(pp.s == static_cast<T>(0));
        return pp.v;
    }

    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            QuatRH<>::axisAngle(Vec3f(0.0f, 1.0f, 0.0f), pi<float>)
        ) == Vec3f(-1.0f, 0.0f, 0.0f)
    );
    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quat<>::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float> / 2.0f
            ) * Quat<>::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float> / 2.0f
            )
        ) == Vec3f(-1.0f, 0.0f, 0.0f)
    );
    static_assert(
        Quat<>::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            pi<float>
        ).inverse() == Quat<>::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            -pi<float>
        )
    );
}