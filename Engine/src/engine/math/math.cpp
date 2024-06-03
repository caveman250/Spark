#include "math.h"

namespace se::math
{
    bool FloatEqual(float a, float b)
    {
        return std::abs(a - b) <= FLT_EPSILON;
    }

    float Radians(float degrees)
    {
        return degrees * 0.01745329251994329576923690768489f;
    }

    float Degrees(float radians)
    {
        return radians * 57.295779513082320876798154814105f;
    }

    float InverseSqrt(float f)
    {
        return 1.f / std::sqrtf(f);
    }

    Mat4 AxisAngle(Vec3 axis, float angle)
    {
        angle = Radians(angle);
        const float c = cosf(angle);
        const float s = sinf(angle);
        const float t = 1.f - cosf(angle);

        if (!FloatEqual(MagnitudeSquared(axis), 1.f))
        {
            axis = Normalized(axis);
        }

        return { t * (axis.x * axis.x) + c,         t * axis.x * axis.y + s * axis.z,   t * axis.x * axis.z - s * axis.y, 0.f,
                   t * axis.x * axis.y - s * axis.z,  t * (axis.y * axis.y) + c,          t * axis.y * axis.z + s * axis.x, 0.f,
                   t * axis.x * axis.z + s * axis.y,  t * axis.y * axis.z - s * axis.x,   t * (axis.z * axis.z) + c,        0.f,
                   0.f,                               0.f,                                0.f,                              1.f };
    }
}
