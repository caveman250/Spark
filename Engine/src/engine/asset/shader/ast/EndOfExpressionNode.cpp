#include "EndOfExpressionNode.h"

namespace se::asset::shader::ast
{
    std::string EndOfExpressionNode::GetDebugString() const
    {
        return "EndOfExpressionNode";
    }

    void EndOfExpressionNode::ToGlsl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }

    void EndOfExpressionNode::ToMtl(ShaderCompileContext&, string::ArenaString& outShader) const
    {
        outShader += ";\n";
    }
}
