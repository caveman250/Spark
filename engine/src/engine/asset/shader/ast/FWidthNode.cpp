#include "FWidthNode.h"

namespace se::asset::shader::ast
{
    std::string FWidthNode::GetDebugString() const
    {
        return "FWidthNode";
    }

    void FWidthNode::ToGlsl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("fwidth(");
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

    void FWidthNode::ToMtl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("fwidth(");
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
