#include "InverseNode.h"

namespace se::asset::shader::ast
{
    std::string InverseNode::GetDebugString() const
    {
        return "InverseNode";
    }

    void InverseNode::ToGlsl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("inverse(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            const auto& child = m_Children[i];
            child->ToGlsl(context, outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }

    void InverseNode::ToMtl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("inverse(");
        for (size_t i = 0; i < m_Children.size(); ++i)
        {
            const auto& child = m_Children[i];
            child->ToMtl(context, outShader);
            if (i < m_Children.size() - 1 && outShader.back() != '-')
            {
                outShader.append(",");
            }
        }
        outShader.append(")");
    }
}
