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
    bool operator==(const RenderState& lhs, const RenderState& rhs)
    {
        return lhs.depthComp == rhs.depthComp &&
                lhs.srcBlend == rhs.srcBlend &&
                lhs.dstBlend == rhs.dstBlend;
    }
}
