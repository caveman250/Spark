#include "AbstractFunctionNode.h"

namespace se::asset::shader::ast
{
    AbstractFunctionNode::AbstractFunctionNode(const std::string& name,
        const std::string& glslName,
        const std::string& metalName)
        : m_Name(name)
        , m_GlslName(glslName)
        , m_MetalName(metalName)
    {
    }

    std::string AbstractFunctionNode::GetDebugString() const
    {
        return m_Name;
    }

    void AbstractFunctionNode::ToGlsl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(m_GlslName);
        outShader.append("(");
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

    void AbstractFunctionNode::ToMtl(ShaderCompileContext& context,
        string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append(m_MetalName);
        outShader.append("(");
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
