#include "AnonymousScopeNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(AnonymousScopeNode)
        DEFINE_SERIALIZED_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string AnonymousScopeNode::GetDebugString() const
    {
        return "AnonymousScopeNode";
    }

    void AnonymousScopeNode::ToGlsl(const ShaderCompileContext& context, string::ArenaString &outShader) const
    {
        outShader.append("{\n");
        for (const auto& child : m_Children)
        {
            child->ToGlsl(context, outShader);
        }
        outShader.append("}\n");
    }

    void AnonymousScopeNode::ToMtl(const ShaderCompileContext& context, string::ArenaString& outShader) const
    {
        outShader.append("{\n");
        for (const auto& child : m_Children)
        {
            child->ToMtl(context, outShader);
        }
        outShader.append("}\n");
    }
}
