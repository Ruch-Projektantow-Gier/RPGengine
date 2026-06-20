#pragma once

#include "Quat.hpp"

namespace rpg::math {
    template <typename T = float, Handedness H = defaultHandedness>
    struct HQuat {
        T s;
        Vec3<T> v;

        constexpr HQuat() = default;
        constexpr HQuat(T w, T x, T y, T z) : s(w), v(x, y, z) {}
        constexpr HQuat(T scalar, Vec3<T> vector) : s(scalar), v(vector) {}
        static constexpr HQuat axisAngle(Vec3<T> axis, T angle) {
            T half_angle = angle / static_cast<T>(2.0);
            return Quat(cos(half_angle), sin(half_angle) * axis);
        }
        static constexpr HQuat zero() {
            T zero = static_cast<T>(0);
            return Quat(zero, Vec3<T>(zero));
        }

        constexpr HQuat& operator+=(HQuat rhs) { s += rhs.s; v += rhs.v; }
        constexpr HQuat& operator-=(HQuat rhs) { s -= rhs.s; v -= rhs.v; }
        constexpr HQuat& operator*=(HQuat rhs) {
            s = (s * rhs.s) - math::dot(v, rhs.v);
            v = (s * rhs.v) + (rhs.s * v) + cross<H>(v, rhs.v);
            return *this;
        }
        constexpr HQuat& operator*=(T rhs) { s *= rhs; v *= rhs; return *this; }
        constexpr HQuat& operator/=(T rhs) { s /= rhs; v /= rhs; return *this; }

        friend constexpr HQuat operator+(HQuat lhs, HQuat rhs) { lhs += rhs; return lhs; }
        friend constexpr HQuat operator-(HQuat lhs, HQuat rhs) { lhs -= rhs; return lhs; }
        friend constexpr HQuat operator*(HQuat lhs, HQuat rhs) { lhs *= rhs; return lhs; }
        friend constexpr HQuat operator*(HQuat lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr HQuat operator/(HQuat lhs, T rhs) { lhs /= rhs; return lhs; }

        friend constexpr HQuat operator*(HQuat lhs, Vec3<T> rhs) { lhs *= rhs; return lhs; }

        friend constexpr bool operator==(HQuat lhs, HQuat rhs) {
            return (lhs.s == rhs.s) && (lhs.v == rhs.v);
        }
        friend constexpr bool operator!=(HQuat lhs, HQuat rhs) {
            return (lhs.s != rhs.s) || (lhs.v != rhs.v);
        }

        constexpr HQuat& conjugateAssign() { v = -v; return *this; }
        constexpr HQuat conjugate() const { return HQuat(s, -v); }

        // norm squared
        constexpr T norm2() const { return (s * s) + math::dot(v, v); }

        // norm / magnitude / length
        constexpr T length() const { return sqrt(norm2()); }

        constexpr HQuat& normalizeAssign() { return *this /= length(); }
        constexpr HQuat normalize() const { return *this / length(); }

        constexpr HQuat& inverseAssign() { conjugateAssign(); return *this /= norm2(); }
        constexpr HQuat inverse() const { return conjugate() / norm2(); }

        constexpr T dot(HQuat rhs) const {
            return (s * rhs.s) + math::dot(v, rhs.v);
        }
    };

    template <Handedness H = defaultHandedness>
    using HQuatf = HQuat<float, H>;

    template <Handedness H = defaultHandedness>
    using HQuatd = HQuat<double, H>;

    template <typename T = float>
    using QuatRH = HQuat<T, Handedness::Right>;

    template <typename T = float>
    using QuatLH = HQuat<T, Handedness::Left>;

    using QuatfRH = HQuat<float, Handedness::Right>;
    using QuatfLH = HQuat<float, Handedness::Left>;
    using QuatdRH = HQuat<double, Handedness::Right>;
    using QuatdLH = HQuat<double, Handedness::Left>;
}