#pragma once
#include <algorithm>

#include "Vec.hpp"

namespace rpg::math {
    template <typename T, size_t X, size_t Y>
    struct Mat {
        using value_type = T;

    private:
        Vec<T, Y> _[X];

    public:
        explicit constexpr Mat() = default;
        explicit constexpr Mat(const T& scalar) { std::fill_n(_, X, Vec<T, Y>(scalar)); }
        constexpr Mat(std::initializer_list<Vec<T, Y>> values) {
            assert(values.size() == X);
            for (size_t i = 0; i < X; ++i) _[i] = values[i];
        }
        constexpr Mat(std::initializer_list<T> values) {
            assert(values.size() == X * Y);
            std::size_t i = 0;
            for (auto s = values.begin(); s != values.end(); s += Y) {
                std::size_t j = 0;
                for (auto s1 = s; s1 != s + Y; ++s1) {
                    _[i][j++] = *s1;
                }
                ++i;
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
        friend constexpr bool operator==(const Mat& lhs, const Mat& rhs) {
            for (std::size_t i = 0; i < X; ++i)
                if (lhs[i] != rhs[i]) return false;
            return true;
        }

        constexpr Vec<T, Y>* begin() { return _; }
        constexpr const Vec<T, Y>* begin() const { return _; }
        constexpr Vec<T, Y>* end() { return _ + X; }
        constexpr const Vec<T, Y>* end() const { return _ + X; }

        constexpr T* data() { return _[0].data(); }
        constexpr const T* data() const { return _[0].data(); }
    };

    template <typename T> using Mat2x2 = Mat<T, 2, 2>;
    using Mat2x2f = Mat2x2<float>;
    using Mat2x2d = Mat2x2<double>;
    template <typename T> using Mat2x3 = Mat<T, 2, 3>;
    using Mat2x3f = Mat2x3<float>;
    using Mat2x3d = Mat2x3<double>;
    template <typename T> using Mat2x4 = Mat<T, 2, 4>;
    using Mat2x4f = Mat2x4<float>;
    using Mat2x4d = Mat2x4<double>;

    template <typename T> using Mat3x2 = Mat<T, 3, 2>;
    using Mat3x2f = Mat3x2<float>;
    using Mat3x2d = Mat3x2<double>;
    template <typename T> using Mat3x3 = Mat<T, 3, 3>;
    using Mat3x3f = Mat3x3<float>;
    using Mat3x3d = Mat3x3<double>;
    template <typename T> using Mat3x4 = Mat<T, 3, 4>;
    using Mat3x4f = Mat3x4<float>;
    using Mat3x4d = Mat3x4<double>;

    template <typename T> using Mat4x2 = Mat<T, 4, 2>;
    template <typename T> using Mat4x3 = Mat<T, 4, 3>;
    template <typename T> using Mat4x4 = Mat<T, 4, 4>;

    template <typename T> using Mat2 = Mat2x2<T>;
    using Mat2f = Mat2<float>;
    using Mat2d = Mat2<double>;

    template <typename T> using Mat3 = Mat3x3<T>;
    using Mat3f = Mat3<float>;
    using Mat3d = Mat3<double>;
    
    template <typename T> using Mat4 = Mat4x4<T>;
    using Mat4f = Mat4<float>;
    using Mat4d = Mat4<double>;

    template <typename T = float, std::size_t Dim = 4>
    constexpr Mat<T, Dim, Dim> identity() {
        Mat m = Mat3<T>(static_cast<T>(0));
        for (size_t i = 0; i < Dim; ++i)
            m[i][i] = static_cast<T>(1);
        return m;
    }

    namespace mat3 {
        template <typename T>
        constexpr Mat3<T> identity() {
            return identity<T, 3>();
        }

        template <typename T>
        constexpr Mat3<T> translation(
            const T& x, const T& y
        ) {
            return Mat3<T> {
                1, 0, x,
                0, 1, y,
                0, 0, 1
            };
        }
        template <typename T>
        constexpr Mat3<T> translation(const Vec2<T>& p) {
            return translation(p.x, p.y);
        }
        template <typename T>
        constexpr Mat3<T> rotation(const Vec3<T> eulerAngles) {
            T sina = sin(eulerAngles.x);
            T cosa = cos(eulerAngles.x);
            T sinb = sin(eulerAngles.y);
            T cosb = cos(eulerAngles.y);
            T sinc = sin(eulerAngles.z);
            T cosc = cos(eulerAngles.z);
            return Mat3<T> {
                cosb * cosc, -cosb * sinc, sinb,
                (cosa * sinc) + (sina * sinb * cosc), (cosa * cosc) - (sina * sinb * sinc), -sina * cosb,
                (sina * sinc) - (cosa * sinb * cosc), (sina * cosc) + (cosa * sinb * sinc), cosa * cosb
            };
        }
        static_assert(translation(1.0f, 1.0f)[0][0] == 1.0f);
        static_assert(
            translation(1.0f, 1.0f) *
            Vec3f(0.0f, 0.0f, 1.0f) == 
            Vec3f(1.0f, 1.0f, 1.0f)
        );
    }

    namespace mat4 {
        template <typename T>
        static constexpr Mat4<T> identity() {
            Mat m = Mat4<T>(static_cast<T>(0));
            for (size_t i = 0; i < 4; ++i)
                m[i][i] = static_cast<T>(1);
            return m;
        }

        template <typename T>
        constexpr Mat4<T> translation(
            const T& x, const T& y, const T& z
        ) {
            return Mat4<T> {
                1, 0, 0, x,
                0, 1, 0, y,
                0, 0, 1, z,
                0, 0, 0, 1
            };
        }
        template <typename T>
        constexpr Mat4<T> translation(const Vec3<T>& p) {
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