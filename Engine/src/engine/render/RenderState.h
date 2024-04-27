#pragma once

namespace se::render
{
    enum class DepthCompare
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
        DepthCompare depthComp;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
}
