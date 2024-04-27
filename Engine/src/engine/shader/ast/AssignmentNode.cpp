#include "AssignmentNode.h"

namespace se::shader::ast
{
    void AssignmentNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += " = ";
    }
}
