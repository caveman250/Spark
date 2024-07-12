#include "PowNode.h"

namespace se::shader::ast
{
    std::string PowNode::GetDebugString() const
    {
        return "PowNode";
    }

    void PowNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("pow(");
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
