module;

#include "engine/string/ArenaString.h"

module Spark.Asset.Shader;
import :EndOfExpressionNode;

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
