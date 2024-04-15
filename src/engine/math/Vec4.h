#pragma once

namespace se::math
{
    struct Vec4
    {
        inline Vec4() : x(0.f), y(0.f), z(0.f), w(0.f)
        {
        }

        inline Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
        {
        }

        union
        {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

            float asArray[4];
        };

        float operator[](int i) const
        {
            return asArray[i];
        }

        Vec4 operator+(const Vec4 &rhs) const;
        Vec4 operator+=(const Vec4 &rhs) const;
        Vec4 operator-(const Vec4 &rhs) const;
        Vec4 operator*(const Vec4 &rhs) const;
        Vec4 operator*(float scalar) const;
        Vec4 operator/(const Vec4 &rhs) const;
        Vec4 operator/(float scalar) const;
        bool operator==(const Vec4 &rhs) const;
        bool operator!=(const Vec4 &rhs) const;
    };

    static Vec4 operator-(const Vec4 &lhs) { return {-lhs.x, -lhs.y, -lhs.z, -lhs.w}; }
}

template <>
    struct std::formatter<se::math::Vec4>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(const se::math::Vec4& obj, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "vec4({}, {}, {}, {})", obj.x, obj.y, obj.z, obj.w);
    }
};
