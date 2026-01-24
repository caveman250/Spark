#include "RenderState.h"

namespace se::render
{
    bool operator==(const RenderState& lhs,
                    const RenderState& rhs)
    {
        return lhs.depthComp == rhs.depthComp &&
               lhs.srcBlend == rhs.srcBlend &&
               lhs.dstBlend == rhs.dstBlend;
    }

    bool operator<(const RenderState& lhs,
                   const RenderState& rhs)
    {
        if (lhs.stencilWriteMask != rhs.stencilWriteMask)
        {
            return lhs.stencilWriteMask < rhs.stencilWriteMask;
        }

        if (lhs.stencilFunc != rhs.stencilFunc)
        {
            return lhs.stencilFunc < rhs.stencilFunc;
        }

        if (lhs.srcBlend != rhs.srcBlend)
        {
            return lhs.srcBlend < rhs.srcBlend;
        }

        if (lhs.dstBlend != rhs.dstBlend)
        {
            return lhs.dstBlend < rhs.dstBlend;
        }

        if (lhs.depthComp != rhs.depthComp)
        {
            return lhs.depthComp < rhs.depthComp;
        }

        if (lhs.stencilReadMask != rhs.stencilReadMask)
        {
            return lhs.stencilReadMask < rhs.stencilReadMask;
        }

        if (lhs.lit != rhs.lit)
        {
            return lhs.lit < rhs.lit;
        }

        return false;
    }
}
