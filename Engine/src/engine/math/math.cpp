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
}
