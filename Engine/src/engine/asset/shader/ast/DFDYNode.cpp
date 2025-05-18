#include "DFDYNode.h"

namespace se::asset::shader::ast
{
    std::string DFDYNode::GetDebugString() const
    {
        return "DFDYNode";
    }

    void DFDYNode::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("dFdy(");
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

    void DFDYNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("dfdy(");
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
