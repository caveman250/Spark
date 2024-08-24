#include "RenderState.h"

DEFINE_SPARK_ENUM_BEGIN(se::render::DepthCompare)
   DEFINE_ENUM_VALUE(DepthCompare, Less)
   DEFINE_ENUM_VALUE(DepthCompare, LessEqual)
   DEFINE_ENUM_VALUE(DepthCompare, Equal)
   DEFINE_ENUM_VALUE(DepthCompare, Greater)
   DEFINE_ENUM_VALUE(DepthCompare, GreaterEqual)
   DEFINE_ENUM_VALUE(DepthCompare, None)
DEFINE_SPARK_ENUM_END()

DEFINE_SPARK_ENUM_BEGIN(se::render::BlendMode)
   DEFINE_ENUM_VALUE(BlendMode, Zero)
   DEFINE_ENUM_VALUE(BlendMode, One)
   DEFINE_ENUM_VALUE(BlendMode, SrcColor)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusSrcColor)
   DEFINE_ENUM_VALUE(BlendMode, DstColor)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusDstColor)
   DEFINE_ENUM_VALUE(BlendMode, SrcAlpha)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusSrcAlpha)
   DEFINE_ENUM_VALUE(BlendMode, DstAlpha)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusDstAlpha)
   DEFINE_ENUM_VALUE(BlendMode, ConstantColor)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusConstantColor)
   DEFINE_ENUM_VALUE(BlendMode, ConstantAlpha)
   DEFINE_ENUM_VALUE(BlendMode, OneMinusConstantAlpha)
   DEFINE_ENUM_VALUE(BlendMode, None)
DEFINE_SPARK_ENUM_END()

namespace se::render
{
    GLuint BlendModeToGLBlendMode(BlendMode::Type blendMode)
    {
        switch (blendMode)
        {
            case BlendMode::Zero:
                return GL_ZERO;
            case BlendMode::One:
                return GL_ONE;
            case BlendMode::SrcColor:
                return GL_SRC_COLOR;
            case BlendMode::OneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case BlendMode::DstColor:
                return GL_DST_COLOR;
            case BlendMode::OneMinusDstColor:
               return GL_ONE_MINUS_DST_COLOR;
            case BlendMode::SrcAlpha:
                return GL_SRC_ALPHA;
            case BlendMode::OneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BlendMode::DstAlpha:
                return GL_DST_ALPHA;
            case BlendMode::OneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
            case BlendMode::ConstantColor:
               return GL_CONSTANT_COLOR;
            case BlendMode::OneMinusConstantColor:
               return GL_ONE_MINUS_CONSTANT_COLOR;
            case BlendMode::ConstantAlpha:
                return GL_CONSTANT_ALPHA;
            case BlendMode::OneMinusConstantAlpha:
                return GL_ONE_MINUS_CONSTANT_ALPHA;
            default:
                SPARK_ASSERT(false);
                return GL_ZERO;
        }
    }

    bool operator==(const RenderState& lhs, const RenderState& rhs)
    {
        return lhs.depthComp == rhs.depthComp &&
                lhs.srcBlend == rhs.srcBlend &&
                lhs.dstBlend == rhs.dstBlend;
    }
}
