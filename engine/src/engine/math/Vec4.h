#pragma once

#include "spark.h"

namespace se::math
{
    struct Vec3;
    struct Vec4
    {
        SPARK_POD_CLASS()

        Vec4() : x(0.f), y(0.f), z(0.f), w(0.f)
        {
        }

        Vec4(const Vec3& vec, float _w);

        Vec4(const float _x,
             const float _y,
             const float _z,
             const float _w)
            : x(_x), y(_y), z(_z), w(_w)
        {
        }

        explicit Vec4(const float scalar) : x(scalar), y(scalar), z(scalar), w(scalar)
        {
        }

        SPARK_MEMBER(Serialized)
        float x;

        SPARK_MEMBER(Serialized)
        float y;

        SPARK_MEMBER(Serialized)
        float z;

        SPARK_MEMBER(Serialized)
        float w;

        float& operator[](size_t i);
        const float& operator[](size_t i) const;

        Vec4& operator+=(const Vec4 &rhs);
        Vec4& operator+=(float scalar);
        Vec4& operator-=(const Vec4 &rhs);
        Vec4& operator-=(float scalar);
        Vec4& operator*=(const Vec4 &rhs);
        Vec4& operator*=(float scalar);
        Vec4& operator/=(const Vec4 &rhs);
        Vec4& operator/=(float scalar);
    };

    Vec4 operator-(const Vec4 &lhs);
    Vec4 operator+(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator-(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator*(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator*(const Vec4& lhs, float scalar);
    Vec4 operator/(const Vec4& lhs, const Vec4 &rhs);
    Vec4 operator/(const Vec4& lhs, float scalar);
    bool operator==(const Vec4& lhs, const Vec4 &rhs);
    bool operator!=(const Vec4& lhs, const Vec4 &rhs);
}

template <> struct std::formatter<se::math::Vec4>
{
    static constexpr auto parse(const std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const se::math::Vec4& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "vec4({}, {}, {}, {})", obj.x, obj.y, obj.z, obj.w);
    }
};
