#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace se::math
{
    bool FloatEqual(float a, float b);
    float Radians(float degrees);
    float Degrees(float radians);
    float InverseSqrt(float f);
}
