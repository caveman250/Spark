#pragma once

#include "engine/render/opengl/GL_fwd.h"
#include "engine/reflect/Reflect.h"

namespace se::render
{
    DECLARE_SPARK_ENUM_BEGIN(DepthCompare, int)
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    DECLARE_SPARK_ENUM_END()

    DECLARE_SPARK_ENUM_BEGIN(BlendMode, int)
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha,
        None
    DECLARE_SPARK_ENUM_END()

    GLuint BlendModeToGLBlendMode(BlendMode::Type blendMode);

    struct RenderState
    {
        DepthCompare::Type depthComp = DepthCompare::None;
        BlendMode::Type srcBlend = BlendMode::One;
        BlendMode::Type dstBlend = BlendMode::Zero;
        bool lit = false;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
}
