#pragma once

#include "engine/reflect/Reflect.h"

namespace se::render
{
    SPARK_ENUM()
    enum class DepthCompare
    {
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    };

    SPARK_ENUM()
enum class BlendMode
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
enum class StencilFunc
{
        Less,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        None
    };

    struct RenderState : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        DepthCompare depthComp = DepthCompare::None;

        SPARK_MEMBER(Serialized)
        BlendMode srcBlend = BlendMode::One;

        SPARK_MEMBER(Serialized)
        BlendMode dstBlend = BlendMode::Zero;

        SPARK_MEMBER(Serialized)
        uint32_t stencilWriteMask = 0x00;

        SPARK_MEMBER(Serialized)
        StencilFunc stencilFunc = StencilFunc::None;

        SPARK_MEMBER(Serialized)
        uint32_t stencilReadMask = 0x00;

        SPARK_MEMBER(Serialized)
        bool lit = false;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
    bool operator<(const  RenderState& lhs, const RenderState& rhs);
}
