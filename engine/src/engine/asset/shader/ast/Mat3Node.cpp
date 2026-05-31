#include "Mat3Node.h"

namespace se::asset::shader::ast
{
    std::string Mat3Node::GetDebugString() const
    {
        return "Mat3Node";
    }

    void Mat3Node::ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("mat3(");
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

    void Mat3Node::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        auto alloc = outShader.get_allocator();
        outShader.append("float3x3(");
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
