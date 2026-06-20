#pragma once

#include "Vec3.hpp"
#include "Mat.hpp"

namespace rpg::math {
    template <typename T = float>
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

        template <Handedness H = defaultHandedness>
        constexpr Quat& mulAssign(Quat rhs) {
            s = (s * rhs.s) - math::dot(v, rhs.v);
            v = (s * rhs.v) + (rhs.s * v) + cross<H>(v, rhs.v);
            return *this;
        }
        template <Handedness H = defaultHandedness>
        constexpr Quat mul(Quat rhs) const {
            return Quat(
                (s * rhs.s) - math::dot(v, rhs.v),
                (s * rhs.v) + (rhs.s * v) + cross<H>(v, rhs.v)
            );
        }
        constexpr Quat& operator+=(Quat rhs) { s += rhs.s; v += rhs.v; }
        constexpr Quat& operator-=(Quat rhs) { s -= rhs.s; v -= rhs.v; }
        template<Handedness H = defaultHandedness>
        constexpr Quat& operator*=(Quat rhs) { return mulAssign<H>(rhs); }
        constexpr Quat& operator*=(T rhs) { s *= rhs; v *= rhs; return *this; }
        constexpr Quat& operator/=(T rhs) { s /= rhs; v /= rhs; return *this; }

        friend constexpr Quat operator+(Quat lhs, Quat rhs) { lhs += rhs; return lhs; }
        friend constexpr Quat operator-(Quat lhs, Quat rhs) { lhs -= rhs; return lhs; }
        template <Handedness H = defaultHandedness>
        friend constexpr Quat operator*(Quat lhs, Quat rhs) { return lhs.mul<H>(rhs); }
        friend constexpr Quat operator*(Quat lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Quat operator/(Quat lhs, T rhs) { lhs /= rhs; return lhs; }

        friend constexpr Quat operator*(Quat lhs, Vec3<T> rhs) { lhs *= rhs; return lhs; }

        friend constexpr bool operator==(Quat lhs, Quat rhs) {
            return (lhs.s == rhs.s) && (lhs.v == rhs.v);
        }
        friend constexpr bool operator!=(Quat lhs, Quat rhs) {
            return (lhs.s != rhs.s) || (lhs.v != rhs.v);
        }

        constexpr Quat& conjugateAssign() { v = -v; return *this; }
        constexpr Quat conjugate() const { return Quat(s, -v); }

        // norm squared
        constexpr T norm2() const { return (s * s) + math::dot(v, v); }

        // norm / magnitude / length
        constexpr T length() const { return sqrt(norm2()); }

        constexpr Quat& normalizeAssign() { return *this /= length(); }
        constexpr Quat normalize() const { return *this / length(); }

        constexpr Quat& inverseAssign() { conjugateAssign(); return *this /= norm2(); }
        constexpr Quat inverse() const { return conjugate() / norm2(); }

        constexpr T dot(Quat rhs) const {
            return (s * rhs.s) + math::dot(v, rhs.v);
        }

        template<Handedness H = defaultHandedness>
        constexpr Mat3<T> toMat3() const {
            T r = static_cast<T>(2) / norm2();

            T ii = v.x * v.x;
            T jj = v.y * v.y;
            T kk = v.z * v.z;

            T ij = v.x * v.y;
            T jk = v.y * v.z;
            T ik = v.x * v.z;

            T ir = v.x * s;
            T jr = v.y * s;
            T kr = v.z * s;
            
            T one = static_cast<T>(1);
            if constexpr (H == Handedness::Right) return Mat3<T> {
                one - (r * (jj + kk)), r * (ij - kr), r * (ik + jr),
                r * (ij + kr), one - (r * (ii + kk)), r * (jk - ir),
                r * (ik - jr), r * (jk + ir), one - (r * (ii + jj))
            };
            else return Mat3<T> {
                one - (r * (jj + kk)), r * (ij + kr), r * (ik - jr),
                r * (ij - kr), one - (r * (ii + kk)), r * (jk + ir),
                r * (ik + jr), r * (jk - ir), one - (r * (ii + jj))
            };
        }

        template <Handedness H = defaultHandedness>
        constexpr Mat4<T> toMat4() const {
            Mat3<T> m = toMat3<H>();
            return Mat4<T> {
                m[0][0], m[0][1], m[0][2], 0,
                m[1][0], m[1][1], m[1][2], 0,
                m[2][0], m[2][1], m[2][2], 0,
                0, 0, 0, 1
            };
        }
    };

    using Quatf = Quat<float>;
    using Quatd = Quat<double>;

    static_assert(
        Quatf::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            pi<float>
        ).toMat3() == Mat3f {
            -1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, -1.0f
        }
    );

    template <typename T>
    constexpr T dot(Quat<T> lhs, Quat<T> rhs) {
        return lhs.dot(rhs);
    }
    template <Handedness H = defaultHandedness, typename T>
    constexpr Quat<T> mul(Quat<T> lhs, Quat<T> rhs) {
        return lhs.template mul<H>(rhs);
    }

    template <Handedness H = defaultHandedness, typename T>
    constexpr Vec3<T> rotate(Vec3<T> p, Quat<T> q) {
        if (q == Quat<T>::zero()) return p;
        Quat<T> pp = mul<H>(Quat<T>(
            -dot(q.v, p),
            (q.s * p) + cross<H>(q.v, p)
        ), q.inverse());
        assert(pp.s == static_cast<T>(0));
        return pp.v;
    }

    template <Handedness H = defaultHandedness, typename T>
    constexpr Mat3<T> toMat3(Quat<T> q) {
        return q.template toMat3<H>();
    }

    template <Handedness H = defaultHandedness, typename T>
    constexpr Mat4<T> toMat4(Quat<T> q) {
        return q.template toMat4<H>();
    }

    namespace mat3 {
        template <Handedness H = defaultHandedness, typename T>
        constexpr Mat3<T> rotation(Quat<T> q) {
            return toMat3<H>(q);
        }
    }
    namespace mat4 {
        template <Handedness H = defaultHandedness, typename T>
        constexpr Mat4<T> rotation(Quat<T> q) {
            return toMat4<H>(q);
        }
    }

    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quatf::axisAngle(Vec3f(0.0f, 1.0f, 0.0f), pi<float>)
        ) == Vec3f(-1.0f, 0.0f, 0.0f)
    );
    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quatf::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float> / 2.0f
            ) * Quatf::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float> / 2.0f
            )
        ) == Vec3f(-1.0f, 0.0f, 0.0f)
    );
    static_assert(
        Quatf::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            pi<float>
        ).inverse() == Quatf::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            -pi<float>
        )
    );
    static_assert(
        rotate<Handedness::Right>(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quatf::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float>
            )
        ).x == rotate<Handedness::Left>(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quatf::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float>
            )
        ).x
    );
    static_assert(
        rotate(
            Vec3f(1.0f, 0.0f, 0.0f),
            Quatf::axisAngle(
                Vec3f(0.0f, 1.0f, 0.0f),
                pi<float>
            )
        ) == (Quatf::axisAngle(
            Vec3f(0.0f, 1.0f, 0.0f),
            pi<float>
        ).toMat3() * Vec3f(1.0f, 0.0f, 0.0f))
    );
}