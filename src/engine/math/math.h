#pragma once

namespace se::math
{
    static inline bool FloatEqual(float a, float b)
    {
        return std::abs(a - b) <= FLT_EPSILON;
    }
}
