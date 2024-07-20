#include "MainNode.h"

namespace se::asset::shader::ast
{
    std::string MainNode::GetDebugString() const
    {
        return "MainNode";
    }

    void MainNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader.append("void main()\n{\n");
        for (const auto& child : m_Children)
        {
           child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
