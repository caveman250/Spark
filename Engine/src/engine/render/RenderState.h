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
        DepthCompare depthComp = DepthCompare::None;
        bool lit;
    };

    bool operator==(const RenderState& lhs, const RenderState& rhs);
}
