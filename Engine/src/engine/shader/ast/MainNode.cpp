#include "MainNode.h"

namespace se::shader::ast
{
    void MainNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader.append("void main()\n{\n");
        for (auto* child : m_Children)
        {
           child->ToGlsl(outShader);
        }
        outShader.append("}\n");
    }
}
