#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

namespace se::math
{
    static inline bool FloatEqual(float a, float b)
    {
        return std::abs(a - b) <= FLT_EPSILON;
    }
}
