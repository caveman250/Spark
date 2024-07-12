#include "EndOfExpressionNode.h"

namespace se::shader::ast
{
    std::string EndOfExpressionNode::GetDebugString() const
    {
        return "EndOfExpressionNode";
    }

    void EndOfExpressionNode::ToGlsl(string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }
}
