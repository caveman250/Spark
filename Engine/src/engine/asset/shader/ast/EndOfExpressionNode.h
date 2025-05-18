#pragma once
#include "ASTNode.h"

namespace se::asset::shader::ast
{
    class EndOfExpressionNode : public ASTNode
    {
        SPARK_CLASS(EndOfExpressionNode)

        std::string GetDebugString() const override;
        void ToGlsl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
        void ToMtl(ShaderCompileContext& context, string::ArenaString& outShader) const override;
    };
}
