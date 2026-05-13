#pragma once

#include "spark.h"
#include "engine/bits/EnumClassBitwise.h"

namespace se::asset::shader::ast
{
    SPARK_ENUM()
    enum class AstType
    {
        None = 0,
        Int = 1 << 0,
        Float = 1 << 1,
        Vec2 = 1 << 2,
        Vec3 = 1 << 3,
        Vec4 = 1 << 4,
        Mat3 = 1 << 5,
        Mat4 = 1 << 6,
        Void = 1 << 7,
        Sampler2D = 1 << 8,
        Sampler2DReference = 1 << 9,
        Bool = 1 << 10,
        Invalid = 1 << 11
    };

    ENUM_CLASS_ENABLE_BITWISE(AstType)
}
