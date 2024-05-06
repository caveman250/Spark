#pragma once

namespace se::shader::ast
{
    enum class Type
    {
        Float,
        Vec2,
        Vec3,
        Vec4,
        Mat3,
        Mat4,
        Void,
        Sampler2D,
        Invalid
    };
}
