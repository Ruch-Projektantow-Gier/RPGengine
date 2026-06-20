#pragma once
#include "Vec.hpp"

namespace rpg::math {
    template <typename T>
    struct Vec<T, 2> {
        static constexpr size_t Dim = 3;

        T x, y;

        constexpr Vec() = default;
        explicit constexpr Vec(T scalar) : x(scalar), y(scalar) {}
        constexpr Vec(T X, T Y) : x(X), y(Y) {}
        constexpr Vec(std::initializer_list<T> values) :
        x(*values.begin()), y(*(values.begin() + 1)) {
            assert(values.size() == 2);
        }

        constexpr T& operator[](size_t i) {
            switch(i) {
                case 0: return x;
                case 1: return y;
                default: assert(false);
            }
        }
        constexpr const T& operator[](size_t i) const {
            switch(i) {
                case 0: return x;
                case 1: return y;
                default: assert(false);
            }
        }

        constexpr Vec& operator+=(const Vec& rhs) { x += rhs.x; y += rhs.y; return *this; }
        constexpr Vec& operator-=(const Vec& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        constexpr Vec& operator*=(const Vec& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        constexpr Vec& operator/=(const Vec& rhs) { x /= rhs.x; y /= rhs.y; return *this; }

        constexpr Vec& operator*=(T rhs) { x *= rhs; y *= rhs; return *this; }
        constexpr Vec& operator/=(T rhs) { x /= rhs; y /= rhs; return *this; }

        friend constexpr Vec operator+(Vec lhs, const Vec& rhs) { lhs += rhs; return lhs; }
        friend constexpr Vec operator-(Vec lhs, const Vec& rhs) { lhs -= rhs; return lhs; }
        friend constexpr Vec operator*(Vec lhs, const Vec& rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator/(Vec lhs, const Vec& rhs) { lhs /= rhs; return lhs; }

        friend constexpr Vec operator*(Vec lhs, T rhs) { lhs *= rhs; return lhs; }
        friend constexpr Vec operator*(T lhs, Vec rhs) { return rhs * lhs; }
        friend constexpr Vec operator/(Vec lhs, T rhs) { lhs /= rhs; return lhs; }
        constexpr Vec operator-() { return Vec(-x, -y); }

        constexpr size_t dim() const { return Dim; }

        constexpr T dot(const Vec& rhs) const {
            return (x * rhs.x) + (y * rhs.y);
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

    template<typename T> using Vec2 = Vec<T, 2>;
    using Vec2f32 = Vec2<float>;
    using Vec2f64 = Vec2<double>;
    using Vec2i32 = Vec2<int32_t>;
    using Vec2u32 = Vec2<uint32_t>;

    using Vec2f = Vec2f32;
    using Vec2d = Vec2f64;
    using Vec2i = Vec2i32;
    using Vec2u = Vec2u32;

    template <typename T>
    constexpr Vec2<T> rotate(Vec2<T> v, T angle) {
        T s = sin(angle);
        T c = cos(angle);
        return Vec<T, 2>(
            (v.x * c) - (v.y * s),
            (v.x * s) + (v.y * c)
        );
    }
}