#include "DotNode.h"

namespace se::asset::shader::ast
{
    std::string DotNode::GetDebugString() const
    {
        return "DotNode";
    }

    void DotNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("dot(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            auto* child = m_Children[i];
            child->ToGlsl(outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
