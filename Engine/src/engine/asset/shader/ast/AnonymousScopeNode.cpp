#include "AnonymousScopeNode.h"

namespace se::asset::shader::ast
{
    DEFINE_SPARK_CLASS_BEGIN(AnonymousScopeNode)
        DEFINE_MEMBER(m_Children)
    DEFINE_SPARK_CLASS_END()

    std::string AnonymousScopeNode::GetDebugString() const
    {
        return "AnonymousScopeNode";
    }

    void AnonymousScopeNode::ToGlsl(string::ArenaString &outShader) const
    {
        outShader.append("{\n");
        for (const auto& child : m_Children)
        {
            child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
