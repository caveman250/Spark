module;

#include "spark.h"


export module Spark.Math:Vec3;

namespace se::math
{
    struct Vec4;
    export struct Vec3
    {
        SPARK_POD_CLASS()

        Vec3() : x(0.f), y(0.f), z(0.f)
        {
        }

        Vec3(float scalar) : x(scalar), y(scalar), z(scalar)
        {
        }

        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
        {
        }

        Vec3(const Vec4& vec);

        SPARK_MEMBER(Serialized)
        float x;

        SPARK_MEMBER(Serialized)
        float y;

        SPARK_MEMBER(Serialized)
        float z;

        float& operator[](size_t i);
        const float& operator[](size_t i) const;

        Vec3& operator+=(const Vec3& rhs);
        Vec3& operator+=(float scalar);
        Vec3& operator-=(const Vec3& rhs);
        Vec3& operator-=(float scalar);
        Vec3& operator*=(const Vec3& rhs);
        Vec3& operator*=(float scalar);
        Vec3& operator/=(const Vec3& rhs);
        Vec3& operator/=(float scalar);

        static Vec3 Max() { return Vec3(std::numeric_limits<float>::max()); }
    };

    export Vec3 operator-(const Vec3& lhs);
    export Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
    export Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
    export Vec3 operator*(const Vec3& lhs, const Vec3& rhs);
    export Vec3 operator*(const Vec3& lhs, float scalar);
    export Vec3 operator/(const Vec3& lhs, const Vec3& rhs);
    export Vec3 operator/(const Vec3& lhs, float scalar);
    export bool operator==(const Vec3& lhs, const Vec3& rhs);
    export bool operator!=(const Vec3& lhs, const Vec3& rhs);

    export float MagnitudeSquared(const Vec3& vec);
    export float Magnitude(const Vec3& vec);
    export Vec3 Normalized(const Vec3& vec);
    export float Dot(const Vec3& v1, const Vec3& v2);
    export Vec3 Cross(const Vec3& v1, const Vec3& v2);
    export Vec3 Min(const Vec3& a, const Vec3& b);
    export Vec3 Max(const Vec3& a, const Vec3& b);
}

export template <>
struct std::formatter<se::math::Vec3>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <class FormatContext>
    auto format(const se::math::Vec3& obj, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "vec3({}, {}, {})", obj.x, obj.y, obj.z);
    }
};

export template <>
struct std::hash<se::math::Vec3>
{
    std::size_t operator()(const se::math::Vec3& vec) const
    {
        size_t hashX = std::hash<float>()(vec.x);
        size_t hashY = std::hash<float>()(vec.y);
        size_t hashZ = std::hash<float>()(vec.z);

        return hashX ^ (hashY << 1) ^ (hashZ << 2);
    }
};
