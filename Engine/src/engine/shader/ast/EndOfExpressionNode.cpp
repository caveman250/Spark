#include "EndOfExpressionNode.h"

namespace se::shader::ast
{
    void EndOfExpressionNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }
}
