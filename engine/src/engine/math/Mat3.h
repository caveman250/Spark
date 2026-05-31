#pragma once

#include "Vec3.h"

namespace se::math
{
    class Mat4;

    class Mat3
    {
        SPARK_POD_CLASS()
    public:
        Mat3();
        Mat3(const Mat3& other);
        explicit Mat3(float scalar);
        Mat3(float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2);

        Mat3(const Vec3& v0,
            const Vec3& v1,
            const Vec3& v2);

        Mat3(const Mat4& m);

        Vec3& operator[](size_t i);
        const Vec3& operator[](size_t i) const;

        Mat3& operator=(const Mat3& m);
        Mat3& operator+=(const Mat3& m);
        Mat3& operator-=(const Mat3& m);
        Mat3& operator*=(const Mat3& m);
        Mat3& operator*=(float scalar);
        Mat3& operator/=(const Mat3& m);
        Mat3& operator/=(float scalar);

    private:
        SPARK_MEMBER(Serialized)
        std::array<Vec3, 3> m_Value;
    };

    Mat3 operator+(const Mat3& lhs, const Mat3 &rhs);
    Mat3 operator-(const Mat3& lhs, const Mat3 &rhs);
    Mat3 operator*(const Mat3& lhs, const Mat3 &rhs);
    Mat3 operator*(const Mat3& lhs, float scalar);
    Mat3 operator/(const Mat3& lhs, const Mat3 &rhs);
    Mat3 operator/(const Mat3& lhs, float scalar);
    bool operator==(const Mat3& lhs, const Mat3 &rhs);
    bool operator!=(const Mat3& lhs, const Mat3 &rhs);

    Mat3 Inverse(const Mat3& m);
}

template <> struct std::formatter<se::math::Mat3>
{
    static constexpr auto parse(const std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const se::math::Mat3& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "mat3({}, {}, {}\n     {}, {}, {}\n     {}, {}, {})", obj[0].x, obj[0].y, obj[0].z, obj[1].x, obj[1].y, obj[1].z, obj[2].x, obj[2].y, obj[2].z, obj[3].x, obj[3].y, obj[3].z);
    }
};
