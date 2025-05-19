#include "RenderState.h"

namespace se::render
{
    bool operator==(const RenderState& lhs, const RenderState& rhs)
    {
        return lhs.depthComp == rhs.depthComp &&
                lhs.srcBlend == rhs.srcBlend &&
                lhs.dstBlend == rhs.dstBlend;
    }
}
