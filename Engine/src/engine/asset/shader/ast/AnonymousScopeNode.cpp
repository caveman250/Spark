#include "AnonymousScopeNode.h"

namespace se::asset::shader::ast
{
    std::string AnonymousScopeNode::GetDebugString() const
    {
        return "AnonymousScopeNode";
    }

    void AnonymousScopeNode::ToGlsl(ShaderCompileContext& context, string::ArenaString &outShader) const
    {
        outShader.append("{\n");
        for (const auto& child : m_Children)
        {
            child->ToGlsl(context, outShader);
        }
        outShader.append("}\n");
    }

    void AnonymousScopeNode::ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader.append("{\n");
        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }
        outShader.append("}\n");
    }
}
