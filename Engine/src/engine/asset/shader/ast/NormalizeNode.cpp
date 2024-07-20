#include "NormalizeNode.h"

namespace se::asset::shader::ast
{
    std::string NormalizeNode::GetDebugString() const
    {
        return "NormalizeNode";
    }

    void NormalizeNode::ToGlsl(string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("normalize(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            const auto& child = m_Children[i];
            child->ToGlsl(outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
