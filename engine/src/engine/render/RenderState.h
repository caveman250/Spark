#pragma once

#include "engine/render/opengl/GL_fwd.h"
#include "engine/reflect/Reflect.h"

namespace se::render
{
    SPARK_ENUM()
    enum class DepthCompare : int
    {
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    };

    SPARK_ENUM()
enum class BlendMode : int
{
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
    };

    SPARK_ENUM()
enum class StencilFunc : int
{
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    };

    struct RenderState
    {
        DepthCompare depthComp = DepthCompare::None;
        BlendMode srcBlend = BlendMode::One;
        BlendMode dstBlend = BlendMode::Zero;
        uint32_t stencilWriteMask = 0x00;
        StencilFunc stencilFunc = StencilFunc::None;
        uint32_t stencilReadMask = 0x00;
        bool lit = false;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
}
