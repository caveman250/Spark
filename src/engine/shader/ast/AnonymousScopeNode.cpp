#include "AnonymousScopeNode.h"

namespace se::shader::ast
{
    void AnonymousScopeNode::ToGlsl(string::ArenaString &outShader) const
    {
        outShader.append("{\n");
        for (auto* child : Children)
        {
            child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
