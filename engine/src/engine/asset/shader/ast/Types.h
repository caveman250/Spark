#pragma once

#include "engine/reflect/Reflect.h"

namespace se::asset::shader::ast
{
    SPARK_ENUM()
    enum class AstType : int
    {
        Int,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Mat3,
        Mat4,
        Void,
        Sampler2D,
        Bool,
        Invalid
    };
}
