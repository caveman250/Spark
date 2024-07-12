#include "AssignmentNode.h"

namespace se::shader::ast
{
    std::string AssignmentNode::GetDebugString() const
    {
        return "AssignmentNode";
    }

    void AssignmentNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += " = ";
    }
}
