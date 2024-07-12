#include "AnonymousScopeNode.h"

namespace se::shader::ast
{
    std::string AnonymousScopeNode::GetDebugString() const
    {
        return "AnonymousScopeNode";
    }

    void AnonymousScopeNode::ToGlsl(string::ArenaString &outShader) const
    {
        outShader.append("{\n");
        for (auto* child : m_Children)
        {
            child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
