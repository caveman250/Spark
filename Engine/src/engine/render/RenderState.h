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
        None
    DECLARE_SPARK_ENUM_END()

    DECLARE_SPARK_ENUM_BEGIN(StencilFunc, int)
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    DECLARE_SPARK_ENUM_END()

    struct RenderState
    {
        DepthCompare::Type depthComp = DepthCompare::None;
        BlendMode::Type srcBlend = BlendMode::One;
        BlendMode::Type dstBlend = BlendMode::Zero;
        uint32_t stencilWriteMask = 0x00;
        StencilFunc::Type stencilFunc = StencilFunc::None;
        uint32_t stencilReadMask = 0x00;
        bool lit = false;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
}
